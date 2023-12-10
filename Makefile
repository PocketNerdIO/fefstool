CC=g++

all: fefstool

fefstool: argparse/argparse.c misc.c statwrap.c sibo_fefs.cpp fefstool.cpp
	$(CC) argparse/argparse.c misc.c statwrap.c sibo_fefs.cpp fefstool.cpp -o fefstool

mingw32: argparse/argparse.c misc.c statwrap.c sibo_fefs.cpp fefstool.cpp
	i686-w64-mingw32-clang argparse/argparse.c misc.c statwrap.c sibo_fefs.cpp fefstool.cpp -o bin/win32/fefstool.exe

mingw64: argparse/argparse.c misc.c statwrap.c sibo_fefs.cpp fefstool.cpp
	x86_64-w64-mingw32-clang argparse/argparse.c misc.c statwrap.c sibo_fefs.cpp fefstool.cpp -o bin/win64/fefstool.exe
