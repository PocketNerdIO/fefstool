CC=gcc

all: siboimg

siboimg: argparse/argparse.c siboimg.c
	$(CC) argparse/argparse.c siboimg.c -o siboimg

mingw32: argparse/argparse.c siboimg.c
	i686-w64-mingw32-clang argparse/argparse.c siboimg.c -o bin/win32/siboimg.exe

mingw64: argparse/argparse.c siboimg.c
	x86_64-w64-mingw32-clang argparse/argparse.c siboimg.c -o bin/win64/siboimg.exe
