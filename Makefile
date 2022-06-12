#---------------------------------------------------------
# Target file to be compiled by default
#---------------------------------------------------------
MAIN = main
#---------------------------------------------------------
# Source files to be compiled when necessary
#---------------------------------------------------------
SRC = init draw ball cue
#---------------------------------------------------------
# CC is the compiler to be used
#---------------------------------------------------------
CC = gcc
#---------------------------------------------------------
# Directories of files
#---------------------------------------------------------
ODIR = ./obj
SDIR = ./source
HDIR = ./headers
LDIR = ./lib
#---------------------------------------------------------
# CFLAGS are the options passed to the compiler
#---------------------------------------------------------
CFLAGS = -Wall -g -I$(HDIR) -L$(LDIR)
#---------------------------------------------------------
# OBJS are the object files to be linked
#---------------------------------------------------------
OBJS = $(patsubst %, $(ODIR)/%, $(addsuffix .o, $(MAIN) $(SRC)))
#---------------------------------------------------------
# LIBS are the external libraries to be used
#---------------------------------------------------------
LIBS = -lptask -lpthread -lrt -lm `allegro-config --libs`
#---------------------------------------------------------
# Dependencies
#---------------------------------------------------------
$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) $(LIBS)

$(ODIR)/$(MAIN).o: $(MAIN).c
	$(CC) -c $(CFLAGS) -o $@ $< $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $< $(LIBS)

#---------------------------------------------------
# Use `make clean` to remove unwanted files, such as .o files
#---------------------------------------------------
clean:
	rm -rf $(ODIR)/* *.o $(MAIN)