#!/bin/sh

me=`whoami`

chroot_rootfs=etch-rootfs
dist_rootfs=dist

if [ $me != "root" ]; then
    echo "Need to run this script as root or perhaps with fakeroot"
    exit 1
fi

if [ ! -x $chroot_rootfs ]; then
    echo "Directory $chroot_rootfs does not exist, run setup-build-env.sh first"
    exit 1
fi



install -d $dist_rootfs
cp -rap $chroot_rootfs/* $dist_rootfs/
rm -rf $dist_rootfs/root/* $dist_rootfs/usr/games $dist_rootfs/usr/share/doc $dist_rootfs/usr/share/man $dist_rootfs/usr/include $dist_rootfs/usr/local/include $dist_rootfs/usr/local/share $dist_rootfs/usr/local/man $dist_rootfs/usr/bin/qemu-arm-static $dist_rootfs/var/cache/apt/archives/ $dist_rootfs/var/cache/apt/archives/ $dist_rootfs/var/lib/apt/lists/

tar -C $dist_rootfs/dev/ --mtime=2011-03-10 -xzf devel-helpers/dev.tar.gz

# Create a tarball
tar -czf dist.tar.gz $dist_rootfs/

echo "Done. Now copy dist.tar.gz and $dist_rootfs/bin/busybox to the"
echo "board, and unpack with /path/to/busybox tar -xzf dist.tar.gz somewhere under"
echo "/mnt/hda/."
echo
echo "Then chroot into the new filesystem with"
echo "  /path/to/busybox chroot ."
echo "in the path where you unpacked dist.tar.gz"
