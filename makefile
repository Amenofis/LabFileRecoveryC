CC = gcc
CFLAGS = -g -Wall

SRC = src
ODIR = obj
BUILD_DIR = build

EXEC_NAME = indexer

debug: all

all: createdirs $(ODIR)/utils.o $(ODIR)/terminos.o
	$(CC) $(CFLAGS) $(ODIR)/utils.o $(ODIR)/terminos.o $(SRC)/buscador.c -o $(EXEC_NAME)

createdirs:
	mkdir -p $(ODIR)
	mkdir -p $(BUILD_DIR)

$(ODIR)/utils.o: $(SRC)/utils.c $(SRC)/utils.h $(SRC)/modelo.h
	$(CC) -g $(SRC)/utils.c $(OPTIONS) -c -o $(ODIR)/utils.o

$(ODIR)/terminos.o: $(SRC)/terminos.c $(SRC)/terminos.h $(SRC)/modelo.h
	$(CC) -g $(SRC)/terminos.c $(OPTIONS) -c -o $(ODIR)/terminos.o

clean:
	$(RM) $(ODIR) -r
	$(RM) $(BUILD_DIR) -r
