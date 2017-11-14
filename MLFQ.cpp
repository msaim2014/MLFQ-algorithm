#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;

//*****************************************************************************
//P R O G R A M  H E A D E R
//  Name:  Muhammad Saim
//	Z#:  23306519
//	Instructor:  Dr.Tami Sorgente
//	Class:  Computer Operating System
//	Term:  Fall 2017
//	Scheduling Algorithm Project
//	Due Date:   November 13, 2017
//	Due Time:  11:59PM
//	Points:  40
//	
//  Description: This program simulates Multi Level Feedback Queue (MLFQ) The 
//		MLFQ algorithm runs the highest priority process in the ready queue. 
//		Each priority has its own Time Quantum (TQ) that indicates how long the 
//		process will run for. For this simulation Queue1 used RR scheduling with
//		TQ=6, Queue2 used RR scheduling with TQ=12, and Queue3 used FCFS. Every
//		time a process is preempted it will go back to the ready queue with its 
//		remaining CPU time left. Although when preempted for TQ it will go back 
//		to the ready queue with its remaining CPU time left and the process will 
//		have its priority reduced
//******************************************************************************

class process
{
public:
	string name;
	int priority;
	int CPUBurstAndIO[100];
	int currentCPUBurst;
	int currentIO;
	int responseTime;
	int waitTime;
	int turnAroundTime;
	process *previous;
	process *next;
};

