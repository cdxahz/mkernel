#!/bin/bash
if [ -f "Makefile" ]; then
	rm Makefile
fi

os=`uname`
if [ "$os" = "Darwin" ]; then
	cp Makefile.mac Makefile
else
	cp Makefile.gnu Makefile
fi

##make

make ARCH=arm CROSS_COMPILE=/opt/local/bin/arm-none-eabi-

