CC=gcc

all: psirom

psirom: argparse/argparse.c psion-rom-extractor.c
	$(CC) argparse/argparse.c psion-rom-extractor.c -o psirom

mingw32: argparse/argparse.c psion-rom-extractor.c
	i686-w64-mingw32-clang argparse/argparse.c psion-rom-extractor.c -o bin/win32/psirom.exe

mingw64: argparse/argparse.c psion-rom-extractor.c
	x86_64-w64-mingw32-clang argparse/argparse.c psion-rom-extractor.c -o bin/win64/psirom.exe
