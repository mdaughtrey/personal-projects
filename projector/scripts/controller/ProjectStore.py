import sqlite3
import os
import pdb
import threading

class ProjectStore():
    FilesPerContainer = 999
    def __init__(self, logger, dblocation = './', overwrite = False):
        self._dbroot = dblocation
#        self._conn = None
        self._overwrite = overwrite
        self._logger = logger
        self._logger.debug("ProjectStore init")
        self._dbLock = threading.Lock()
#        if False == os.path.isdir(dblocation):
#            os.makedirs(dblocation)

    def _initDb(self, projectname):
        conn = sqlite3.connect(projectname)
        cur = conn.cursor()
        cur.execute('''CREATE TABLE picdata (
                    processing integer,
                    rawfile TEXT,
                    container TEXT,
                    precrop TEXT,
                    autocrop TEXT,
                    fused TEXT,
                    titleframe TEXT
                    )''')
        cur.execute('''CREATE TABLE videodata (
            container TEXT,
            processing integer
            )''')
        cur.execute('''CREATE TABLE taskcontrol (
            task TEXT
            )''')
        cur.execute('''CREATE UNIQUE INDEX picdata_rawfile_container ON picdata(rawfile, container)''')

        conn.commit()
        conn.close()

    def _openDb(self, project):
        '''
        if False == os.path.isdir(self._dbroot):
            os.makedirs(self._dbroot)
            self._initDb(project)
        else:
            '''
        project += 'db'
        dblocation = "%s/%s" % (os.path.abspath(self._dbroot), project)
        if True == os.path.isfile(dblocation):
            if True == self._overwrite:
                os.remove(self._dbroot)
        else:
            self._initDb(dblocation)

        #self._logger.debug('Connecting to %s' % dblocation)
        return sqlite3.connect('%s' % dblocation)

    def rawFileExists(self, project, container, file):
        with self._dbLock:
            conn = self._openDb(project)
            cursor = conn.cursor()
            cursor.execute("SELECT COUNT(*) FROM picdata WHERE container='%s' AND rawfile='%s'" % (container, file))
            result = cursor.fetchall()
            conn.commit()
            conn.close()
            if 1 == result[0][0]:
                return True

            return False

    def newRawFile(self, project,container, filename):
        insert = "(processing, rawfile,container) values(0, '%s','%s')" % (filename, container)
        statement = "INSERT OR REPLACE INTO picdata %s" % insert
        with self._dbLock:
            conn = self._openDb(project)
            cursor = conn.cursor()
            cursor.execute(statement)
            conn.commit()
            conn.close()

    def _getPendingWork(self, projectname, statement):
        with self._dbLock:
            conn = self._openDb(projectname)
            cursor = conn.cursor()
            statements = ['PRAGMA temp_store=MEMORY;',
                    'BEGIN TRANSACTION;',
                    statement,
                    'UPDATE picdata SET processing=1 WHERE rowid IN (SELECT rowid from ttable);',
                    'END TRANSACTION;',
                    'SELECT * FROM ttable;'
                    ]

            for statement in statements:
                try:
                    cursor.execute(statement)

                except sqlite3.OperationalError as ee:
                    pass
