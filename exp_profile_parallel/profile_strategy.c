#define _GNU_SOURCE //这个检验测试宏定义了所有的标准，而且包括了所有库函数中对linux相应的扩展（即任何标准的代码均可以编译）

#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/signalfd.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <time.h>

#ifndef PFM_INC
#include <perfmon/pfmlib_perf_event.h>
#define PFM_INC
#endif

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <pthread.h>
#include <memory.h>
#include <signal.h>
#include <argp.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "benchmark.h"
#include "cache.h"
#include "exp_data.h"
#include "expect.h"
#include "pagemap.h"
#include "perf_counter.h"
#include "redirect.h"
#include "lfsr.h"
#include "tsc.h"
#include "benchmark_list.h"
//调试宏
//#define PRINT_TEST
#define USE_THREAD

//控制宏
#define YES 1
#define NO 0
#define STRATEGY_NO_DETECT 0
#define STRATEGY_A 1
#define STRATEGY_B 2

//bubble的相关控制变量
//***********************************************************************************************
//采样的指令数(初定40亿条)
#define SAMPLE_INSTRUCTIONS 100000000 //十亿
//用于进行时间采样的共享内存大小(这里没有使用，但是必须)
#define RING_BUFFER 8

//并行参数
int parallel_num;
int parallel_benchmark_id[4];
int parallel_start_loc[4];
int parallel_end_loc[4];
//事件链指针(用于访问所有需要测量的事件计数器)
ctr_list_t *events_list[4];
pid_t parallel_pid[4];
int parallel_cpu[4] = {1, 2, 3, 0};
/*与perf事件通知相关*/
//用于接受事件通知的共享内存起始地址
static void *shared_mem[4];
//用于接受通知的pollfd结构
static struct pollfd event_arrive[4];
//实验数据
uint64_t perf[4][4];
int parallel_is_finish = 0;
pthread_t parallel_con_tid[4];

//benchmark一共可处于几种指定状态
static int assign_state_num = 3;
//当前处于的指定状态
static int cur_assign_state = 0;

//当前bubble的大小（由cur_bubble*BUBBLE_SPAN决定）
#define bubble_size (cur_bubble * BUBBLE_SPAN)
//随机访问核
static unsigned long dump[100];

//随机访问集的大小(bullee_size/2)
#define footprint_size (bubble_size / 2)
//随机访问集的初始位置
#define data_chunk gb_bubble_zone

//随机数生成器
#define r (rand % footprint_size)
//流式访问集的大小
#define bw_stream_size (bubble_size / 2)
//流式访问集的初始位置
#define bw_data (data_chunk + footprint_size)
//把流式访问集合改成随机
static unsigned long dump1[100];

//实验几个射击线程的描述符
static pthread_t tid_assign, tid_bubble_random, tid_bubble_stream;

//记录所有数据的数据链表
static profile_data_list_t profile_global = {NULL, NULL};

//记录实验数据的两个目录
static const char *dir_txt = "/exp_data_txt_100";
static const char *dir_pb = "/exp_data_pb";

//*****************************************************************************************************************
//!!!!!!!!!!
uint64_t perf_parallel[4];

char **filename_out;
FILE *ABN_out; //输出文件
pid_t cur_pid;
//!!!!!!!!!!

//全局指示变量
////////////////////////////////////////////////////////////////////////////////////
//指示是否完成
static int is_finish = NO;

//指示当前测试的benchmark
static int cur_benchmark = 0;
//需要测试的benchmark个数
static int benchmark_num = 9;

//一共需要多少个不同大小的bubble(0~16mb，一共17个)(同时也是每一个benchmark需要重启的次数)
static int bubble_num = 1;
//当前处于第几个bubble处（0~16MB）
static int cur_bubble = 0;

//cur_strategy 是主线程和子线程共享访问变量，需要用互斥量进行原子性保护
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

//指示当前在哪个策略下生成数据
static int cur_strategy = STRATEGY_NO_DETECT;
////////////////////////////////////////////////////////////////////////////////////

