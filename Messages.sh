#! /bin/sh
$EXTRACTRC src/*.kcfg >> rc.cpp || exit 11
$XGETTEXT rc.cpp src/*.cpp interfaces/*.cpp plugins/*/*.cpp plugins/*/*.h -o $podir/kontact.pot
