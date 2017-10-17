// File:	my_pthread.c
// Author:	Yujie REN
// Date:	09/23/2017

// name:
// username of iLab:
// iLab Server: test

#include "my_pthread_t.h"
#define MEM 64000
#define MAX_THREADS 100

pLevels * running_qs = NULL;
queue * waiting_queue = NULL;
context_node * current = NULL;
queue * join_queue = NULL;
exit_node * exit_list = NULL;

//Update Flags
flagCalled fc = NONE;
int firstThread = 1;
unsigned int maintenanceCount = 0;

//Timer
struct itimerval itv;

uint threadCount = 1;



/* create a new thread */
int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg) {
	
	printf("In pthread_create\n");
	
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
		
		enqueuee(new_node, running_qs -> rqs[0]);
		
		//Gets old thread context and creates old thread node 
		old_thread_block -> thread_context = old_context;
		old_thread_block -> tid = 0;
		old_thread_block -> thread_priority = 0;
		
		old_node -> thread_block = old_thread_block;
		old_node -> next = NULL;
		
		current = old_node;
		
		enqueuee(old_node, running_qs -> rqs[0]);
		
		
		//Both context nodes are enqueueed to the running queue
		
	} else {
		//Creates new thread context and new thread node 
		new_context -> uc_link=0;
 		new_context -> uc_stack.ss_sp=malloc(MEM);
 		new_context -> uc_stack.ss_size=MEM;
 		new_context -> uc_stack.ss_flags=0;
 		makecontext(new_context, (void*)function, 1, arg);
		new_thread_block -> thread_context = new_context;
		new_thread_block -> tid = threadCount;
		new_thread_block -> thread_priority = 0;
		
		new_node -> thread_block = new_thread_block;
		new_node -> next = NULL;
		
		enqueuee(new_node, running_qs -> rqs[0]);
	}
	
	threadCount++;
	
	scheduler(0);

	return 0;
}

/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield() {
	return 0;
};

/* terminate a thread */
void my_pthread_exit(void *value_ptr) {
	
	printf("In pthread_exit\n");
	
	int i;
	exit_node * e = (exit_node *) malloc(sizeof(exit_node));
	e -> next = NULL;
	e -> tid = current->thread_block->tid;
	e -> value_ptr = value_ptr;
	context_node * temp = NULL;
	
	fc = PEXIT;
	
	for (i = 0; i < get_specific_count(join_queue); i++) {
		temp = dequeuee(join_queue);
		if (temp->thread_block->join_id == current->thread_block->tid) {
			temp->thread_block->value_ptr = value_ptr;
			temp->thread_block->thread_priority = 0;
			enqueuee(temp, running_qs -> rqs[0]);
			break;
		} else {
			enqueuee(temp, join_queue);
		}
	}
	
	if (exit_list == NULL) {
		exit_list = e;
	} else {
		e->next = exit_list;
		exit_list = e;
	}
	
	scheduler(0);
	
	return;
}

/* wait for thread termination */
int my_pthread_join(my_pthread_t thread, void **value_ptr) {
	
	exit_node * temp = exit_list;
	
	if ((uint)thread > threadCount-1) {
		//Thread doesn't exist
		return -1;
	}
	
	while (temp != NULL) {
		if (temp->tid == thread) {
			*value_ptr = temp->value_ptr;
			return 1;
		} else {
			temp = temp -> next;
		}
	}
	
	current->thread_block->join_id = thread;
	
	fc = JOIN;
	
	scheduler(0);
	
	*value_ptr = current->thread_block->value_ptr;
	
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
		running_qs->rqs[i]->priority = i;
	}
	waiting_queue = (queue *) malloc(sizeof(queue));
	waiting_queue->front = NULL;
	waiting_queue->back = NULL;
	waiting_queue->priority = -1;
	
	join_queue = (queue *) malloc(sizeof(queue));
	join_queue->front = NULL;
	join_queue->back = NULL;
	join_queue->priority = -1;
	
	
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
			printf("New context found\n");
			if (new_context != NULL) {
				break;
			}
		}
		if (new_context == NULL) {
			//all queues empty, do something
		}
		
		if (current != NULL) {
		
			if (new_context->thread_block->tid == current->thread_block->tid) {
				printf("Main twice \n");
				if (new_context->next != NULL) {
					new_context = new_context->next;
					printf("Had next: tid is %d \n", new_context->thread_block->tid);
				} else {
					for (i = i+1; i < NUM_PRIORITIES; i++) {
						new_context = running_qs -> rqs[i] -> front;
						if (new_context != NULL) {
							break;
						}
					}
				}
				//enqueuee(dequeuee(running_qs->rqs[current->thread_block->thread_priority]), running_qs->rqs[current->thread_block->thread_priority]);
				if (new_context == NULL) {
					new_context = current;
				}
			}
		}		
		/*int t = current->thread_block->thread_priority;
		itv.it_value.tv_usec = ((25 + 25 * t) * 1000) % 1000000;
		itv.it_value.tv_sec = 0;
		itv.it_interval = itv.it_value;
		 
		if(setitimer(ITIMER_REAL, &itv, NULL) == -1){
			//print error
			return;
		}*/
		
		
		
		if (fc == PEXIT) {
			printf("SET\n");
			fc = NONE;
			current = new_context;
			setcontext(new_context->thread_block->thread_context);
		} else {
			printf("SWAP\n");
			context_node * old = current;
			current = new_context;
			swapcontext(old->thread_block->thread_context, new_context->thread_block->thread_context);
		}
		
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
	 
}

//Updates queues if necessary. Return 1 if current thread should resume
int updateQueue(){
	
	//Determine what to do with current thread
	switch(fc) {
		case NONE: return 0; break;
		case TIMER: break;
		case YIELD: break;
		case PEXIT:
			printf("PEXIT FLAG\n");
			dequeuee(running_qs -> rqs[current->thread_block->thread_priority]); 
			freeContext(current);
			current = NULL;
			break;
		case JOIN: break;
	}
	
	
	
	if (maintenanceCount > MAINT_CYCLE) {
		//move oldest (lowest priority) threads to the end of the highest priority queue
	}
	
	return 0;
}

void freeContext(context_node * freeable) {
	free(freeable->thread_block->thread_context->uc_stack.ss_sp);
	free(freeable->thread_block->thread_context);
	free(freeable->thread_block);
	free(freeable);
}

/* Queues */

void enqueuee(context_node * enter_thread, queue * Q){
	if(Q->front == NULL){
		Q -> front = enter_thread;
		Q -> back = enter_thread;
		enter_thread -> next = NULL;
	} else {
		Q -> back -> next = enter_thread;
		Q -> back = enter_thread;
	}
}

context_node * dequeuee(queue * Q){

	context_node * temp;
	if(get_specific_count(Q) == 0){
		return NULL;
	} else {
		temp = Q -> front;
		Q -> front = temp -> next;
	}
	
	return temp;
}

int get_specific_count(queue * Q){
	
	int count = 0;
	context_node * temp = Q -> front;
	while(temp!=NULL){
		count++;
		temp = temp -> next;
	}
	return count;
	
}