//用于同步信息的枚举
typedef enum
{
    SYNC_WAITING = 0, //等待
    SYNC_GO,          //执行
    SYNC_ABORT,       //终止
} sync_type_t;        //同步信息（target的状态）

typedef struct
{
    sync_type_t type;
} sync_msg_t;

//固定变量
////////////////////////////////////////////////////////////////////////////////////
//主线程cpu集合
//0123
//4567
#define cpu_main_num 1
//main的cpu蹭一下子指定策略的
static int cpu_mains[cpu_main_num] = {0};

//assign线程cpu集合
#define cpu_assign_num 1
static int cpu_assign_threads[cpu_assign_num] = {1};

//benchmark进程cpu集合
#define cpu_benchmark_num 1
static int cpu_benchmarks[cpu_benchmark_num] = {3};

//bubble随机访问线程cpu集合
#define cpu_bubble_random_num 1
static int cpu_bubble_randoms[cpu_bubble_random_num] = {2};

//bubble流式访问线程cpu集合
#define cpu_bubble_stream_num 1
static int cpu_bubble_streams[cpu_bubble_stream_num] = {1};

//原来的工作目录
char oldpath[PATH_MAX];
;
//主线程睡眠时间(单位：秒)
static int sleep_time = 20;

//序列化的文件
const char *file_name = "no_assign_policy_benchmark_feature_data_v2.txt";

//信号描述数组
extern const char *const sys_siglist[];

//主线程与子线程之间的同步（线程围栏，只有所有进程中的所有线程对该变量操作后才能继续运行不然一直阻塞）
static pthread_barrier_t pbt;
/////////////////////////////////////////////////////////////////////////////////////

//记录数据的数据链
benchmark_data_list_t benchmark_data_list = {NULL, NULL};

//子线程执行的函数(指定策略A)
static void *assign_A(void *arg)
{
    pthread_t tid;
    tid = pthread_self();
    //线程绑定cpu
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    int i;
    for (i = 0; i < cpu_assign_num; i++)
    {
        CPU_SET(cpu_assign_threads[i], &cpu_set);
    }
    EXPECT(pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu_set) == 0);
    //首先设定分离线程，这样在主线程就无需连接，子线程结束时自动清理内核的结构，避免僵尸线程的产生
    pthread_detach(tid);
    //设定线程取消类型，（异步取消），这样主线程发起取消通知时会立即取消线程
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int strategy, ret;
    //pthread_barrier_wait(&pbt);
    //策略指定代码
    while (1)
    {
        assign_low_policy(0);
    }
}

//子线程执行的函数(指定策略B)
static void *assign_B(void *arg)
{
    pthread_t tid;
    tid = pthread_self();
    //线程绑定cpu
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    int i;
    for (i = 0; i < cpu_assign_num; i++)
    {
        CPU_SET(cpu_assign_threads[i], &cpu_set);
    }
    EXPECT(pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu_set) == 0);
    //首先设定分离线程，这样在主线程就无需连接，子线程结束时自动清理内核的结构，避免僵尸线程的产生
    pthread_detach(tid);
    //设定线程取消类型，（异步取消），这样主线程发起取消通知时会立即取消线程
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int strategy, ret;
    //pthread_barrier_wait(&pbt);
    //策略指定代码
    while (1)
    {
        assign_high_policy(0);
    }
}
/*
 * NODE：
 * （1）需要确定随机访问模式和流式访问模式是在一个线程中包含，然后在不同核上跑同时包含两个模式的两个一样的线程
 * ,还是在不同核上跑不同的模式的线程。
 * （2）bubble写完，明天需要写完benchmark启动的控制部分代码。
 */

//解除内存映射（关闭计数器文件描述符时，准备重新启动benchmark时需要先解除之前的共享内存）
static void unmap()
{
    munmap(shared_mem, (1 + RING_BUFFER) * 4096);
}

