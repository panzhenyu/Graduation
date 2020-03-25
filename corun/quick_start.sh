EVENTS="-e PERF_COUNT_HW_CPU_CYCLES -e PERF_COUNT_HW_INSTRUCTIONS -e PERF_COUNT_HW_CACHE_MISSES -e PERF_COUNT_HW_CACHE_REFERENCES"

source ../config/spec.cfg

TASKS=
OUTPUT=

for ((i = 1; i <= $#; i++))
do
    if [ ${!i} == "-o" ]
    then
        i=`expr $i + 1`
        OUTPUT="-o ${!i}"
    else
        task_name=${!i}
        TASKS="$TASKS -t ${!task_name}"
    fi
done

echo "TASKS:$TASKS"
echo "OUTPUT:$OUTPUT"

sudo ./corun $EVENTS $TASKS $OUTPUT