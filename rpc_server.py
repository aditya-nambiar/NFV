#!/usr/bin/python

from SimpleXMLRPCServer import SimpleXMLRPCServer, SimpleXMLRPCRequestHandler
import os, sys

#sys.stderr = open('rpc_server_responses.txt', 'w')

class RequestHandler(SimpleXMLRPCRequestHandler):
    rpc_paths = ('/RPC2',)

if(sys.argv[1] == "mme"):
	server = SimpleXMLRPCServer(("localhost", 20000), requestHandler=RequestHandler, allow_none=True)
	server.register_introspection_functions()
	def mme():
		os.system("./mme &")
	server.register_function(mme)
	server.serve_forever()

if(sys.argv[1] == "hss"):
	server = SimpleXMLRPCServer(("localhost", 20001), requestHandler=RequestHandler, allow_none=True)
	server.register_introspection_functions()
	def hss():
		os.system("./hss &")
	server.register_function(hss)
	server.serve_forever()

if(sys.argv[1] == "sgw"):
	server = SimpleXMLRPCServer(("localhost", 20002), requestHandler=RequestHandler, allow_none=True)
	server.register_introspection_functions()
	def sgw():
		os.system("./sgw &")
	server.register_function(sgw)
	server.serve_forever()

if(sys.argv[1] == "pgw"):
	server = SimpleXMLRPCServer(("localhost", 20003), requestHandler=RequestHandler, allow_none=True)
	server.register_introspection_functions()
	def pgw():
		os.system("./pgw &")
	server.register_function(pgw)
	server.serve_forever()

if(sys.argv[1] == "ran"):
	server = SimpleXMLRPCServer(("localhost", 20004), requestHandler=RequestHandler, allow_none=True)
	server.register_introspection_functions()
	def ran():
		os.system("./ran")
	server.register_function(ran)
	server.serve_forever()




