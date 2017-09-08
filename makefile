CC=gcc
SRC=src
ODIR=obj
BUILD_DIR=build
EXEC_NAME=indexer
OPTIONS=-Wall

all: createdirs $(ODIR)/utils.o $(ODIR)/terminos.o
	$(CC) $(ODIR)/utils.o $(ODIR)/terminos.o $(SRC)/buscador.c $(OPTIONS) -o $(BUILD_DIR)/$(EXEC_NAME)

createdirs:
	mkdir $(ODIR)
	mkdir $(BUILD_DIR)

$(ODIR)/utils.o: $(SRC)/utils.c $(SRC)/utils.h $(SRC)/modelo.h
	$(CC) $(SRC)/utils.c $(OPTIONS) -c -o $(ODIR)/utils.o

$(ODIR)/terminos.o: $(SRC)/terminos.c $(SRC)/terminos.h $(SRC)/modelo.h
	$(CC) $(SRC)/terminos.c $(OPTIONS) -c -o $(ODIR)/terminos.o