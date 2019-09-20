import os

execname = "TBA.exe"

basedir = "D:\Downloads\projects\TBA\NEWEST\\"

makefile = open(basedir+"makefile","w+")

filesources = ["src/ui/","src/tools/","src/common/","src/game/","src/"]

srcs=[]
heads=[]

for dir in filesources:
	srcs.extend([dir+src.split(".")[0] for src in os.listdir(basedir+dir) if src[-4:] == ".cpp"])
	heads.extend([dir+src.split(".")[0] for src in os.listdir(basedir+dir) if src[-2:] == ".h"])

makestr = ""

binlist = ["bin"+src[src.rfind('/'):]+".o" for src in srcs]

makestr += execname+": "+" ".join(binlist)+"\n\tg++ -std=c++11 "+" ".join(binlist)+" -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image src/resource/icon.rs -o "+execname+"\n\n"

for src in srcs:
	head=""
	if src in heads:
		head = src+".h"
	makestr += "bin/{1}.o: {0}.cpp {2} \n\tg++ -std=c++11 -c {0}.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -o bin/{1}.o\n\n".format(src,src[src.rfind('/')+1:],head)
	
makefile.write(makestr)
makefile.close()
