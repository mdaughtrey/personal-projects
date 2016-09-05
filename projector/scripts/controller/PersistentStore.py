import sqlite3
import os
import pdb

class PersistentStore():
    def __init__(self, logger, dblocation = './', overwrite = False):
        self._dbroot = dblocation
#        self._conn = None
        self._overwrite = overwrite
        self._logger = logger
        self._logger.debug("PersistentStore init")

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

    def newRawFile(self, project,container, filename):
        insert = "(processing, rawfile,container) values(0, '%s','%s')" % (filename, container)
        statement = "INSERT OR REPLACE INTO picdata %s" % insert
        conn = self._openDb(project)
        cursor = conn.cursor()
        cursor.execute(statement)
        conn.commit()
        conn.close()

    def _getPendingWork(self, projectname, statement):
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

    def toBePrecropped(self, projectname, limit = 10):
        statement = '''CREATE TEMPORARY TABLE ttable AS SELECT rowid,container,rawfile FROM picdata
                 WHERE precrop IS NULL AND processing != 1 ORDER BY rawfile LIMIT %s;''' % limit
        self._logger.debug(statement)
        return self._getPendingWork(projectname, statement)

    def markPrecropped(self, project, container, filename):
        conn = self._openDb(project);
        cursor = conn.cursor()
        cursor.execute("UPDATE picdata SET precrop='%s',processing=0 WHERE container='%s' and rawfile='%s'"
                % (filename, container, filename))
        conn.commit()
        conn.close()

    def toBeAutocropped(self, projectname, limit = 10):
        statement = '''CREATE TEMPORARY TABLE ttable AS SELECT rowid,container,precrop FROM picdata
                 WHERE precrop IS NOT NULL AND autocrop IS NULL AND processing != 1
                 ORDER BY container,precrop LIMIT %s;''' % limit
        self._logger.debug(statement)
        return self._getPendingWork(projectname, statement)


    def markAutocropped(self, project, container, filename):
        conn = self._openDb(project);
        cursor = conn.cursor()
        cursor.execute("UPDATE picdata SET autocrop='%s',processing=0 WHERE container='%s' and rawfile='%s'"
                % (filename, container, filename))
        conn.commit()
        conn.close()

    def abortAutocrop(self, project, container, file1, file2, file3):
        self._logger.warning("TODO abortAutocrop")
        pass


