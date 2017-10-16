// File:	my_pthread.c
// Author:	Yujie REN
// Date:	09/23/2017

// name:
// username of iLab:
// iLab Server: test

#include "my_pthread_t.h"
#include "context_queue.c"
#define MEM 64000

queue * running_queue = NULL;
queue * waiting_queue = NULL;
/*

For testing

*/

void *mythread(void *arg){
	myarg_t *m = (myarg_t *) arg;
	printf("a: %d b: %d\n", m->a, m->b);
	return NULL;
}

/*

 For testing

*/

int thread_scheduler(){
	
}

/* create a new thread */
int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg) {
	
	ucontext_t new_context;
	//Set thread status 
	tcb * new_thread_block = (tcb *)malloc(sizeof(tcb));
	context_node * new_node = (context_node *)malloc(sizeof(context_node));
	

	//Initializes the context of the thread 
	int get_context_status = getcontext(&new_context);

	if(get_context_status == -1){
		printf("ERROR in getcontext()");
		return 0;
	//Need to initialize the entire tcb 	
	} else {
		new_context.uc_link=0;
 		new_context.uc_stack.ss_sp=malloc(MEM);
 		new_context.uc_stack.ss_size=MEM;
 		new_context.uc_stack.ss_flags=0;
 		makecontext(&new_context, (void*)function, 1, arg);
		new_thread_block -> thread_context = new_context;
	}

	new_node -> thread_block = new_thread_block;
	new_node -> next = NULL;


	if (running_queue == NULL){
		running_queue = (queue *) malloc(sizeof(queue));
	} 
	enqueue(new_node, running_queue);
	
	


	return 0;
}

/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield() {
	return 0;
};

/* terminate a thread */
void my_pthread_exit(void *value_ptr) {
};

/* wait for thread termination */
int my_pthread_join(my_pthread_t thread, void **value_ptr) {
	return 0;
};

/* initial the mutex lock */
int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr) {
	return 0;
};

/* aquire the mutex lock */
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex) {
	return 0;
};

/* release the mutex lock */
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex) {
	return 0;
};

/* destroy the mutex */
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex) {
	return 0;
};


int main(){

	my_pthread_t mp;
	int rc;
	myarg_t args1;
	myarg_t args2;
	myarg_t args3;
	args1.a = 10;
	args1.b = 20;
	args2.a = 10;
	args2.b = 20;
	args3.a = 10;
	args3.b = 20;
	rc = my_pthread_create(&mp, NULL, mythread, &args1);
	rc = my_pthread_create(&mp, NULL, mythread, &args2);
	rc = my_pthread_create(&mp, NULL, mythread, &args3);
		

	return 0;

}

