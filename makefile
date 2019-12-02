TBA.exe: bin/Font.o bin/Screen.o bin/ScreenUtils.o bin/Window.o bin/Log.o bin/Utility.o bin/Clock.o bin/Common.o bin/Game.o bin/KeyFuncs.o bin/Keys.o bin/Tile.o bin/Behavior.o bin/Block.o bin/Character.o bin/Command.o bin/CommandFuncs.o bin/CommandUtils.o bin/Container.o bin/Equipment.o bin/FloatingText.o bin/GameObject.o bin/Input.o bin/Inventory.o bin/Item.o bin/ItemManifest.o bin/Projectile.o bin/Statistics.o bin/Structure.o bin/World.o bin/Main.o
	g++ -std=c++11 bin/Font.o bin/Screen.o bin/ScreenUtils.o bin/Window.o bin/Log.o bin/Utility.o bin/Clock.o bin/Common.o bin/Game.o bin/KeyFuncs.o bin/Keys.o bin/Tile.o bin/Behavior.o bin/Block.o bin/Character.o bin/Command.o bin/CommandFuncs.o bin/CommandUtils.o bin/Container.o bin/Equipment.o bin/FloatingText.o bin/GameObject.o bin/Input.o bin/Inventory.o bin/Item.o bin/ItemManifest.o bin/Projectile.o bin/Statistics.o bin/Structure.o bin/World.o bin/Main.o -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image src/resource/icon.rs -lpthread -fpermissive  -O3 -o TBA.exe

bin/Font.o: src/ui/Font.cpp src/ui/Font.h src/tools/Log.h src/tools/Log.cpp src/tools/Utility.h src/tools/Utility.cpp  
	g++ -std=c++11 -c src/ui/Font.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Font.o

bin/Screen.o: src/ui/Screen.cpp src/ui/Screen.h  src/ui/Font.h src/ui/Font.cpp src/common/Tile.h src/common/Tile.cpp src/game/GameObject.h src/game/GameObject.cpp  
	g++ -std=c++11 -c src/ui/Screen.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Screen.o

bin/ScreenUtils.o: src/ui/ScreenUtils.cpp  
	g++ -std=c++11 -c src/ui/ScreenUtils.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/ScreenUtils.o

bin/Window.o: src/ui/Window.cpp src/ui/Window.h src/tools/Log.h src/tools/Log.cpp src/tools/Utility.h src/tools/Utility.cpp  src/ui/Screen.h src/ui/Screen.cpp  
	g++ -std=c++11 -c src/ui/Window.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Window.o

bin/Log.o: src/tools/Log.cpp src/tools/Log.h  
	g++ -std=c++11 -c src/tools/Log.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Log.o

bin/Utility.o: src/tools/Utility.cpp src/tools/Utility.h  
	g++ -std=c++11 -c src/tools/Utility.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Utility.o

bin/Clock.o: src/common/Clock.cpp src/common/Clock.h  
	g++ -std=c++11 -c src/common/Clock.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Clock.o

bin/Common.o: src/common/Common.cpp src/common/Common.h  src/common/Game.h src/common/Game.cpp  
	g++ -std=c++11 -c src/common/Common.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Common.o

bin/Game.o: src/common/Game.cpp src/common/Game.h src/ui/Window.h src/ui/Window.cpp src/tools/Log.h src/tools/Log.cpp src/game/GameObject.h src/game/GameObject.cpp src/game/World.h src/game/World.cpp src/game/Character.h src/game/Character.cpp src/game/Command.h src/game/Command.cpp  src/common/Clock.h src/common/Clock.cpp  
	g++ -std=c++11 -c src/common/Game.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Game.o

bin/KeyFuncs.o: src/common/KeyFuncs.cpp src/common/KeyFuncs.h  
	g++ -std=c++11 -c src/common/KeyFuncs.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/KeyFuncs.o

bin/Keys.o: src/common/Keys.cpp  
	g++ -std=c++11 -c src/common/Keys.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Keys.o

bin/Tile.o: src/common/Tile.cpp src/common/Tile.h src/game/Character.h src/game/Character.cpp src/game/Block.h src/game/Block.cpp  
	g++ -std=c++11 -c src/common/Tile.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Tile.o

bin/Behavior.o: src/game/Behavior.cpp src/game/Behavior.h  
	g++ -std=c++11 -c src/game/Behavior.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Behavior.o

bin/Block.o: src/game/Block.cpp src/game/Block.h  src/game/GameObject.h src/game/GameObject.cpp  
	g++ -std=c++11 -c src/game/Block.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Block.o

