#include "../../macro.h"

int main(int argc, char *argv[]) {
    if (argc != 2)
        raler(1, "Usage: %s <file>", argv[0]);

    int fd;
    char *tab;
    CHK(fd = open(argv[1], O_RDONLY));

    struct stat st;
    CHK(fstat(fd, &st));

    tab = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (tab == MAP_FAILED)
        raler(1, "mmap");

    CHK(close(fd));

    char *buff = tab + st.st_size - 1;
    int cpt = 0;
    while (buff > tab && *buff != '\n') {
        buff--;
        cpt++;
    }

    if (buff >= tab) {
        if (*buff == '\n') {
            buff++;
        }
        CHK(write(1, buff, cpt));
    }

    CHK(munmap(tab, st.st_size));
}
// TEST !!