EVENTS="-e PERF_COUNT_HW_CPU_CYCLES -e PERF_COUNT_HW_INSTRUCTIONS -e PERF_COUNT_HW_CACHE_MISSES -e PERF_COUNT_HW_CACHE_REFERENCES"

SPEC_HOME=~/Desktop/spec-cpu2006/benchspec/CPU2006
bzip="$SPEC_HOME/401.bzip2/exe/bzip2_base.amd64-m64-gcc42-nn $SPEC_HOME/401.bzip2/data/test/input/dryer.jpg"
mcf="$SPEC_HOME/429.mcf/exe/mcf_base.amd64-m64-gcc42-nn $SPEC_HOME/429.mcf/data/test/input/inp.in"

# sudo ./corun $EVENTS -t "./echo_str 1" -t "./echo_str 2"
sudo ./corun $EVENTS -t "$bzip" -t "$mcf"
sudo ./corun $EVENTS -t "$bzip"
sudo ./corun $EVENTS -t "$mcf"
