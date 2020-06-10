import os
import pdb
from multiprocessing import Process
import threading
import subprocess
import time

def trampoline(object):
#    object._logger.debug("Worker")
    while object._wmrunning:
        object._workerManager()
#    object._logger.info("workerManager ends")

class JobManager():
# width x height +x + y offset
    PrecropS8Geometry="2912x2200+200+0"
    Precrop8mmGeometry="2868x1800+300+500"
    JobLimit = 4 
#    WorkerManagerControl = False # True # 'Off'
    DisablePrecrop = False
    DisableAutocrop = False

    def schedulableTasks(self):
        tasks = self._pstore.taskList(self._config.project)
        self._scheduledTasks = []
        taskTable = { 
            "pc": self._schedulePrecrop,
            "ac": self._scheduleAutocrop,
            "tf": self._scheduleTonefuse,
            "gt": self._scheduleGenTitle,
            "gc": self._scheduleGenContent }

        for task in tasks:
            try:
                self._scheduledTasks.append(taskTable[task])
            except KeyError as ee:
                self._logger.error("Unknown task [%s]" % task)

    def __init__(self, logger, pstore, fileman, config, root):
        self._logger = logger
        self._pstore = pstore
        self._fileman = fileman
        self._workers = []
        self._logger.debug("JobManager init")
        self._root = root
        self._generateTitles = False
        self._config = config
#        self._scheduledTasks = [
#            self._schedulePrecrop,
#            self._scheduleAutocrop,
#            self._scheduleTonefuse,
#            self._scheduleGenTitle,
#            self._scheduleGenChunk]

#        if config.workers is not None:
#            self._scheduledTasks = []
#            taskTable = { 
#                "pc": self._schedulePrecrop,
#                "ac": self._scheduleAutocrop,
#                "tf": self._scheduleTonefuse,
#                "gt": self._scheduleGenTitle,
#                "gc": self._scheduleGenChunk}
#
#            for worker in config.workers:
#                try:
#                    self._scheduledTasks.append(taskTable[worker])
#                except KeyError as ee:
#                    logger.debug("Unknown workers parameter [%s]" % worker)
#                    sys.exit(1)
#
#
        if 'disable' == self._config.jobmode: # JobManager.WorkerManagerControl:
            return
        self._wmrunning = True
        if 'proc' == self._config.jobmode: # JobManager.WorkerManagerControl:
            self._thread = threading.Thread(target = trampoline, args = (self,))
            self._thread.start()
        elif 'inline' == self._config.jobmode:
            self._workerManager()
#        if 'proc' == mode: # JobManager.WorkerManagerControl:
#            self._thread = threading.Thread(target = worker, args = (self,))
#            self._thread.start()
#        elif 'inline' == mode:
#            self._workerManager()

    def _schedulePrecrop(self, freeWorkers):
        scheduled = 0
        toBePrecropped = self._pstore.toBePrecropped(self._config.project, freeWorkers)
        if toBePrecropped:
            for (rowid, container, filename) in toBePrecropped:
                self._logger.debug("Container %s filename %s" % (container, filename))
                jobargs = (self._config.project, container, filename)
                if 'inline' == self._config.jobmode: # JobManager.WorkerManagerControl == True:
                    self._vmPrecrop(*jobargs)
                else:
                    self._workers.append(Process(target = self._vmPrecrop, args = jobargs))
                    self._workers[-1].start()
                scheduled += 1
        return scheduled
 
    def _scheduleAutocrop(self, freeWorkers):
        scheduled = 0
        todo = self._pstore.toBeAutocropped(self._config.project, freeWorkers * 3)
        while len(todo) >= 3:
