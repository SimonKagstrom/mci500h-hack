#!/bin/sh

debootstrap/debootstrap --second-stage
mv sources.list /etc/apt/sources.list
apt-get update
dpkg --set-selections < /debian-pkgs

apt-get -y dselect-upgrade
apt-get clean
dpkg -i local-packages/*.deb

cp -rap update-rootfs/* .

rm -rf finish-setup.sh debian-pkgs local-packages update-rootfs
