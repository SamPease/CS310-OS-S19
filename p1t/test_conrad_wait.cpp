#include<'thread.cc'>
#include<stdlib.h>

int iterator = 0;

int currentThread;
int[] correctOrder = {};

int main(argc, argv[]*){
	void* a;
	thread_libinit((thread_startfunc_t)start, a);
	return 0;

}


void start(void* a){
	int* t1;
	int* t2;
	int* t3;
	*t1=1;
	*t2=2;
	*t3=3;


	thread_create((thread_startfunc_t)startIncrementing, t1);
	thread_create((thread_startfunc_t)startIncrementing, t2);
	thread_create((thread_startfunc_t)startIncrementing, t3);
}

void startIncrementing(int* a){
	


	std::cout<<"Thread " << *a << "Ready and rying to obtain lock 1" <<std::endl;
	thread_lock(1);
	std::cout<<"Thread " << *a << "Obtained lock 1" <<std::endl;
	currentThread = myID;
	std::cout<<"Thread " << *a << "Yielding" <<std::endl;
	thread_yield();
	std::cout<<"Thread " << *a << "Yielding" <<std::endl;
	thread_yield();
	std::cout<<"Thread " << *a << "Signaling CV 1" <<std::endl;
	thread_signal(1,1);
	std::cout<<"Thread " << *a << "Waiting on CV 1" <<std::endl;
	thread_wait(1,1);
	std::cout<<"Thread " << *a << "Waking up on CV 1" <<std::endl;
	std::cout<<"Thread " << *a << "Unlocking lock 1" <<std::endl;
	thread_unlock(1);

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







