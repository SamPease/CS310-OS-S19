#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


using namespace std;

void grandchild(void* c) {
  cout << "Grandchild lock" << endl;
  thread_lock(1);
  cout << "Grandchild signal" << endl;
  thread_signal(1, 3);
  cout << "Grandchild wait" << endl;
  thread_wait(1, 4);
  cout << "Grandchild unlock" << endl;
  thread_unlock(1);
  cout << "Grandchild finished" << endl;
}

void child(void* b){
  cout << "Child locked" << endl;
  thread_lock(1);
  cout << "Child signal" << endl;
  thread_signal(1, 2);
  cout << "Child wait" << endl;
  thread_wait(1, 3);
  cout << "Child awake and unlock" << endl;
  thread_unlock(1);
  cout << "Child finished" << endl;
}


void parent(void* a) {
  cout << "Parent locked" << endl;
  thread_lock(1);
  cout << "Parent creating threads" << endl;
  thread_create( (thread_startfunc_t) child, NULL);
  thread_yield();
  thread_create( (thread_startfunc_t) grandchild, NULL);
  cout << "Parent wait" << endl;
  thread_wait(1, 2);
  cout << "Parent awake and signal" << endl;
  thread_signal(1, 4);
  cout << "Parent unlock" << endl;
  thread_unlock(1);
  cout << "Parent finished" << endl;
}

int main() {
  thread_libinit( (thread_startfunc_t) parent, NULL);
}
