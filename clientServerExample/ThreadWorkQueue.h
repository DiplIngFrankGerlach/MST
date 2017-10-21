/*********************************************************************************
* A work queue for multithreaded operation
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/

#ifndef THREAD_WORK_QUEUE
#define THREAD_WORK_QUEUE7

#include <pthread.h>
#include <queue>

using namespace std;

/*a work queue which can be used to provide work for many threads*/
template<class Payload>
class ThreadWorkQueue
{
   pthread_mutex_t _mutex;
   pthread_cond_t _ptCondition;

   queue<Payload> _payloads;
public:
   ThreadWorkQueue()
   {
      pthread_mutex_init(&_mutex,NULL);
      pthread_cond_init(&_ptCondition,NULL);
   }

   /* remove work from the queue in a multithread-safe way */
   void getWork(Payload& payload)
   {
      pthread_mutex_lock(&_mutex);
      pthread_cond_wait(&_ptCondition,&_mutex);
     
      payload = _payloads.front(); 
      _payloads.pop();
     
      pthread_mutex_unlock(&_mutex);
   }

   /* insert into the queue in a multithread-safe way */
   void insertWork(const Payload& pl)
   {
      pthread_mutex_lock(&_mutex);

        _payloads.push(pl);
	   
      pthread_mutex_unlock(&_mutex);
      pthread_cond_signal(&_ptCondition);  
   }
   
   /* destroy queue */
   ~ThreadWorkQueue()
   {
     pthread_cond_destroy(&_ptCondition);
     pthread_mutex_destroy(&_mutex);
   }
};


#endif
