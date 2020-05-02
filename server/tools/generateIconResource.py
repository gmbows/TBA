import os,sys

os.system("D:\\Downloads\\MinGW\\bin\\windres.exe --use-temp-file src\\resource\\icon.rc -O coff -o src\\resource\\icon.rs")
print "Icon generated"
x = raw_input("")