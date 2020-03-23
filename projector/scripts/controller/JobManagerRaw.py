import os
import re
import io
import pdb
from multiprocessing import Process
from shutil import copyfile
import threading
from subprocess import check_call, Popen, PIPE, check_output, STDOUT, CalledProcessError
import time
from JobManager import JobManager
import numpy
#import tempfile
#import imageio
from PIL import Image
import glob

def trampoline(object):
    #object._logger.debug("Worker")
    while object._wmrunning:
        object._workerManager()
    #object._logger.info("workerManager ends")

class JobManagerRaw(JobManager):
#    PrecropS8Geometry="2912x2200+0+250"
#    Precrop8mmGeometry="2868x1800+300+500"
    JobLimit = 9 
##    WorkerManagerControl = False # True # 'Off'
#    DisablePrecrop = True
#    DisableAutocrop = False
#
    def schedulableTasks(self):
        tasks = self._pstore.taskList(self._config.project)
        #self._logger.debug("tasks %s" % tasks)
        self._scheduledTasks = []
        JobManagerRaw.taskTable = { 
            "im": self._scheduleImport,
            "rf": self._scheduleProcessRaw,
            "pc": self._schedulePrecrop,
            "ac": self._scheduleAutocrop,
            "tf": self._scheduleTonefuse,
            "gt": self._scheduleGenTitle,
            "gc": self._scheduleGenContent 
            }

        for task in tasks:
            try:
                self._scheduledTasks.append(JobManagerRaw.taskTable[task])
            except KeyError as ee:
                self._logger.error("Unknown task [%s]" % task)

    def __init__(self, logger, pstore, fileman, config, root):
        self._precropframe = open(fileman.getPrecropAdjFile(config.project)).read()
        JobManager.__init__(self, logger, pstore, fileman, config, root)
#
#        if 'disable' == self._config.jobmode: # JobManager.WorkerManagerControl:
#            return
#        self._wmrunning = True
        if 'proc' == self._config.jobmode: # JobManager.WorkerManagerControl:
            pass
#            self._thread = threading.Thread(target = trampoline, args = (self,))
#            self._thread.start()
        elif 'inline' == self._config.jobmode:
            self._workerManager()
##        if 'proc' == mode: # JobManager.WorkerManagerControl:
##            self._thread = threading.Thread(target = worker, args = (self,))
##            self._thread.start()
##        elif 'inline' == mode:
##            self._workerManager()

    def _scheduleImport(self, freeWorkers):
        for file in glob.iglob("%s/*.raw" % self._config.iimport):
            tag = re.search('[0-9]*[abc]?.raw', file)
            if tag is None:
                logger.error("Cannot find tag on %s" % file)
                sys.exit(1)
            tag = tag.group()[6]
            container, filename = self._pstore.getNextImageLocation(self._config.project, tag)
            self._fileman.newImport(file, self._config.project, container, filename, tag)
            self._pstore.newRawFile(self._config.project, container, filename, tag)
        sys.exit(0)
    
    def _scheduleProcessRaw(self, freeWorkers):
        self._logger.debug("scheduleProcessRaw")
        scheduled = 0
        todo = self._pstore.toBeConverted(self._config.project, freeWorkers)
        self._logger.debug('toBeConverted %s' % str(todo))
