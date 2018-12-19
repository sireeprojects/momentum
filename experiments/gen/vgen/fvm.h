#ifndef __FVM__
#define __FVM__


#include <stdio.h>
#include <stdint.h>


enum fvm_t {
   increment,
   decrement,
   fixed,
   rnd
};


// increment in a range
// decrement in a range
// random in a range
// step through a list, forward
// step through a list, backward
// step through a list, reverse
// waht happens if the values exhaust, loop and start from the beginign


struct fvm_s {
   uint32_t start;
   uint32_t type;
   uint32_t step;
   uint32_t repeat_count;
   uint32_t init_value;
   uint32_t value;
   /* internal */
   uint32_t prev_value;
   uint32_t loop;
   uint32_t tmp;
};


uint64_t get_next_value (struct fvm_s *spec)
{
   if (spec->start==1) {
      spec->prev_value = spec->init_value;
      spec->start=0;
   }

   switch (spec->type)
   {
      case increment:{
         spec->tmp = spec->prev_value + ((spec->start==1)?0:spec->step);
         break;
         };

      case decrement:{
         spec->tmp = spec->prev_value - ((spec->start==1)?0:spec->step);
         break;
         };

      case fixed:{
         spec->tmp = spec->value;
         break;
         };

      case rnd:{
         spec->tmp = rand();
         break;
         };
   }
   spec->loop = spec->loop+1;
   spec->prev_value = spec->tmp;
   return spec->tmp;
}


#endif
