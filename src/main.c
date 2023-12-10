#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>



int main() {
    size_t B_Size = 1024;
    char file_name[128];
    if(fgets(file_name,128,stdin) == NULL) {
        perror("No filename");
        return 3;
    }
    
    int pos = strlen(file_name) - 1;
    if (file_name[pos] == '\n') {
        file_name[pos] = '\0';
    }

    int file = open(file_name, O_RDWR, B_Size/2);
    if( file == -1) {
        perror("No such file");
        return 1;
    }

    int buf_file = open("Buff.txt", O_RDWR | O_CREAT, 0666);
    if( buf_file == -1) {
        perror("No such file");
        return 7;
    }

    if(ftruncate(buf_file, B_Size) == -1) {
        return 4;
    }

    char* mapped_data = (char*)mmap(NULL, B_Size, PROT_READ | PROT_WRITE, MAP_SHARED, buf_file, 0);
    if( mapped_data == MAP_FAILED) {
        perror("error with memory map");
        close(buf_file);
        return 4;
    }
    //закинь дату из теста в бафф.тхт
    if(read(file,mapped_data,B_Size/2) == -1) {
            return 8;
        };
    close(file);
    


    pid_t pid = fork();
    if (pid == -1) {
        perror("problem with fork");
        return 2;
    }
    if (pid == 0) { //child

        execlp("./child", "./child",mapped_data, NULL);
    }
    else {  //parent

        int status;
        waitpid(pid, &status,0);
        if(!WIFEXITED(status)){
            perror("error with child proccess");
            return 10;
        }
        
        printf("%s \n",mapped_data + (B_Size/2 + 1));
        
    }

    memset(mapped_data,0,B_Size);
    if(msync(mapped_data,B_Size,MS_SYNC) == -1) {
        perror("failed to sinchronize");
        return -1;
    }

    munmap(mapped_data, 1024);
    close(buf_file);
    return 0;
}