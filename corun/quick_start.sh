EVENTS="-e PERF_COUNT_HW_CPU_CYCLES -e PERF_COUNT_HW_INSTRUCTIONS -e PERF_COUNT_HW_CACHE_MISSES -e PERF_COUNT_HW_CACHE_REFERENCES"

eval `cat ../config/spec.cfg`

sudo ./corun $EVENTS -t "$bzip" -t "$mcf"
# sudo ./corun $EVENTS -t "$bzip"
# sudo ./corun $EVENTS -t "$mcf"
# sudo ./corun $EVENTS -t "./echo_str 1" -t "./echo_str 2"
