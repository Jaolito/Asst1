#include "my_pthread_t.h"


unsigned int threads_created;


int get_specific_count(queue * Q){
	
	int count = 0;
	context_node * temp = Q -> front;
	while(temp!=NULL){
		count++;
		temp = temp -> next;
	}
	return count;
	
}

int get_total_count(){
	return threads_created;
}


/* Need to figure out how to increase total count e.g. if a thread is moved from 
the waiting queue to the running queue the total number of threads should not increase*/
void enqueue(tcb * enter_thread, Queue * Q){
	if(get_specific_count(Q) == 0){
		Q -> front = enter_thread;
		Q -> back = enter_thread;
		
	} else {
		Q -> back -> next = enter_thread;
		Q -> back = enter_thread;
	}
}

tcb * deqeue(queue * Q){

	context_node * temp;
	tcb * rtn;
	if(get_specific_count(Q) == 0){
		return NULL;
	} else {
		temp = Q -> front;
		Q -> front = temp -> next;
	}
	rtn = temp -> thread_block;
	free(temp);
	return rtn;
}

