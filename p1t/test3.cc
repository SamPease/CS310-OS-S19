#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>

using namespace std;
void error(void *a){
  int b = 1;
}

void parent(void *a) {

  if (thread_libinit((thread_startfunc_t) error, (void *) 100) == 0){
   // cout<<"Fail"<<endl;
    exit(1);
  }
  //cout<<"Pass"<<endl;
  exit(0);
}

int main() {
  thread_libinit( (thread_startfunc_t) parent, (void *) 100); 
}
