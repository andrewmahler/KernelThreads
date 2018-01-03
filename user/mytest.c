// Do not modify this file. It will be replaced by the grading scripts
// when checking your project.

#include "types.h"
#include "stat.h"
#include "user.h"

void
testfunc(void* arg){
    printf(1, "hello we are in the tester function!!\n");
}


int
main(int argc, char *argv[])
{
    int x = 6;
    void* arg = &x; 
    thread_create(&testfunc, arg);
    thread_join();
    // join(NULL);
    exit();
}
