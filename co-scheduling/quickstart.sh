if [ $# != 2 ]
then
	echo "usage: ./quickstart.sh taskNum processorNum"
fi

python3 AutoTest.py $1 $2
python3 MergeOutput.py output/mergeOut.json
python3 Json2Metrics.py output/mergeOut.json output/metric.json
python3 Metric2Excel.py output/metric.json output/metric.xls
