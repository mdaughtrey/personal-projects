#!/usr/bin/python

import sqlite3

conn = sqlite3.connect('test.db')

cur = conn.cursor()

cur.execute('CREATE TABLE test (rawfile text, ignore boolean)')
cur.execute("INSERT INTO test values ('testfile', 'FALSE')")

conn.commit()
conn.close()
