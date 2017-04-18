# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

all:
	platformio -f run

upload:
	platformio -f run --target upload

clean:
	platformio -f run --target clean

program:
	platformio -f run --target program

uploadfs:
	platformio -f run --target uploadfs

update:
	platformio -f update