#        tempdir = tempfile.TemporaryDirectory()
        if todo:
            for (rowid, container, filename,tag) in todo:
                self._logger.debug("Container %s filename %s" % (container, filename))
                jobargs = (self._config.project, container, filename, tag, rowid)
                self._logger.info("Calling %s" % str(jobargs))
                if 'inline' == self._config.jobmode: # JobManager.WorkerManagerControl == True:
                    self._vmConvert(*jobargs)
                else:
                    self._workers.append(Process(target = self._vmConvert, args = jobargs))
                    self._workers[-1].start()
                scheduled += 1
        return scheduled

    def _schedulePrecrop(self, freeWorkers):
        scheduled = 0
        toBePrecropped = self._pstore.toBePrecropped(self._config.project, freeWorkers)
        self._logger.debug('toBePrecropped %s' % str(toBePrecropped))
        
        if toBePrecropped:
            for (rowid, container, filename, tag) in toBePrecropped:
                self._logger.debug("Container %s filename %s" % (container, filename))
                jobargs = (self._config.project, container, filename, tag, rowid)
                if 'inline' == self._config.jobmode: # JobManager.WorkerManagerControl == True:
                    self._vmPrecrop(*jobargs)
                else:
                    self._workers.append(Process(target = self._vmPrecrop, args = jobargs))
                    self._workers[-1].start()
                scheduled += 1
        return scheduled

    def _scheduleAutocrop(self, freeWorkers):
        scheduled = 0
        toBeAutocropped = self._pstore.toBeAutocropped(self._config.project, freeWorkers)
        self._logger.debug('toBeAutocropped untrimmed %s' % str(toBeAutocropped))
        if toBeAutocropped:
            #toBeAutocropped = filter(lambda x: not x[0]%3, zip(range(len(toBeAutocropped)), toBeAutocropped))
            #self._logger.debug('toBeAutocropped1 %s' % str(toBeAutocropped))
            #toBeAutocropped = zip(*toBeAutocropped[::-1])[1]
            #self._logger.debug('toBeAutocropped2 %s' % str(toBeAutocropped))
            for (rowid, container, filename) in toBeAutocropped:
                jobargs = (self._config.project, rowid, container, filename, self._config.single)
                if 'inline' == self._config.jobmode: # JobManager.WorkerManagerControl == True:
                    self._vmAutocrop(*jobargs)
                else:
                    self._workers.append(Process(target = self._vmAutocrop, args = jobargs))
                    self._workers[-1].start()
                    scheduled += 1
        return scheduled

    def _scheduleTonefuse(self, freeWorkers):
        scheduled = 0
        toBeFused = self._pstore.toBeFused(self._config.project, freeWorkers * 3)
        self._logger.debug("toBeFused %s" % str(toBeFused))
        while len(toBeFused) >= 3:
            jobargs = (self._config.project, toBeFused[0][1], toBeFused[0][2], toBeFused[1][2], toBeFused[2][2])
 
            if 'inline' == self._config.jobmode: # JobManager.WorkerManagerControl == True:
                self._vmTonefuse(*jobargs)
            else:
                self._workers.append(Process(target = self._vmTonefuse, args = jobargs))
                self._workers[-1].start()
                scheduled += 1
            del toBeFused[:3]
        return scheduled
 
    def _scheduleGenTitle(self, freeWorkers):
        if False == self._generateTitles: return 0
        if 'inline' == self._config.jobmode: 
            self._vmGenTitle(self._config.project, self._root)
            return 0
        else:
            self._workers.append(Process(target = self._vmGenTitle,
                args = (self._config.project, self._root)))
            self._workers[-1].start()
        self._generateTitles = False
        return 1    

    def _scheduleGenContent(self, freeWorkers):
#        chunks, processed = self._pstore.getVideoChunk(self._config.project)
#        todo = [ee.encode('ascii', 'ignore') for ee in chunks if ee not in processed]
#        if 0 == len(todo):
#            return 0
        if 'inline' == self._config.jobmode: 
            self._vmGenContent(self._config.project, self._root)
        else:
            self._workers.append(Process(target = self._vmGenContent,
                args = (self._config.project, self._root)))
            self._workers[-1].start()
        return 1    
#
#    def shutdown(self):
#        self._wmrunning = False
#        map(lambda pp: pp.terminate(), self._workers)
#
    def _workerManager(self):
        #self._logger.debug("_workerManager")
        self._workerCleanup()
        self.schedulableTasks()
       #self._logger.debug("_workerManager")

        freeWorkers = JobManager.JobLimit - len(self._workers)
        if 0 == freeWorkers:
            self._logger.info("No free workers")
            time.sleep(1)
            return

        #self._logger.debug("_scheduledTasks %s" % self._scheduledTasks)
        for task in self._scheduledTasks:
            self._logger.debug("Task %s" % task)
            if freeWorkers: freeWorkers -= task(freeWorkers)

        if freeWorkers:
            time.sleep(5)


    def _vmTonefuse(self, project, container, file1, file2, file3):
        self._logger.info("Tonefuse %s %s %s %s %s" % (project, container, file1, file2, file3))
        sourceDir = os.path.abspath(self._fileman.getAutocropDir(project, container))
        source1 = "%s/%sa.jpg" % (sourceDir, file1)
        source2 = "%s/%sb.jpg" % (sourceDir, file2)
        source3 = "%s/%sc.jpg" % (sourceDir, file3)
        outputfile = self._fileman.getTonefuseDir(project, container) + '/%s.jpg' % file1
        jobargs = ('enfuse', '--hard-mask', '--saturation-weight=0.1',
        '--output', outputfile, source1, source2, source3)
        self._logger.info("Calling %s" % ' '.join(jobargs))
        try:
            #check_call(jobargs)
            output = check_output(jobargs, stderr=STDOUT)
            self._logger.info(output)
            self._logger.info("Done %s %s %s" % (project, container, outputfile))
            self._pstore.markFused(project, container, file1)
            self._logger.debug("_wmTonefuse Done")

        except CalledProcessError as ee:
            self._logger.error("Tonefuse failed rc %d $s" % (ee.returncode, ee.output))
            self._pstore.abortTonefuse(project, container, file1, file2, file3)