//绑定cpu
void pin_process(pid_t pid, int cpu, int num)
{
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpu, &cpu_set);
    if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set) == -1)
        printf("error");
}

static void init_cache()
{
    hugepage_alloc();
    //	printf("1\n");
    fill_all_space();
    //	printf("2\n");
    init_cache_zone(0);
    //	printf("3\n");
    init_policy_pointer_chase();
    printf("4\n");
    //添加bubble访问区的初始化
    init_bubble_zone();
    printf("5\n");
}

static void initialize()
{
    //1.保存原来的工作路径
    getcwd(oldpath, PATH_MAX);
    //2.hugepage申请，空间初始化（清零），初始化cache访问区
    init_cache();
    //3.初始化pfm
    event_counter_initialize();
    //4.初始化事件链
    for (int i = 0; i < parallel_num; i++)
        init_events_list(&events_list[i], benchmark_events_id, benchmark_events_name, 4, SAMPLE_INSTRUCTIONS);
    //5.绑定cpu
    // pin_process(0, cpu_mains, cpu_main_num);
}

//设定父进程跟踪子进程
static void
setup_target(void *data /*NULL*/)
{
    EXPECT_ERRNO(ptrace(PTRACE_TRACEME, 0, NULL, NULL) != -1);
}

//父子进程间进行同步方法
static void
sync_send(int fd, const sync_msg_t *msg)
{
    EXPECT_ERRNO(/*发送信息*/ send(fd, msg, sizeof(sync_msg_t), 0) == sizeof(sync_msg_t));
}

//接收fd指向的socket的信息
static void
sync_wait(int fd, sync_msg_t *msg)
{
    EXPECT_ERRNO(recv(fd, msg, sizeof(sync_msg_t), MSG_WAITALL) == sizeof(sync_msg_t));
}

//发送消息
static void
sync_send_simple(int fd, sync_type_t type)
{
    sync_msg_t msg =
        {
            .type = type,
        };

    sync_send(fd, &msg);
}
//接收信息并且验证消息是否为type类型，如果是SYNC_ABORT类型则停止当前进程
static void
sync_wait_simple(int fd, sync_type_t type)
{
    sync_msg_t msg;

    sync_wait(fd, &msg);
    if (msg.type == SYNC_ABORT)
    {
        fprintf(stderr, "Abort signalled while doing child synchronization.\n");
        exit(EXIT_FAILURE);
    }
    EXPECT(msg.type == type);
}

//字符串链接
char *join3(char *s1, char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    if (result == NULL)
        exit(1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

//父子进程同步，并且子进程执行新程序（exec）
static pid_t
ctrs_execvp_cb(ctr_list_t *list,
               void (*child_callback)(void *data), void *callback_data,
               const char *file, char *const argv[])
{
    pid_t pid;
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_SEQPACKET, 0, fds) /*创建匿名socket对，分别由fds[0]和fds[1]对应一个socket*/)
    {
        perror("Failed to setup socket pair");
        return -1;
    }
    pid = fork(); //创建子进程，但是该方法的返回由进程调度子系统决定，如果是子进程返回0，父进程返回大于零的值
    if (pid == -1)
    {
        perror("Fork failed");
        return -1;
    }
    if (/*在子进程中执行pid为0*/ pid == 0) /*子进程执行代码*/
    {
        close(fds[0]);      //为了避免竞争条件，需关闭一个socket的引用（关闭fds[0]）
        if (child_callback) //这里在子进程中设置用它的父进程跟踪它，我猜是为了控制子进程的行为
            child_callback(callback_data);
#ifdef PRINT_TEST
        printf("pid %d send message to father\n", pid);
#endif
        sync_send_simple(fds[1], SYNC_WAITING); //向父进程发送信息（SYNC_WAITING）
        sync_wait_simple(fds[1], SYNC_GO);      //接收父进程的消息，父进程没有往socket写消息就阻塞
        close(fds[1]);
        fprintf(stderr, "Starting target...\n");
        execvp(file, argv);
        fprintf(stderr, "%s: %s", file, strerror(errno)); //还能执行到这说明上面绑定新程序没有成功，target执行失败
        exit(EXIT_FAILURE);
    }
    else /*父进程执行代码*/
    {
        close(fds[1]);                          //为了避免竞争条件，需关闭一个socket的引用（关闭fds[1]）
        sync_wait_simple(fds[0], SYNC_WAITING); //接收子进程的消息，子进程没有发送则阻塞
        // //为target进程绑定性能计数器，配置好需要测量的量
        // if (cur_pid == pid_paralle[1])
        // {
        //     if (attach_ctrs(events_list, pid) == -1)
        //     {
        //         sync_send_simple(fds[0], SYNC_ABORT); //绑定失败则向target进程发送SYNC_ABORT
        //         exit(EXIT_FAILURE);
        //     }
        //     //进行事件接受注册
        //     event_arrive.fd = events_list->head->fd;
        //     event_arrive.events = POLLIN;
        //     shared_mem = mmap(0, (1 + RING_BUFFER) * 4096, PROT_READ, MAP_SHARED, event_arrive.fd, 0);

        // }
        //设置benchmark为实时进程
        struct sched_param param;
        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
        sched_setscheduler(pid, SCHED_FIFO, &param);
        sync_send_simple(fds[0], SYNC_GO); //成功向target发送SYNC_GO
        close(fds[0]);
        return pid;
    }
}

