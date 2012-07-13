#include <iostream>
#include <stdlib.h>
#include <stdio.h>

int main()
{
  const char*s = "06";
  int c = strtoul(s, NULL, 16);
  
  //printf("c = %d",c);
  std::cout << "c is " << c << std::endl;
}
