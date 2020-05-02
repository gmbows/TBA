import os,sys

for f in os.listdir("bin"):
    print("Deleted "+f);
    os.remove("bin/"+f)

#os.system("python tools/wincfg/win_opt_configure_autoinclude.py")
if "/home" in os.path.dirname(os.path.realpath(__file__)):
	print("Configured makefile for linux")
	os.system("python tools/linuxcfg/linux_opt_autoinclude.py")
else:
	print("Configured makefile for windows")
	os.system("py tools/wincfg/win_opt_configure_autoinclude.py")
#basedir = "D:\Downloads\projects\TBA\NEWEST\\"
