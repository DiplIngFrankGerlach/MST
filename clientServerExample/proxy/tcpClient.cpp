#include "BufferedSocket.h"





int main(int argc, char** argv)
{
   if( argc == 2 )
   {
      BufferedSocket bs;
      if( bs.connect(argv[1],1235) )
      {
         uint8_t buffer[11];
         
         if( bs.read(buffer,10) )
         {
            buffer[10]=0;
            cout << ((const char*)buffer) << endl;
         }
      }
       
   }
        
}
