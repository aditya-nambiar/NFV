#!/usr/bin/python

import xmlrpclib

mme = xmlrpclib.ServerProxy('http://localhost:20000')
hss = xmlrpclib.ServerProxy('http://localhost:20001')
sgw = xmlrpclib.ServerProxy('http://localhost:20002')
pgw = xmlrpclib.ServerProxy('http://localhost:20003')
ran = xmlrpclib.ServerProxy('http://localhost:20004')

print "comes here 1"
mme.mme()
print "comes here 2"
hss.hss()
sgw.sgw()
pgw.pgw()
ran.ran()