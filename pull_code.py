#!/usr/bin/python
import os, sys
from datetime import datetime

time = datetime.now().strftime("%B %d %H:%M hours")
status = "git status"
add = 'git add .'
commit = "git commit -a -m " + "\"" +  time + "\""
pull = "git pull -u origin master"
os.system(status)
proceed = raw_input('Proceed? (y/n): ')
if (proceed == 'y'):
	os.system(commit)
	os.system(pull)
	os.system("make")
else:
	print "Aborting pull.."