//选择执行的benchmark
static pid_t choice(int cur_idx)
{
    pid_t pid;
    switch (cur_idx)
    {
    case 0:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env508);
        break;
    case 1:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env510);
        break;
    case 2:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env511);
        break;
    case 3:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env519);
        break;
    case 4:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env526);
        break;
    case 5:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env538);
        break;
    case 6:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env544);
        break;
    case 7:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env619);
        break;
    case 8:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env638);
        break;
    case 9:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env644);
        break;
    case 10:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env500);
        break;
    case 11:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env502);
        break;
    case 12:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env505);
        break;
    case 13:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env520);
        break;
    case 14:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env523);
        break;
    case 15:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env525);
        break;
    case 16:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env531);
        break;
    case 17:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env541);
        break;
    case 18:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env557);
        break;
    case 19:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env600);
        break;
    case 20:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env602);
        break;
    case 21:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env605);
        break;
    case 22:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env620);
        break;
    case 23:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env623);
        break;
    case 24:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env625);
        break;
    case 25:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env631);
        break;
    case 26:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env641);
        break;
    case 27:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env657);
        break;

    case 28:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env503);
        break;
    case 29:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env603);
        break;
    case 30:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env548);
        break;
    case 31:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env648);
        break;
    case 32:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env507);
        break;
    case 33:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env607);
        break;
    case 34:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env521);
        break;
    case 35:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env621);
        break;
    case 36:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env527);
        break;
    case 37:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env627);
        break;
    case 38:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env549);
        break;
    case 39:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env649);
        break;
    case 40:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env554);
        break;
    case 41:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env654);
        break;
    case 42:
        chdir(oldpath);
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env628);
        break;
    case 43:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_gromacs);
        //chdir(path_table[11]);
        //pid  = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[11], env_reporter);
        break;
    case 44:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_bzip);
        break;
    case 45:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_mcf);
        break;
    case 46:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_calculix);
        break;
    case 47:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_sphinx3);
        break;
    case 48:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_lbm);
        break;
    case 49:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_libquantum);
        break;
    case 50:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_soplex);
        break;
    case 51:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_gcc);
        break;
    case 52:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_dealII);
        break;
    case 53:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_povray);
        break;
    case 54:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_omnetpp);
        break;
    case 55:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_xalancbmk);
        break;
    case 56:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_hmmer);
        break;
    case 57:
        chdir(path_table[cur_idx]);
        pid = ctrs_execvp_cb(events_list, &setup_target, NULL, bin_table[cur_idx], env_reporter);
        break;
    }
    return pid;
}

