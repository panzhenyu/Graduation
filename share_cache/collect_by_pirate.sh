eval `cat ../config/pirate.cfg`

TMPFILE_HOME=/tmp
PYTHON=python2
RM=rm
PIRATE_ARGS="-c 1 -C 0 --sample-period=100000 -e PERF_COUNT_HW_CACHE_MISSES -e PERF_COUNT_HW_CACHE_REFERENCES"
OUTPUT=
COMMAND=

if [ $# -lt 2 ]
then
    echo usage: collect_by_pirate.sh OUTPUT_FILE COMMAND
    exit -1
fi

# get arguments

i=1
for arg in "$@"
do
    if [ $i -eq 1 ]
    then
        OUTPUT=$OUTPUT$arg
    else
        COMMAND=$COMMAND"$arg "
    fi
    i=`expr $i + 1`
done
echo "-->OUTPUT = $OUTPUT"
echo "-->COMMAND = $COMMAND"

# run pirate and get data file

echo "-->Enter directory ${PIRATE_HOME}"
cd ${PIRATE_HOME}
echo "-->Running pirate"
sudo ./${PIRATE} ${PIRATE_ARGS} -o ${TMPFILE_HOME}/result.pb -- ${COMMAND}
echo "-->Leave directory ${PIRATE_HOME}"
cd -

# anti-serialize the data file

echo "-->Enter directory ${PIRATE_HOME}/python"
cd ${PIRATE_HOME}/python
echo "-->Dump to txt file"
${PYTHON} ${DUMPTXT} ${TMPFILE_HOME}/result.pb > "${OLDPWD}/${OUTPUT}.txt"
echo "-->Dump to csv file"
${PYTHON} ${DUMPCSV} ${TMPFILE_HOME}/result.pb > "${OLDPWD}/${OUTPUT}.csv"
echo "-->Leave directory ${PIRATE_HOME}/python"
cd -


# clean temp file

echo "-->Clean temp file ${TMPFILE_HOME}/result.pb"
sudo ${RM} ${TMPFILE_HOME}/result.pb

