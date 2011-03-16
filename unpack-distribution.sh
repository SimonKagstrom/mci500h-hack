#!/bin/sh

if [ $# -ne 1 ] ; then
    echo "Usage: unpack-distribution.sh IP-addr"
    echo
    echo "Will unpack the distribution via SSH on the target"
    exit 1
fi

if [ ! -e dist.tar.gz ]; then
    echo "dist.tar.gz does not exist, run build-distribution.sh first"
    exit 1
fi

ip=$1
echo "Will copy and unpack to $ip"
cat dist.tar.gz | ssh root@$ip "/mnt/hda/busybox tar -C /mnt/hda/ -xzf -"
echo "Done"
