import sqlite3
import os
import pdb
import threading

class PersistentStore():
    def __init__(self, logger, dblocation = './', overwrite = False):
        self._dbroot = dblocation
#        self._conn = None
        self._overwrite = overwrite
        self._logger = logger
        self._logger.debug("PersistentStore init")
        self._dbLock = threading.Lock()

    def _initDb(self, projectname):
        conn = sqlite3.connect(projectname)
        cur = conn.cursor()
        cur.execute('''CREATE TABLE picdata (
                    processing integer,
                    rawfile TEXT,
                    container TEXT,
                    precrop TEXT,
                    autocrop TEXT,
                    fused TEXT
                    )''')
        cur.execute('''CREATE UNIQUE INDEX picdata_rawfile_container ON picdata(rawfile, container)''')

        conn.commit()
        conn.close()

    def _openDb(self, project):
        '''
        if False == os.path.isdir(self._dbroot):
            os.mkdir(self._dbroot)
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

        self._logger.debug('Connecting to %s' % dblocation)
        return sqlite3.connect('%s' % dblocation)

    def rawFileExists(self, project, container, filename):
        with self._dbLock:
            conn = self._openDb(project)
            cursor = conn.cursor()
            cursor.execute("SELECT COUNT(*) FROM picdata WHERE container='%s' AND rawfile='%s'" % (container, filename))
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
                    'UPDATE picdata SET processing=1 WHERE rowid IN (SELECT rowid from ttable)',
                    'END TRANSACTION',
                    'SELECT * FROM ttable;'
                    ]

            for statement in statements:
                try:
                    cursor.execute(statement)

                except sqlite3.OperationalError as ee:
                    self._logger.error(ee.message)

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

    def toBePrecropped(self, projectname, limit = 10):
        statement = '''CREATE TEMPORARY TABLE ttable AS SELECT rowid,container,rawfile FROM picdata
                 WHERE precrop IS NULL AND processing != 1 ORDER BY rawfile LIMIT %s;''' % limit
        self._logger.debug(statement)
        return self._getPendingWork(projectname, statement)

    def markPrecropped(self, project, container, filename):
        self.simpleUpdate(project, "UPDATE picdata SET precrop='%s',processing=0 WHERE container='%s' and rawfile='%s'"
                % (filename, container, filename))

    def toBeAutocropped(self, projectname, limit = 10):
        statement = '''CREATE TEMPORARY TABLE ttable AS SELECT rowid,container,precrop FROM picdata
                 WHERE precrop IS NOT NULL AND autocrop IS NULL AND processing != 1
                 ORDER BY container,precrop LIMIT %s;''' % limit
        self._logger.debug(statement)
        return self._getPendingWork(projectname, statement)

    def markAutocropped(self, project, container, file1, file2, file3):
        for ff in [file1, file2, file3]:
            self.simpleUpdate(project, "UPDATE picdata set autocrop='%s' WHERE container='%s' and rawfile='%s'"
            % (ff, container, ff))

        self.simpleUpdate(project, "UPDATE picdata SET processing=0 WHERE container='%s' and rawfile in ('%s','%s','%s')"
                % (container, file1, file2, file3))

    def abortAutocrop(self, project, container, file1, file2, file3):
        self._logger.warning("TODO abortAutocrop")

    def toBeFused(self, project, limit = 10):
        statement = '''CREATE TEMPORARY TABLE ttable AS SELECT rowid,container,autocrop FROM picdata
                 WHERE autocrop IS NOT NULL AND fused IS NULL AND processing != 1
                 ORDER BY container,autocrop LIMIT %s;''' % limit
        self._logger.debug(statement)
        return self._getPendingWork(project, statement)

    def markFused(self, project, container, file1, file2, file3):
        for ff in [file1, file2, file3]:
            self.simpleUpdate(project, "UPDATE picdata set fused='%s' WHERE container='%s' and autocrop='%s'"
            % (file1, container, ff))

        self.simpleUpdate(project, "UPDATE picdata SET processing=0 WHERE container='%s' and autocrop in ('%s','%s','%s')"
                % (container, file1, file2, file3))

    def abortTonefuse(self, project, container, file1, file2, file3):
        self._logger.warning("TODO abortTonefuse")

