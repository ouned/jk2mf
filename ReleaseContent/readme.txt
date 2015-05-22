*****************************
            JK2MF
*****************************

Author:  ouned ( PeTrol, asc*Twilight', Twitch )
Website: http://jk2.ouned.de/jk2mf/
Version: 3.0.5

--------------
About JK2MF
--------------
JK2MF is a plugin for Jedi Knight II that implements an anticheat system, HTTP-Downloads
and a lot of basejk fixes in a mod independent way. You can use JK2MF together with
DS-Online, DCMod, Twimod and most other mods.

--------------
Installation
--------------

!METHOD 3 IS THE RECOMMENDED ONE!

It may seem confusing to have three different methods but jk2mf was already there before the source release of jk2 and
needed some tricky ways to modify jk2.
If you install jk2mf for the first time you should use method three.

There are three ways to install JK2MF on your server.
Method 1 works only if you can access the startscript of the jk2ded. (only still there for not annoying serveradmins with reinstallation)
Method 2 is a tricky way for which it is only required to have limited FTP access.
Method 3 is the only one working for 1.02 and the new recommended way for both 1.02 and 1.04.

WARNING: If you want to use the anticheat system and/or the HTTP-Downloads scroll down for additional install instructions.

--- METHOD 1 ---
Works for: Linux and Windows JK2 1.04

Windows:
Take the jk2mf.exe, jk2mf.dll file and the jk2mf directory out of the zip file you downloaded and unpack it into
the same directory in which your jk2ded.exe or jk2mp.exe is located.
Example to start your server: jk2mf.exe +exec server.cfg
Check if jk2mf prints a message on the console, if it does, everything is working.

Linux:
Take the "jk2mf.so" file out of the zip you downloaded and unpack it anywhere you want to have it.
Unpack the jk2mf directory into the same directory which contains your jk2ded.
Example to start your server: LD_PRELOAD="/home/jk2/jk2mf.so" /home/jk2/jk2ded +map ffa_bespin
Check if jk2mf prints a message on the console, if it does, everything is working.
Do not set the LD_PRELOAD variable on an extra line in your shell script, it will not work.

NOTE: It's better to use the original executables (jk2ded) released by lucasarts because
      the fixes made by aluigi and others are tricky and not as clean as the fixes used by jk2mf.
      You can get them from http://jk2files.com
      But you can still use modified jk2deds if you want to.

--- METHOD 2 ---
Works for: Linux and Windows JK2 1.04

Unpack the jk2mpgamex86.dll + jk2mf.dll (win) or jk2mpgamei386.so + jk2mf.so (linux) files and the jk2mf directory into your
current mod folder (fs_game cvar, default is "base").
Now you need to copy your mod's (or basejk's) jk2mpgame.qvm file into yourmodsfolder/vm and rename it to modgame.qvm.
Open your server configuration and add the following line BEFORE loading a map:
  set vm_game "0"
You can now use /rcon exec server.cfg to execute the script.
JK2MF should now run on your server.

ADVANCED USERS: You can specific if your mod is a shared library or a qvm using the jk2mf_vmgame cvar.

--- METHOD 3 ---
Works for: Linux and Windows JK2 1.04 and 1.02

Instead of the original jk2ded you need to use my so called jk2mfded to install jk2mf.
You can find the jk2mfded for linux and windows in the jk2mfded directory of the archive.

Unpack the jk2mf.dll (win) or jk2mf.so (linux) into the same directory which contains your jk2mfded.
Now you can run your server like you ever would, it will load jk2mf automatically.

No seperate jk2mfded for 1.04 and 1.02 is needed it works with both.
To see the exact changes in jk2mfded see jk2mfded/readme.txt.

--------------
Fixes
--------------

Every fix can be turned off by setting it's cvar to "0".
With DS-Online, Twimod or DCMod there is no reason at all to turn any of these fixes off expect the wpglowing fix.