#    def _vmAutocropShort(self, project, container, file1, file2, file3):
#        self._logger.info("vmAutoCropShort %s %s %s %s %s" % (project, container, file1, file2, file3))
#        sourceDir = os.path.abspath(self._fileman.getRawFileDir(project, container))
#        source1 = "%s/%s" % (sourceDir, file1)
#        source2 = "%s/%s" % (sourceDir, file2)
#        source3 = "%s/%s" % (sourceDir, file3)
#        outputdir = self._fileman.getAutocropDir(project, container)
#        open("%s/%s" % (outputdir, file1),'w').write(open(source1).read())
#        open("%s/%s" % (outputdir, file2),'w').write(open(source2).read())
#        open("%s/%s" % (outputdir, file3),'w').write(open(source3).read())
#        self._pstore.markAutocropped(project, container, file1, file2, file3)
#
    def _vmAutocrop(self, project, rowid, container, filename,single):
        self._logger.info("Autocrop %s %s %s %s" % (project, rowid, container, filename))
#        pdb.set_trace()
        source = os.path.abspath(self._fileman.getPrecropDir(project, container)) + '/'
        #source = os.path.abspath(self._fileman.getConvertedDir(project, container)) + '/'
        dest = os.path.abspath(self._fileman.getAutocropDir(project, container))
        jobargs = ['../autocroph.py', '-d', '--filenames',
            '%s,%s,%s' % (source + filename + 'a.jpg',source + filename + 'b.jpg',source + filename + 'c.jpg'),
            '--mode', self._config.film,
            '--output-dir', dest,
            '--adjfile', self._fileman.getAutocropAdjFile(project)]
        if single:
            jobargs.append('--single')
        self._logger.debug("Calling %s" % str(jobargs))
        try:
#            #check_call(jobargs)
            fileno = self._logger.handlers[0].stream.fileno()
            retcode = check_call(jobargs, stderr=fileno, stdout=fileno)
            self._logger.debug("retcode %u" % retcode)
#            self._logger.info("Done %s %s %s" % (project, container, outputdir))
            if 0 == retcode:
                self._pstore.markAutocropped(project, filename, rowid)
            else:
                self._logger.error("Failed (%u) jobargs %s" % (retcode, str(jobargs)))
            self._logger.debug("_wmAutocrop Done")
#
        except CalledProcessError as ee:
            print(ee.output)
#            self._logger.error("autocrop failed rc %s $s" % (str(ee.returncode), str(ee.output)))
#            self._pstore.abortAutocrop(project, container, file1, file2, file3)

    def _vmConvert(self, project, container, filename, tag, rowid):
        source = os.path.abspath(self._fileman.getRawFileLocation(project, container, filename))
        raw = os.path.abspath(self._fileman.getConvertedDir(project, container) + "/%s" % filename)
        #ppm = tempdir + "/%s.ppm" % tag
        jpg = raw + "%s.jpg" % tag
#        raw = raw + "%s.RAW" % tag
        source += "%s.RAW" % tag
        #self._logger.info("source %s raw %s ppm %s jpg %s" % (source, raw, ppm, jpg))
        self._logger.info("source %s %s" % (source, jpg))
        #if False == os.path.isfile(jpg):
#        copyfile(source, raw)
        
        jobargs = ('/personal-projects/projector/raspiraw/dcraw/dcraw', '-c', source)
        self._logger.debug("Calling %s in %s" % (' '.join(jobargs), os.getcwd()))
   #     retcode = call(jobargs)
        try:
            ppmdata = check_output(jobargs)
            self._logger.debug("dcraw returns %u bytes" % len(ppmdata))

        except:
            self._logger.error("dcraw fail")
            return

        try:
            jobargs = ('convert', '-', jpg)
            #check_call(jobargs, io.BytesIO(ppmdata))
            p = Popen(jobargs, stdin=PIPE)
            o = p.communicate(input=ppmdata)
        except:
            self._logger.error("convert fail")
            return

        #os.remove(raw)
        #os.remove(ppm)
        self._pstore.markConverted(project, filename, tag, rowid)

