import os, sys
from datetime import datetime

time = datetime.now().strftime("%B %d %H:%M hours")
status = "git status"
add = 'git add .'
commit = "git commit -a -m " + "\"" +  time + "\""
push = "git push -u origin master"
os.system(status)
proceed = raw_input('Proceed?')
if (proceed == 'y'):
	os.system(commit)
	os.system(push)
elif (proceed == 'a'):
	os.system(add)
	os.system(commit)
	os.system(push)
else:
	print "Aborting push.."
