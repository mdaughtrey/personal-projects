#!/usr/bin/python

import cgi, os, SocketServer, sys, time, urllib
from SimpleHTTPServer import SimpleHTTPRequestHandler
from StringIO import StringIO

class DirectoryHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        print " %s" % self.path
        self.wfile.write("")
#    def list_directory(self, path):
#        try:
#            list = os.listdir(path)
#        except os.error:
#            self.send_error(404, "No permission to list directory")
#            return None
#
#        list.sort(key=lambda a: a.lower())
#        f = StringIO()
#        displaypath = cgi.escape(urllib.unquote(self.path))
#        f.write('<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">')
#        f.write("<html>\n<title>Directory listing for %s</title>\n" % displaypath)
#        f.write("<body>\n<h2>Directory listing for %s</h2>\n" % displaypath)
#        f.write("<hr>\n<ul>\n")
#        for name in list:
#            fullname = os.path.join(path, name)
#            displayname = linkname = name
#            date_modified = time.ctime(os.path.getmtime(fullname))
#            # Append / for directories or @ for symbolic links
#            if os.path.isdir(fullname):
#                displayname = name + "/"
#                linkname = name + "/"
#            if os.path.islink(fullname):
#                displayname = name + "@"
#                # Note: a link to a directory displays with @ and links with /
#            f.write('<li><a href="%s">%s - %s</a>\n'
#                % (urllib.quote(linkname), cgi.escape(displayname), date_modified))
#        f.write("</ul>\n<hr>\n</body>\n</html>\n")
#        length = f.tell()
#        f.seek(0)
#        self.send_response(200)
#        encoding = sys.getfilesystemencoding()
#        self.send_header("Content-type", "text/html; charset=%s" % encoding)
#        self.send_header("Content-Length", str(length))
#        self.end_headers()
#        return f

httpd = SocketServer.TCPServer(("", 80), DirectoryHandler)
print "serving at port", 80
httpd.serve_forever()
