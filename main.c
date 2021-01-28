#include "kmalloc.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<assert.h>

int main(int argc, char** argv){
    char mem[1024 * 4] = {0};
    if(kmalloc_init(mem, sizeof(mem), 32) < 0){
        printf("init kmalloc error\n");
        return -1;
    }
    MemMerit merit;
    kmalloc_merit(&merit);
    time_t t;
    srand((unsigned)time(&t));
    void* ptrs[10] = {NULL};
    printf("%d bytes,%d chunks\n",merit.free_bytes,merit.free_chunks);
    for(int k=0;k<5;k++){
        for(int i = 0; i < 10; i++){
            size_t size = sizeof(char) * (rand() % 323);
            ptrs[i] = kmalloc(size);
            assert((size_t)ptrs[i] % 32 == 0);
            kmalloc_merit(&merit);   
            printf("alloc:%d,free:%d bytes,%d chunks\n", size,merit.free_bytes,merit.free_chunks);
        }
        printf("============================\n");
        int pt[] = {5,9,6,3,7,1,0,8,4,2};
        for(int i = 0; i < 10; i++){
            if(ptrs[pt[i]]){
                kfree(ptrs[pt[i]]);
            }
            kmalloc_merit(&merit);
            printf("%d bytes,%d chunks\n",merit.free_bytes,merit.free_chunks);
            size_t size = sizeof(char) * (rand() % 323);
            if(size % 5 == 0){
                void* p = kmalloc(size);
                kmalloc_merit(&merit);   
                printf("alloc:%d,free:%d bytes,%d chunks\n", size,merit.free_bytes,merit.free_chunks);
                kfree(p);
            }
        }
        printf("============================\n");
    }
    printf("============================\n");
    kmalloc_merit(&merit);
    printf("%d bytes,%d chunks\n",merit.free_bytes,merit.free_chunks);
    return 0;
}
