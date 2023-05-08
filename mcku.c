#include <stdio.h>
#include <stdlib.h>
#include "mcku.h"

#define SCHED	0
#define PGFAULT	1
#define EXIT	2
#define TSLICE	5

struct pcb *current = 0;
char *ptbr = 0;
struct handlers{
       void (*sched)(char);
       void (*pgfault)(char);
       void (*exit)(char);
}kuos;

void ku_reg_handler(int flag, void (*func)(char)){
	switch(flag){
		case SCHED:
			kuos.sched = func;
			break;
		case PGFAULT:
			kuos.pgfault = func;
			break;
		case EXIT:
			kuos.exit = func;
			break;
		default:
			exit(0);
	}
}

int ku_traverse(char va){
	int pt_index, pa;
	char *pte;

	pt_index = (va & 0xF0) >> 4;
	// va & 0xF0 : va의 하위 4비트를 0으로 만들기
	// >> 4 : va의 상위 4비트 값을 오른쪽으로 이동
	pte = ptbr + pt_index; 

	// if va가 225일때, (11100001일 때)
	// 00000000 + 00001110 = 00001110이 pte가 된다. 

	// printf("pt_index: %d, pte: %s", pt_index, pte);

	if(!*pte)
		return -1;
	pa = ((*pte & 0xFC) << 2) + (va & 0x0F);
	// (*pte & 0xFC) : pte의 하위 2비트를 0으로 만들기
	//  << 2 : 왼쪽으로 2비트씩 시프트, 결과적으로 pte의 값을 2비트 왼쪽으로 이동
	// (va & 0x0F) : va의 하위 4비트 값 더하기

	// 00001100 -> 00110000 -> 001100001
	// 결과적으로 va 225는 pa 49로 변환

	return pa;
}


void ku_os_init(void){
	ku_reg_handler(SCHED, ku_scheduler);
	ku_reg_handler(PGFAULT, ku_pgfault_handler);
	ku_reg_handler(EXIT, ku_proc_exit);
}


void ku_run_cpu(void){
	unsigned char va;
    char sorf;
	int addr, pa, i;

	do{
		if(!current)
			exit(0);

		for( i=0 ; i<TSLICE ; i++){
			if(fscanf(current->fd, "%d", &addr) == EOF){
				kuos.exit(current->pid);
				break;
			}
			va = addr & 0xFF;
			pa = ku_traverse(va);

			if(pa >= 0){
				sorf = 'S';
			}
			else {
				/* Generates a page fault */
				kuos.pgfault(va);
				pa = ku_traverse(va);

				if(pa < 0){
					/* No free page frames */
					kuos.exit(current->pid);
					break;
				}
				sorf = 'F';
			}

			printf("%d: %d -> %d (%c)\n", current->pid, va, pa, sorf);
			// pa값이 offset으로 출력이 되는듯
		}

		kuos.sched(current->pid);
	}while(1);
}

int main(int argc, char *argv[]){
	// argv[0] ?
	// argv[1]는 argc
	// argv[2]는 txt파일명 (proc.txt)
	/* System initialization */
	ku_os_init();

	/* Per-process initialization */
	ku_proc_init(atoi(argv[1]), argv[2]);
	/* Process execution */
	ku_run_cpu();

	return 0;
}