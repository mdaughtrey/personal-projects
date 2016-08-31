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
        conn = sqlite3.connect('%s' % projectname)
        cur = conn.cursor()
        cur.execute('''CREATE TABLE picdata (
            rawfile text,
            container text,
            precrop text,
            autocrop text,
            fused text
            )''')
        cur.execute('''CREATE UNIQUE INDEX picdata_idx ON picdata(rawfile)''')

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
        dblocation = "%s/%s" % (self._dbroot, project)
        if True == os.path.isfile(dblocation):
            if True == self._overwrite:
                os.remove(self._dbroot)
        else:
            self._initDb(project)

        return sqlite3.connect('%s' % project)

    def newRawFile(self, project,container, filename):
        insert = "(rawfile,container) values('%s','%s')" % (filename, container)
        statement = "INSERT OR REPLACE INTO picdata %s" % insert
        conn = self._openDb(project)
        cursor = conn.cursor()
        cursor.execute(statement)
        conn.commit()
        conn.close()

    def unprocessedRaw(self, projectname, limit=10):
        conn = self._openDb(projectname)
        cursor = conn.cursor()
        files = cursor.execute(
                "SELECT container,rawfile FROM picdata WHERE precrop is null limit %d" % limit)
        result = cursor.fetchall()
        conn.close()
        return result

    def markPrecropped(self, project, container, filename):
        conn = self._openDb(project);
        cursor = conn.cursor()
        cursor.execute("UPDATE picdata SET precrop='%s' WHERE container='%s' and rawfile='%s'"
                % (filename, container, filename))
        conn.commit()
        conn.close()





