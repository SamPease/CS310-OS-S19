#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


using namespace std;
int e = 1;
int d = 2;
void grandchild(void* c) {
  d = 3;
  if (thread_lock(1)!=0)
    exit(1);
  e = 2;
  if(thread_unlock(1))
    exit(1);
}

void child(void* b){
  thread_yield();
 
  if(d==2)
    exit(1);
 
  if(thread_lock(1)!=0)
    exit(1);
  
  if(thread_yield()!=0)
    exit(1);
 
  if(thread_yield()!=0)
    exit(1);

  if(e!=2)
    exit(1);

  thread_unlock(1);

}


void parent(void* a) {
    thread_create((thread_startfunc_t) child, a);
    thread_create((thread_startfunc_t) grandchild, a);
}

int main() {
  thread_libinit( (thread_startfunc_t) parent, NULL);
}
