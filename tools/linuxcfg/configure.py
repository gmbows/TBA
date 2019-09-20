import os

execname = "TBA"
basedir = "../../"

makefile = open(basedir+"makefile","w+")

uisource_dir = "src/ui/"
srcs = [uisource_dir+src.split(".")[0] for src in os.listdir(basedir+uisource_dir) if src[-4:] == ".cpp"]
heads = [uisource_dir+src.split(".")[0] for src in os.listdir(basedir+uisource_dir) if src[-2:] == ".h"]

gsource_dir = "src/game/"
srcs.extend([gsource_dir+src.split(".")[0] for src in os.listdir(basedir+gsource_dir) if src[-4:] == ".cpp"])
heads.extend([gsource_dir+src.split(".")[0] for src in os.listdir(basedir+gsource_dir) if src[-2:] == ".h"])

main_dir = "src/"
srcs.extend([main_dir+src.split(".")[0] for src in os.listdir(basedir+main_dir) if src[-4:] == ".cpp"])
heads.extend([main_dir+src.split(".")[0] for src in os.listdir(basedir+main_dir) if src[-2:] == ".h"])


makestr = ""

binlist = ["bin/"+src.replace(gsource_dir,"").replace(uisource_dir,"").replace(main_dir,"")+".o" for src in srcs]

makestr += execname+": "+" ".join(binlist)+"\n\tg++ -std=c++11 "+" ".join(binlist)+" `sdl2-config --cflags --libs` -lSDL2main -lSDL2_image -o "+execname+"\n\n"

for src in srcs:
	head=""
	if src in heads:
		head = src+".h"
	makestr += "bin/{1}.o: {0}.cpp {2} \n\tg++ -std=c++11 -c {0}.cpp `sdl2-config --cflags --libs` -lSDL2main -lSDL2_image -o bin/{1}.o\n\n".format(src,src.replace(gsource_dir,"").replace(uisource_dir,"").replace(main_dir,""),head)

makefile.write(makestr)
makefile.close()
