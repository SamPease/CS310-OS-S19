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
  thread_wait(1,1);
  	//exit(0);
}

void lock2(void *a) {
  	//exit(0);
}

void lock3(void *a) {
  	//exit(0);
}

void parent(void* a){
	thread_create((thread_startfunc_t)lock1, a);
}

int main() {
  thread_libinit( (thread_startfunc_t) parent, (void *) 100); 
}
