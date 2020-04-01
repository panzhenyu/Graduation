cd /home/zhangyi/SH/
./mount_hugepage.sh 
./disable_prefetcher.sh
./close_cpu.sh
./cpufreq.sh

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

