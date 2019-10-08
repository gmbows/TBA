TBA: bin/Font.o bin/Screen.o bin/Window.o bin/ScreenUtils.o bin/Log.o bin/StringFuncs.o bin/Tile.o bin/KeyFuncs.o bin/Keys.o bin/Common.o bin/Game.o bin/Structure.o bin/Input.o bin/Statistics.o bin/Command.o bin/Block.o bin/Item.o bin/Container.o bin/GameObject.o bin/World.o bin/FloatingText.o bin/Projectile.o bin/Behavior.o bin/Inventory.o bin/ItemManifest.o bin/Character.o bin/Equipment.o bin/CommandFuncs.o bin/Attributes.o bin/Main.o
	g++ -std=c++11 bin/Font.o bin/Screen.o bin/Window.o bin/ScreenUtils.o bin/Log.o bin/StringFuncs.o bin/Tile.o bin/KeyFuncs.o bin/Keys.o bin/Common.o bin/Game.o bin/Structure.o bin/Input.o bin/Statistics.o bin/Command.o bin/Block.o bin/Item.o bin/Container.o bin/GameObject.o bin/World.o bin/FloatingText.o bin/Projectile.o bin/Behavior.o bin/Inventory.o bin/ItemManifest.o bin/Character.o bin/Equipment.o bin/CommandFuncs.o bin/Attributes.o bin/Main.o `sdl2-config --libs --cflags` -o TBA

bin/Font.o: src/ui/Font.cpp src/ui/Font.h 
	g++ -std=c++11 -c src/ui/Font.cpp `sdl2-config --libs --cflags` -o bin/Font.o

bin/Screen.o: src/ui/Screen.cpp src/ui/Screen.h 
	g++ -std=c++11 -c src/ui/Screen.cpp `sdl2-config --libs --cflags` -o bin/Screen.o

bin/Window.o: src/ui/Window.cpp src/ui/Window.h 
	g++ -std=c++11 -c src/ui/Window.cpp `sdl2-config --libs --cflags` -o bin/Window.o

bin/ScreenUtils.o: src/ui/ScreenUtils.cpp  
	g++ -std=c++11 -c src/ui/ScreenUtils.cpp `sdl2-config --libs --cflags` -o bin/ScreenUtils.o

bin/Log.o: src/tools/Log.cpp src/tools/Log.h 
	g++ -std=c++11 -c src/tools/Log.cpp `sdl2-config --libs --cflags` -o bin/Log.o

bin/StringFuncs.o: src/tools/StringFuncs.cpp src/tools/StringFuncs.h 
	g++ -std=c++11 -c src/tools/StringFuncs.cpp `sdl2-config --libs --cflags` -o bin/StringFuncs.o

bin/Tile.o: src/common/Tile.cpp src/common/Tile.h 
	g++ -std=c++11 -c src/common/Tile.cpp `sdl2-config --libs --cflags` -o bin/Tile.o

bin/KeyFuncs.o: src/common/KeyFuncs.cpp src/common/KeyFuncs.h 
	g++ -std=c++11 -c src/common/KeyFuncs.cpp `sdl2-config --libs --cflags` -o bin/KeyFuncs.o

bin/Keys.o: src/common/Keys.cpp  
	g++ -std=c++11 -c src/common/Keys.cpp `sdl2-config --libs --cflags` -o bin/Keys.o

bin/Common.o: src/common/Common.cpp src/common/Common.h 
	g++ -std=c++11 -c src/common/Common.cpp `sdl2-config --libs --cflags` -o bin/Common.o

bin/Game.o: src/common/Game.cpp src/common/Game.h 
	g++ -std=c++11 -c src/common/Game.cpp `sdl2-config --libs --cflags` -o bin/Game.o

bin/Structure.o: src/game/Structure.cpp src/game/Structure.h 
	g++ -std=c++11 -c src/game/Structure.cpp `sdl2-config --libs --cflags` -o bin/Structure.o

bin/Input.o: src/game/Input.cpp src/game/Input.h 
	g++ -std=c++11 -c src/game/Input.cpp `sdl2-config --libs --cflags` -o bin/Input.o

bin/Statistics.o: src/game/Statistics.cpp src/game/Statistics.h 
	g++ -std=c++11 -c src/game/Statistics.cpp `sdl2-config --libs --cflags` -o bin/Statistics.o

bin/Command.o: src/game/Command.cpp src/game/Command.h 
	g++ -std=c++11 -c src/game/Command.cpp `sdl2-config --libs --cflags` -o bin/Command.o

bin/Block.o: src/game/Block.cpp src/game/Block.h 
	g++ -std=c++11 -c src/game/Block.cpp `sdl2-config --libs --cflags` -o bin/Block.o

bin/Item.o: src/game/Item.cpp src/game/Item.h 
	g++ -std=c++11 -c src/game/Item.cpp `sdl2-config --libs --cflags` -o bin/Item.o

bin/Container.o: src/game/Container.cpp src/game/Container.h 
	g++ -std=c++11 -c src/game/Container.cpp `sdl2-config --libs --cflags` -o bin/Container.o

bin/GameObject.o: src/game/GameObject.cpp src/game/GameObject.h 
	g++ -std=c++11 -c src/game/GameObject.cpp `sdl2-config --libs --cflags` -o bin/GameObject.o

bin/World.o: src/game/World.cpp src/game/World.h 
	g++ -std=c++11 -c src/game/World.cpp `sdl2-config --libs --cflags` -o bin/World.o

bin/FloatingText.o: src/game/FloatingText.cpp src/game/FloatingText.h 
	g++ -std=c++11 -c src/game/FloatingText.cpp `sdl2-config --libs --cflags` -o bin/FloatingText.o

bin/Projectile.o: src/game/Projectile.cpp src/game/Projectile.h 
	g++ -std=c++11 -c src/game/Projectile.cpp `sdl2-config --libs --cflags` -o bin/Projectile.o

bin/Behavior.o: src/game/Behavior.cpp src/game/Behavior.h 
	g++ -std=c++11 -c src/game/Behavior.cpp `sdl2-config --libs --cflags` -o bin/Behavior.o

bin/Inventory.o: src/game/Inventory.cpp src/game/Inventory.h 
	g++ -std=c++11 -c src/game/Inventory.cpp `sdl2-config --libs --cflags` -o bin/Inventory.o

bin/ItemManifest.o: src/game/ItemManifest.cpp src/game/ItemManifest.h 
	g++ -std=c++11 -c src/game/ItemManifest.cpp `sdl2-config --libs --cflags` -o bin/ItemManifest.o

bin/Character.o: src/game/Character.cpp src/game/Character.h 
	g++ -std=c++11 -c src/game/Character.cpp `sdl2-config --libs --cflags` -o bin/Character.o

bin/Equipment.o: src/game/Equipment.cpp src/game/Equipment.h 
	g++ -std=c++11 -c src/game/Equipment.cpp `sdl2-config --libs --cflags` -o bin/Equipment.o

bin/CommandFuncs.o: src/game/CommandFuncs.cpp src/game/CommandFuncs.h 
	g++ -std=c++11 -c src/game/CommandFuncs.cpp `sdl2-config --libs --cflags` -o bin/CommandFuncs.o

bin/Attributes.o: src/game/Attributes.cpp src/game/Attributes.h 
	g++ -std=c++11 -c src/game/Attributes.cpp `sdl2-config --libs --cflags` -o bin/Attributes.o

bin/Main.o: src/Main.cpp  
	g++ -std=c++11 -c src/Main.cpp `sdl2-config --libs --cflags` -o bin/Main.o

