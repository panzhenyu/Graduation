cd /home/zhangyi/SH/
./mount_hugepage.sh 
./disable_prefetcher.sh
./close_cpu.sh
./cpufreq.sh

cd /home/zhangyi/4000/predict_program_proformance/profile_performance_variety

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


cd /home/zhangyi/4000/predict_program_proformance/exp_parallel_reporter_benchmark

./profile_strategy
cp reporter_benchmark_out reporter_benchmark1

./profile_strategy
cp reporter_benchmark_out reporter_benchmark2

./profile_strategy
cp reporter_benchmark_out reporter_benchmark3

./profile_strategy
cp reporter_benchmark_out reporter_benchmark4

./profile_strategy
cp reporter_benchmark_out reporter_benchmark5


cd /home/zhangyi/4000/predict_program_proformance/exp_parallel_two_benchmark

./profile_strategy
cp two_benchmark_out two_benchmark1

./profile_strategy
cp two_benchmark_out two_benchmark2

./profile_strategy
cp two_benchmark_out two_benchmark3

./profile_strategy
cp two_benchmark_out two_benchmark4

./profile_strategy
cp two_benchmark_out two_benchmark5

