#!/bin/sh

debootstrap/debootstrap --second-stage
apt-get update
dpkg --set-selections < /debian-pkgs

apt-get -y upgrade
rm -f finish-setup.sh debian-pkgs
