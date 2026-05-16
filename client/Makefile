
.PHONY: build test

all: test build

client.3dsx:
	make -C 3ds

client.nds:
	make -C nds

build: client.nds client.3dsx

test:
	g++ common/translator.test.cpp -o tests.out && ./tests.out; rm ./tests.out