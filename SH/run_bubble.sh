cd /home/zhangyi/SH/
./mount_hugepage.sh 
./disable_prefetcher.sh
./close_cpu.sh
./cpufreq.sh

cd /home/zhangyi/4000/predict_program_proformance/profile_performance_variety_3part/

./profile_strategy
cp -R result/ result1

./profile_strategy
cp -R result/ result2

./profile_strategy
cp -R result/ result3

./profile_strategy
cp -R result/ result4

./profile_strategy
cp -R result/ result5

