#!/bin/sh

me=`whoami`

if [ $me != "root" ]; then
    echo "Need to run this script as root or perhaps with fakeroot"
    exit 1
fi

echo "Doing debootstrap for Debian etch"
debootstrap --arch=arm --foreign etch etch-rootfs http://archive.debian.org/debian/
if [ $? -ne 0 ]; then
    echo "debootstrap failed!"
    exit 1
fi
cp -rap devel-helpers/rootfs/* etch-rootfs/

echo "Done. Now copy qemu-arm-static into etch-rootfs/usr/bin/ and chroot into etch-rootfs"
