.PHONY: all network test clean

export BUILD_DIR = $(shell echo `pwd`/build)

all : network

network :
	$(MAKE) -C src

test :
	$(MAKE) -C test

clean:
	$(MAKE) -C clean
	$(MAKE) -C clean
