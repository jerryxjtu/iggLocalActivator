all:
	xcrun -sdk iphoneos gcc -arch armv7 -o iggactive -O3 -Wall -Wl,-dead_strip aes.c iggcrypt.c utils.c main.c
	ldid -S ./iggactive

install:
	