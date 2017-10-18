#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "my_pthread_t.h"

my_pthread_t tid[2];
int counter;

void* doSomeThing(void *arg)
{
    unsigned long i = 0;
    counter += 1;
    printf("\n Job %d started\n", counter);

    for(i=0; i<(0xFFFFFFFF);i++);
    printf("\n Job %d finished by %d\n", counter, getCurrent());
    
    my_pthread_exit(NULL);
}

int main(void)
{
    int i = 0;
    int err;

    while(i < 2)
    {
        err = my_pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }

    my_pthread_join(tid[0], NULL);
    my_pthread_join(tid[1], NULL);

    return 0;
}
