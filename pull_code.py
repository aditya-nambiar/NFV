import os, sys
from datetime import datetime
from sys import version_info

py3 = version_info[0]

res = raw_input("Please enter your name: ")
if(res == 'y'):
	print res
else:
	print "wrong"
