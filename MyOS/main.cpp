#include "file_manage/file_manage.h"

#include <iostream>

extern std::vector<o_file *> filelist;

int main() {
    // printf("%d\n",disk_searchline(1000, "hom"));
    // recycle_block(4000);
    // printf("%d\n",assign_block());
    char name[5] = {'t', 'e', 's', 't', '\0'};
    printf("hello\n");
    Handle h = file_open(name, F_CREATE | F_WRITE | F_READ);
    printf("debug-main: h %d\n", h);
    printf("debug-main: size %zu\n", filelist.size());
    printf("debug-main: name %s\n", filelist[h]->fcb->filename.c_str());
    for (int i = 0; i < 4; i++)
        printf("debug-main %d\n", filelist[h]->fcb->direct_index(i));
    file_write(h, 4, name);

    file_seek(h, 0, FILE_SEEK_SET);

    char buffer[512]{};
    file_read(h, 4, buffer);
    std::cout << "File read: " << buffer << '\n';
    file_close(h);
    return 0;
}
