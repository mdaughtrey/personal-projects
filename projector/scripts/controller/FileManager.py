import os
import pdb
from threading import Lock
import subprocess

class FileManager():
    mtxGetDir = Lock()
    def __init__(self, logger, fileroot = './'):
        self._logger = logger
        self._fileRoot = fileroot
        self._logger.debug("FileManager init root %s" % self._fileRoot)

    def _getdir(self, project, container, dirname):
        targetDir = "%s/%s/%s/%s" % (self._fileRoot, project, container, dirname)
        FileManager.mtxGetDir.acquire()
        if False == os.path.isdir(targetDir):
            self._logger.debug("Creating %s" % targetDir)
            try:
                os.makedirs(targetDir)
            except:
                pass
        FileManager.mtxGetDir.release()
        return targetDir

    def newFile(self, fileData, project, container, filename):
        targetDir = self._getdir(project, container, 'rawfile')
        targetFile = "%s/%s" % (targetDir, filename)
        self._logger.debug("Saving %u to %s" % (len(fileData), targetFile))
        try:
            open(targetFile, 'w').write(fileData)
        except ee:
            logger.error("Write to %s failed, %s" % (targetFile, ee.message))
        
    def getRawFileLocation(self, project, container, filename):
        return "%s/%s" % (self._getdir(project, container, 'rawfile'), filename)

    def getRawFileDir(self, project, container):
        return self._getdir(project, container, 'rawfile')

    def getPrecropDir(self, project, container):
        return self._getdir(project, container, 'precrop')

    def getAutocropDir(self, project, container):
        return self._getdir(project, container, 'autocrop')

    def getTonefuseDir(self, project, container):
        return self._getdir(project, container, 'fused')

    def getAdjFile(self, project):
        return "%s/%s/frameadj.txt" % (self._fileRoot, project)

    def newTitleFile(self, fileStream, project, titlepage):
        targetDir = "%s/%s/" % (self._fileRoot, project)
        if False == os.path.isdir(targetDir):
            return ['BADPROJECT']

        target = "%s/title%s.txt" % (targetDir, titlepage)
        self._logger.debug("Saving to %s" % target)
        open(target, 'w').write(fileStream)
        return ['OK']

    def newReferenceFile(self, fileStream, project, urlArgs):
        targetDir = "%s/%s/" % (self._fileRoot, project)
        targetFile = "%s/reference%c.raw.bz2" % (targetDir, urlArgs['refindex'])
#	    targetFile = "%s/reference%c.raw" % (targetDir, urlArgs['refindex'])
        try:
            open(targetFile, 'w').write(fileStream)
#	        jobargs = ('bzip2', '-d', sourceFile, "> %s/%s" % (targetDir, targetFile))
#            output = subprocess.check_output(jobargs, stderr=subprocess.STDOUT)
        except ee:
            logger.error("Write to %s failed, %s" % (targetFile, ee.message))
	return['OK']

#    def rawToBeProcessed(self, project):
#        targetDir = "%s/%s/" % (self._fileRoot, project)
#        pass

    def deleteProject(self, projectname):
        return 'TODO'