#    def _vmPrecrop(self, project, container, filename):
#        self._logger.info("_vmPrecrop short %s %s %s" % (project, container, filename))
#        sourcefile = os.path.abspath(self._fileman.getRawFileLocation(project, container, filename))
#        targetfile = os.path.abspath(self._fileman.getPrecropDir(project, container) + "/%s" % filename)
#
##        pdb.set_trace() 
#        jobargs = ('/home/mattd/personal-projects/projector/raspiraw/dcraw/dcraw', sourcefile)
#        retcode = call(jobargs)
#        sourcefile = sourcefile.replace('.RAW','.ppm')
#        refFile = imageio.imread("%s/reference0.bmp" % self._config.saveroot)
#        rgbRef = numpy.full(refFile.shape, 255, dtype=numpy.uint8)
#        delta = rgbRef - refFile
#        source = imageio.imread(sourcefile)
#        rgbsource = numpy.full(source.shape, 255, dtype=numpy.uint8)
#        adjust = source - delta
#        imageio.imwrite(targetfile.replace(".RAW", ".JPG"), adjust)
#        self._pstore.markPrecropped(project, container, filename)
##

    def _vmPrecrop(self, project, container, filename, tag, rowid):
        self._logger.info("Precrop %s %s %s" % (project, container, filename))
        sourcefile = os.path.abspath(self._fileman.getConvertedLocation(project, container, filename)) + tag + '.jpg'
        targetfile = os.path.abspath(self._fileman.getPrecropDir(project, container) + "/%s" % filename) + tag + '.jpg'
#        pdb.set_trace()
        jobargs = ('convert', sourcefile,
            #'-strip', '-flop', '-flip',
#            '-contrast-stretch', '0',
#            '-normalize',
            '-strip', 
            '-crop', self._precropframe,
#            '-crop', {'8mm': JobManager.Precrop8mmGeometry,
#                'super8': JobManager.PrecropS8Geometry}[self._config.film],
            targetfile)
        self._logger.debug("Calling %s" % ' '.join(jobargs))
#        pdb.set_trace()
        if True == JobManager.DisablePrecrop:
            self._logger.debug("Precrop disabled")
        else:
            try:
                #retcode = call(jobargs)
                self._logger.info( check_output(jobargs, stderr=STDOUT))
                self._logger.info("Done %s %s %s" % (project, container, filename))
                self._pstore.markPrecropped(project, filename, tag, rowid)

            except CalledProcessError as ee:
                self._logger.error("precrop failed rc %d $s" % (ee.returncode, ee.output))

        self._logger.debug("_vmPrecrop Done")

    def _vmGenTitle(self, project, root):
        jobargs = ('../gentitle.sh', '-p', project, '-r', root)
        self._logger.debug("Calling %s" % ' '.join(jobargs))
#        retcode = check_call(jobargs)
        try:
            self._logger.info( check_output(jobargs, stderr=STDOUT))

        except CalledProcessError as ee:
            self._logger.error("gentitle failed rc %d $s" % (ee.returncode, ee.output))

    def _vmGenContent(self, project, root):
#        self._pstore.markChunkProcessing(project, container)
        jobargs = ('../gencontent.sh', '-p', project, '-r', root)
#
#        try:
#            self._logger.info(jobargs)
#            #output = check_output(jobargs)
#            check_call(jobargs, stderr=STDOUT)
#            #self._logger.debug(output)
#            self._pstore.markChunkProcessed(project, container)
#
#        except CalledProcessError as ee:
#            self._logger.error("gencontent failed rc %d $s" % (ee.returncode, ee.output))
#        #retcode = call(jobargs)
#        #if 0 == retcode:
#        #return retcode
#
#    def _workerCleanup(self):
#        #self._logger.debug("Workers before cleanup: %d" % len(self._workers))
#        self._workers = [xx for xx in self._workers if xx.is_alive()]
#        #self._logger.debug("Workers after cleanup: %d" % len(self._workers))
#
#    def quiescent(self):
#        if len(self._workers):
#            return False
#        return True
#
#    def genTitle(self):
#        self._generateTitles = True
#
    def uploadsDone(self, project):
        pass
        #self._pstore.setTask(project, ['rf'])
        #self._pstore.setTask(project, ['ac','tf','gc'])
        #self._pstore.setTask(project, ['pc','ac','tf','gc'])
