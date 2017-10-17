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

pLevels * running_qs = NULL;
queue * waiting_queue = NULL;
context_node * current = NULL;

//Update Flags
flagCalled fc = NONE;
int firstThread = 1;
unsigned int maintenanceCount = 0;

//Timer
struct itimerval itv;

unsigned int threadCount = 1;
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
	
	ucontext_t *new_context, *old_context;
	new_context = (ucontext_t *) malloc(sizeof(ucontext_t));
	getcontext(new_context);
	tcb * new_thread_block = (tcb *)malloc(sizeof(tcb));
	context_node * new_node = (context_node *) malloc(sizeof(context_node));
	
	if (firstThread) {
		firstThread = 0;
		
		createScheduler();
		
		old_context = (ucontext_t *) malloc(sizeof(ucontext_t));
		context_node * old_node = (context_node *) malloc(sizeof(context_node));
		tcb * old_thread_block = (tcb *)malloc(sizeof(tcb));
		getcontext(old_context);
		
		//Creates new thread context and new thread node 
		new_context -> uc_link=0;
 		new_context -> uc_stack.ss_sp=malloc(MEM);
 		new_context -> uc_stack.ss_size=MEM;
 		new_context -> uc_stack.ss_flags=0;
 		makecontext(new_context, (void*)function, 1, arg);
		new_thread_block -> thread_context = new_context;
		new_thread_block -> tid = 1;
		new_thread_block -> thread_priority = 0;
		
		new_node -> thread_block = new_thread_block;
		new_node -> next = NULL;
		
		enqueue(new_node, running_qs -> rqs[0]);
		
		//Gets old thread context and creates old thread node 
		old_thread_block -> thread_context = old_context;
		old_thread_block -> tid = 0;
		old_thread_block -> thread_priority = 0;
		
		old_node -> thread_block = old_thread_block;
		old_node -> next = NULL;
		
		current = old_node;
		
		enqueue(old_node, running_qs -> rqs[0]);
		
		
		//Both context nodes are enqueued to the running queue
		
	} else {
		//Creates new thread context and new thread node 
		new_context -> uc_link=0;
 		new_context -> uc_stack.ss_sp=malloc(MEM);
 		new_context -> uc_stack.ss_size=MEM;
 		new_context -> uc_stack.ss_flags=0;
 		makecontext(new_context, (void*)function, 1, arg);
		new_thread_block -> thread_context = new_context;
		new_thread_block -> tid = 1;
		new_thread_block -> thread_priority = 0;
		
		new_node -> thread_block = new_thread_block;
		new_node -> next = NULL;
		
		enqueue(new_node, running_qs -> rqs[0]);
	}
	
	scheduler(0);

	return 0;
}

/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield() {
	return 0;
};

/* terminate a thread */
//Must restore previous context, also must handle if user does a return
void my_pthread_exit(void *value_ptr) {
	
	return;
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


/* ******************* SCHEDULER ******************* */


void createScheduler() {
	
	int i;
	running_qs = (pLevels *) malloc(sizeof(pLevels));
	
	for (i = 0; i < NUM_PRIORITIES; i++) {
		running_qs->rqs[i] = (queue *) malloc(sizeof(queue));
		running_qs->rqs[i]->front = NULL;
		running_qs->rqs[i]->back = NULL;
		running_qs->rqs[i]->current_executing_thread = NULL;
		running_qs->rqs[i]->priority = i;
	}
	waiting_queue = (queue *) malloc(sizeof(queue));
	waiting_queue->front = NULL;
	waiting_queue->back = NULL;
	waiting_queue->current_executing_thread = NULL;
	waiting_queue->priority = 10;
	
	//scheduler called when timer goes off
	signal(SIGALRM, scheduler);
}

void scheduler(int signum) {
	
	if (updateQueue()) {
		//Let current thread resume
	} else {
		maintenanceCount++;
		int i;
		context_node * new_context;
		for (i = 0; i < NUM_PRIORITIES; i++) {
			new_context = running_qs -> rqs[i] -> front;
			if (new_context != NULL) {
				break;
			}
		}
		
		if (new_context == NULL) {
			//all queues empty, do something
		}
		
		swapcontext(current->thread_block->thread_context, new_context->thread_block->thread_context);
		
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
	 
	 //Set timer
	 itv.it_value.tv_usec = ((25 + 25 * current->thread_block->thread_priority) * 1000) % 1000000;
	 itv.it_value.tv_sec = 0;
	 itv.it_interval = itv.it_value;
	 
	 if(setitimer(ITIMER_REAL, &itv, NULL) == -1){
		 //print error
        return;
    }
}

//Updates queues if necessary. Return 1 if current thread should resume
int updateQueue(){
	
	//Determine what to do with current thread
	switch(fc) {
		case NONE: break;
		case TIMER: break;
		case YIELD: break;
		case PEXIT: break;
	}
	
	
	
	if (maintenanceCount > MAINT_CYCLE) {
		//move oldest (lowest priority) threads to the end of the highest priority queue
	}
}
