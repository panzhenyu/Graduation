CC=gcc
LD=gcc

LIBS=-lpthread
ifneq ($(wildcard /usr/include/perfmon/pfmlib_perf_event.h),)
LIBS+=-lpfm
else ifneq ($(wildcard /usr/local/include/perfmon/pfmlib_perf_event.h),)
LIBS+=-L/usr/local/lib -lpfm
else
$(error Can not find libpfm4)
endif

CFLAGS=-g -Wall -std=gnu99

all: profile_strategy

profile_strategy.o: benchmark.h cache.h exp_data.h expect.h pagemap.h perf_counter.h redirect.h lfsr.h tsc.h benchmark_list.h

cache.o: pagemap.h cache.h

exp_data.o: perf_counter.h exp_data.h

pagemap.o: pagemap.h

perf_counter.o: perf_counter.h expect.h

redirect.o: exp_data.h redirect.h

profile_strategy: exp_data.o pagemap.o cache.o perf_counter.o redirect.o profile_strategy.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	$(RM) *.o profile_strategy

.PHONY: all clean
