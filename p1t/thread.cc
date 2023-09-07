#include "interrupt.h"
#include "thread.h"
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <ucontext.h>
#include <queue>
#include <map>

struct thread_context {
	ucontext_t* context;
	//map that indicates which locks the thread is on
	//and maps lock int to whether or not it has the lock
	unsigned int threadID;

};

int initCount = 0;
unsigned int ID = 0;
thread_context current_context;
thread_context prev_context;

std::map<unsigned int,unsigned int> locks;
std::queue<thread_context> ready_contexts;
std::map<unsigned int, std::queue<thread_context>*> blocked_contexts;
std::map<unsigned int, std::queue<thread_context>*> waiting_contexts;



int check_init() {
	if (initCount != 1) {
		return(-1);
	}
	return(0);
}

void func_complete(thread_startfunc_t thread_func, void* arg) {
	interrupt_enable();
	thread_func(arg);
	interrupt_disable();


	if (!ready_contexts.empty()) {
		if (prev_context.context != NULL) {
			delete [] (char*) prev_context.context->uc_stack.ss_sp;
			delete prev_context.context;
		}

		prev_context = current_context;
		current_context = ready_contexts.front();
		ready_contexts.pop();

		setcontext(current_context.context);
	}
	else {
		interrupt_enable();
		std::cout << "Thread library exiting.\n";
		exit(0);
	}
}

int thread_start(thread_startfunc_t func, void *arg, thread_context* new_context) {
	ID++;
	ucontext_t* ucontext_ptr;
	try {
	    ucontext_ptr = new ucontext_t;
	}
	catch (std::bad_alloc& ba) {
	    return(-1);
	}

	if (getcontext(ucontext_ptr) == -1){
		return(-1);
	}

	try {
		char* stack = new char[STACK_SIZE];
		ucontext_ptr->uc_stack.ss_sp = stack;
		ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
		ucontext_ptr->uc_stack.ss_flags = 0;
		ucontext_ptr->uc_link = NULL;
	}
	catch (std::bad_alloc& ba) {
		return(-1);
	}

	new_context->context = ucontext_ptr;
	new_context->threadID = ID;
	makecontext(ucontext_ptr, (void (*)()) func_complete,
		2, func, arg);

	return(0);
}

int thread_libinit(thread_startfunc_t func, void *arg) {
	interrupt_disable();
	initCount++;
	if (check_init() == -1) {
	    initCount--;
		interrupt_enable();
		return -1;
	}

	if(thread_start(func, arg, &current_context) == -1){
		initCount--;
		interrupt_enable();
		return -1;
	}
	setcontext(current_context.context);

	exit(0);
}
int thread_create(thread_startfunc_t func, void *arg) {
	interrupt_disable();
	if (check_init() == -1) {
		interrupt_enable();
		return -1;
	}

	thread_context new_context;
	if (thread_start(func, arg, &new_context) == -1) {
		interrupt_enable();
		return -1;
	}

	ready_contexts.push(new_context);
	interrupt_enable();
	return(0);
}

int thread_yield(void) {

	interrupt_disable();
	if (check_init() == -1) {
		interrupt_enable();
		return -1;
	}
	if (!ready_contexts.empty()) {
		thread_context old_context = current_context;
		ready_contexts.push(current_context);

		current_context = ready_contexts.front();
		ready_contexts.pop();
		swapcontext(old_context.context, current_context.context);
	}
	interrupt_enable();
	return(0);
}

int lock_func(unsigned int lock) {
    if (check_init() == -1) {
		return -1;
	}

	if (locks.count(lock) && locks.at(lock) == current_context.threadID){
		return(-1);
	}

	if (locks.count(lock) < 1) {

		locks.insert(std::make_pair(lock,current_context.threadID));
		try {
			blocked_contexts.insert(std::make_pair(lock,
				new std::queue<thread_context>));
		}
		catch (std::bad_alloc& ba) {
			return -1;
		}
	}
	else {

		std::queue<thread_context>* lock_blocked_contexts = blocked_contexts.at(lock);
		lock_blocked_contexts->push(current_context);

		thread_context old_context = current_context;

		if(ready_contexts.empty()){
			interrupt_enable();
			std::cout<<"Thread library exiting.\n";
			exit(0);
		}
		else{
			current_context = ready_contexts.front();
			ready_contexts.pop();
		}
		swapcontext(old_context.context, current_context.context);
	}
	return 0;
}