bin/Character.o: src/game/Character.cpp src/game/Character.h src/common/Tile.h src/common/Tile.cpp  src/game/Behavior.h src/game/Behavior.cpp  src/game/Inventory.h src/game/Inventory.cpp  src/game/Equipment.h src/game/Equipment.cpp  
	g++ -std=c++11 -c src/game/Character.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Character.o

bin/Command.o: src/game/Command.cpp src/game/Command.h  
	g++ -std=c++11 -c src/game/Command.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Command.o

bin/CommandFuncs.o: src/game/CommandFuncs.cpp src/game/CommandFuncs.h  src/game/Command.h src/game/Command.cpp  
	g++ -std=c++11 -c src/game/CommandFuncs.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/CommandFuncs.o

bin/CommandUtils.o: src/game/CommandUtils.cpp src/game/CommandUtils.h  src/game/GameObject.h src/game/GameObject.cpp  src/game/Command.h src/game/Command.cpp src/tools/Utility.h src/tools/Utility.cpp  
	g++ -std=c++11 -c src/game/CommandUtils.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/CommandUtils.o

bin/Container.o: src/game/Container.cpp src/game/Container.h  src/game/GameObject.h src/game/GameObject.cpp  src/game/Inventory.h src/game/Inventory.cpp src/common/Tile.h src/common/Tile.cpp src/common/Common.h src/common/Common.cpp src/tools/Utility.h src/tools/Utility.cpp  
	g++ -std=c++11 -c src/game/Container.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Container.o

bin/Equipment.o: src/game/Equipment.cpp src/game/Equipment.h  src/game/Item.h src/game/Item.cpp  src/game/ItemManifest.h src/game/ItemManifest.cpp  
	g++ -std=c++11 -c src/game/Equipment.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Equipment.o

bin/FloatingText.o: src/game/FloatingText.cpp src/game/FloatingText.h src/common/Common.h src/common/Common.cpp src/ui/Screen.h src/ui/Screen.cpp  src/game/GameObject.h src/game/GameObject.cpp src/tools/Utility.h src/tools/Utility.cpp  
	g++ -std=c++11 -c src/game/FloatingText.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/FloatingText.o

bin/GameObject.o: src/game/GameObject.cpp src/game/GameObject.h src/tools/Utility.h src/tools/Utility.cpp  src/game/Inventory.h src/game/Inventory.cpp  
	g++ -std=c++11 -c src/game/GameObject.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/GameObject.o

bin/Input.o: src/game/Input.cpp src/game/Input.h  
	g++ -std=c++11 -c src/game/Input.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Input.o

bin/Inventory.o: src/game/Inventory.cpp src/game/Inventory.h  src/game/Item.h src/game/Item.cpp src/tools/Utility.h src/tools/Utility.cpp  
	g++ -std=c++11 -c src/game/Inventory.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Inventory.o

bin/Item.o: src/game/Item.cpp src/game/Item.h  src/game/ItemManifest.h src/game/ItemManifest.cpp  
	g++ -std=c++11 -c src/game/Item.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Item.o

bin/ItemManifest.o: src/game/ItemManifest.cpp src/game/ItemManifest.h  
	g++ -std=c++11 -c src/game/ItemManifest.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/ItemManifest.o

bin/Projectile.o: src/game/Projectile.cpp src/game/Projectile.h  src/game/GameObject.h src/game/GameObject.cpp src/common/Tile.h src/common/Tile.cpp src/common/Common.h src/common/Common.cpp  
	g++ -std=c++11 -c src/game/Projectile.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Projectile.o

bin/Statistics.o: src/game/Statistics.cpp src/game/Statistics.h  
	g++ -std=c++11 -c src/game/Statistics.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Statistics.o

bin/Structure.o: src/game/Structure.cpp src/game/Structure.h  
	g++ -std=c++11 -c src/game/Structure.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Structure.o

bin/World.o: src/game/World.cpp src/game/World.h  src/game/Character.h src/game/Character.cpp src/common/Tile.h src/common/Tile.cpp  src/game/Structure.h src/game/Structure.cpp src/ui/Font.h src/ui/Font.cpp  
	g++ -std=c++11 -c src/game/World.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/World.o

bin/Main.o: src/Main.cpp  
	g++ -std=c++11 -c src/Main.cpp -I /include/SDL2 -L /lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread  -fpermissive -O3 -o bin/Main.o

