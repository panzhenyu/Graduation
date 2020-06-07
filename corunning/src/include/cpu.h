#ifndef _CPU_H
#define _CPU_H

typedef int cpu_t;

cpu_t core_num();
int malloc_cpu();
void cpu_err_report(int error);
int valid_cpu(cpu_t cpu);
int pin_cpu(cpu_t cpu);

#endif
