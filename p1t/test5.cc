#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>

int iter = 0;

using namespace std;
void testBroadcasta(void*){
	cout<<"Running Thread"<<endl;
	thread_lock(1);
	cout<<"Got Lock"<<endl;
	thread_wait(1,3);
		cout<<"Got Out of Wait"<<endl;


	iter++;
	thread_unlock(1);
}

void testBroadcastb(void*){
	cout<<"Running Loser Thread"<<endl;

	thread_lock(1);
	cout<<"Got Lock"<<endl;
	thread_wait(1,4);
	iter++;
	thread_unlock(1);
}

void testBroadcastc(void*){
	cout<<"Running Broadcast Thread"<<endl;

	thread_lock(1);
	cout<<"Got Lock"<<endl;
	thread_broadcast(1,3);
	thread_unlock(1);
}

void parent(void *a) {

	thread_create((thread_startfunc_t)testBroadcasta, (void *) 100);
	thread_create((thread_startfunc_t)testBroadcasta, (void *) 100);
	thread_create((thread_startfunc_t)testBroadcastb, (void *) 100);
	thread_create((thread_startfunc_t)testBroadcastc, (void *) 100);
	thread_yield();
	thread_yield();
	thread_yield();

	if(iter!=2)
		exit(1);
	exit(0);
}

int main() {
  thread_libinit( (thread_startfunc_t) parent, (void *) 100);
}
