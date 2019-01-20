CC=gcc

psirom: psion-rom-extractor.c
	$(CC) -o psirom psion-rom-extractor.c