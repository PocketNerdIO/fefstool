CC=gcc

all: fefstool

fefstool: argparse/argparse.c fefstool.cpp
	$(CC) argparse/argparse.c statwrap.c fefstool.cpp -o fefstool

mingw32: argparse/argparse.c fefstool.cpp
	i686-w64-mingw32-clang argparse/argparse.c statwrap.c siboimg.cpp -o bin/win32/fefstool.exe

mingw64: argparse/argparse.c fefstool.cpp
	x86_64-w64-mingw32-clang argparse/argparse.c statwrap.c siboimg.cpp -o bin/win64/fefstool.exe
