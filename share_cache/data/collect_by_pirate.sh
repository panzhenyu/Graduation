PIRATE_HOME=/home/panda/Desktop/pirate
TMPFILE_HOME=/tmp

PYTHON=python2
RM=rm

PIRATE=perfpirate
DUMPTXT=pirate_dump.py
DUMPCSV=pirate2csv.py

PIRATE_ARGS="-c 1 -C 0 --sample-period=100000 -e BRANCH_INSTRUCTIONS_RETIRED -e MISPREDICTED_BRANCH_RETIRED"
OUTPUT=
COMMAND=

# get arguments

i=1
for arg in "$@"
do
    if [ $i == 1 ]
    then
        OUTPUT+=$arg
    else
        COMMAND+="$arg "
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

