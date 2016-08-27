import os
import pdb

class FileManager():
    def __init__(self, logger, fileroot = './'):
        self._logger = logger
        self._fileRoot = fileroot
        self._logger.debug("FileManager init root %s" % self._fileRoot)

    def newFileStream(self, project, container, fileStream):
        targetDir = "%s/%s/%s/" % (self._fileRoot, project, container)
        if False == os.path.isdir(targetDir):
            self._logger.debug("Creating %s" % targetDir)
            os.makedirs(targetDir)
        target = "%s/%s" % (targetDir, fileStream.filename)
        self._logger.debug("Saving to %s" % target)
        fileStream.save(target)

    def getRawFileLocation(self, project, container, filename):
        return "%s/%s/%s/%s" % (self._fileRoot, project, container, filename)

    def getPrecropDir(self, project):
        pcdir = "%s/%s/precropped" % (self._fileRoot, project)
        if False == os.path.isdir(pcdir):
            os.makedirs(pcdir)
        return pcdir
