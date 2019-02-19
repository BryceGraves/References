#include <linux/kernel.h>
#include <stdint.h>
#include <signal.h>
//#include<linux/module.h> 


atomic_t *test;

int main(int argc, char *argv[])
{
  
}

int test_init(void) 
{
  atomic_set(test,2);
  atomic_add(2,test);
}


void test_exit(void)
{
}
module_init(test_init);
module_exit(test_exit);
