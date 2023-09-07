#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>

using namespace std;

int numSodas = 0;
int maxSodas = 2;
static const unsigned int soda_lock = 2;
static const unsigned int has_rors = 2;

void consumer(void *a) {
    thread_lock(soda_lock);

    while (numSodas == 0) {
        thread_wait(soda_lock, has_rors);
    }

    numSodas--;

    thread_broadcast(soda_lock, has_rors);

    thread_unlock(soda_lock);
}

void producer(void *a) {
    thread_lock(soda_lock);

    while (numSodas == maxSodas) {
        thread_wait(soda_lock, has_rors);
    }

    numSodas++;

    thread_broadcast(soda_lock, has_rors);

    thread_unlock(soda_lock);
}

void parent(void* a) {
    thread_create( (thread_startfunc_t) producer, NULL);
    thread_create( (thread_startfunc_t) producer, NULL);
    thread_create( (thread_startfunc_t) producer, NULL);
    thread_create( (thread_startfunc_t) consumer, NULL);
    thread_create( (thread_startfunc_t) consumer, NULL);
    thread_create( (thread_startfunc_t) producer, NULL);
    thread_create( (thread_startfunc_t) consumer, NULL);
    thread_create( (thread_startfunc_t) producer, NULL);
    thread_create( (thread_startfunc_t) consumer, NULL);
    thread_create( (thread_startfunc_t) consumer, NULL);
    thread_create( (thread_startfunc_t) producer, NULL);
    thread_create( (thread_startfunc_t) consumer, NULL);
    thread_create( (thread_startfunc_t) consumer, NULL);
    thread_create( (thread_startfunc_t) producer, NULL);
    thread_create( (thread_startfunc_t) consumer, NULL);
}
 
int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 100)) {
    cout << "thread_libinit failed\n";
    exit(1);
  }
}