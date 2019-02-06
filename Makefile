CC=gcc

psirom: argparse/argparse.c psion-rom-extractor.c
	$(CC) argparse/argparse.c psion-rom-extractor.c -o psirom