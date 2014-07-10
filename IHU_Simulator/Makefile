#specific
SOURCES=main.c cmd_api.c
EXE_EXP4=dummy_exp4
EXE_IHU=dummy_ihu

#generic
CC=gcc
CFLAGS=-g -Wall -std=c99
IHU_D=-D DUMMY_IHU

all: $(EXE_IHU) $(EXE_EXP4)

$(EXE_IHU): $(SOURCES)
		$(CC) $(CFLAGS) -D DUMMY_IHU $(SOURCES) -o $(EXE_IHU)

$(EXE_EXP4): $(SOURCES)
		$(CC) $(CFLAGS) -D DUMMY_EXP4 $(SOURCES) -o $(EXE_EXP4)

clean: 
	rm -rf *.o $(EXE_IHU) $(EXE_EXP4)