class Algorithm
{
public:
	Algorithm();
	void initializeQueue(process initial[], int elements);
	void sendToQueue(process current);
	void sendToReady(process current);
	process current();
	process last();
	void print(process current, int time);
	void printReadyQueue();
	void printIOQueue();
	void deQueue();
	void deQueueSpecific(process transfer);
	void adjustQueue(int IO);
	void readyQueuePrint();
	void IOQueuePrint();
	bool isEmpty();
	void reduce();
	bool transfer();
	process getFinishIO();
	friend bool isCompleted(process transfer);
	bool hasOne();
	int checkPriority(process current);
	bool preempt(process current);
	process increasePriority(process current, int priorityTime);
	process searchHigherPriority(process current);
	bool isHigher(process current);
	process decreaseBurst(process previous, int i);
	bool isFinished(process previous, int time);
	void setResponseTime(int time, process now);
	void computeWaitTime(process initial[]);
	void printFinished();
	void printCompletedQueue();
private:
	process *front;
	process *back;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: Algorithm()
//Description: This is the default constructor that is used to initialize the doubly linked list.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
Algorithm::Algorithm() {
	front = 0;
	back = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: initializeQueue
//Description: This function is used to create a linked list from an array of processes.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::initializeQueue(process readyQueue[], int elements) {
	int i = 0;
	//finding the size of the array
	int array[] = { 1,2,3,4,5,6,7,8 };
	int num2 = sizeof(array) / sizeof(*array);
	int num = sizeof(readyQueue) / sizeof(*readyQueue);
	process *created;
	do {
		//initializing the linked list
		if (isEmpty()) {
			created = new process;
			created->name = readyQueue[i].name;
			created->priority = readyQueue->priority;
			for (int j = 0; readyQueue[i].CPUBurstAndIO[j] != 0; j++) {
				created->CPUBurstAndIO[j] = readyQueue[i].CPUBurstAndIO[j];
			}
			created->currentCPUBurst = readyQueue[i].currentCPUBurst;
			created->currentIO = readyQueue[i].currentIO;
			created->responseTime = readyQueue[i].responseTime;
			created->waitTime = readyQueue[i].waitTime;
			created->turnAroundTime = readyQueue[i].turnAroundTime;
			front = back = created;
			i++;
		}
		//adding a node to the back
		else {
			created = new process;
			back->next = created;
			created->previous = back;
			back = created;
			created->name = readyQueue[i].name;
			created->priority = readyQueue->priority;
			for (int j = 0; readyQueue[i].CPUBurstAndIO[j] != 0; j++) {
				created->CPUBurstAndIO[j] = readyQueue[i].CPUBurstAndIO[j];
			}
			created->currentCPUBurst = readyQueue[i].currentCPUBurst;
			created->currentIO = readyQueue[i].currentIO;
			created->responseTime = readyQueue[i].responseTime;
			created->waitTime = readyQueue[i].waitTime;
			created->turnAroundTime = readyQueue[i].turnAroundTime;
			i++;
		}
	} while (i < elements);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: deQueue
//Description: This function deletes the front of the queue.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::deQueue() {
	if (isEmpty()) {
		return;
	}
	//deleting when there is one node
	else if (front->next == 0) {
		delete front;
		front = 0;
		back = 0;
	}
	//deleting when there are many nodes
	else {
		process *temp = front;
		front = front->next;
		front->previous = 0;
		delete temp;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: deQueueSpecific
//Description: This function deletes a specific element from the linked list.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::deQueueSpecific(process transfer) {
	process *p = front;
	if (isEmpty()) {
		return;
	}
	//deleting when there is one node
	else if (front->next == 0 && p->name == transfer.name) {
		delete p;
		front = 0;
		back = 0;
	}
	else {
		//finding the correct node
		for (p = front; p != 0; p = p->next) {
			//if the node is in the front
			if (p == front && p->name == transfer.name) {
				process *q = p->next;
				q->previous = 0;
				front = q;
				delete p;
				p = q;
				break;
			}
			//if it's in the back
			else if (p == back && p->name == transfer.name) {
				process *q = p->previous;
				q->next = 0;
				back = q;
				delete p;
				p = q;
				break;
			}
			//if it's in the middle
			else if (p->name == transfer.name) {
				process *q = p->previous;
				q->next = p->next;
				q->next->previous = q;
				delete p;
				p = q;
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: sendToQueue
//Description: This function sends a process to the back of the queue
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::sendToQueue(process current) {
	process *created;
	//if there are no nodes
	if (isEmpty()) {
		created = new process;
		created->name = current.name;
		created->priority = current.priority;
		for (int j = 0; current.CPUBurstAndIO[j-1] != -1; j++) {
			created->CPUBurstAndIO[j] = current.CPUBurstAndIO[j];
		}
		created->currentCPUBurst = current.currentCPUBurst;
		created->currentIO = current.currentIO;
		created->responseTime = current.responseTime;
		created->waitTime = current.waitTime;
		created->turnAroundTime = current.turnAroundTime;
		front = back = created;
	}
	//adding to the back
	else {
		created = new process;
		back->next = created;
		created->previous = back;
		back = created;
		created->name = current.name;
		created->priority = current.priority;
		for (int j = 0; current.CPUBurstAndIO[j-1] != -1; j++) {
			created->CPUBurstAndIO[j] = current.CPUBurstAndIO[j];
		}
		created->currentCPUBurst = current.currentCPUBurst;
		created->currentIO = current.currentIO;
		created->responseTime = current.responseTime;
		created->waitTime = current.waitTime;
		created->turnAroundTime = current.turnAroundTime;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: sendToReady
//Description: This function inserts a node into the ready queue based on priority
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::sendToReady(process current) {
	process *created;
	//initializing the values of created
	created = new process;
	created->name = current.name;
	created->priority = current.priority;
	for (int j = 0; current.CPUBurstAndIO[j-1] != -1; j++) {
		created->CPUBurstAndIO[j] = current.CPUBurstAndIO[j];
	}
	created->currentCPUBurst = current.currentCPUBurst;
	created->currentIO = current.currentIO;
	created->responseTime = current.responseTime;
	created->waitTime = current.waitTime;
	created->turnAroundTime = current.turnAroundTime;

	//setting created in empty list
	if (isEmpty()) {
		front = back = created;
	}
	//setting created when there is one node
	else if (front->next == 0 && front->priority>created->priority) {
		front->previous = created;
		created->next = front;
		front = front->previous;
		front->previous = 0;
	}
	else {
		//finding where to insert created
		process *point = back;
		while(point->priority>created->priority && point!=front){
			point = point->previous;
		}
		//inserting in the back
		if (point == back) {
			back->next = created;
			created->previous = back;
			back = back->next;
			back->next = 0;
		}
		//inserting in the front
		else if (point == front && point->priority>created->priority) {
			front->previous = created;
			created->next = front;
			front = front->previous;
			front->previous = 0;
		}
		//inserting in the middle
		else {
			created->previous = point;
			created->next = point->next;
			point->next = created;
			created->next->previous = created;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: isEmpty
//Description: This function is used to see if the linked list has nodes in it.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Algorithm::isEmpty() {
	if (front == 0) {
		return true;
	}
	else {
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: readyQueuePrint
//Description: This function prints the readyQueue. It informs the user of the process name and CPU burst.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::readyQueuePrint() {
	process *i = front;
	int p = 0;
	if (front == 0) {
		cout << "             [Empty]" << endl;
	}
	else {
		//printing the ready queue
		for (i = front; i != back->next; i = i->next) {
			cout <<"             "<<left<< setw(9) <<i->name <<setw(9)<< i->CPUBurstAndIO[i->currentCPUBurst] <<i->priority<< endl;
			//cout << "               " << i->name << "      " << i->CPUBurstAndIO[i->currentCPUBurst] << "      " << i->priority << endl;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: IOQueuePrint
//Description:This function prints the IOQueue. It informs the user of the process name and current IO
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::IOQueuePrint() {
	process *i = front;
	int p = 0;
	if (front == 0) {
		cout << "             [Empty]" << endl;
	}
	else {
		//printing the IO queue
		for (i = front; i != back->next; i = i->next) {
			cout << "               " <<left<<setw(9)<< i->name <<setw(5)<< "      " << i->currentIO << endl;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: print
//Description: This function prints the current time and name of the process
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::print(process current, int time) {
	cout << "Current Time: " << time << endl;
	cout << "Now Running: " << current.name << endl;
	cout << "---------------------------------------------------------" << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: printReadyQueue
//Description: This function sets up the display for printing the ready queue.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::printReadyQueue() {
	cout << "Ready Queue: Process  Burst  Queue: " << endl;
	readyQueuePrint();
	cout << endl;
	cout << "---------------------------------------------------------" << endl;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: printIOQueue
//Description: This function sets up the display for printing the IO queue.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::printIOQueue() {
	cout << "Now in I/O:  Process  Remaining I/O Time" << endl;
	IOQueuePrint();
	cout << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: printFinished
//Description: This function calculates the average Wait, Response, and Turnaround times. Then it prints 
//			   all the Wait, Response, and Turnaround times for each process with the averages 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::printFinished() {
	double avgWait = 0;
	double avgResponse = 0;
	double avgTurnaround = 0;
	//calculating the averages
	for (process *q = front; q != 0; q = q->next) {
		avgWait = avgWait + q->waitTime;
		avgResponse = avgResponse + q->responseTime;
		avgTurnaround = avgTurnaround + q->turnAroundTime;
	}
	avgWait = avgWait / 8;
	avgResponse = avgResponse / 8;
	avgTurnaround = avgTurnaround / 8;
	//printing the final results of wait, response, and turnaround times with averages
	cout << "Process      Wait Time   Turnaround Time   Response Time" << endl;
	for (process *p = front; p != 0; p = p->next) {
		cout << left << setw(17) << p->name << setw(14) << p->waitTime << setw(14) << p->turnAroundTime << p->responseTime << endl;
	}
	cout << "Averages         " << left << setw(14) << avgWait << setw(14) << avgTurnaround << avgResponse << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: current
//Description: This function returns the first element of the queue. 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
process Algorithm::current() {
	process IDLE{ "IDLE" };
	if (front == 0) {
		return IDLE;
	}
	process *p = front;
	return *p;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: last
//Description: This function returns the last element of the queue.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

process Algorithm::last() {
	process *p = back;
	return *p;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: transfer
//Description: This function returns true if there is a process in IO queue that has completed it's IO time.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Algorithm::transfer() {
	process *p = front;
	if (isEmpty() == false) {
		//finding the process that has finished it's IO time
		for (p = front; p != 0; p = p->next) {
			if (p->currentIO == 0) {
				return true;
			}
		}
		return false;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: getFinishIO
//Description: This function returns the process in IO queue that has finished it's IO time.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
process Algorithm::getFinishIO() {
	//finds the process that has finished it's IO
	for (process *p = front; p != 0; p = p->next) {
		if (p->currentIO == 0) {
			return *p;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: reduce
//Description: This function reduces the current IO of all the process in IO Queue by one.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::reduce() {
	if (front == 0) {
		return;
	}
	//decrements the current IO of one element list
	else if (front->next == 0) {
		front->currentIO--;
	}
	else {
		//decrements all the current IOs of the list
		for (process *p = front; p != 0; p = p->next) {
			p->currentIO--;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: adjustQueue
//Description: This function sets the current IO and determines what the current CPU burst is.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::adjustQueue(int IO) {
	process *point = back;
	//updates the current IO and current CPU burst of the process
	point->currentIO = IO;
	point->currentCPUBurst = point->currentCPUBurst + 2;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: preempt
//Description: This function returns true if the current burst of the process is greater than the TQ time.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Algorithm::preempt(process current) {
	int num = current.priority;
	int priorityTimes[] = { 0, 6, 12, current.CPUBurstAndIO[current.currentCPUBurst] };
	num = priorityTimes[num];
	int cnum = current.CPUBurstAndIO[current.currentCPUBurst];
	
	if (cnum < num) {
		return false;
	}
	else if (cnum == num) {
		return false;
	}
	//returns true because the process was preempted
	else {
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: increasePriority
//Description: This function returns the process, but if the process was preempted for TQ then the priority
//				will be increased.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
process Algorithm::increasePriority(process current,int priorityTime) {
	int num = current.priority;
	int priorityTimes[] = { 0, 6, 12, current.CPUBurstAndIO[current.currentCPUBurst] };
	num = priorityTimes[num];
	int cnum = current.CPUBurstAndIO[current.currentCPUBurst];

	if (cnum < num) {
		return current;
	}
	else if (cnum == num) {
		return current;
	}
	//preemption for time quantum increases the priority of the process
	else {
		cnum = cnum - priorityTime;
		current.CPUBurstAndIO[current.currentCPUBurst]=cnum;
		if (current.priority != 3) {
			current.priority = current.priority + 1;
		}
		return current;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: decreaseBurst
//Description: This function decreases the burst time of a process after it has been preempted(remaining burst time).
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
process Algorithm::decreaseBurst(process previous, int i) {
	previous.CPUBurstAndIO[previous.currentCPUBurst] = i;
	return previous;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: checkPriority
//Description: This function returns the burst time of the process depending on priority.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Algorithm::checkPriority(process current) {
	int num = current.priority;
	int priorityTimes[] = { 0, 6, 12, current.CPUBurstAndIO[current.currentCPUBurst]};
	num = priorityTimes[num];
	int cnum = current.CPUBurstAndIO[current.currentCPUBurst];
	if ( cnum < num ) {
		return cnum;
	}
	else if (cnum == num) {
		return cnum;
	}
	//gives the time of how long the process is going to run for
	else {
		return num;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: isCompleted
//Description:  This function returns true if a process has completed a burst
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool isCompleted(process transfer) {
	if (transfer.CPUBurstAndIO[transfer.currentCPUBurst] == 0) {
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: hasOne
//Description: This function returns true if there is only one node in the linked list.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Algorithm::hasOne() {
	if (front != 0 && front->next == 0) {
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: searchHigherPriority
//Description: This function returns the process in the ready queue that has a higher priority. (if there is one)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
process Algorithm::searchHigherPriority(process current) {
	if (isEmpty()) {
		return current;
	}
	//the front overrides the current process
	if (current.priority > front->priority) {
		return *front;
	}
	else {
		return current;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: isHigher
//Description: This function returns true if there is a process in the ready queue that has a higher priority.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Algorithm::isHigher(process current) {
	if (front == 0) {
		return false;
	}
	//the front overrides the current process
	else if (current.priority > front->priority) {
		return true;
	}
	else {
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: isFinished
//Description: This function checks to see if the process has completed all bursts. If it has then it will
//				set the turnaround time and return true.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Algorithm::isFinished(process previous, int time) {
	process *p = front;
	//checks to see if the last CPU burst has been reached
	if (previous.CPUBurstAndIO[(previous.currentCPUBurst)+1] == -1) {
		while (p->name != previous.name) {
			p = p->next;
		}
		//sets the turnaround time to the current time
		p->turnAroundTime = time;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: setResponeTime
//Description: This function sets the response time to the current time if it has not already been set.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::setResponseTime(int time, process now) {
	process *p = front->next;
	while (p != 0 && p->name != now.name) {
		p = p->next;
	}
	if (p == 0) {
		return;
	}
	//sets the response time if it has not already been set
	if (p->responseTime == 0) {
		p->responseTime = time;
	}
	else {
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: computeWaitTime
//Description: This function totals all the IO and Burst times and subtracts if from the turnaround time
//				to get the wait time.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::computeWaitTime(process initial[]) {
	int totalBurstAndIO = 0;
	int burstOrIO = 0;
	//totals up all the bursts and IO times of a process
	for (process *i = front; i != back->next; i = i->next) {
		totalBurstAndIO = 0;
		for (int j = 0; i->CPUBurstAndIO[j] != -1; j++) {
			burstOrIO = i->CPUBurstAndIO[j];
			totalBurstAndIO = totalBurstAndIO + burstOrIO;
		}
		//sets the wait time
		i->waitTime = i->turnAroundTime - totalBurstAndIO;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name: printCompletedQueue
//Description: This function prints all the processes that have been completed. 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Algorithm::printCompletedQueue() {
	if (front == 0) {
		cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
		return;
	}
	//prints all the completed processes in the order they finish
	else {
		cout << "---------------------------------------------------------" << endl;
		cout << "Completed: ";
		for (process *p = front; p != 0; p = p->next) {
			cout << p->name << " ";
		}
	}
	cout << endl << endl << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
}

int main() {
	//the different queues
	Algorithm Finished;
	Algorithm Completed;
	Algorithm ReadyQueue;
	Algorithm IOQueue;
	process IO;
	int time = 0;
	//hard coded process values
	process p1{ "P1", 1, { 4, 15, 5, 31, 6, 26, 7, 24, 6, 41, 4, 51, 5, 16, 4, -1}, 0, 0 };
	process p2{ "P2", 1, { 9, 28, 11, 22, 15, 21, 12, 28, 8, 34, 11, 34, 9, 29, 10, 31, 7, -1 }, 0, 0 };
	process p3{ "P3", 1, { 24, 28, 12, 21, 6, 27, 17, 21, 11, 54, 22, 31, 18, -1 }, 0, 0 };
	process	p4{ "P4", 1, { 15, 35, 14, 41, 16, 45, 18, 51, 14, 61, 13, 54, 16, 61, 15, -1 }, 0, 0 };
	process p5{ "P5", 1, { 6, 22, 5, 21, 15, 31, 4, 26, 7, 31, 4, 18, 6, 21, 10, 33, 3, -1 }, 0, 0 };
	process p6{ "P6", 1, { 22, 38, 27, 41, 25, 29, 11, 26, 19, 32, 18, 22, 6, 26, 6, -1 }, 0, 0 };
	process p7{ "P7", 1, { 4, 36, 7, 31, 6, 32, 5, 41, 4, 42, 7, 39, 6, 33, 5, 34, 6, 21, 9, -1 }, 0, 0 };
	process p8{ "P8", 1, { 5, 14, 4, 33, 6, 31, 4, 31, 6, 27, 5, 21, 4, 19, 6, 11, 6, -1 }, 0, 0 };

	//initializing everything
	process initial[] = { p1, p2, p3, p4, p5, p6, p7, p8 };
	int elements = sizeof(initial) / sizeof(*initial);
	ReadyQueue.initializeQueue(initial, elements);
	Finished.initializeQueue(initial, elements);

	//setting the needed variables
	int endTime, j = 0;
	process previous=ReadyQueue.current();
	process now=ReadyQueue.current();
	process transfer;
	int priorityTime = 0;
	bool preempt=false;

	//stops when the ready queue and IO queue are both empty
	while (ReadyQueue.isEmpty() != true || IOQueue.isEmpty() != true) {
		//keeps track of the burst being decremented
		for (int i = ReadyQueue.current().CPUBurstAndIO[j]; i >= 0; i--) {
			//runs initial process P1
			if (time == 0) {
				cout << "MULTI LEVEL FEEDBACK QUEUE" << endl;
				endTime = ReadyQueue.current().CPUBurstAndIO[j];
				previous = ReadyQueue.current();
				ReadyQueue.deQueue();
				ReadyQueue.print(now, time);
				ReadyQueue.printReadyQueue();
				IOQueue.printIOQueue();
				cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
				now = ReadyQueue.current();
				time++;
			}
			else {
				//checks for preemption
				if (ReadyQueue.isHigher(previous) == true) {
					process temp = ReadyQueue.searchHigherPriority(now);
					ReadyQueue.print(temp, time);
					previous = ReadyQueue.decreaseBurst(previous, i);
					//checks to see if the process has finished it's burst
					if (isCompleted(previous) == true) {
						IOQueue.sendToQueue(previous);
						IOQueue.adjustQueue(IOQueue.last().CPUBurstAndIO[(previous.currentCPUBurst) + 1]);
						ReadyQueue.deQueueSpecific(previous);
					}
					else {
						ReadyQueue.sendToReady(previous);
					}
					previous = ReadyQueue.current();
					ReadyQueue.deQueue();
					now = ReadyQueue.current();
					ReadyQueue.printReadyQueue();
					IOQueue.printIOQueue();
					Completed.printCompletedQueue();
					priorityTime = ReadyQueue.checkPriority(previous);
					preempt = ReadyQueue.preempt(previous);
					endTime = time + priorityTime;
					i = previous.CPUBurstAndIO[previous.currentCPUBurst];

				}
				//process has completed it's burst
				if (time == endTime) {
					//check if the process was preempted or not
					if (preempt == true) {
						previous = ReadyQueue.increasePriority(previous,priorityTime);
						ReadyQueue.sendToReady(previous);
					}
					//determines if all the burst have completed
					else if (Finished.isFinished(previous, time) == true) {
						IOQueue.deQueueSpecific(previous);
						Completed.sendToQueue(previous);
					}
					//pushes the process to IO if it was preempted
					else if (isCompleted(previous) == false) {
						IOQueue.sendToQueue(previous);
						j = IOQueue.last().currentCPUBurst;
						IOQueue.adjustQueue(IOQueue.last().CPUBurstAndIO[j + 1]);
					}
					now = ReadyQueue.current();
					//breaks out so the program can finish
					if (now.name == "IDLE" && ReadyQueue.isEmpty() == true && IOQueue.isEmpty() == true) {
						cout << "Current Time: " << time << endl;
						cout << "DONE!" << endl << endl;
						Completed.printCompletedQueue();
						break;
					}
					Finished.setResponseTime(time, now);
					ReadyQueue.print(now, time);
					previous = ReadyQueue.current();
					ReadyQueue.deQueue();
					ReadyQueue.printReadyQueue();
					IOQueue.printIOQueue();
					Completed.printCompletedQueue();
					priorityTime = ReadyQueue.checkPriority(now);
					preempt = ReadyQueue.preempt(now);
					endTime = time + priorityTime;
					i = now.CPUBurstAndIO[now.currentCPUBurst];
				}
				time++;
				IOQueue.reduce();
				//forever loop to find the processes that have completed their IO time 
				//and transfers them to the ready queue
				for (;;) {
					if (IOQueue.transfer() == true) {
						transfer = IOQueue.getFinishIO();
						IOQueue.deQueueSpecific(transfer);
						ReadyQueue.sendToReady(transfer);
						if (ReadyQueue.hasOne() && i == 0) {
							if (now.name == "IDLE") {
								endTime = time;
							}
							else {
								endTime = time + transfer.CPUBurstAndIO[transfer.currentCPUBurst];
							}
						}
					}
					else {
						break;
					}
				}
			}
		}
	}
	//calculates the CPU utilization
	double utilization = 0;
	for (int i = 0; i <= elements - 1; i++)
		for (int j = 0; initial[i].CPUBurstAndIO[j] > 0; j = j + 2) {
			utilization = initial[i].CPUBurstAndIO[j] + utilization;
		}
	//prints the final results of the simulation
	utilization = (utilization / (time - 1)) * 100;
	cout << "CPU utilization is: " << utilization << endl;
	Finished.computeWaitTime(initial);
	Finished.printFinished();
	return 0;
}