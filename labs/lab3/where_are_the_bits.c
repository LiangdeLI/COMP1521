// where_are_the_bits.c ... determine bit-field order
// COMP1521 Lab 03 Exercise
// Written by ...

#include <stdio.h>
#include <stdlib.h>

struct _bit_fields {
   unsigned int a : 23,
                b : 8,
                c : 1;
};

typedef struct _bit_fields Fields;

union fields {
   unsigned int i;
   Fields bits;
};

int main(void)
{
   union fields f;
   f.bits.a = 1;
   f.bits.b = 1;
   f.bits.c = 1;
   unsigned int mask = 0b01<<31;
   //printf("Is a at begin? ");
   unsigned int result = f.i & mask;
   int i;
   for(i=0; i<32; i++)
   {
      result = f.i & mask;
      if (result) printf("1");
      else printf("0");
      mask = mask>>1; 
   }
   printf("\n");
   //if (result) printf("Yes\n");
   //else printf("No\n");
   
   //printf("%ul\n",sizeof(x));

   return 0;
}
