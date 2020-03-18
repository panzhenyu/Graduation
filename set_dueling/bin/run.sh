SRC_DIR=../src

sudo ./set_dueling $1
python3 ${SRC_DIR}/python/plot.py $1
