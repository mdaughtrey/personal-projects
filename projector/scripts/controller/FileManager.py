import os
import pdb

class FileManager():
    def __init__(self, logger, fileroot = './'):
        self._logger = logger
        self._fileRoot = fileroot
        self._logger.debug("FileManager init root %s" % self._fileRoot)


    def _getdir(self, project, container, dirname):
        targetDir = "%s/%s/%s/%s" % (self._fileRoot, project, container, dirname)
        if False == os.path.isdir(targetDir):
            self._logger.debug("Creating %s" % targetDir)
            os.makedirs(targetDir)
        return targetDir

    def newFile(self, fileData, project, container, filename):
        targetDir = self._getdir(project, container, 'rawfile')
        targetFile = "%s/%s" % (targetDir, filename)
        self._logger.debug("Saving to %s" % targetFile)
        try:
            open(targetFile, 'w').write(fileData)
        except ee:
            logger.error("Write to %s failed, %s" % (targetFile, ee.message))
        
    def getRawFileLocation(self, project, container, filename):
        return "%s/%s" % (self._getdir(project, container, 'rawfile'), filename)

    def getPrecropDir(self, project, container):
        return self._getdir(project, container, 'precrop')

    def getAutocropDir(self, project, container):
        return self._getdir(project, container, 'autocrop')

    def getTonefuseDir(self, project, container):
        return self._getdir(project, container, 'fused')

    def newTitleFile(self, fileStream, project, titlepage):
        targetDir = "%s/%s/" % (self._fileRoot, project)
        if False == os.path.isdir(targetDir):
            return ['BADPROJECT']

        target = "%s/title%s.txt" % (targetDir, titlepage)
        self._logger.debug("Saving to %s" % target)
        open(target, 'w').write(fileStream)
        return ['OK']

    def deleteProject(self, projectname):
        return 'TODO'
