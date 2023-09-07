#include <fstream>
#include "thread.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>




void thread1a(void* a){
	thread_lock(1);
}

void thread1b(void* a){
	thread_lock(1);
	std::cout<<"Fail"<<std::endl;
	exit(1);
}


void start1(void *a){
	thread_create(thread1a, a);
	thread_create(thread1b, a);
}

//test thread actually holds a lock when it requests it
int main(int argc, char*argv[]){
	std::cout<<"Testing thread locks on lock"<<std::endl;
	thread_libinit(start1, argv);
}