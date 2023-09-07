#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>

using namespace std;

int sleeper=0;
static const unsigned int LOCK_VALUE = 2;
static const unsigned int CV_VALUE = 2;

void child(void *a) {
    thread_lock(LOCK_VALUE);
    if (sleeper > 0) {
      thread_signal(LOCK_VALUE, CV_VALUE);
    }
    thread_unlock(LOCK_VALUE);
}

void parent(void *a) {
    thread_create((thread_startfunc_t) child, (void *) 100);
    thread_lock(LOCK_VALUE);
    sleeper++;
    thread_wait(LOCK_VALUE, CV_VALUE);
    sleeper--;
    thread_unlock(LOCK_VALUE);
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 100)) {
    cout << "thread_libinit failed\n";
    exit(1);
  }
}
