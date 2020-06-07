EVENTS="-e PERF_COUNT_HW_INSTRUCTIONS \
        -e PERF_COUNT_HW_CPU_CYCLES \
        -e PERF_COUNT_HW_CACHE_MISSES \
        -e PERF_COUNT_HW_CACHE_REFERENCES"
CONFIG="-c ./config/benchmark.json"
OUTPUT="-o ./output.txt"
TASKS="-t test1 -t test2"
sudo ./corun $CONFIG $TASKS $EVENTS $OUTPUT -p 100000