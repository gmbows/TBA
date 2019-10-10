import os,sys

for f in os.listdir("bin"):
    print("Deleted "+f);
    os.remove("bin/"+f)

#os.system("python tools/wincfg/win_opt_configure_autoinclude.py")
