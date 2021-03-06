/*
 *  (C) 2001 Dave Jones.
 *
 *  Licensed under the terms of the GNU GPL License version 2.
 *
 *  Centaur specific parts.
 */
#include <stdio.h>
#include "../x86info.h"
#include "centaur.h"

static char *centaur_nameptr;
#define add_to_cpuname(x)   centaur_nameptr += snprintf(centaur_nameptr, sizeof(x), "%s", x);

void identify_centaur(struct cpudata *cpu)
{
	char *nameptr;

	centaur_nameptr = nameptr = cpu->name;

	switch (tuple(cpu) & 0xff0) {
		case 0x540:
			add_to_cpuname("Winchip C6");
			//transistors = 5400000;
			//fab_process = "0.35 micron CMOS";
			//die_size = "88 sq.mm";
			//introduction_date = "September 1997";
			//pipeline_stages = 6;
			break;
		case 0x580:
			switch (cpu->stepping) {
			case 0 ... 6:
				add_to_cpuname("Winchip 2");
				break;
			case 7 ... 9:
				add_to_cpuname("Winchip 2A");
				break;
			case 0xA ... 0xF:
				add_to_cpuname("Winchip 2B");
				break;
			}
			break;
		case 0x590:
			add_to_cpuname("Winchip 3");
			break;

		/* Family 6 is when VIA bought out Cyrix & Centaur
		 * This is the CyrixIII family. */
		case 0x660:
			add_to_cpuname("VIA Cyrix 3 (Samuel) [C5A]");
				//pipeline_stages = 12;
				//1.8-2.0V
				//CPGA
				//75mm
				//0.18 Al
				//500-733MHz
				//11.3 mil transistors
				//June 6 2000
				break;
		case 0x670:
				switch (cpu->stepping) {
				case 0 ... 7:
					add_to_cpuname("VIA C3 (Samuel 2) [C5B]");
					//pipeline_stages = 12;
					//1.6V
					//CPGA/EBGA
					//52mm
					//0.15u Al
					//650-800MHz
					//15.2 mil transistors
					//Mar 25 2001
					break;
				case 8 ... 0xf:
					add_to_cpuname("VIA C3 (Ezra) [C5C]");
					//pipeline_stages = 12;
					//1.35V
					//CPGA/EBGA
					//52mm
					//0.15u/0.13u hybrid Al
					//800-1000MHz
					//15.4 mil transistors
					//Sep 11 2001
					break;
				}
				break;
		/* Ezra-T is much like Ezra but reworked to run in Pentium III Tualatin sockets. */
		case 0x680:	add_to_cpuname("VIA C3 (Ezra-T) [C5M/C5N]");
					//pipeline_stages = 12;
					//CPGA/EBGA/uPGA2/uFCPGA
					//900-1200MHz
					//56mm
					//0.15/0.13u hybrid (Cu)
					//15.5 mil transistors
					//C5N=copper interconnectrs
					//2002
					break;
		case 0x690:	add_to_cpuname("VIA C3 (Nehemiah) [C5XL]");
					//pipeline_stages = 16;
					//2 SSE units
					//first C3 to run FPU at full clock speed (previous ran at 50%)
					//1100-1300
					//0.13 (Cu)
					//die_size = "78 sq. mm"; (C5X)
					//die_size = "54 sq. mm"; (C5XL)
					//January 22 2003
					break;
		case 0x6A0:	switch (cpu->stepping) {
					case 0:
					case 8 ... 0xF:
						add_to_cpuname("VIA C3 (Esther) [C7-M]");
						break;
					case 1 ... 7:
						add_to_cpuname("VIA C3 (Ruth) [C7-M]");
						break;
					}
					break;

	// C5P introduced the HW AES
	// C5YL
	// C5X
	// CZA

		default:
			add_to_cpuname("Unknown VIA CPU");
			break;
	}
}


static void decode_centaur_cacheinfo(struct cpudata *cpu)
{
	unsigned int eax, ebx, ecx, edx;

	if (cpu->maxei >= 0x80000005) {
		/* TLB and cache info */
		cpuid(cpu->number, 0x80000005, &eax, &ebx, &ecx, &edx);
		printf("Cache info\n");
		printf(" L1 Instruction cache: %dKB, %d-way associative, %d lines per tag, line size=%d bytes.\n",
			edx >> 24, (edx >> 16) & 0xff, (edx >> 8) & 0xff, edx & 0xff);
		printf(" L1 Data cache: %dKB %d-way associative, %d lines per tag, line size=%d bytes.\n",
			ecx >> 24, (ecx >> 16) & 0xff, (ecx >> 8) & 0xff, ecx & 0xff);
		if (cpu->maxei >= 0x80000006) {
			cpuid (cpu->number, 0x80000006, &eax, &ebx, &ecx, &edx);
			if ((cpu->family==6) && (cpu->model==7 || cpu->model==8))
				/* Work around errata. */
				printf(" L2 (on CPU) cache: %dKB %d-way associative, %d lines per tag, line size=%d bytes.\n",
					ecx >> 24, (ecx >> 16) & 0x0f, (ecx >> 8) & 0x0f, ecx & 0xff);
			else
				printf(" L2 (on CPU) cache: %dKB %d-way associative, %d lines per tag, line size=%d bytes.\n",
					ecx >> 16, (ecx >> 12) & 0x0f, (ecx >> 8) & 0x0f, ecx & 0xff);
		}
		printf("TLB info\n");
		cpuid(cpu->number, 0x80000005, &eax, &ebx, &ecx, &edx);
		printf(" Instruction TLB: %d-way associative. %d entries.\n", (ebx >> 8) & 0xff, ebx & 0xff);
		printf(" Data TLB: %d-way associative. %d entries.\n", ebx >> 24, (ebx >> 16) & 0xff);
	}

	/* check on-chip L2 cache size */
}


void display_centaur_info(struct cpudata *cpu)
{
	if (cpu->maxei == 0)
		return;

	decode_centaur_cacheinfo(cpu);

	if (cpu->family == 6 && show_registers)
		dump_C3_MSR(cpu);
}
