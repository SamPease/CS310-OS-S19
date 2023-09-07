
#include <fstream>
#include "thread.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>

int iterator = 0;

int currentThread = -1;
struct iWrapper{
	int i;
};
//int[] correctOrder = {};






void startIncrementing(void* b){
	int a = ((iWrapper*)b)->i;


	std::cout<<"Thread " << a << " Ready and rying to obtain lock 1" <<std::endl;
	thread_lock(1);
	std::cout<<"Thread " << a << " Obtained lock 1" <<std::endl;
	currentThread = a;
	std::cout<<"Thread " << a << " Yielding" <<std::endl;
	thread_yield();
	std::cout<<"Thread " << a << " Yielding" <<std::endl;
	thread_yield();
	std::cout<<"Thread " << a << " Signaling CV 1" <<std::endl;
	thread_signal(1,1);
	std::cout<<"Thread " << a << " Waiting on CV 1" <<std::endl;
	thread_wait(1,1);
	std::cout<<"Thread " << a << " Waking up on CV 1" <<std::endl;
	std::cout<<"Thread " << a << " Unlocking lock 1" <<std::endl;
	thread_unlock(1);

}

void start(void* x){
	std::cout<< "Starting to make Threads"<<std::endl;

	iWrapper* t1 = new iWrapper;
	iWrapper* t2 = new iWrapper;
	iWrapper* t3 = new iWrapper;

	t1->i = 1;
	t2->i = 2;
	t3->i = 3;


	thread_create((thread_startfunc_t)startIncrementing, (void*)t1);
	thread_create((thread_startfunc_t)startIncrementing, (void*)t2);
	thread_create((thread_startfunc_t)startIncrementing, (void*)t3);
}

int main(int argc, char *argv[]){
	std::cout<< "Starting"<<std::endl;

	void* a;
	thread_libinit((thread_startfunc_t)start, a);
	return 0;

}

// void checkThread(int* a, char* op){


// 	if(strcmp(op,"lock")){

// 	}

// 	if(strcmp(op,"unlock")){
		
// 	}

// 	if(strcmp(op,"wait")){
		
// 	}

// 	if(strcmp(op,"yield")){
		
// 	}

// 	if(strcmp(op,"signal")){
		
// 	}


// }







