import os, sys
from datetime import datetime

time = datetime.now().strftime("%B %d %H:%M hours")
status = "git status"
commit = "git commit -a -m " + "\"" +  time + "\""
push = "git push -u origin master"
os.system(status);
os.system(commit);
os.system(push);
print commit
