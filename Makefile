all:
	xcrun -sdk iphoneos gcc -arch armv7 -o iggactivatorM1 -O3 -Wall -Wl,-dead_strip aes.c iggcrypt.c utils.c main.c
	ldid -S ./iggactivatorM1
	xcrun -sdk iphoneos gcc -arch armv7 -o iggactivatorM2 -O3 -Wall -Wl,-dead_strip -DMOTHOD2 aes.c iggcrypt.c utils.c main.c
	ldid -S ./iggactivatorM2

install:
