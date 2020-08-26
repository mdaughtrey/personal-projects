import sqlite3
import os
import pdb
import threading

class ControllerStore():
    def __init__(self, logger, dblocation = './', overwrite = False):
        self._dbroot = dblocation
#        self._conn = None
        self._overwrite = overwrite
        self._logger = logger
        self._logger.debug("ControllerStore init")
        self._dbLock = threading.Lock()

    def _initDb(self, projectname):
        conn = sqlite3.connect(projectname)
        cur = conn.cursor()
        cur.execute('''CREATE TABLE projects (
                    projectname TEXT,
                    projectroot TEXT
                    )''')
        cur.execute('''CREATE UNIQUE INDEX projectname ON projects(projectname)''')

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

    def projectInfo(self, projectname = None):
        with self._dbLock:
            conn = self._openDb(project)
            cursor = conn.cursor()
            if projectname is not None:
                cursor.execute("SELECT projectname, projectroot FROM projects WHERE projectname='%s'" % projectname)
            else:
                cursor.execute("SELECT projectname, projectroot from projects")
            result = cursor.fetchall()
            conn.commit()
            conn.close()
            return True

#    def setProject(self, name):
#        root = 'TODO'
#        insert = "(projectname, projectroot) values('%s','%s')" % (name, root)
#        statement = "INSERT OR REPLACE INTO projects %s" % insert
#        with self._dbLock:
#            conn = self._openDb(name)
#            cursor = conn.cursor()
#            cursor.execute(statement)
#            conn.commit()
#            conn.close()
#
    def deleteProject(self, projectname):
        with self._dbLock:
            conn = self._openDb(project)
            cursor = conn.cursor()
            cursor.execute("DELETE FROM projects WHERE projectname='%s' and projectroot='%s'" % (projectname, projectname))
            conn.commit()
            conn.close()

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
        statement = '''CREATE TEMPORARY TABLE ttable AS SELECT rowid,container,converted FROM picdata
                 WHERE precrop IS NULL AND processing != 1 ORDER BY converted LIMIT %s;''' % limit
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

#    def markFused(self, project, container, file1, file2, file3):
#        for (file, tag) in zip([file1, file2, file3],["a", "b", "c"]):
#            self.simpleUpdate(project, "UPDATE picdata set fused='%s',fusedtag='%s' WHERE container='%s' and autocrop='%s'"
#            % (file, tag, container, ff))
#
#        self.simpleUpdate(project, "UPDATE picdata SET processing=0 WHERE container='%s' and autocrop in ('%s','%s','%s')"
#                % (container, file1, file2, file3))

    def abortTonefuse(self, project, container, file1, file2, file3):
        self._logger.warning("TODO abortTonefuse")

