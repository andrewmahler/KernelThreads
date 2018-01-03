#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

#define PG_SIZE 4096

void lock_acquire(lock_t* lock) // Andrew
{
    while(xchg(&lock->flag, 1) == 1);
        //spin-wait, do nothing...
}

void lock_release(lock_t* lock) // Andrew
{
    lock->flag = 0;
}

void lock_init(lock_t* lock) // Andrew 
{
    lock->flag = 0;
}

typedef struct _stackNode{
    int pid;
    void* actualStack;
    struct _stackNode* next;
} stackNode;

stackNode* head = NULL;

void addStackNode(void* actualStack, int pid) {
    stackNode* new = malloc(sizeof(stackNode));
    new->pid = pid;
    new->actualStack = actualStack;
    new->next = NULL;

    stackNode* curr = head;

    if (head == NULL) {
        head = new;
    } else {
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new;
    }
}

void removeStackNode(int pid) {
    stackNode* curr = head;
    if (curr != NULL) {
        stackNode* toRemove = head;
        printf(1, "curr was not null\n");
        printf(1, "curr->pid = %d\n", curr->pid);
        printf(1, "arg pid = %d\n", pid);
        if (curr->pid == pid) {
            head = curr->next;
            free(curr->actualStack);
            free(curr);
            printf(1, "curr was in first position\n");
            return;
        }
        printf(1, "curr was not in first position\n");
        while (curr->next->pid != pid) {
            curr = curr->next;
        }
        toRemove = curr->next;
        curr->next = curr->next->next;
        free(toRemove->actualStack);
        free(toRemove);
    }
}

void printStackNodes() {
    stackNode* curr = head;
    while(curr != NULL) {
        printf(1, "PID: %d\n", curr->pid);
        printf(1, "STACK: %x\n", curr->actualStack);
        curr = curr->next;
    }
}
int thread_create(void (*start_routine)(void*), void* arg) // Andrew
{
    // call malloc to create new user stack
    // pass this new space to clone
    void* actualStack = NULL;
    void* stack = (void*)malloc((uint)((2*PG_SIZE)-1));  // TODO should we hardcode??
    actualStack = stack;
    if (stack == NULL) {
        printf(1, "stack was NULL\n");
        exit();
    }
    if ((int)stack % (int)PG_SIZE != 0) {
        int offset = (int)stack % (int)PG_SIZE;
        int remainder = PG_SIZE - offset;
        stack = stack + remainder;
    }
    int retval = clone(start_routine, arg, (void*)stack);
    addStackNode(actualStack, retval);
    return retval;
}

int thread_join(void) // Andrew
{
    int retval;
    void* stackptr;      
    retval = join(&stackptr); // Andrew
    printf(1,"returned from join\n");
    if (stackptr == NULL) {
        printf(1, "free() in join error because stackptr is NULL\n");
        return -1;
    }
    if (retval != -1){
        printf(1, "Before remove:\n");
        printStackNodes();
        removeStackNode(retval);
        printf(1, "After remove:\n");
        printStackNodes();
    }
    return retval; 
}


char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, void *vsrc, int n)
{
  char *dst, *src;
  
  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}