#            testargs = {todo[0][1]: 1}
#            testargs[todo[1][1]] = 1
#            testargs[todo[2][1]] = 1
#            if len(testargs.keys()) > 1:
#                self._logger.error("Mismatched container values")
#                del todo[:3]
 
            jobargs = (self._config.project, todo[0][1], todo[0][2], todo[1][2], todo[2][2])
 
            if 'inline' == self._config.jobmode: # JobManager.WorkerManagerControl == True:
                self._vmAutocrop(*jobargs)
            else:
                self._workers.append(Process(target = self._vmAutocrop, args = jobargs))
                self._workers[-1].start()
                scheduled += 1
            del todo[:3]
        return scheduled

    def _scheduleTonefuse(self, freeWorkers):
        scheduled = 0
        todo = self._pstore.toBeFused(self._config.project, freeWorkers * 3)
        while len(todo) >= 3:
            jobargs = (self._config.project, todo[0][1], todo[0][2], todo[1][2], todo[2][2])
 
            if 'inline' == self._config.jobmode: # JobManager.WorkerManagerControl == True:
                self._vmTonefuse(*jobargs)
            else:
                self._workers.append(Process(target = self._vmTonefuse, args = jobargs))
                self._workers[-1].start()
                scheduled += 1
            del todo[:3]
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
        chunks, processed = self._pstore.getVideoChunkStatus(self._config.project)
        todo = [ee.encode('ascii', 'ignore') for ee in chunks if ee not in processed]
        if 0 == len(todo):
            return 0
        if 'inline' == self._config.jobmode: 
            self._vmGenContent(self._config.project, self._root, todo[0])
        else:
            self._workers.append(Process(target = self._vmGenContent,
                args = (self._config.project, self._root, todo[0])))
            self._workers[-1].start()
        return 1    

    def shutdown(self):
        self._wmrunning = False
        map(lambda pp: pp.terminate(), self._workers)

    def _workerManager(self):
        self._workerCleanup()
        self.schedulableTasks()

        freeWorkers = JobManager.JobLimit - len(self._workers)
        if 0 == freeWorkers:
#            self._logger.info("No free workers")
            time.sleep(1)
            return

        for task in self._scheduledTasks:
            if freeWorkers: freeWorkers -= task(freeWorkers)

        if freeWorkers:
            time.sleep(5)

    def _vmTonefuse(self, project, container, file1, file2, file3):
        self._logger.info("Tonefuse %s %s %s %s %s" % (project, container, file1, file2, file3))
        sourceDir = os.path.abspath(self._fileman.getAutocropDir(project, container))
        source1 = "%s/%s" % (sourceDir, file1)
        source2 = "%s/%s" % (sourceDir, file2)
        source3 = "%s/%s" % (sourceDir, file3)
        outputfile = self._fileman.getTonefuseDir(project, container) + '/%s' % file1
        jobargs = ('enfuse', '--output', outputfile, source1, source2, source3)
        self._logger.info("Calling %s" % ' '.join(jobargs))
        try:
            #subprocess.check_call(jobargs)
            output = subprocess.check_output(jobargs, stderr=subprocess.STDOUT)
            self._logger.info(output)
            self._logger.info("Done %s %s %s" % (project, container, outputfile))
            self._pstore.markFused(project, container, file1, file2, file3)
            self._logger.debug("_wmTonefuse Done")

        except subprocess.CalledProcessError as ee:
            self._logger.error("Tonefuse failed rc %d $s" % (ee.returncode, ee.output))
            self._pstore.abortTonefuse(project, container, file1, file2, file3)

    def _vmAutocropShort(self, project, container, file1, file2, file3):
        self._logger.info("vmAutoCropShort %s %s %s %s %s" % (project, container, file1, file2, file3))
        sourceDir = os.path.abspath(self._fileman.getPrecropDir(project, container))
        source1 = "%s/%s" % (sourceDir, file1)
        source2 = "%s/%s" % (sourceDir, file2)
        source3 = "%s/%s" % (sourceDir, file3)
        outputdir = self._fileman.getAutocropDir(project, container)
        open("%s/%s" % (outputdir, file1),'w').write(open(source1).read())
        open("%s/%s" % (outputdir, file2),'w').write(open(source2).read())
        open("%s/%s" % (outputdir, file3),'w').write(open(source3).read())
        self._pstore.markAutocropped(project, container, file1, file2, file3)

    def _vmAutocrop(self, project, container, file1, file2, file3):
