#include "kernel/types.h"
#include "user/user.h"

#define FLAG_A 64
#define FLAG_D 128
#define FLAG_AD (FLAG_A | FLAG_D)

#define BIG_HEAP_SIZE (3 * 4096 + 2048)

int global_var = 12345;
int global_array[8] = {0, 1, 2, 3, 4, 5, 6, 7};

void
print_block(const char *name)
{
  printf("\n~~~~~~~~~~~~~~~~~~~~ %s ~~~~~~~~~~~~~~~~~~~~\n", name);
}

void
print_check(const char *name, void *addr, int len)
{
  int a = pgcheck_ad(addr, len, FLAG_A);
  int d = pgcheck_ad(addr, len, FLAG_D);
  int ad = pgcheck_ad(addr, len, FLAG_AD);

  printf("%s: A=%d D=%d A|D=%d\n", name, a, d, ad);
}

void
clear_and_report(const char *name, void *addr, int len)
{
  int ret = pgclear_ad(addr, len, FLAG_AD);
  printf("pgclear_ad(%s) = %d\n\n", name, ret);
}

void
print_check_all(int *stack_var, int *stack_array, char *heap_array)
{
  print_check("global_var", &global_var, sizeof(global_var));
  print_check("stack_var", stack_var, sizeof(*stack_var));
  print_check("stack_array[100]", &stack_array[100], sizeof(stack_array[100]));
  print_check("global_array[5]", &global_array[5], sizeof(global_array[5]));
  print_check("heap_array[0]", &heap_array[0], 1);
  print_check("heap_array[4096]", &heap_array[4096], 1);
  print_check("heap_array whole big block", heap_array, BIG_HEAP_SIZE);
}

void
clear_rep_all(int *stack_var, int *stack_array, char *heap_array)
{
  clear_and_report("global_var", &global_var, sizeof(global_var));
  clear_and_report("stack_var", stack_var, sizeof(*stack_var));
  clear_and_report("stack_array[100]", &stack_array[100], sizeof(stack_array[100]));
  clear_and_report("global_array[5]", &global_array[5], sizeof(global_array[5]));
  clear_and_report("heap_array[0]", &heap_array[0], 1);
  clear_and_report("heap_array[4096]", &heap_array[4096], 1);
  clear_and_report("heap_array whole big block", heap_array, BIG_HEAP_SIZE);
}

int
main(void)
{
  int stack_var = 54321;
  int stack_array[256];
  char *heap_array;
  int temp = 0;

  for(int i = 0; i < 256; i++)
    stack_array[i] = i;

  printf("\nTest started.\n");

  print_block("error checks");
  printf("bad flags = %d\n", pgcheck_ad(&global_var, 4, 1));
  printf("bad addr = %d\n", pgcheck_ad((void*)0xFFFFFFFF, 4, FLAG_A));
  print_block("done with error checks");

  print_block("page table at start");
  pgtprint();

  heap_array = (char *)malloc(BIG_HEAP_SIZE);
  if(heap_array == 0){
    printf("malloc failed\n");
    exit(1);
  }

  for(int i = 0; i < BIG_HEAP_SIZE; i++)
    heap_array[i] = (char)(i & 0x7f);

  print_block("after heap allocation");
  pgtprint();

  print_block("initial flags check");
  print_check_all(&stack_var, stack_array, heap_array);

  print_block("clear A/D flags for all test variables");
  clear_rep_all(&stack_var, stack_array, heap_array);

  print_block("page table after clearing A/D flags");
  pgtprint();

  print_block("flags after clearing A/D");
  print_check_all(&stack_var, stack_array, heap_array);

  temp += global_var;
  temp += stack_var;
  temp += stack_array[100];
  temp += global_array[5];
  temp += heap_array[0];
  temp += heap_array[4096];
  temp += heap_array[8192];

  if(temp == 123456789)
    printf("impossible temp = %d\n", temp);

  print_block("page table after read");
  pgtprint();

  print_block("flags after read");
  print_check_all(&stack_var, stack_array, heap_array);

  print_block("clearing A/D flags again");
  clear_rep_all(&stack_var, stack_array, heap_array);

  print_block("flags after second clearing");
  print_check_all(&stack_var, stack_array, heap_array);

  global_var += 1;
  stack_var += 2;
  stack_array[100] += 3;
  global_array[5] += 4;
  heap_array[0] += 5;
  heap_array[4096] += 6;
  heap_array[8192] += 7;

  print_block("page table after write");
  pgtprint();

  print_block("flags after write");
  print_check_all(&stack_var, stack_array, heap_array);

  free(heap_array);

  print_block("page table after free");
  pgtprint();

  printf("\nTest finished.\n");
  exit(0);
}