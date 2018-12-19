// Bug
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=14932

#include <stdio.h>
#include <stddef.h>
#include <iostream>
using namespace std;

struct TestStruct {
  int array[13];
};

struct TempStruct {
  int index;
};

int array_offset(struct TempStruct *index)
{
  return offsetof(struct TestStruct, array[index->index]);
}

int main(int argc, char **argv)
{
  struct TempStruct tmp = {3};
  printf("Offset of array[3] is %d.\n", array_offset(&tmp));
}
