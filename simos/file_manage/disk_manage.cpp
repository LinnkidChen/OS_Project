#include "disk_manage.h"

Addr assign_block() {
    FILE *assed = fopen("DISK/assed.disk", "r+");
    for (int i = 0; i < BLOCK_NUM; i++) {
        if (fgetc(assed) == '0') {
            fseek(assed, -1, SEEK_CUR);
            fwrite("1", sizeof(char), 1, assed);
            fclose(assed);
            FILE *disk = fopen(DISK(i * BLOCK_SIZE).c_str(), "w+");
            fclose(disk);
            return i * BLOCK_SIZE;
        }
    }
    return -1;
}

int recycle_block(Addr addr) {
    if (addr == -1)
        return 0;
    FILE *assed = fopen("DISK/assed.disk", "r+");
    fseek(assed, (addr / BLOCK_SIZE) * sizeof(char), SEEK_SET);
    fwrite("0", sizeof(char), 1, assed);
    fclose(assed);
    FILE *disk = fopen(DISK(addr).c_str(), "w+");
    fclose(disk);
    return 1;
}