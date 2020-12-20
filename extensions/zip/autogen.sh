#!/bin/sh

echo "generaing build information using aclocal, autoheader, automake and autoconf"

libtoolize
aclocal
autoheader
automake --include-deps --add-missing --copy 
autoconf
