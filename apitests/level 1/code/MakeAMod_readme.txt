JK2 Game Source. Copyright (C) 2001-2002 Raven Softare, Lucas Arts

The Game Source is broken out into 3 areas.

game - governs the game, runs on the server side.
cgame - governs the client side of the game, runs on the client side.
ui - handles the ui on the client side.

Making a quick mod is very straightforward. This document assumes Microsoft Visual C++ v6.xx. It covers making a slight mod to the game source, recompiling for debugging and rebuilding the VMs for distribution.

Slow Rockets - TestMod
----------------------
1. Open up the jk2mp-SDK.dsw in Microsoft Visual C++.
2. Set the "game" project as the active project.
3. Open the "g_local.h" file and change the GAMEVERSION define from "basejk" to "TestMod"
4. Save "g_local.h"
5. Open the "g_weapon.c" file.
6. Go to line 85 and change the 900 to 300. The old line reads:
	
	#define	ROCKET_VELOCITY				900

The new line should read

	#define	ROCKET_VELOCITY				300

7. Save "g_weapon.c"
8. Perform a Build All command and it should build a DLL for the game. 

At this point you have two options. You can run the debugger, choosing 'jk2MP.exe' as the executable host which will load your DLL for debugging or you can build the VMs for distribution. When you release mods, you must build new VMs and put them in a pk3 file.

Building the VMs requires the files contained in the 'bin' path are available for execution ( lcc.exe and q3asm.exe )
(The batch files included will reference them indirectly.)

To build the sample vm for the slow rocket test, do the following:

1. Open a DOS window.
2. Go to your mods game directory and run the 'game.bat' file.

This ultimately produces a 'jk2mpgame.qvm' in the \base\vm\ path. 

5. Make a "TestMod" path under your JK2 directory. This will be a sibling to 'base'
6. Move 'jk2mpgame.qvm' to "\YourJK2Path\TestMod\vm\"
7. Run JK2MP with the following command line "jk2mp +set fs_game TestMod"
8. "TestMod" should be the referenced game and you should be able to catch up with and outrun your rockets.


Each of the areas contain a batch file "game.bat", "cgame.bat", and "ui.bat" which will build the appropriate vm files.

-----------------------------------------------
Using Visual Studio to Build and Debug your Mod

1. Create a directory to hold your Mod in the STEF directory
       \YourJK2Path\TestMod\
2. In VC, open Project->Settings
3. On the Debug tab Category General: 
   -set the "Executable for debug session" to your jk2MP.exe
   -Set the "Program arguments" to:  +set fs_cdpath yourfolderforjk2 +set r_fullscreen 0 +set viewlog 1 +set fs_game TestMod
   (note, if you don't have the "set fs_game TestMod", you will need to create
    a TestMod.pk3 file for the mod to show up in the menu - see below)
4. On the link tab, change path of the "Output file name" to your TestMod directory

Do this for each of projects you are making modifications to (cgame, game, ui)

-------------------------------------------------
Making my Mod show up on the Mod list in the game

You need to have a TestMod.pk3 file in your mod directory
before it will show up on the in-game menu. Create a "description.txt"
file with some information about your game mod. Use WinZIP to create 
the TestMod.pk3 file.  You can now put the .dll files in this directory, 
or create a "vm" directory and place the .qvm files in the vm directory.


	
