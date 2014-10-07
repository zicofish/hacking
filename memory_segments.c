#include <stdio.h>

int global_var;
int global_initialized_var = 5;

void function(){
    int stack_var;
    printf("the function's stack_var is at address 0x%08x\n", &stack_var);
}

int main(){
    int stack_var1;
    int stack_var2;
    static int static_initialized_var = 5;
    static int staic_var;
    int *heap_var_ptr;

    heap_var_ptr = (int *) malloc(4);

    printf("global_initialized_var is at address 0x%08x\n", &global_initialized_var);
    printf("static_initialized_var is at address 0x%08x\n", &static_initialized_var);

    printf("static_var is at address 0x%08x\n", &staic_var);
    printf("global_var is at address 0x%08x\n", &global_var);

    printf("heap_var is at address 0x%08x\n", heap_var_ptr);

    printf("stack_var is at address 0x%08x, 0x%08x\n", &stack_var1, &stack_var2);
    function();
}
    