#        return self._vmAutocropShort(project,container, file1, file2, file3)
        self._logger.info("Autocrop %s %s %s %s %s" % (project, container, file1, file2, file3))
        sourceDir = os.path.abspath(self._fileman.getPrecropDir(project, container))
        source1 = "%s/%s" % (sourceDir, file1)
        source2 = "%s/%s" % (sourceDir, file2)
        source3 = "%s/%s" % (sourceDir, file3)
        outputdir = self._fileman.getAutocropDir(project, container)
        jobargs = ('../autocrop2.py', '-s', '--filenames',
            '%s,%s,%s' % (source1, source2, source3),
            '--mode', self._config.film,
            '--output-dir', outputdir)
        self._logger.debug("Calling %s" % ' '.join(jobargs))
        try:
            #subprocess.check_call(jobargs)
            self._logger.debug(subprocess.check_output(jobargs, stderr=subprocess.STDOUT))
            self._logger.info("Done %s %s %s" % (project, container, outputdir))
            self._pstore.markAutocropped(project, container, file1, file2, file3)
            #self._logger.debug("_wmAutocrop Done")

        except subprocess.CalledProcessError as ee:
            self._logger.error("autocrop2 failed rc %d $s" % (ee.returncode, ee.output))
            self._pstore.abortAutocrop(project, container, file1, file2, file3)

    def _vmPrecropShort(self, project, container, filename):
        self._logger.info("Precrop short %s %s %s" % (project, container, filename))
        sourcefile = os.path.abspath(self._fileman.getRawFileLocation(project, container, filename))
        targetfile = os.path.abspath(self._fileman.getPrecropDir(project, container) + "/%s" % filename)
        open(targetfile, 'w').write(open(sourcefile).read())
        self._pstore.markPrecropped(project, container, filename)


    def _vmPrecrop(self, project, container, filename):
        self._logger.info("Precrop %s %s %s" % (project, container, filename))
        sourcefile = os.path.abspath(self._fileman.getRawFileLocation(project, container, filename))
        targetfile = os.path.abspath(self._fileman.getPrecropDir(project, container) + "/%s" % filename)
        jobargs = ('convert', sourcefile,
            #'-strip', '-flop', '-flip',
            '-strip', 
            '-crop', {'8mm': JobManager.Precrop8mmGeometry,
                'super8': JobManager.PrecropS8Geometry}[self._config.film],
            targetfile)
        self._logger.debug("Calling %s" % ' '.join(jobargs))
        if True == JobManager.DisablePrecrop:
            self._logger.debug("Precrop disabled")
        else:
            try:
                #retcode = subprocess.call(jobargs)
                self._logger.info( subprocess.check_output(jobargs, stderr=subprocess.STDOUT))
                self._logger.info("Done %s %s %s" % (project, container, filename))
                self._pstore.markPrecropped(project, container, filename)

            except subprocess.CalledProcessError as ee:
                self._logger.error("precrop failed rc %d $s" % (ee.returncode, ee.output))

        self._logger.debug("_vmPrecrop Done")

    def _vmGenTitle(self, project, root):
        jobargs = ('../gentitle.sh', '-p', project, '-r', root)
#        retcode = subprocess.call(jobargs)
        try:
            self._logger.info( subprocess.check_output(jobargs, stderr=subprocess.STDOUT))

        except subprocess.CalledProcessError as ee:
            self._logger.error("gentitle failed rc %d $s" % (ee.returncode, ee.output))

    def _vmGenContent(self, project, root, container):
        self._pstore.markChunkProcessing(project, container)
        jobargs = ('../gencontent.sh', '-p', project, '-r', root)

        try:
            self._logger.info(jobargs)
            output = subprocess.check_output(jobargs)
            #subprocess.check_call(jobargs, stderr=subprocess.STDOUT)
            #self._logger.debug(output)
            self._pstore.markChunkProcessed(project, container)

        except subprocess.CalledProcessError as ee:
            self._logger.error("gencontent failed rc %d $s" % (ee.returncode, ee.output))
        #retcode = subprocess.call(jobargs)
        #if 0 == retcode:
        #return retcode

    def _workerCleanup(self):
#        self._logger.debug("Workers before cleanup: %d" % len(self._workers))
        self._workers = [xx for xx in self._workers if xx.is_alive()]
#        self._logger.debug("Workers after cleanup: %d" % len(self._workers))

    def quiescent(self):
        if len(self._workers):
            return False
        return True

    def genTitle(self):
        self._generateTitles = True

    def uploadsDone(self, project):
        self._pstore.setTask(project, ['pc','ac','tf','gc'])
