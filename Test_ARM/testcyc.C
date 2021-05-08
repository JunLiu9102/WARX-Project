#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <testarmcycle.h>

 
int main()
{
 
  unsigned int start,end;
  unsigned int result[3]; 
  unsigned long long timer1,timer2;
  //uint64_t timer1,time2;
  u32 pmcr_el;
 
  pmcr_el = armv8pmu_pmcr_read();
  printf("\nPMCR_EL0 Register:%lX ", pmcr_el);
  timer1 = arch_counter_get_cntpct();
  //printf("\nCPU Cycle Count:0x%llX ",timer);
  printf("\nCPU Cycle Count:%llu",timer1);  
  sleep(5);
  timer2 = arch_counter_get_cntpct();
  //printf("\nCPU Cycle Count:0x%llX \n",timer);
  printf("\nCPU Cycle Count:%llu",timer2);
  printf("\n cost %llu cycles",timer2-timer1);
  asm volatile("mrs %0, PMOVSCLR_EL0" : "=r"(pmcr_el));
  printf(" Register PMOVSCLR_EL0:0x%lX \n", pmcr_el); 
   
  asm volatile("mrs %0, pmuserenr_el0" : "=r"(pmcr_el));
  printf(" Register pmuserenr_el0:0x%lX \n", pmcr_el);
 
  asm volatile("mrs %0, PMCNTENSET_EL0" : "=r"(pmcr_el));
  printf(" Register PMCNTENSET_EL0:0x%lX \n", pmcr_el);
 
  asm volatile("mrs %0, PMCCFILTR_EL0" : "=r"(pmcr_el));
  printf(" Register PMCCFILTR_EL0:0x%lX \n", pmcr_el);
 
  asm volatile("mrs %0, PMCNTENCLR_EL0" : "=r"(pmcr_el));
  printf(" Register PMCNTENCLR_EL0:0x%lX \n", pmcr_el);
 
  asm volatile("mrs %0, PMOVSSET_EL0" : "=r"(pmcr_el));
  printf(" Register PMOVSSET_EL0:0x%lX \n", pmcr_el);
 
  return 0;
 
}