#                    self._logger.error(ee.message)

            result = cursor.fetchall()
            conn.close()
        return result

    def simpleUpdate(self, project, statement):
        with self._dbLock:
            conn = self._openDb(project)
            cursor = conn.cursor()
            cursor.execute(statement)
            conn.commit()
            conn.close()

    def toBePrecropped(self, projectname, limit):
        statement = '''CREATE TEMPORARY TABLE ttable AS SELECT rowid,container,rawfile FROM picdata
                 WHERE precrop IS NULL AND processing != 1 ORDER BY container,rawfile LIMIT %s;''' % limit
        self._logger.debug(statement)
        return self._getPendingWork(projectname, statement)

    def markPrecropped(self, project, container, filename):
        self.simpleUpdate(project, "UPDATE picdata SET precrop='%s',processing=0 WHERE container='%s' and rawfile='%s'"
                % (filename, container, filename))

    def toBeAutocropped(self, projectname, limit):
        statement = '''CREATE TEMPORARY TABLE ttable AS SELECT rowid,container,precrop FROM picdata
                 WHERE precrop IS NOT NULL AND autocrop IS NULL AND processing != 1
                 ORDER BY container,precrop LIMIT %s;''' % limit
        #self._logger.debug(statement)
        return self._getPendingWork(projectname, statement)

    def markAutocropped(self, project, container, file1, file2, file3):
        for ff in [file1, file2, file3]:
            self.simpleUpdate(project, "UPDATE picdata set autocrop='%s' WHERE container='%s' and rawfile='%s'"
            % (ff, container, ff))

        self.simpleUpdate(project, "UPDATE picdata SET processing=0 WHERE container='%s' and rawfile in ('%s','%s','%s')"
                % (container, file1, file2, file3))

    def abortAutocrop(self, project, container, file1, file2, file3):
        self._logger.warning("TODO abortAutocrop")

    def toBeFused(self, project, limit):
        statement = '''CREATE TEMPORARY TABLE ttable AS SELECT rowid,container,autocrop FROM picdata
                 WHERE autocrop IS NOT NULL AND fused IS NULL AND processing != 1
                 ORDER BY container,autocrop LIMIT %s;''' % limit
        #self._logger.debug(statement)
        return self._getPendingWork(project, statement)

    def markFused(self, project, container, file1, file2, file3):
        for ff in [file1, file2, file3]:
            self.simpleUpdate(project, "UPDATE picdata set fused='%s' WHERE container='%s' and autocrop='%s'"
            % (file1, container, ff))

        self.simpleUpdate(project, "UPDATE picdata SET processing=0 WHERE container='%s' and autocrop in ('%s','%s','%s')"
                % (container, file1, file2, file3))

    def abortTonefuse(self, project, container, file1, file2, file3):
        self._logger.warning("TODO abortTonefuse")

    def taskList(self, project):
        with self._dbLock:
            conn = self._openDb(project)
            cursor = conn.cursor()
            statement = "SELECT task from taskcontrol"
            cursor.execute(statement)
            tlist = cursor.fetchall()
            conn.close()
        return [ee[0] for ee in tlist]

    def getNextImageLocation(self, project):
        with self._dbLock:
            # Find the most recent container
            conn = self._openDb(project)
            cursor = conn.cursor()
            cursor.execute("SELECT DISTINCT(container) FROM picdata")
            containers = cursor.fetchall()
            if 0 == len(containers): # New project
                contDir = "%s/%s/100" % (self._dbroot, project)
                if False == os.path.isdir(contDir):
                    os.makedirs(contDir)
                return "100", "000000.JPG"

            containers.sort()
            cursor = conn.cursor()
            cursor.execute("SELECT rawfile FROM picdata WHERE container='%s'" % containers[-1])
            files = cursor.fetchall()
            conn.close()

        self._logger.debug("len(files) %u" % len(files))

        if len(files) >= ProjectStore.FilesPerContainer:
            newContainer = int(containers[-1][0]) + 1
            newFile = 0
        else:
            files.sort()
            newFile = int(files[-1][0].split('.')[0]) + 1
            newContainer = int(containers[-1][0])

        newDir = "%s/%s/%u" % (self._dbroot, project, newContainer)
        if False == os.path.isdir(newDir):
            os.makedirs(newDir)
        self._logger.debug("newContainer %u newFile %u" % (newContainer, newFile))
        return "%03u" % int(newContainer), "%06u.JPG" % newFile

    def getVideoChunkStatus(self, project):
        with self._dbLock:
            conn = self._openDb(project)
            cursor = conn.cursor()
            statement = "SELECT container,COUNT(container) FROM picdata GROUP BY container"
            cursor.execute(statement)
            containers = cursor.fetchall()
            cursor.execute("SELECT container FROM videodata")
            processed = cursor.fetchall()
            conn.close()
        containers = [ee[0] for ee in containers] # if ee[1] >= ProjectStore.FilesPerContainer]
        return containers, [ee[0] for ee in processed]

    def markChunkProcessing(self, project, container):
        statement = ''.join([
            "INSERT OR REPLACE INTO videodata (processing, container) "
            "VALUES(1, '%s')" % container])

        self.simpleUpdate(project, statement)

    def markChunkProcessed(self, project, container):
        statement = "UPDATE videodata SET processing=0 where CONTAINER='%s'" % container
        self.simpleUpdate(project, statement)
