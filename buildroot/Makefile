all: build


buildroot-2010.11.tar.bz2:
	wget http://buildroot.net/downloads/buildroot-2010.11.tar.bz2

buildroot-2010.11: buildroot-2010.11.tar.bz2
	tar -xf $<

build: buildroot.config buildroot-2010.11
	cp $< buildroot-2010.11/.config
	cd buildroot-2010.11 && make
