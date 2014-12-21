#include <thread>
#include <string>
#include <iostream>
#include <unistd.h>
#include "stdio.h"

using namespace std;

void task1(string msg)
{
	for(int i=0;i<=100;i++)
	{
	//cout <<"task1 say:" <<msg;
	sleep(1);
	cout.flush();
	}
}
void task2(string msg)
{
	cout << "task 2:" << msg;
	string input;
do{
	cout << "Nhap vao mot lenh:! ";
	//scanf("%s",&input);
	getline(cin,input);
}while(input == 'q');
}
int main()
{
thread t1(task1,"Hello\n");
thread t2(task2,"...");
t1.join();
t2.join();
return 0;
}