//让子进程继续运行
static void
my_ptrace_cont(int pid, int signal)
{
    int temp;
    //继续执行。pid表示被跟踪的子进程，signal为0则忽略引起调试进程中止的信号，若不为0则继续处理信号signal。
    if (temp = ptrace(PTRACE_CONT, pid, NULL, (void *)((long)signal)) == -1)
    {
        perror("Failed to continue child process");
        abort();
    }
}

//主线程睡眠
static void main_sleep()
{
    struct timespec request;
    if (clock_gettime(CLOCK_REALTIME, &request) == -1)
    {
        perror("clock_gettime err\n");
        exit(EXIT_FAILURE);
    }
    request.tv_sec += sleep_time;
    int s;
    s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &request, NULL);
    if (s != 0)
    {
        perror("clock_nanosleep err\n");
        exit(EXIT_FAILURE);
    }
}

//改变指定的替换策略
static void change_strategy()
{
#ifdef USE_THREAD
    int ret;
    ret = pthread_mutex_lock(&mtx);
    if (ret != 0)
    {
        perror("Can not lock mtx in main\n");
        exit(EXIT_FAILURE);
    }
#endif
    if (cur_strategy == STRATEGY_A)
        cur_strategy = STRATEGY_B;
    else
        cur_strategy = STRATEGY_A;
#ifdef USE_THREAD
    ret = pthread_mutex_unlock(&mtx);
    if (ret != 0)
    {
        perror("Can not unlock mtx in main\n");
        exit(EXIT_FAILURE);
    }
#endif
}

//记录数据（策略a时需要创建新元素，策略b时只需要在数据链尾部的元素添加数据即可）
static void record_data()
{
    if (cur_strategy == STRATEGY_A)
    {
        benchmark_feature_t *bft;
        bft = create_Item(benchmark_name[cur_benchmark]);
        record_data_strategy_a(bft, events_list);
        add_Item(&benchmark_data_list, bft);
    }
    else
    {
        record_data_strategy_b(benchmark_data_list.tail, events_list);
    }
}

void print_data(uint64_t *data, ctr_list_t *list)
{
    int len;
    len = ctrs_len(list);
    int idx_event;
    ctr_t *cur;
    ssize_t ret;
    for (idx_event = 0, cur = list->head; cur; cur = cur->next, idx_event++)
    {
        ret = read(cur->fd, &data[idx_event], sizeof(uint64_t));
        if (ret == 0)
        {
            perror("got EOF while reading counter\n");
            exit(EXIT_FAILURE);
        }
        else if (ret != sizeof(uint64_t))
            fprintf(stderr,
                    "Warning: Got short read. Expected %i bytes, "
                    "but got %i bytes.\n",
                    sizeof(uint64_t), ret);
    }
}

