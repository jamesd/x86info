#ifndef _CENTAUR_H
#define _CENTAUR_H

struct cpudata;

void dump_C3_MSR (struct cpudata *cpu);
void decode_longhaul(struct cpudata *cpu);
void decode_powersaver(struct cpudata *cpu);
#endif /* _CENTAUR_H */
