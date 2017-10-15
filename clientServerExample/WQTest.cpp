#include <iostream>
#include <stdint.h>
#include <pthread.h>

#include "ThreadWorkQueue.h"

using namespace std;

ThreadWorkQueue<int> workQueue;

pthread_mutex_t _outputSync;


void* workProcedure(void* arg)
{
   int work;
   workQueue.getWork(work);

   pthread_mutex_lock(&_outputSync);
   cout << "Work:" << work << endl;
   pthread_mutex_unlock(&_outputSync);
   return NULL;
}


int main()
{

   pthread_mutex_init(&_outputSync,NULL);
  
   const int nThreads = 100;
   pthread_t threads[nThreads];
   for(uint8_t i=0; i < nThreads; i++)
   {
      pthread_create(&threads[i],NULL,workProcedure,NULL);
   }

   int x;
   cin >> x;

   //now create some work

   for(uint8_t i=0; i < 10; i++)
   {
      workQueue.insertWork(i);
   }
   cin >> x;

   for(uint8_t i=0; i < 10; i++)
   {
      workQueue.insertWork(i+20);
   }
   cin >> x;

   
}
