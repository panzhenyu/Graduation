CC = gcc
LD = gcc
LDFLAGS =
RM = rm
PY = python3
HOME = /home/panda/Desktop/Graduation/set_dueling
BIN_DIR = $(HOME)/bin
BUILD_DIR = $(HOME)/build
INCLUDE_DIR = $(HOME)/src/include
CFLAGS = -c -I $(INCLUDE_DIR)/
OBJS = $(BUILD_DIR)/cache.o $(BUILD_DIR)/set_dueling.o $(BUILD_DIR)/main.o \
       $(BUILD_DIR)/process_prio.o $(BUILD_DIR)/pincpu.o
PLOT_PARAM =

$(BUILD_DIR)/cache.o: cache.c $(INCLUDE_DIR)/*
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/set_dueling.o: set_dueling.c $(INCLUDE_DIR)/*
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/process_prio.o: process_prio.c $(INCLUDE_DIR)/*
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/pincpu.o: pincpu.c $(INCLUDE_DIR)/*
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/main.o: main.c $(INCLUDE_DIR)/*
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/set_dueling: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

all: $(BIN_DIR)/set_dueling

clean:
	$(RM) $(BUILD_DIR)/*.o $(BIN_DIR)/set_dueling

.PHONY: all clean
