import os,sys

gameSrcf = open("../src/common/Game.cpp","r")
gameSrc = gameSrcf.readlines()
gameSrcf.close()

objectString = "\t\t\tnew\t\t"

name = input("Command name? \n->")
aliases = ""#input("Comma separated alias list?\n->")
argc = ""#input("Minimum argument count?\n->")
hasEC = input("Does this command have errorchecking? [y/n] \n->")

aliasString = ""
ecString = ""
funcString = name+"Func"

#new		Command("stop",{},0,stopFunc),

if hasEC.lower() == 'y':
	hasEC = True
	ecString = ","+name+"EC"
else:
	hasEC = False
if len(aliases) > 0:
	aliasString = '{"'+aliases.replace(',','","')+'"}'
else:
	aliasString = "{}"

objectString += 'Command({0},{3}{4}),\n'.format(name,aliasString,argc,funcString,ecString)

declaration = "\n\t//{0}\n\tstd::string {1}Func(Command*, const std::vector<std::string>&);\n".format(name.title(),name)

definition = "\n//{0}\nstd::string {1}Func(Command* command, const std::vector<std::string> &args)".format(name.title(),name)+" {\n\n}\n"

if hasEC:
	declaration += "\tbool {0}EC(Command*, const std::vector<std::string>&);\n".format(name)
	definition += "bool {0}EC(Command* command, const std::vector<std::string> &args)".format(name)+" {\n\n}\n"

print("===============")
print("GOING IN Game.cpp")
print("===============")
print(objectString)
print()
print("====================")
print("GOING IN CommandFuncs.h")
print("====================")
print(declaration)
print()
print("======================")
print("GOING IN CommandFuncs.cpp")
print("======================")
print(definition)
print()

correct = input("Does this look correct? [y/n]\n->")
if correct.lower() != 'y':
	print( "Exiting");exit()


#Backup file before writing
gamesrcbackup = open("../backup/Game.cpp","w+")
gamesrcbackup.write(''.join(gameSrc))
gamesrcbackup.close()

#any place before the command list

commandStart = 0
line = gameSrc[0]

while line.strip() != "////":
	commandStart += 1
	line = gameSrc[commandStart]

gameSrc.insert(commandStart,objectString)

#Replace with game.cpp ->
gameSrcModified = open("../src/common/Game.cpp","w+")
gameSrcModified.write(''.join(gameSrc))
gameSrcModified.close()

fileList = ["CommandFuncs.h","CommandFuncs.cpp"]

for f in fileList:
	sourceFile = open("../src/game/"+f,"r")
	fBackup = open("../backup/"+f,"w+")
	fBackup.write(sourceFile.read())
	fBackup.close()
	sourceFile.close()

headers = open("../src/game/CommandFuncs.h","a")
headers.write(declaration)
headers.close()

body = open("../src/game/CommandFuncs.cpp","a")
body.write(definition)
body.close()
