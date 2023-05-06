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

// pid�� 1�� �÷��� current�� �־��ִ� �����ٸ�
void ku_scheduler(char pid) {
	int count = 0;

	do {
		current = &pcbs[++pid % processLength];
	} while (current->isExit && count++ < processLength);

	if (count == processLength)
		current = NULL;
}


void ku_pgfault_handler(char pid){
	// page fault�� �߻��� virtual address�� ���ڷ� �޴´�.
	// �������� pid������, ��ǻ� virtual address�̴�.
	// ������ ���� ��巹���� �ٷ�� ���� �ƴϰ� 
	// ��巹���� �䳻���� ���� �ٷ�� �ִ� ���̴�.
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
		// ���� �Ҵ縸 �صΰ� ���� �ȳ־��൵ �ǳ�..?
		pcbs[i].isExit = false;
		
	}

	char* appendNewLine(char* str) {
		char* newstr = malloc(strlen(str) + 2);
		strcpy(newstr, str);
		strcat(newstr, "\n");
		return newstr;
	}

}
