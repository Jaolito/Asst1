#include <stdio.h>
#include <stdlib.h>
#include "my_pthread_t.h"

typedef struct __myarg_t { 
	int a;
	int b; 
} myarg_t;

void *mythread(void *arg){
	myarg_t *m = (myarg_t *) arg;
	printf("a: %d b: %d\n", m->a, m->b);
	my_pthread_exit(arg);
}

int main() {
	
	my_pthread_t mp;
	int rc;
	myarg_t args1;
	myarg_t args2;
	myarg_t args3;
	args1.a = 10;
	args1.b = 20;
	args2.a = 30;
	args2.b = 40;
	args3.a = 50;
	args3.b = 60;
	rc = my_pthread_create(&mp, NULL, mythread, &args1);
	rc = my_pthread_create(&mp, NULL, mythread, &args2);
	rc = my_pthread_create(&mp, NULL, mythread, &args3);		

	return 0;	
}

