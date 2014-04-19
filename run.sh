#!/bin/bash
make
qemu-system-i386 -no-kvm -no-reboot -curses -kernel $1
