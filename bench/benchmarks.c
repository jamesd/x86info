#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdio.h>
#include "../x86info.h"
#include "bench.h"

#ifdef __linux__
#include <asm/unistd.h>
#endif

#include <sys/types.h>
#include <unistd.h>

void show_benchmarks(struct cpudata *cpu __attribute__((unused)))
{
	int tmp = 0;

#ifdef __linux__
	TIME(asm volatile("int $0x80" :"=a" (tmp) :"0" (__NR_getppid)), "int 0x80");
#endif
	TIME(asm volatile("cpuid": : :"ax", "dx", "cx", "bx"), "cpuid");

	//TIME(asm volatile("addl $1,0(%esp)"), "addl");
	//TIME(asm volatile("lock ; addl $1,0(%esp)"), "locked add");

	TIME(asm volatile("bswap %0" : "=r" (tmp) : "0" (tmp)), "bswap");
}
