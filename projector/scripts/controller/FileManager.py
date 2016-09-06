import os
import pdb

class FileManager():
    def __init__(self, logger, fileroot = './'):
        self._logger = logger
        self._fileRoot = fileroot
        self._logger.debug("FileManager init root %s" % self._fileRoot)

    def newFileStream(self, fileStream, project, container, filename):
        targetDir = "%s/%s/%s/" % (self._fileRoot, project, container)
        if False == os.path.isdir(targetDir):
            self._logger.debug("Creating %s" % targetDir)
            os.makedirs(targetDir)
            for subdir in ['precrop','autocrop','fused']:
                os.makedirs('%s/%s' % (targetDir, subdir))
        target = "%s/%s" % (targetDir, filename)
        self._logger.debug("Saving to %s" % target)
        fileStream.save(target)

    def getRawFileLocation(self, project, container, filename):
        return "%s/%s/%s/%s" % (self._fileRoot, project, container, filename)

    def getPrecropDir(self, project, container):
        return "%s/%s/%s/precrop" % (self._fileRoot, project, container)

    def getAutocropDir(self, project, container):
        return "%s/%s/%s/autocrop" % (self._fileRoot, project, container)

    def getTonefuseDir(self, project, container):
        return "%s/%s/%s/fused" % (self._fileRoot, project, container)