seta jk2mf_fixq3infoboom "1"
seta jk2mf_fixforcecrash "1"
seta jk2mf_fixnamecrash "1"
seta jk2mf_fixq3filluic "1"
seta jk2mf_fixq3fillmcc "2"
seta jk2mf_fixflooding "900"
seta jk2mf_fixq3msgboom "1"
seta jk2mf_fixturretcrash "1"
seta jk2mf_fixq3cbufexec "1"
seta jk2mf_fixq3dirtrav "1"
seta jk2mf_fixrconblocking "1"
seta jk2mf_fixwpglowing "0" // READ THE DESCRIPTION BEFORE TURNING THIS ON
seta jk2mf_fixnamediff "1"
seta jk2mf_fixrocketbug "1"
seta jk2mf_fixteamcrash "1"
seta jk2mf_fixsaberstealing "1"
seta jk2mf_fixudpflooding "15"
seta jk2mf_fixgalaking "1"
seta jk2mf_fixbrokenmodels "1"

jk2mf_fixq3infoboom:
  If turned on this will fix q3infoboom and your server will not crash.
jk2mf_fixforcecrash:
  If turned on each time the client modifys the forcestring jk2mf will check it
  and if it's corrupt a valid forcestring will be used.
  This can cause problems with mods that are changing the forcesystem.
jk2mf_fixnamecrash:
  If activated the name will be checked by a whitelist of characters which jk2 can display
  without problems. All others are filtered out.
jk2mf_fixq3filluic:
  If activated each connecting client will be checked if it sends some jk2 specific userinfo.
  If not it's a q3fill attempt and the connection will be refused.
jk2mf_fixq3fillmcc:
  If higher than zero this is the max number of connecting clients per IP.
  So standard is that a maximum of two people using the same IP can connect at the same time.
  This does not mean that only two clients with the same IP can join the server. It affects only connecting clients.
jk2mf_fixflooding:
  If higher then zero this is the minimum time in milliseconds a client has to wait till he can change his userinfo again.
  If the value is less then "10" it's still interpreted in seconds to be backward compatible.
  Turn this off if your mod already includes that. (DS-Online for example has that)
jk2mf_fixq3msgboom:
  If turned on all useles vsay commands will be disabled and a maximum of characters for clientcommands will be set so
  that only the exploiting client will start to lagg.
jk2mf_fixturretcrash:
  If turned on and some crashers are trying to crash the server with turretcrash the server will detect that and
  removes all the many tempEntitys so that the maximum will not be reached.
jk2mf_fixq3cbufexec:
  If activated all votestrings will be checked for seperator chars and rejected if the string contains one.
jk2mf_fixq3dirtrav:
  If activated all download request which don't request a file inside the gamedata directory or dont't target
  a pk3 file will be rejected.
jk2mf_fixrconblocking:
  If activated the 500msec limit between different rcon requests will be disabled so that every request is accepted.
  Warning: If your rconpassword is very weak it will be easier to bruteforce your password!
jk2mf_fixwpglowing:
  This will fix the glowing around the player when the server ran for a longer time and the player starts to charge a weapon.
  HUGE WARNING: This resets the level.time on each new map which may be used crosslevel by the mod you are using.
                If you want to use it, TEST it first.
                However, it works wonderful with basejk.
