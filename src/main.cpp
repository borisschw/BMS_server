#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>

#include "pthread.h"
#include "bmsServer.h"


using namespace std;

#define NUM_THREADS 5

typedef void * (*THREADFUNCPTR)(void *);

int main () {
   pthread_t thread;
   int rc;
   int i;

   BmsServer *bms = new BmsServer();

   pthread_create(&thread, NULL, (void * (*)( void *)) &BmsServer::read_bms_data, bms);

   while(1)
   {
      cout<<" Doing something "<< "\n";

      sleep(1);
   }

//    for( i = 0; i < NUM_THREADS; i++ ) {
//       cout << "main() : creating thread, " << i << endl;
//       rc = pthread_create(&threads[i], NULL, PrintHello, NULL);//(void *)&i);

//       if (rc) {
//          cout << "Error:unable to create thread," << rc << endl;
//          exit(-1);
//       }
//    }
//   pthread_exit(NULL);
}