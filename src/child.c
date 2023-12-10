#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>



float solving_func(char * nums) {
    float answer = strtof(nums, NULL);

    float divider = 0;
    int flag = 0;
    for (int i = 0; i < strlen(nums)+1; ++i) {
        if ((flag == 0) && ((nums[i] == ' ') || (nums[i] == '\0'))) {    
            flag = 1;
        }
        else if ((flag == 1) && (nums[i] != ' ') && (nums[i] != '\0')) {   
            divider = divider*10 + ((int)nums[i] - 48); 
        }
        else if ((flag == 1) && ((nums[i] == ' ') || (nums[i] == '\0'))) {
            if (divider == 0) {
                perror("Division by 0");
                exit(1);
            }
            answer = answer / divider;
            divider = 0;
        }
    }

    return answer;
}

int main(int argc, char const *argv[])
{
    size_t B_Size = 1024;
    char answer_char[64];
    float answer;
    size_t i = B_Size/2 + 1;

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

    answer = solving_func(mapped_data);
    
    if(gcvt(answer, 12, answer_char) == NULL) {
        perror("gcvt problem");
        return 6;
    }
    
    strcpy(mapped_data + i,answer_char);
    if(msync(mapped_data,B_Size,MS_SYNC) == -1) {
        perror("failed to sinchronize");
        return -1;
    }
    return 0;
}
