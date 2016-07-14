#!/bin/sh

printf "Generating Files.... \n"

aclocal >/dev/null 2>&1 
autoheader
automake --add-missing --foreign 
autoconf
autoreconf -i

printf "Your are now ready to run configure\nDone\n";
