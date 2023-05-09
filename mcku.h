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
int numOfProcess = 0;

extern struct pcb* current;
extern char* ptbr;

char* deleteNewLine(char* str);

int freelist[64]; // 0 is empty, 1 is filled

void ku_scheduler(char pid) {
    int count = 0;
    do {
        current = &pcbs[++pid % numOfProcess];
        ptbr = current->pgtable;
    } while (current->isExit && count++ < numOfProcess);

    if (count >= numOfProcess) {
        printf("all process was exited\n");
        current = NULL;
    }
}

void ku_pgfault_handler(char va) {
    unsigned char vpn = (va & 0xF0) >> 4; // va의 하위 4비트 0으로 만들고 오른쪽으로 4칸 옮기기
    int pfn = 0;
    for (int i = 0; i < 64; i++) {
        if (freelist[i] == -1) {
            freelist[i] = vpn;
            pfn = i;
            // printf("pfn set %d -> %d\n", vpn, pfn);
            current->pgtable[vpn] = (pfn << 2) + 0x01;
            return;
        }
    }
    current->pgtable[vpn] = 0;
    return;
}


void ku_proc_exit(char pid) {
    pcbs[pid].isExit = true;

    for (int i = 0; i < 16; i++) {
        if (pcbs[pid].pgtable[i] != 0) {
            // pte가 0이 아님, 즉 페이지를 사용했음
            int usedpfn = ((pcbs[pid].pgtable[i] & 0xFC) >> 2);
            freelist[usedpfn] = -1;
            // printf("사용했던 PFN : %d\n", usedpfn);
        }
    }
    free(pcbs[pid].pgtable);
    fclose(pcbs[pid].fd);
    printf("proc_exit 호출 : %d번 프로세스 종료\n", pid);
}


void ku_proc_init(int nprocs, char* flist) {
    numOfProcess = nprocs;
    FILE* fileList = fopen(flist, "r");
    pcbs = malloc(sizeof * pcbs * nprocs);
    size_t len = 0;

    for (int i = 0; i < 64; i++) {
        freelist[i] = -1;
    }

    char filename[20];
    for (int i = 0; i < nprocs; i++) {
        fscanf(fileList, "%s", filename);
        pcbs[i].fd = fopen(filename, "r");
        pcbs[i].pid = i;
        pcbs[i].pgtable = malloc(sizeof * pcbs->pgtable * 16);
        for (int j = 0; j < 16; j++) {
            pcbs[i].pgtable[j] = 0;
        }
        pcbs[i].isExit = false;
    }
    fclose(fileList);
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