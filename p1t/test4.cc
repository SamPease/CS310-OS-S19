#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>

using namespace std;

static const unsigned int LOCK_VALUE = 2;
static const unsigned int LOCK_VALUE2 = 3;

void child(void *a) {
    thread_lock(LOCK_VALUE2);
    thread_yield();
    thread_unlock(LOCK_VALUE2);
}

void parent(void *a) {
    thread_lock(LOCK_VALUE);
    thread_create((thread_startfunc_t) child, (void *) 100);
    thread_yield();
    if(thread_lock(LOCK_VALUE) == -1) {
      thread_unlock(LOCK_VALUE);
    }
    if (thread_unlock(LOCK_VALUE) == -1) {
      std::cout << "No esta bueno\n";
    }
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 100)) {
    cout << "thread_libinit failed\n";
    exit(1);
  }
}
