#include <string.h>
#include <stdio.h>
#include <stdbool.h>

struct pcb {
    unsigned char pid;
    FILE* fd;
    char* pgtable;
    bool isExit;
};

struct pcb* pcbs;
int processLength;

extern struct pcb* current;
extern char* ptbr;

char* deleteNewLine(char* str);

int freelist[64]; // 0 is empty, 1 is filled

void ku_scheduler(char pid) {
    int count = 0;
    do {
        current = &pcbs[++pid % processLength];
        ptbr = current->pgtable;
    } while (current->isExit && count++ < processLength);

    if (count >= processLength) {
        // printf("all process was exited\n");
        exit(0);
    }
}

void ku_pgfault_handler(char va) {
    int pt_index = (va & 0xF0) >> 4; // va의 하위 4비트 0으로 만들기
    ptbr[pt_index] = (va & 0x0F) + 1; // va의 상위 4비트 0으로 만들기
}


void ku_proc_exit(char pid) {
    printf("proc_exit is called\n");
    pcbs[pid].isExit = true;
    free(pcbs[pid].pgtable);
    fclose(pcbs[pid].fd);
}


void ku_proc_init(int nprocs, char* flist) {
    processLength = nprocs;
    FILE* fileList = fopen(flist, "r");
    pcbs = malloc(sizeof * pcbs * nprocs);
    size_t len = 0;

    for (int i = 0; i < 64; i++) {
        freelist[i] = 0;
    }

    for (int i = 0; i < nprocs; i++) {
        char* processFileName = NULL;
        getline(&processFileName, &len, fileList);
        processFileName = deleteNewLine(processFileName);

        pcbs[i].fd = fopen(processFileName, "r");
        pcbs[i].pid = i;
        pcbs[i].pgtable = malloc(sizeof * pcbs->pgtable * 16);
        printf("%d\n", pcbs[i].pgtable);
        pcbs[i].isExit = false;

        free(processFileName);
    }
    current = &pcbs[0];
    ptbr = current->pgtable;
}

char* deleteNewLine(char* str) {
    if (str[strlen(str) - 1] != '\n') 
        return str;

    char* newStr = (char*)malloc(strlen(str) - 1);

    for (int i = 0; i < strlen(str) - 1; i++) 
        newStr[i] = str[i];

    return newStr;
}