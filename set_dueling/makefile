CC = gcc
CFLAGS = -c
LD = gcc
LDFLAGS =
RM = rm
PY = python3
BUILD = ./build
OBJS = $(BUILD)/cache.o $(BUILD)/set_dueling.o $(BUILD)/main.o
PLOT_PARAM =

$(BUILD)/cache.o: cache.c cache.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD)/set_dueling.o: set_dueling.c set_dueling.h cache.h tsc.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD)/main.o: main.c set_dueling.h cache.h
	$(CC) $(CFLAGS) $< -o $@

set_dueling: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

all: set_dueling

clean:
	$(RM) $(BUILD)/*.o ./set_dueling

plot:
	$(PY) plot.py $(PLOT_PARAM)

.PHONY: all clean plot