import os

execname = "TBA"

basedir = os.path.dirname(os.path.realpath(__file__))+"/../../"

makefile = open(basedir+"makefile","w+")

filesources = ["src/ui/","src/tools/","src/common/","src/game/","src/"]

def getDeps(filename):
	try:
		f = open(basedir+filename.replace("/","\\")+".h","r")
	except:
		return ""
	header = f.readlines()
	deps = " "
	for i in range(min(10,len(header))):
		try:
			line = header[i]
		except:
			print(filename);return ""
		if '#include "' in line:
			if "/" not in line:
				line = line.replace("#include ","").replace('"',"").replace("../","").replace("\n","") + " "
				line = filename[:filename.rfind('/')+1]+line
				line = " "+line + line[:line.rfind('.')+1]+"cpp "	
				deps += line
			else:
				line = line.replace("#include ","").replace('"',"").replace("../","").replace("\n","") + " "
				line = line + line[:line.rfind('.')+1]+"cpp"
				line = "src/"+line.replace(" "," src/")+ " "
				deps += line
	f.close()
	return deps
	

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
	deps = getDeps(src)
	makestr += "bin/{1}.o: {0}.cpp {2} \n\tg++ -std=c++11 -c {0}.cpp `sdl2-config --libs --cflags` -lSDL2main -lSDL2_image -O2 -o bin/{1}.o\n\n".format(src,src[src.rfind('/')+1:],head + deps)
	
makefile.write(makestr)
makefile.close()