//子线程执行的函数
static void *threadFunc_con_parallel(void *arg)
{
    int i = *(int *)arg;
    printf("线程 %d \n", i);
    // 5.等待接受事件到达通知（这里是4依条指令）
    for (int loc = parallel_start_loc[i]; loc < parallel_end_loc[i]; loc++)
    {
        if (poll(&event_arrive[i], 1, -1) < 0)
        {
            perror("poll() in SIGTRAP fail!\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("线程 %d POLL IN\n", i);
    pthread_barrier_wait(&pbt); /* 获取屏障锁 */
}

static void handle_child_stopped(pthread_t tid, pid_t pid, pid_t pid_policy, int signal, int *flag)
{
    switch (signal)
    {
    case SIGTRAP:
        printf("所有进程都已经启动！！\n");
        //把所有程序先跑到起点暂停
        for (int i = 0; i < parallel_num; i++)
        {
            disable_all_event_counter(events_list[i]);
            reset_all_event_counter(events_list[i]);
            enable_all_event_counter(events_list[i]);
            // kill(parallel_pid[i], SIGCONT);
            ptrace(PTRACE_CONT, parallel_pid[i], NULL, 0);
            for (int loc = 0; loc < parallel_start_loc[i]; loc++)
            {
                if (poll(&event_arrive[i], 1, -1) < 0)
                {
                    perror("poll() in SIGTRAP fail!\n");
                    exit(EXIT_FAILURE);
                }
            }
            ptrace(PTRACE_DETACH, parallel_pid[i]);
            kill(parallel_pid[i], SIGSTOP);
            printf("进程%d 到达测试位置\n", i);
        }
        /* 初始化屏障 */
        pthread_barrier_init(&pbt, NULL, 2); //2-》只需要到一个，其余全去死
        // //每个并行程序一个控制线程
        int t[100];
        for (int i = 0; i < parallel_num; i++)
        {
            //清空计数器
            disable_all_event_counter(events_list[i]);
            reset_all_event_counter(events_list[i]);
            enable_all_event_counter(events_list[i]);
            //!!!这里直接传入i而不是t会有问题，此处传入是地址，在线程内看到i会改变mmp
            t[i] = i;
            EXPECT(pthread_create(&parallel_con_tid[t[i]], NULL, threadFunc_con_parallel, &t[i]) == 0);
        }
        //让程序开始执行
        for (int i = 0; i < parallel_num; i++)
        {
            ptrace(PTRACE_CONT, parallel_pid[i], NULL, 0);
            // kill(parallel_pid[i], SIGCONT);
        }
        pthread_barrier_wait(&pbt); /* 获取屏障锁 */ //过了这说明已经有一个线程执行完毕，程序跑完指定程序段
        printf("有一个进程已经跑完到位！！！\n");
        for (int i = 0; i < parallel_num; i++)
            disable_all_event_counter(events_list[i]);
        for (int i = 0; i < parallel_num; i++)
        {
            //关控制线程
            pthread_cancel(parallel_con_tid[i]);
            print_data(perf[i], events_list[i]); //ca参数!!!!
            close_ctrs(events_list[i]);
            //向benchmark发送SIGKILL
            kill(parallel_pid[i], SIGKILL);
        }
        printf("结束多个进程！！！！！！！！！\n");
        *flag = 1;
        fprintf(ABN_out, "PARALLEL_NUM:%d \n",parallel_num);
        for (int i = 0; i < parallel_num; i++)
        {
            fprintf(ABN_out, "%s ", benchmark_name[parallel_benchmark_id[i]]);
            for (int j = 0; j < 4; j++)
                fprintf(ABN_out, "%lld ", perf[i][j]);
            fprintf(ABN_out, "%f \n", (double)perf[i][0] / (double)perf[i][1]);
        }
        break;
    case SIGSTOP:
        break;
    default:
        // *flag = 1;
        fprintf(stderr, "Unexpected signal (%s) receive by child process %d\n", strsignal(signal), pid);
        // my_ptrace_cont(pid, signal);
        break;
    }
}

static void handle_child_sig_status(int *flag, pthread_t tid, pid_t pid, pid_t pid_policy, pid_t sig_pid, int status)
{
#ifdef PRINT_TEST
    printf("benchmsrk pis is %d------sig_pid id %d\n", pid, sig_pid);
#endif
    assert(pid == sig_pid);
    if (WIFEXITED(status)) //子线程执行的函数(指定策略A)
    {

#ifdef PRINT_TEST
        printf("Child process exit normally , main thread sleep to long!\n");
#endif
        //处理子进程正常退出
    }
    else if (WIFSIGNALED(status))
    {
        //处理子进程因被信号杀死而终止
#ifdef PRINT_TEST
        printf("Child proescc %s has been kill by %s\n", benchmark_name[cur_benchmark], strsignal(WTERMSIG(status)));
#endif
            
        //这里是整个实验逻辑的控制中心，小心编写，考虑周全了再写
        for (int i = 0; i < parallel_num; i++)
        {
            fprintf(ABN_out, "%s ", benchmark_name[parallel_benchmark_id[i]]);
            for (int j = 0; j < 4; j++)
                fprintf(ABN_out, "%lld ", perf[i][j]);
            fprintf(ABN_out, "%f \n", (double)perf[i][0] / (double)perf[i][1]);
        }

        //控制
        *flag = 1;
        is_finish = YES;
    }
    else if (WIFSTOPPED(status))
    {
        //处理子进程因信号而停止
#ifdef PRINT_TEST
        printf("Child process %s has been stopped by %s\n", benchmark_name[cur_benchmark], strsignal(WSTOPSIG(status)));
#endif
        if (pid == parallel_pid[parallel_num - 1])
            handle_child_stopped(tid, pid, pid_policy, WSTOPSIG(status), flag);
        else
        {
            *flag = 1;
        }
    }
    else
    {
        //处理其他情况
        EXPECT(0);
    }
}

static void handle_sig_from_childprocess(int *flag, int sfd, pthread_t tid, pid_t pid, pid_t pid_policy)
{
    struct signalfd_siginfo fdsi;
    EXPECT(read(sfd, &fdsi, sizeof(fdsi)) == sizeof(fdsi));
    if (fdsi.ssi_pid == pid)
    {
        switch (fdsi.ssi_signo)
        {
        case SIGINT:
            //程序出问题了按下CTRL+C强行终止整个系统的运行
            printf("program make some problem, typed CTRL+C to terminate!\n");
            kill(pid, SIGKILL);
            EXPECT(0);
        case SIGCHLD:
        {
            //父进程收到SIGCHLD,子进程终止，获取状态进行处理
            int status;
            EXPECT_ERRNO(waitpid(fdsi.ssi_pid, &status, WNOHANG) > 0);
            handle_child_sig_status(flag, tid, pid, pid_policy, fdsi.ssi_pid, status);
        }
        break;
        }
    }
    else
    {
#ifdef PRINT_TEST
        printf("fdsi.ssi_pid is %d----pid_policy is %d\n", fdsi.ssi_pid, pid_policy);
#endif
        EXPECT_ERRNO(waitpid(fdsi.ssi_pid, NULL, WNOHANG) > 0);
    }
}

//创建SIGINT，SIGCHLD的信号接收文件描述符（为了通过读文件的方式来获取信号，编写位置可控的信号处理逻辑）
static int create_sig_fd()
{
    int sfd;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGINT);
    EXPECT_ERRNO(sigprocmask(SIG_BLOCK, &mask, NULL) != -1);
    EXPECT_ERRNO((sfd = signalfd(-1, &mask, 0)) != -1);
    return sfd;
}

/*
 * 经验教训(1）：
 *内核再给某个进程发送信号时是有开销的，不是马上送达的，在内核发送信号到信号到达之间，如果对该进程进行wait()系列操作获取
 *其状态信息，此时进程的状态是未知的或者是上一次的状态，所以这样写程序或导致逻辑混乱。
 *解决方法：
 *千万不要不管SIGCHLD信号而直接对子进程进行wait系列操作，这样往往会使程序逻辑混乱的。
 *阻塞掉感兴趣的信号(多数就是指SIGCHLD)(让信号的默认处理不再生效)，然后通过某种方式捕获感兴趣的信号
 *（poll，sigsuspend，sigwaitinfo等），在信号处理函数
 *中对导致进程发生改变的信号的情况进行一样。
 *
 * 经验教训(2):
 * 如果子进程设置了让父进程跟踪他，那么父进程必须wait来获得子进程的状态，因为在子进程exec执行新程序时内核会给子进程发送SIGTRAP使
 * 子进程停止，此时子进程会给父进程回复SIGCHLD信号来告知父进程，自己处于被跟踪的停止态，父进程需要通过wait来读取子进程的状态，然后
 * 就可以对子进程进行跟踪和控制了，如果不理会子进程的SIGTRAP停止态，父进程无法跟踪和控制子进程的执行
 */

static void wait_subprocess(int sfd, pthread_t tid, pid_t pid, pid_t pid_policy)
{
    int status;
    int flag = 0;
    //sfd = create_sig_fd();
    struct pollfd pfd[] = {{sfd, POLLIN, 0}};
    while (!flag)
    {
        if (poll(pfd, sizeof(pfd) / sizeof(*pfd), -1) != -1)
        {
            if (pfd[0].revents & POLLIN)
            {
                handle_sig_from_childprocess(&flag, sfd, tid, pid, pid_policy);
            }
        }
        else if (errno != EINTR)
            EXPECT_ERRNO(0);
    }
}

//子线程执行的函数
static void *threadFunc(void *arg)
{

    fprintf(stderr, "threadFunc open\n");
    pthread_t tid;
    tid = pthread_self();
    //线程绑定cpu
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpu_assign_threads[0], &cpu_set);
    EXPECT(pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu_set) == 0);
    int strategy, ret;
    //策略指定代码
    while (1)
    {
        pthread_mutex_lock(&mtx);
        strategy = cur_strategy;
        pthread_mutex_unlock(&mtx);
        //A high b low
        if (strategy == STRATEGY_A)
        {
            assign_high_policy(0);
        }
        else if (strategy == STRATEGY_B)
        {
            assign_low_policy(0);
        }
        else if (strategy == STRATEGY_NO_DETECT)
        {
            //            assign_follow_policy(0);
        }
    }
}

