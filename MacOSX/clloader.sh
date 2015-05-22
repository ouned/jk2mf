#!/bin/sh

libpath="$1/Contents/Resources/jk2mfclient.dylib"
samepath="$1/../Jedi Knight II.app/Contents/Jedi Knight II MP.app/Contents/MacOS/Jedi Knight II MP"

if [ -f "$samepath" ]
then
	DYLD_INSERT_LIBRARIES="$libpath" "$samepath"
else
	DYLD_INSERT_LIBRARIES="$libpath" "/Applications/Jedi Knight II.app/Contents/Jedi Knight II MP.app/Contents/MacOS/Jedi Knight II MP"
fi