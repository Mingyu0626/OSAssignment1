#include <string.h>
#include <stdbool.h>

struct pcb{
	char pid;
	FILE *fd;
	char *pgtable;
	/* Add more fields if needed */
	bool isExit;
};

//PCB Struct Arrays
struct pcb* pcbs;

// Num of Processes
int processLength;

// to use from mcku.c
extern struct pcb* current;

// pid를 1씩 올려서 current에 넣어주는 스케줄링
void ku_scheduler(char pid) {
	int count = 0;

	do {
		current = &pcbs[++pid % processLength];
	} while (current->isExit && count++ < processLength);

	if (count == processLength)
		current = NULL;
}


void ku_pgfault_handler(char pid){
	// page fault가 발생한 virtual address를 인자로 받는다.
	// 변수명은 pid이지만, 사실상 virtual address이다.
	// 하지만 실제 어드레스를 다루는 것은 아니고 
	// 어드레스를 흉내내는 값을 다루고 있는 것이다.
	/* Your code here */
}


void ku_proc_exit(char pid){
	/* Your code here */
	pcbs[pid].isExit = true;
}


void ku_proc_init(int nprocs, char *flist){
	/* Your code here */
	processLength = nprocs;
	FILE* fl = fopen(flist, "r");
	pcbs = malloc(sizeof * pcbs * nprocs);
	size_t len = 0;
	for (int i = 0; i < nprocs; i++) {
		char* processFileName = NULL;
		getline(&processFileName, &len, fl);
		if (i == nprocs - 1) processFileName = appendNewLine(processFileName);


		pcbs[i].fd = fopen(processFileName, "r");
		pcbs[i].pid = i;
		pcbs[i].pgtable = malloc(sizeof * pcbs->pgtable * 16);
		// 공간 할당만 해두고 값은 안넣어줘도 되나..?
		pcbs[i].isExit = false;
		
	}

	char* appendNewLine(char* str) {
		char* newstr = malloc(strlen(str) + 2);
		strcpy(newstr, str);
		strcat(newstr, "\n");
		return newstr;
	}

}