int thread_lock(unsigned int lock) {
	interrupt_disable();
    if (lock_func(lock) == -1) {
        interrupt_enable();
        return -1;
    }
	interrupt_enable();
	return(0);
}

int thread_unlock(unsigned int lock) {
	interrupt_disable();
	if (check_init() == -1) {

		interrupt_enable();
		return -1;
	}
	if (!locks.count(lock) || locks.at(lock)!=current_context.threadID) {

		interrupt_enable();
		return -1;
	}

	std::queue<thread_context>* freed_contexts = blocked_contexts.at(lock);

	if (!freed_contexts->empty()) {
		thread_context next_context = freed_contexts->front();
		ready_contexts.push(next_context);
		freed_contexts->pop();
		locks.at(lock) = next_context.threadID;
	}
	else {
		locks.erase(lock);
		blocked_contexts.erase(lock);
		delete freed_contexts;
	}
	interrupt_enable();
	return(0);
}

int thread_wait(unsigned int lock, unsigned int cond) {
	interrupt_disable();
	if (check_init() == -1) {
		interrupt_enable();
		return -1;
	}

	if (locks.count(lock)<1 || locks.at(lock) != current_context.threadID) {
		interrupt_enable();
		return -1;
	}

	std::queue<thread_context>* freed_contexts = blocked_contexts.at(lock);

	if (!freed_contexts->empty()) {
		thread_context next_context = freed_contexts->front();
		locks.at(lock) = next_context.threadID;
		ready_contexts.push(next_context);
		freed_contexts->pop();
	}
	else {
		locks.erase(lock);
		blocked_contexts.erase(lock);
		delete freed_contexts;
	}


	if (waiting_contexts.count(cond) < 1) {
		try {
			waiting_contexts.insert(std::make_pair(cond,
				new std::queue<thread_context>));
		}
		catch (std::bad_alloc& ba) {
			interrupt_enable();
			return -1;
		}
	}
	std::queue<thread_context>* condition_contexts =
		waiting_contexts.at(cond);
	condition_contexts->push(current_context);

	thread_context prev_context = current_context;

	if(ready_contexts.empty()){
		interrupt_enable();

		std::cout<<"Thread library exiting.\n";
		exit(0);
	}
	else{
		current_context = ready_contexts.front();
		ready_contexts.pop();
	}

	swapcontext(prev_context.context, current_context.context);
    if (lock_func(lock) == -1) {
        interrupt_enable();
        return -1;
    }
    interrupt_enable();
	return(0);
}

int thread_signal(unsigned int lock, unsigned int cond) {
	interrupt_disable();
	if (check_init() == -1) {
		interrupt_enable();
		return -1;
	}
	if (waiting_contexts.count(cond) > 0) {
		std::queue<thread_context>* condition_contexts =
			waiting_contexts.at(cond);

		ready_contexts.push(condition_contexts->front());
		condition_contexts->pop();

		if (condition_contexts->empty()) {
			waiting_contexts.erase(cond);
			delete condition_contexts;
		}
	}
	interrupt_enable();
	return(0);
}

int thread_broadcast(unsigned int lock, unsigned int cond) {
	interrupt_disable();
	if (check_init() == -1) {
		interrupt_enable();
		return -1;
	}

	if (waiting_contexts.count(cond) > 0) {
		std::queue<thread_context>* condition_contexts =
			waiting_contexts.at(cond);

		while (!condition_contexts->empty()) {
			ready_contexts.push(condition_contexts->front());
			condition_contexts->pop();
		}

		waiting_contexts.erase(cond);
		delete condition_contexts;

	}
	interrupt_enable();
	return(0);
}
