#include <fstream>
#include "thread.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
int orderBoardLock = 1;

int posted = 2834;
int postedlast = 48420;
int finished =29348;
int notposted = 0;
int madeSandwich = 90234;
int fullBoard = 2459872;
int notFullBoard=999999;
int addedStuff = 234747;

int orderSize;
int numthreads;
int recentCash=-1;

std::vector<int> orders;
std::vector<int> cashiers;
std::vector<int> activeCashiers;

int debugMath = 2;


//simple assignemtn wrapper to match cashier ID with a file to take
//orders from 
struct Assignment{
	int cnum;
	char* file;
};

void printOrders(){
	std::cout<< "Orders"<<std::endl;
	for(int i = 0; i<orders.size(); i++){
		std::cout << orders.at(i) << std::endl;
	}
	std::cout<< "Cashiers"<<std::endl;
	for(int i = 0; i<cashiers.size(); i++){
		std::cout << cashiers.at(i) << std::endl;
	}
	std::cout<< "ActiveCashiers"<<std::endl;
	for(int i = 0; i<activeCashiers.size(); i++){
		std::cout << activeCashiers.at(i) << std::endl;
	}
}

//checks to see if all cashiers are finished
bool allDone(){
	for(int i = 0; i<activeCashiers.size(); i++){
		if(activeCashiers.at(i)!=finished){
			return false;
		}
	}
	return true;
}

//checks to see if any active cashiers have not posted
bool allPosted(){
	//printOrders();
	for(int i=0; i<activeCashiers.size(); i++){
		if(activeCashiers.at(i) == notposted){
			return false;
		}
	}
	return true;
}

void takeOrders(Assignment* file){
  //thread_wait(orderBoardLock, 1);

  std::ifstream infile(file->file);
  std::vector<int> myOrders;

  int myID = file->cnum;
  int myOrder;

  while (infile >> myOrder){
		myOrders.push_back(myOrder);
		
	}
 	
 	for(int i = 0; i < myOrders.size(); i++){

		thread_lock(orderBoardLock);
		
	    while(orders.size() == orderSize){
	    	thread_wait(orderBoardLock, notFullBoard);
	    }
	    
	    myOrder = myOrders.at(i);
	    if(i == myOrders.size()-1){
	    	activeCashiers.at(myID)=postedlast;
	    }
	    else{
	    	activeCashiers.at(myID)=posted;
	    }
	   	
	    orders.push_back(myOrder);
	    cashiers.push_back(myID);


        std::cout << "POSTED: cashier " << myID << " sandwich " << myOrder << std::endl; 

        thread_signal(orderBoardLock, fullBoard);
	    thread_wait(orderBoardLock, myID);
	    thread_unlock(orderBoardLock);
	}
	//printOrders();

	//orderSize= orderSize-1;
	//std::cerr<<"Cashier done :" << myID<<std::endl;

	//thread_broadcast()
}

void makeOrders(void * blah){
//	printOrders();
	int recentMake = -1;
	thread_lock(orderBoardLock);
	

	while(orders.size()>0 || !allDone()){
	//	printOrders();

		//std::cout<<"in main loop on maker"<<std::endl;
		 while(orders.size()!=orderSize && !allPosted()){
		 	//std::cout<<"waiting"<<std::endl;
		 	// 		printOrders();

		 	//std::cerr<<"orders is :" << orders.size()<<" deisgnated size: "<<orderSize<<std::endl;
		 	thread_wait(orderBoardLock, fullBoard);

		 }
			//	printOrders();

		int closest = -80000;
		int index = -1;
		// if(recentMake == -1){
		// 	//closest = orders.at(0);
		// 	//index = 0;
		// }
		//else{
			for(int i=0; i<orders.size(); i++){
				if(abs(recentMake-orders.at(i)) < abs(recentMake - closest)){
					closest = orders.at(i);
					index=i;
				}
			}
		//}
	
		recentMake = closest;
		recentCash = cashiers.at(index);
		//std::cout<<recentCash <<std::endl;
		
		if(activeCashiers.at(recentCash) == posted){
			activeCashiers.at(recentCash) = notposted;
		}
		else{
			activeCashiers.at(recentCash) = finished;
		}
			
		
        std::cout << "READY: cashier " << cashiers.at(index) << " sandwich " << closest << std::endl; 

		orders.erase(orders.begin()+index);
		cashiers.erase(cashiers.begin()+index);
	
		thread_signal(orderBoardLock, notFullBoard);
		thread_signal(orderBoardLock, recentCash);
		//thread_wait(orderBoardLock, fullBoard);
  
	}

	thread_unlock(orderBoardLock);

}


void start(void * a){
	//std::cout<<"start called"<<std::endl;
	char **argv = (char**)a;
	
	if(debugMath == 3){
		srand(time(0));
		int r = atoi(argv[2]);
		start_preemptions(true, true, r);
	}

	thread_lock(orderBoardLock);
	for(int i=0; i<numthreads; i++){

		char* file = argv[i+debugMath];
		Assignment*temp = new Assignment;
		temp->cnum = i;
		temp->file = file; 
		activeCashiers.push_back(0);
		thread_create((thread_startfunc_t)takeOrders,temp);
		
	}
	thread_unlock(orderBoardLock);
	//std::cout<<"unlocking parent"<<std::endl;
	makeOrders(a);
	 //return 0;
}
int main(int argc, char *argv[]){
	//std::cout<<"called"<<std::endl;
	// THIS IS ONLY FOR DEBUGGING PURPOSES COMMENT BEFORE SUBMITTING
	debugMath = 3;


	numthreads = argc-debugMath;
	orderSize = atoi(argv[1]);
	//std::cerr << "There are: "<<argc <<"Order SIze: "<< orderSize <<std::endl;
	
	thread_libinit((thread_startfunc_t)start, argv);
	return 0;
}
