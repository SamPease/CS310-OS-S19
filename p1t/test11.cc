#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


using namespace std;

void x() {
  cout << "thread x action 1" << endl;
  thread_lock(1);
  thread_wait(1, 1);
  cout << "thread action 2" << endl;
  thread_unlock(1);
  thread_signal(1, 1);
}

void y() {
  cout << "thread y action 1" << endl;
  thread_signal(1, 1);
  thread_wait(1, 1);
  cout << "thread y action 2" << endl;
}

void manager() {
  thread_create( (thread_startfunc_t) x, NULL);
  thread_create( (thread_startfunc_t) y, NULL);
}

int main() {
  thread_libinit( (thread_startfunc_t) manager, NULL);
}
