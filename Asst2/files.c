#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void list(const char *file, int in)
{
    DIR *d;
    struct dirent *dir;
    if (!(d = opendir(file)))
        return;
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_DIR) {
            char place[1024];
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;
            snprintf(place, sizeof(place), "%s/%s", file, dir->d_name);
            printf("%*s-> %s\n", in, "", dir->d_name);
            list(place, in + 2);
        } else {
            printf("%*s- %s\n", in, "", dir->d_name);
        }
    }
    closedir(d);
}
 
int main(void)
{
	list(".",0);
    return(0);
}

