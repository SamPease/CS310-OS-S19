

#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>




void t1(void* a){
	if (thread_lock(1)==-1)
		std::cout<<"Fail 1"<<std::endl;
	
	if (thread_yield()==-1)
		std::cout<<"Fail yield 1"<<std::endl;
	
	if (thread_unlock(1) == -1)
		std::cout<<"Fail unlock 1"<<std::endl;
	
	if (thread_lock(2)== -1)
		std::cout<<"Fail lock 2"<<std::endl;
}

void t2(void* a){
	if (thread_lock(2)==-1)
		std::cout<<"Fail 2"<<std::endl;
	
	if (thread_yield()==-1)
		std::cout<<"Fail 2"<<std::endl;

	if (thread_unlock(2)==-1)
		std::cout<<"Fail unlock 2"<<std::endl;

	if (thread_lock(1)==-1)
		std::cout<<"Fail lock 1"<<std::endl;
	
}

void parent(void* a){

	thread_create((thread_startfunc_t) t1, a);
	thread_create((thread_startfunc_t) t2, a);

}

int main(int argc, char* argv[]){
	void* a;
	thread_libinit((thread_startfunc_t) parent, a);
}


