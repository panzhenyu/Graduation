source ../config/spec.cfg

EVENTS="-e PERF_COUNT_HW_CPU_CYCLES -e PERF_COUNT_HW_INSTRUCTIONS -e PERF_COUNT_HW_CACHE_MISSES -e PERF_COUNT_HW_CACHE_REFERENCES"
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
        cmd=${!task_name}
        TASKS="$TASKS -t \"$cmd\""
    fi
done

echo "TASKS:$TASKS"
echo "OUTPUT:$OUTPUT"

finalCmd="sudo ./corun ${EVENTS} ${TASKS} ${OUTPUT}"
eval $finalCmd
