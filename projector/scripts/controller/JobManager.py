import os
import pdb
from multiprocessing import Process
import threading
import subprocess
import time

def worker(object):
    object._logger.debug("Worker")
    while object._wmrunning:
        object._workerManager(object._projectname)
    object._logger.debug("workerManager ends")

class JobManager():
    JobLimit = 10
#    WorkerManagerControl = False # True # 'Off'
    DisablePrecrop = False
    DisableAutocrop = False

    def __init__(self, logger, pstore, fileman, mode, project, film, root):
        self._logger = logger
        self._pstore = pstore
        self._fileman = fileman
        self._workers = []
        self._logger.debug("JobManager init")
        self._projectname = project
        self._mode = mode
        self._film = film
        self._root = root

        if 'disable' == mode: # JobManager.WorkerManagerControl:
            return
        self._wmrunning = True
        if 'proc' == mode: # JobManager.WorkerManagerControl:
            self._thread = threading.Thread(target = worker, args = (self,))
            self._thread.start()
        elif 'inline' == mode:
            self._workerManager(self._projectname)

    def shutdown(self):
        self._wmrunning = False
        map(lambda pp: pp.terminate(), self._workers)

    def _workerManager(self, projectname):
        def schedulePrecrop(self, freeWorkers):
            scheduled = 0
            toBePrecropped = self._pstore.toBePrecropped(projectname, freeWorkers)
            if toBePrecropped:
                for (rowid, container, filename) in toBePrecropped:
                    self._logger.debug("Container %s filename %s" % (container, filename))
                    jobargs = (projectname, container, filename)
                    if 'inline' == self._mode: # JobManager.WorkerManagerControl == True:
                        self._vmPrecrop(*jobargs)
                    else:
                        self._workers.append(Process(target = self._vmPrecrop, args = jobargs))
                        self._workers[-1].start()
                    scheduled += 1
            return scheduled

        def scheduleAutocrop(self, freeWorkers):
            scheduled = 0
            todo = self._pstore.toBeAutocropped(projectname, freeWorkers * 3)
            while len(todo) >= 3:
                testargs = {todo[0][1]: 1}
                testargs[todo[1][1]] = 1
                testargs[todo[2][1]] = 1
                if len(testargs.keys()) > 1:
                    self._logger.error("Mismatched container values")
                    del todo[:3]

                jobargs = (projectname, todo[0][1], todo[0][2], todo[1][2], todo[2][2])

                if 'inline' == self._mode: # JobManager.WorkerManagerControl == True:
                    self._vmAutocrop(*jobargs)
                else:
                    self._workers.append(Process(target = self._vmAutocrop, args = jobargs))
                    self._workers[-1].start()
                scheduled += 1
                del todo[:3]
            return scheduled

        def scheduleTonefuse(self, freeWorkers):
            scheduled = 0
            todo = self._pstore.toBeFused(projectname, freeWorkers * 3)
            while len(todo) >= 3:
                testargs = {todo[0][1]: 1}
                testargs[todo[1][1]] = 1
                testargs[todo[2][1]] = 1
                if len(testargs.keys()) > 1:
                    self._logger.error("Mismatched container values")
                    del todo[:3]

                jobargs = (projectname, todo[0][1], todo[0][2], todo[1][2], todo[2][2])

                if 'inline' == self._mode: # JobManager.WorkerManagerControl == True:
                    self._vmTonefuse(*jobargs)
                else:
                    self._workers.append(Process(target = self._vmTonefuse, args = jobargs))
                    self._workers[-1].start()
                scheduled += 1
                del todo[:3]
            return scheduled

        def scheduleGenTitle(self, freeWorkers):
            if False == self._generateTitles: return 0
            if 'inline' == self._mode: 
                self._vmGenTitle(self._projectname, self._root)
                return 0
            else:
                self._workers.append(Process(target = self._vmGenTitle,
                    args = (self._projectname, self._root)))
                self._workers[-1].start()
            self._generateTitles = False
            return 1    

        def scheduleGenChunk(self, freeWorkers):
            chunks, processed = self._pstore.getVideoChunkStatus(self._projectname)
            todo = [ee.encode('ascii', 'ignore') for ee in chunks if ee not in processed]
            if 0 == len(todo):
                return 0
            if 'inline' == self._mode: 
                self._vmGenChunk(self._projectname, self._root, todo[0])
            else:
                self._workers.append(Process(target = self._vmGenChunk,
                    args = (self._projectname, self._root, todo[0])))
                self._workers[-1].start()
            return 1    

        self._workerCleanup()
        freeWorkers = JobManager.JobLimit - len(self._workers)
        if 0 == freeWorkers:
            self._logger.debug("No free workers")
            time.sleep(1)
            return

        for scheduler in [scheduleGenChunk]:
        #for scheduler in [schedulePrecrop, scheduleAutocrop, scheduleTonefuse,
        #    scheduleGenTitle, scheduleGenChunk]:
            if freeWorkers: freeWorkers -= scheduler(self, freeWorkers)

        if freeWorkers:
            time.sleep(5)

    def _vmTonefuse(self, project, container, file1, file2, file3):
        self._logger.debug("Tonefuse %s %s %s %s %s" % (project, container, file1, file2, file3))
        sourceDir = os.path.abspath(self._fileman.getAutocropDir(project, container))
        source1 = "%s/%s" % (sourceDir, file1)
        source2 = "%s/%s" % (sourceDir, file2)
        source3 = "%s/%s" % (sourceDir, file3)
        outputfile = self._fileman.getTonefuseDir(project, container) + '/%s' % file1
        jobargs = ('enfuse', '--output', outputfile, source1, source2, source3)
        self._logger.debug("Calling %s" % ' '.join(jobargs))
        try:
            subprocess.check_call(jobargs)
            self._logger.debug("Done %s %s %s" % (project, container, outputfile))
            self._pstore.markFused(project, container, file1, file2, file3)
            self._logger.debug("_wmTonefuse Done")

        except subprocess.CalledProcessError as ee:
            self._logger.error("Tonefuse failed rc %d $s" % (ee.returncode, ee.output))
            self._pstore.abortTonefuse(project, container, file1, file2, file3)

    def _vmAutocrop(self, project, container, file1, file2, file3):
        self._logger.debug("Autocrop %s %s %s %s %s" % (project, container, file1, file2, file3))
        sourceDir = os.path.abspath(self._fileman.getPrecropDir(project, container))
        source1 = "%s/%s" % (sourceDir, file1)
        source2 = "%s/%s" % (sourceDir, file2)
        source3 = "%s/%s" % (sourceDir, file3)
        outputdir = self._fileman.getAutocropDir(project, container)
        jobargs = ('../autocrop.py', '--filenames',
            '%s,%s,%s' % (source1, source2, source3),
            '--mode', self._film,
            '--output-dir', outputdir)
        self._logger.debug("Calling %s" % ' '.join(jobargs))
        try:
            subprocess.check_call(jobargs)
            self._logger.debug("Done %s %s %s" % (project, container, outputdir))
            self._pstore.markAutocropped(project, container, file1, file2, file3)
            self._logger.debug("_wmAutocrop Done")

        except subprocess.CalledProcessError as ee:
            self._logger.error("autocrop failed rc %d $s" % (ee.returncode, ee.output))
            self._pstore.abortAutocrop(project, container, file1, file2, file3)

    def _vmPrecrop(self, project, container, filename):
        self._logger.debug("Precrop %s %s %s" % (project, container, filename))
        sourcefile = os.path.abspath(self._fileman.getRawFileLocation(project, container, filename))
        targetfile = os.path.abspath(self._fileman.getPrecropDir(project, container) + "/%s" % filename)
        jobargs = ('convert', sourcefile,
            '-strip', '-flop', '-flip',
            '-crop', "%dx%d+%d+%d" % (2700, 1900, 1280, 530),
            targetfile)
        self._logger.debug("Calling %s" % ' '.join(jobargs))
        retcode = 0
        if True == JobManager.DisablePrecrop:
            self._logger.debug("Precrop disabled")
        else:
            retcode = subprocess.call(jobargs)
        self._logger.debug("Done %s %s %s rc %d" % (project, container, filename, retcode))
        if 0 == retcode:
            self._pstore.markPrecropped(project, container, filename)
        self._logger.debug("_vmPrecrop Done")
        return retcode

    def _vmGenTitle(self, project, root):
        jobargs = ('../gentitle.sh', '-p', project, '-r', root)
        retcode = subprocess.call(jobargs)
        return retcode

    def _vmGenChunk(self, project, root, container):
        self._pstore.markChunkProcessing(project, container)
        jobargs = ('../gencontent.sh', '-p', project, '-r', root)
        retcode = subprocess.call(jobargs)
        if 0 == retcode:
            self._pstore.markChunkProcessed(project, container)
        return retcode

    def _workerCleanup(self):
        self._logger.debug("Workers before cleanup: %d" % len(self._workers))
        self._workers = [xx for xx in self._workers if xx.is_alive()]
        self._logger.debug("Workers after cleanup: %d" % len(self._workers))

    def quiescent(self):
        if len(self._workers):
            return False
        return True

    def genTitle(self):
        self._generateTitles = True


