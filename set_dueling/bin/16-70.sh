echo "clean img and result!"
./clean_result.sh
for i in {16..70};do
	./run.sh $i
done
echo "16 - 70 done!"