jk2mf_fixnamediff:
  If a player renames himself to - for example - "^7" the name is filtered and set to Padawan (because it's empty). But only in the mod itself and not in the
  engine, so the name will still be empty in the serverinfo.
  If activated jk2mf will get the name out of the mod everytime it's changed and sets it in the engine.
jk2mf_fixrocketbug:
  When turned on the rocketLockTime is reset everytime a client changes his weapon, uses the force or goes into an animation.
jk2mf_fixteamcrash:
  There is a way to crash empty servers using the "team" cvar. If activated malformed team cmds are filtered out.
jk2mf_fixsaberstealing:
  Spectators can steal the saber of a client by following it and then jumping out of the following mode.
  This fix will not allow a saberEntityNum for spectators.
jk2mf_fixudpflooding:
  Maximum connectionless UDP packets answered per ip / per second.
  You should not go below 15 with it because programs like HLSW tend to not work correctly with your server then.
jk2mf_fixgalaking:
  This is 1.02 only stuff.
  Blocks use of model galak_mech on the server.
jk2mf_fixbrokenmodels:
  This is 1.02 only stuff.
  Blocks use of models kyle/fpls kyle/fpls2 and all morgan skins except morgan/default_mp, morgan/red and morgan/blue

--------------
Anticheat
--------------

Use the following cvar to toggle the anticheat system on and off
seta jk2mf_anticheat "1"
seta jk2mf_forceplugin "1"

If jk2mf_anticheat is activated and jk2mf_forceplugin disabled the anticheat is optional and players can see
who is cheatfree using the "/jk2mfinf players" command.
If you don't want to let players join in case they don't have the plugin installed switch jk2mf_forceplugin to 1.

Load a new map or even better restart the whole server after changing one of those cvars.

If turned on, all players need the clientside to join on the server.
The clientside can be downloaded from the website and if a player doesn't have it, he gets a message in the console which
contains the link. The clientside from the website is just an updater, which means the "real" clientside will be downloaded by it.
This has the advantage that players don't need to install a new clientside by their self if a new version is released.

The anticheat system detects and blocks
- code modifications ( jkbot, mombot, cvar unlockers, opengl hacks and more )
- unallowed clientside modifications ( cgame.qvm )
- movement scripts

WARNING DO NOT FORGET:
Do not forget to place all the clientmods ( cgame.qvm ) you want to allow into the "jk2mf/clplugins" folder.
Example: If your server is running DS-Online v1.32, it's recommended to place the base and the DS-Online clientside cgame.qvm into this folder.
         The names of them don't matter.

--------------
HTTP Server
--------------

seta jk2mf_httpdownloads "1"
seta jk2mf_httpserverport "2420" // "0" will automatically choose a port if you dont care

If activated the server will start a simple http server in the background which allows clients to download maps, mods and other pk3 files
automatically just like with sv_allowdownload but a lot faster.
Don't forget to open the port (TCP) specified at jk2mf_httpserverport if required.

NOTE: On linux every port lower then 1024 is blocked by the system if the server is not running with root privileges.

WARNING:  Do never have spaces in pk3 file names. Pk3 files with spaces in their name can't be downloaded.
          Do never use ".PK3". The files suffix should always be lowercase. (".pk3")

--- Autodownload lists ---

whitelist.txt:
  If you enabled this restriction only pk3 files listed in this file can be downloaded.
  This can be useful if you have pk3 files on your server that shouldn't be downloaded even if used.
blacklist.txt:
  If you enabled this restriction pk3 files listed in this file can not be downloaded.
  This can be useful if you have pk3 files on your server that shouldn't be downloaded even if used.
reflist.txt:
  If you enabled this list, pk3 files that are listed will be downloaded no matter if they are currently used.
  This is only useful in special cases.

In most cases you don't need any of these lists.
You don't need them to fix q3dirtrav, because this is done with "jk2mf_fixq3dirtrav".
This lists affect both, sv_allowdownload and HTTP-Downloads.

--------------
API
--------------

NOTE: This is only for mod developers. Everyone else can safely ignore this.

Since JK2MF 2.0 there is an API for mod developers avaiable which can be used to integrate some more features into the game code.
To use the API in your mod copy all the *.c and *.h files in the api directory into your mod's "game" source directory.
You will overwirte two files but I'm pretty sure you didn't modify any of them anyways. (If you did, merge them)
Open the file "g_local.h" and add
	#include "jk2mf_api.h"
AFTER
	#include "g_public.h"

You are now ready to use all the new syscalls JK2MF implements.
For more information have a look into the "jk2mf_api.h" file and the examples.

If you did everything correctly your mod will still run on servers without JK2MF. ( with less features )
It will also be forward compatible with future JK2MF versions.

--------------
Credits
--------------
- NTxC for his awesome technical help
- Deathspike & Daggolin for the forcecrash fix
- The mongoose HTTP-Server project
- The libcurl HTTP-Client project
- Ravensoft, Lucasarts for an awesome game
- Daggolin & stulle for fixing the MacOSX client
- Daggolin for parts of the 1.02 port