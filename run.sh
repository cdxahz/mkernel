#!/bin/bash
# make
qemu-system-i386 -no-kvm -no-reboot -curses -m 512 -kernel $1
