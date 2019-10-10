import os

execname = "TBA"

basedir = os.path.dirname(os.path.realpath(__file__))+"/../../"

makefile = open(basedir+"makefile","w+")

filesources = ["src/ui/","src/tools/","src/common/","src/game/","src/"]

srcs=[]
heads=[]

for dir in filesources:
	srcs.extend([dir+src.split(".")[0] for src in os.listdir(basedir+dir) if src[-4:] == ".cpp"])
	heads.extend([dir+src.split(".")[0] for src in os.listdir(basedir+dir) if src[-2:] == ".h"])

makestr = ""

binlist = ["bin"+src[src.rfind('/'):]+".o" for src in srcs]

makestr += execname+": "+" ".join(binlist)+"\n\tg++ -std=c++11 "+" ".join(binlist)+" `sdl2-config --libs --cflags` -lSDL2main -lSDL2_image -O2 -o "+execname+"\n\n"

for src in srcs:
	head=""
	if src in heads:
		head = src+".h"
	makestr += "bin/{1}.o: {0}.cpp {2} \n\tg++ -std=c++11 -c {0}.cpp `sdl2-config --libs --cflags` -lSDL2main -lSDL2_image -O2 -o bin/{1}.o\n\n".format(src,src[src.rfind('/')+1:],head)
	
makefile.write(makestr)
makefile.close()
