CC = gcc
LD = gcc
LDFLAGS =
RM = rm
PY = python3
HOME = ..
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

bind: strategy_bind.c pincpu.c process_prio.c ${INCLUDE_DIR}/*
	$(CC) strategy_bind.c pincpu.c process_prio.c -I ${INCLUDE_DIR}/ -o $(BIN_DIR)/strategy_bind

all: $(BIN_DIR)/set_dueling $(INCLUDE_DIR)/* ./*.c

clean:
	$(RM) $(BUILD_DIR)/*.o $(BIN_DIR)/set_dueling $(BIN_DIR)/strategy_bind

.PHONY: all clean bind
