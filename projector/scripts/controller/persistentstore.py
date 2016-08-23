import sqlite3
import os

class PersistentStore():
    def __init__(self, logger, dblocation = './', overwrite = False):
        self._dblocation = dblocation
#        self._conn = None
        self._overwrite = overwrite
        self._logger = logger
        self._logger.debug("PersistentStore init")

    def _initDb(self, projectname):
        conn = sqlite3.connect('%s/%s' % (self._dblocation, projectname))
        cur = conn.cursor()
        cur.execute('''CREATE TABLE picdata (
            rawfile text,
            container text)''')

        conn.commit()
        conn.close()

    def _openDb(self, project):
        if False == os.path.isfile(self._dblocation):
            self._initDb(project)
        else:
            if True == self._overwrite:
                os.remove(self._dblocation)
                self._initDb(project)

        return sqlite3.connect('%s/%s' % (self._dblocation, project))

    def newRawFile(self, project, filename, container):
        conn = self._openDb(project).cursor().execute('''INSERT INTO picdata
                rawfile,container rawfile='%s',container='%s')''' % (filename, container))
        conn.commit()
        conn.close()

