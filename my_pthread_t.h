// File:	my_pthread_t.h
// Author:	Yujie REN
// Date:	09/23/2017

// name:
// username of iLab:
// iLab Server: 
#ifndef MY_PTHREAD_T_H
#define MY_PTHREAD_T_H

#define _GNU_SOURCE
#define NUM_PRIORITIES 5
#define MAINT_CYCLE 50

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>




typedef uint my_pthread_t;

typedef struct threadControlBlock {
	/* add something here */
	unsigned int status;
	my_pthread_t tid;
	/* must still account for thread attributes */
	ucontext_t thread_context;
	unsigned int thread_priority;
} tcb; 

typedef struct context_node { 
	tcb * thread_block;
	struct context_node * next;
} context_node;

typedef struct queue {
	/* Pointer to the current context being executed */
	context_node * current_executing_thread;
	/* Pointer to the front of the running_queue */
	context_node * front;
	context_node * back;
	unsigned int priority;
} queue;

typedef struct Queue {
	unsigned int priority;
	my_pthread_t current_running_thread;
	tcb * context_queue[];
} Queue;

//Priority levels for running queues
typdef struct pLevels {
	queue * rqs[NUM_PRIORITIES];
} pLevels;

//Flags used to determine why the scheduler was called
typedef enum {NONE, TIMER, YIELD, EXIT} flagCalled;


/* mutex struct definition */
typedef struct my_pthread_mutex_t {
	/* add something here */
} my_pthread_mutex_t;

/* define your data structures here: */

// Feel free to add your own auxiliary data structures

/* a global queue that stores a list of all the created threads */

typedef struct __myarg_t { 
	int a;
	int b; 
} myarg_t;





/* Function Declarations: */

/* create a new thread */
int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg);

/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield();

/* terminate a thread */
void my_pthread_exit(void *value_ptr);

/* wait for thread termination */
int my_pthread_join(my_pthread_t thread, void **value_ptr);

/* initial the mutex lock */
int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);

/* aquire the mutex lock */
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex);

/* release the mutex lock */
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex);

/* destroy the mutex */
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex);




#endif
