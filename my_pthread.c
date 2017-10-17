// File:	my_pthread.c
// Author:	Yujie REN
// Date:	09/23/2017

// name:
// username of iLab:
// iLab Server: test

#include "my_pthread_t.h"
#include "context_queue.c"
#define MEM 64000
#define MAX_THREADS 100

pLevels * running_queue = NULL;
queue * waiting_queue = NULL;
context_node * current = NULL;

//Update Flags
flagCalled fc = NONE;
unsigned int maintenanceCount = 0;

my_pthread_t thread_ids[MAX_THREADS];
ucontext_t sche_context;


/*

For testing

*/

void *mythread(void *arg){
	myarg_t *m = (myarg_t *) arg;
	printf("a: %d b: %d\n", m->a, m->b);
	my_pthread_exit(arg);
}



/*

 For testing

*/

int thread_scheduler(/*my_pthread_t id*/){
	//running_queue ->context_queue[id] -> thread_priority = 3;
 	
 	
	int i;
	for (i = 0; i<MAX_THREADS; i++){
		if(thread_ids[i] == 0 || thread_ids[i] == 2){
			continue;
		} else{
			getcontext(&sche_context);
			running_queue -> current_running_thread = running_queue -> context_queue[i] -> tid;
			swapcontext(&sche_context, &(running_queue -> context_queue[i] -> thread_context));
			printf("IN SCHEDULER");
		}
	}

	
}
uint get_thread_id(){
	uint i;
	for(i=0; i<MAX_THREADS; i++){
		if(thread_ids[i] != 1){
			printf("thread_ids: %d\n", thread_ids[i]);
			return i;
		}
	}
}

/* create a new thread */
int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg) {
	
	ucontext_t new_context;
	uint new_tid = get_thread_id();
	//Set thread status 
	tcb * new_thread_block = (tcb *)malloc(sizeof(tcb));
	//context_node * new_node = (context_node *)malloc(sizeof(context_node));
	

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

	//new_node -> thread_block = new_thread_block;
	//new_node -> next = NULL;

	new_thread_block -> tid = new_tid;
	thread_ids[new_tid] = 1;
	if (running_queue == NULL){
		running_queue = (Queue *) malloc(sizeof(Queue));
	} 
	running_queue -> context_queue[new_tid] = new_thread_block;
	//enqueue(new_node, running_queue);
	
	


	return 0;
}

/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield() {
	return 0;
};

/* terminate a thread */
//Must restore previous context, also must handle if user does a return
void my_pthread_exit(void *value_ptr) {
	my_pthread_t current = running_queue ->current_running_thread;
	running_queue -> context_queue[current] = NULL;
	thread_ids[current] = 0;
	thread_scheduler();
}

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
	args2.a = 30;
	args2.b = 40;
	args3.a = 50;
	args3.b = 60;
	rc = my_pthread_create(&mp, NULL, mythread, &args1);
	rc = my_pthread_create(&mp, NULL, mythread, &args2);
	rc = my_pthread_create(&mp, NULL, mythread, &args3);

	thread_scheduler();
		

	return 0;

}


/* ******************* SCHEDULER ******************* */


void createScheduler() {
	
	int i;
	running_queue = (pLevels *) malloc(sizeof(pLevels));
	
	for (i = 0; i < NUM_PRIORITIES; i++) {
		running_queue->rqs[i] = (queue *) malloc(sizeof(queue));
		running_queue->rqs[i]->front = NULL;
		running_queue->rqs[i]->back = NULL;
		running_queue->rqs[i]->current_executing_thread = NULL;
		running_queue->rqs[i]->priority = i;
	}
	waiting_queue = (queue *) malloc(sizeof(queue));
	waiting_queue->front = NULL;
	waiting_queue->back = NULL;
	waiting_queue->current_executing_thread = NULL;
	waiting_queue->priority = 10;
	
	//scheduler called when timer goes off
	signal(SIGALARM, scheduler);
}

void scheduler() {
	
	if (updateQueue()) {
		//Let current thread resume
	}
	
	//Else pick the next thread
	/*
	 * maintenanceCount++; (After the count hits a certain number, we move the threads from the lowest priority to the top to make sure we don't starve the old ones)
	 * 
	 * Find the head of the queue with the highest priority that's not null
	 * this will be the next thread to run
	 * 
	 * set timer (not implemented yet) to certain time based on priority level
	 * Look at http://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html to implement timer
	 * 
	 * swap context?
	 */
}

//Updates queues if necessary. Return 1 if current thread should resume
int updateQueue{
	
	//Determine what to do with current thread
	switch(fc) {
		case NONE:
		case TIMER:
		case YIELD:
		case EXIT:
	}
	
	if (maintenanceCount > MAINT_CYCLE) {
		//move oldest (lowest priority) threads to the end of the highest priority queue
	}
}
