*****************************
          JK2MFDED
*****************************

Author:  ouned ( PeTrol, asc*Twilight', Twitch )
Website: http://jk2.ouned.de/jk2mf/
Version: 1.5

--------------
About
--------------

This is a new jk2ded, based on the released sourcecode, which works with jk2mf.
It supports version 1.04 and 1.02 in one executable.

--------------
Installtion
--------------

Just use it exactly like you would use the original jk2ded.
It detects if you want to run 1.02 or 1.04 by searching for "assets5.pk3" in the base directory.

If you want to use JK2MF put the "jk2mf.dll" (windows) or "jk2mf.so" (linux) into the same directory.

--------------
Features
--------------

- supports 1.02 and 1.04
- is able to load jk2mf but still works without
- linux 1.04: compiled with g++ and not the intel compiler which removes the annoying libcxa.so.1 dependency
- linux 1.02: does not need any library workarounds, it just works
- faster in some places because of better/modern compiler optimizations
- removed CPU detection code
- increased maximum number of cvars from 1024 to 2048
- windows: ASLR and DEP protection activated (more secure executable)
- q3infoboom fixed (also without jk2mf)
- q3msgboom fixed (because it originally is the 1.04 jk2ded)
- q3dirtrav fixed (also without jk2mf)

For all other fixes use it together with jk2mf.

--------------
Changelog
--------------

------
1.5
------
- increases MAX_PATCH_PLANES from 2048 to 4096, fixes loading of Tatooine_Outpost_2_0.bsp
- jk2mfded_force102 set this in the command line arguments to force 1.02 mode (does not prevent loading of assets5.pk3 etc.)

------
1.4
------
- sv_master2 defaulting to "master.ouned.de"

------
1.3
------
- uses SSE to be faster
- again better compiler optimized on both win32 and linux

------
1.2
------
- linux: compiled with newer gcc version
- linux: better compiler optimizations
- displays it's version on startup