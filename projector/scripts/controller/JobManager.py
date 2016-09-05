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
    WorkerManagerInline = True
    def __init__(self, logger, pstore, fileman):
        self._logger = logger
        self._pstore = pstore
        self._fileman = fileman
        self._workers = []
        self._logger.debug("JobManager init")
        self._projectname = 'uploadtest'
        self._wmrunning = True
        if False == JobManager.WorkerManagerInline:
            self._thread = threading.Thread(target = worker, args = (self,))
            self._thread.start()
        else:
            self._workerManager(self._projectname)

    def shutdown(self):
        self._wmrunning = False
        #while self._thread.is_alive():
        #    self._logger.debug("Waiting on worker thread shutdown")
        #    if False == JobManager.WorkerManagerInline:
        #        self._thread.join()
        #    else:
        #        time.sleep(1)

    def newRawFile(self):
        pass


    def _workerManager(self, projectname):
        def schedulePrecrop(self, freeWorkers):
            scheduled = 0
            toBePrecropped = self._pstore.toBePrecropped(projectname, freeWorkers)
            if toBePrecropped:
                for (rowid, container, filename) in toBePrecropped:
                    self._logger.debug("Container %s filename %s" % (container, filename))
                    args = (projectname, container, filename)
                    if JobManager.WorkerManagerInline == True:
                        self._vmPrecrop(*args)
                    else:
                        self._workers.append(Process(target = self._vmPrecrop, args = args))
                        self._workers[-1].start()
                    scheduled += 1
            return scheduled

        def scheduleAutocrop(self, freeWorkers):
            scheduled = 0
            pdb.set_trace()
            toBeAutocropped = self._pstore.toBeAutocropped(projectname, freeWorkers)
            if toBeAutocropped:
                for (container, file1, file2, file3) in toBeAutocropped:
                    self._logger.debug("Container %s filename %s %s %s" % (container, file1, file2, file3))
                    args = (projectname, container, file1, file2, file3)
                    if jobManager.WorkerManagerInline == True:
                        self._vmAutocrop(*args)
                    else:
                        self._workers.append(Process(target = self._vmAutocrop, args = args))
                    self._workers[-1].start()
                    scheduled += 1
            return scheduled

        self._workerCleanup()
        freeWorkers = JobManager.JobLimit - len(self._workers)
        if 0 == freeWorkers:
            self._logger.debug("No free workers")
            time.sleep(1)
            return

        freeWorkers -= schedulePrecrop(self, freeWorkers)
        if freeWorkers: freeWorkers -= scheduleAutocrop(self, freeWorkers)

        if freeWorkers:
            time.sleep(5)

    def _vmAutocrop(self, project, container, file1, file2, file3):
        self._logger.debug("Autocrop %s %s %s %s %s" % (project, container, file1, file2, file3))
        sourceDir = os.path.abspath(self._fileman.getPrecropDir(self, project, container))
        source1 = "%s/%s" % (sourceDir, file1)
        source2 = "%s/%s "% (sourceDir, file2)
        source3 = "%s/%s "% (sourceDir, file3)
        targetfile = self._fileman.getAutocropDir(project, container) + "/%s" % file1
        jobargs = ('../autocrop.py', '--filenames',
            '%s,%s,%s' % (file1, file2, file3),
            '--mode', '8mm',
            '--outputfile', targetfile)
        retcode = subprocess.call(jobargs)
        #self._logger.debug("Done %s %s %s rc %d" % (project, container, filename, retcode))
        if 0 == retcode:
            self._pstore.markAutocropped(project, container, file1, file2, file3, targetfile)
        self._logger.debug("_wmAutocrop Done")

        return retcode

    def _vmPrecrop(self, project, container, filename):
        self._logger.debug("Precrop %s %s %s" % (project, container, filename))
        sourcefile = os.path.abspath(self._fileman.getRawFileLocation(project, container, filename))
        targetfile = os.path.abspath(self._fileman.getPrecropDir(project, container) + "/%s" % filename)
        jobargs = ('convert', sourcefile,
            '-crop', "%dx%d+%d+%d" % (1755, 1083, 2500, 1680),
            '-flop', '-flip', targetfile)
        retcode = subprocess.call(jobargs)
        self._logger.debug("Done %s %s %s rc %d" % (project, container, filename, retcode))
        pdb.set_trace()
        if 0 == retcode:
            self._pstore.markPrecropped(project, container, filename)
        self._logger.debug("_vmPrecrop Done")

        return retcode

    def _workerCleanup(self):
        self._logger.debug("Workers before cleanup: %d" % len(self._workers))
        self._workers = [xx for xx in self._workers if xx.is_alive()]
        self._logger.debug("Workers after cleanup: %d" % len(self._workers))
