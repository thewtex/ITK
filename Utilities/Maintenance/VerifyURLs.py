#!/usr/bin/env python

import sys
import re
import httplib
import os

# compile regular expression to pull out URLs in ITK
# The ignored characters an common delineators, and not strick to the standard
http_re = re.compile("(http://[^\s<>\{\}\|\]\[\)\(\"]*)")
http_set = set()

for arg in sys.argv[1:]:
    if not os.path.isfile( arg ):
        continue
    f = open( arg, "r" )
    for l in  f.readlines():
        mo =  http_re.search( l )
        if mo is not None:
            http_set.add( mo.group(1))

print "Found ", len( http_set ), " unique URLS."

# compile regular expression to pull out the server address and path
server_re = re.compile( "http://([^/]+)(/?[^\s]*)" )

for url in http_set:
    mo = server_re.search( url )
    server = mo.group( 1 )
    path = mo.group( 2 )

    try:
        #print "Verifying URL: ", url,

        # connect to server and get the path
        conn = httplib.HTTPConnection( server )
        conn.request("GET", path )
        r1 = conn.getresponse()


        if  r1.status == httplib.OK:
            # URL is OK do nothing
            #print "   URL: ", url, r1.status, r1.reason
            pass
        elif r1.status == httplib.MOVED_PERMANENTLY:
            print "ERROR (URL needs update): ", url,
            print r1.status, r1.reason
            print "    Redirected to: ", r1.getheader("location")
            print "    ", r1.read()
        elif r1.status == httplib.FOUND:
            print "WARNING URL: ", url, r1.status, r1.reason
            #print "\t", r1.read()
            pass
        elif r1.status == httplib.FORBIDDEN:
            print "ERROR URL: ", url, r1.status, r1.reason
        elif r1.status == httplib.NOT_FOUND:
            print "ERROR URL: ", url, r1.status, r1.reason
        else:
            pass
            print "UNKNOW URL: ", url, "\"", r1.status, "\"", r1.reason


    except Exception as e:
        print
        print "While connecting to: ", url
        print e
    except:
        print "While connecting to: ", url
        print "Unexpected error:", sys.exc_info()[0]
        raise
    finally:
        conn.close()
