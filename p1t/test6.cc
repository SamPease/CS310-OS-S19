#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>

int val=1;
using namespace std;
void error(void *a){
  int b = 1;
}

void lock1(void *a) {
	thread_lock(10);
	cout<<"Thread1 gets lock"<<endl;
	cout<<"Thread1 yields"<<endl;
  	thread_yield();
  	cout<<"Thread1 gives up lock"<<endl;

  	thread_unlock(10);
  	//exit(0);
}

void lock2(void *a) {

	cout<<"Thread2 tries to get lock"<<endl;

	thread_lock(10);
	cout<<"Thread2 gets lock"<<endl;

  	if(val==3){
  		exit(1);
  	}

  	cout<<"Thread2 gives up lock"<<endl;
	thread_unlock(10);
  	//exit(0);
}

void lock3(void *a) {
	cout<<"Thread3 yields"<<endl;

  	thread_yield();

  	cout<<"Thread3 tries to get lock"<<endl;

  	thread_lock(10);
  	cout<<"Thread3 gets lock and alters val"<<endl;
  	val = 3;
  	cout<<"Thread3 gives lock"<<endl;

  	thread_unlock(10);
  	//exit(0);
}

void parent(void* a){
	thread_create((thread_startfunc_t)lock1, a);
	thread_create((thread_startfunc_t)lock2, a);
	thread_create((thread_startfunc_t)lock3, a);
}

int main() {
  thread_libinit( (thread_startfunc_t) parent, (void *) 100); 
}
