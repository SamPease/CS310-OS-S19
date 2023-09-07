#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>

using namespace std;

int g=0;

void loop(void *a) {
  char *id;
  int i;

  id = (char *) a;
  cout <<"loop called with id " << (char *) id << endl;

  for (i=0; i<5; i++, g++) {
    cout << id << ":\t" << i << "\t" << g << endl;
    if (g == 2 && thread_create((thread_startfunc_t) loop,
      (void *) "grandchild thread")) {
      cout << "thread_create (2) failed\n";
      exit(1);
    }
    else if (g == 2) {
      i++;
      //cout << (char * ) id << " called lock\n";
      thread_lock(2);
      //continue;
    }
    else if (i == 2) {

      thread_lock(2);
    }
    else if (i == 4) {
      //cout << (char * ) id << " called unlock\n";
      thread_unlock(2);
    }
    if (thread_yield()) {
      //cout << "thread_yield failed\n";
      exit(1);
    }
  }
}

void parent(void *a) {
  //start_preemptions(true, false, 12);

  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;
  if (thread_create((thread_startfunc_t) loop, (void *) "child thread")) {
    cout << "thread_create failed\n";
    exit(1);
  }

  loop( (void *) "parent thread");
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 100)) {
    cout << "thread_libinit failed\n";
    exit(1);
  }
}
