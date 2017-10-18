#include <stdio.h>
#include <stdlib.h>
#include "my_pthread_t.h"

typedef struct __myarg_t { 
	int a;
	int b; 
} myarg_t;

void *mythread2(void *arg){
	myarg_t *m = (myarg_t *) arg;
	printf("a: %d b: %d\n", m->a, m->b);
	int rtn = m->a * m->b;
	my_pthread_exit((void*)&rtn);
}

void *mythread3(void *arg){
	my_pthread_yield();
	my_pthread_yield();
	my_pthread_yield();
	my_pthread_yield();
	my_pthread_yield();
	myarg_t *m = (myarg_t *) arg;
	printf("a: %d b: %d\n", m->a, m->b);
	int rtn = m->a * m->b;
	my_pthread_exit((void*)&rtn);
}

void *mythread(void *arg){
	my_pthread_t thread;
	myarg_t arg2;
	arg2.a=30;
	arg2.b=50;
	myarg_t *m = (myarg_t *) arg;
	int * a;
	printf("a: %d b: %d\n", m->a, m->b);
	int rc = my_pthread_create(&thread, NULL, mythread2, &arg2);
	int join = my_pthread_join(thread, (void *) &a);
	printf("a:  %d b : %d rtn: %d\n", m->a,m->b,*(int *)a);
	my_pthread_exit(arg);
}

void *mythread4(void *arg){
	
	
	
	my_pthread_exit(arg);
}


int main() {
	
	my_pthread_t mp;
	my_pthread_t mp2;
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
	rc = my_pthread_create(&mp2, NULL, mythread3, &args1);
	rc = my_pthread_create(&mp, NULL, mythread, &args2);
	my_pthread_join(mp, NULL);
	my_pthread_join(mp2, NULL);
	/*
	rc = my_pthread_create(&mp, NULL, mythread, &args3);
	my_pthread_join(mp, NULL);*/

	return 0;
}