static void do_start()
{

    pid_t pid, pid_policy;
    pthread_t tid;
    //为主线程阻塞掉SIGCHLD，设置关于SIGCHLD的文件描述符，通过文件描述符读取SIGCHLD的信息，编写信号处理逻辑
    int sfd;
    sfd = create_sig_fd();
    //    指定策略
    // EXPECT(pthread_create(&tid, NULL, threadFunc, NULL) == 0);
    //    for(int i=0; i<3; i++)

    cur_strategy = STRATEGY_NO_DETECT;
    for (int i = 0; i < parallel_num; i++)
    {
        //!!!!!benchamrk
        parallel_pid[i] = choice(parallel_benchmark_id[i]); //2mcf
        cur_pid = parallel_pid[i];
        pin_process(parallel_pid[i], parallel_cpu[i] /*cpu0*/, cpu_benchmark_num);
        //绑定事件
        if (attach_ctrs(events_list[i], parallel_pid[i]) == -1)
        {
            exit(EXIT_FAILURE);
        }
        //进行事件接受注册
        event_arrive[i].fd = events_list[i]->head->fd;
        event_arrive[i].events = POLLIN;
        shared_mem[i] = mmap(0, (1 + RING_BUFFER) * 4096, PROT_READ, MAP_SHARED, event_arrive[i].fd, 0);
        wait_subprocess(sfd, tid, parallel_pid[i], pid_policy /*这参数没用*/);
    }
    close(sfd);
}

//析构方法
static void finitialize(ctr_list_t *event_list, benchmark_data_list_t *data_list)
{
    pfm_terminate();
    ctr_t *temp, *cur;
    for (cur = event_list->head; cur; cur = temp)
    {
        temp = cur->next;
        free(cur);
    }
    free(event_list);

    benchmark_feature_t *temp1, *cur1;
    for (cur1 = data_list->head; cur1; cur1 = temp1)
    {
        temp1 = cur1->next;
        free(cur1->strategy_a);
        free(cur1->strategy_b);
        free(cur1);
    }
}
//主线程
int main(int argc, char *argv[])
{
    ABN_out = fopen("PARALLEL_OUT", "a+");
    parallel_num = atoi(argv[1]); //几个并行
    for (int i = 0; i < parallel_num; i++)
    {
        parallel_benchmark_id[i] = atoi(argv[i * 3 + 2]);
        parallel_start_loc[i] = atoi(argv[i * 3 + 3]);
        parallel_end_loc[i] = atoi(argv[i * 3 + 4]);
    }
    initialize();
    do_start();
    for (int i = 0; i < parallel_num; i++)
        finitialize(events_list[i], &benchmark_data_list);
    fclose(ABN_out);
}
