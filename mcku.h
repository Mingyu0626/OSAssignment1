#include <string.h>
#include <stdbool.h>

char* deleteNewLine(char* str);

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

void ku_scheduler(char pid) {
    int count = 0;

    do {
        current = &pcbs[++pid % processLength];
        ptbr = current->pgtable;
    } while (current->isExit && count++ < processLength);

    if (count >= processLength) {
        exit(0);
    }
}

void ku_pgfault_handler(char va) {
    int pt_index = (va & 0xF0) >> 4;
    ptbr[pt_index] = 1;
}


void ku_proc_exit(char pid) {
    pcbs[pid].isExit = true;
}


void ku_proc_init(int nprocs, char* flist) {
    processLength = nprocs;
    FILE* fileList = fopen(flist, "r");
    pcbs = malloc(sizeof * pcbs * nprocs);
    size_t len = 0;

    for (int i = 0; i < nprocs; i++) {
        char* processFileName = NULL;
        getline(&processFileName, &len, fileList);
        processFileName = deleteNewLine(processFileName);

        pcbs[i].fd = fopen(processFileName, "r");
        pcbs[i].pid = i;
        pcbs[i].pgtable = malloc(sizeof * pcbs->pgtable * 16);
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