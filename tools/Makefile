TGTS=buttons/buttons \
	lcd-test/lcd-test \
	remote-control/remote-control \
	volume-control/volume-control \
	tda7468/tda7468 \
	i2c-read/i2c-read

all: $(TGTS)

buttons/buttons: buttons/main.c
	gcc -Wall -o $@ $<

lcd-test/lcd-test: lcd-test/main.c
	gcc -Wall -o $@ $<

remote-control/remote-control: remote-control/main.c
	gcc -Wall -o $@ $<

volume-control/volume-control: volume-control/main.c
	gcc -Wall -o $@ $<

tda7468/tda7468: tda7468/main.c
	gcc -Wall -o $@ $<

i2c-read/i2c-read: i2c-read/main.c
	gcc -Wall -o $@ $<

clean:
	rm -f $(TGTS) *~
