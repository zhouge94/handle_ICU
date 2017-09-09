#!/bin/bash
TOP=$(pwd)
rm -rf tmproot
mkdir -p tmproot
tar xvjf ./tmp/rootfs.tar.bz2 -C tmproot
chown -R root:root tmproot
#chown -R www-data:www-data tmproot/rootfs/var/www
./make_ext4fs -s -T -I -l 790626304 ./tmp/root.img ./tmproot/rootfs
rm -rf tmproot

./make_ext4fs -s -T -I -l 67108864  ./tmp/boot.img boot

