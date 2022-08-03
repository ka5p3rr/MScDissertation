#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "cheriintrin.h"


void print(char *str, void *ptr) {
    printf("%s %lu %lu %lu %lu %u %u\n", str, cheri_address_get(ptr), cheri_base_get(ptr), cheri_length_get(ptr), cheri_offset_get(ptr), cheri_perms_get(ptr), cheri_tag_get(ptr));
}

int main() {
    char ptr[] = "Hello World! Goodbye Universe!";
    char *ptr1;

    /* Initial memory allocation */
    print("ptr: ", ptr);
    printf("ptr:  %s\n", ptr);

    ptr1 = ptr;
    ptr1 += 13;
    print("ptr1:", ptr1);
    printf("ptr1: %s\n", ptr1);


    int *ptr2 = NULL;
    print("", ptr2);
    print("", ++ptr2);

    intptr_t integer_ptr =  (intptr_t) ptr1;
    integer_ptr -= 13;
    print("integer_ptr:", (void *) integer_ptr);
    printf("integer_ptr: %s\n", (char *) integer_ptr);
    
    int integers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int *integers_ptr = integers;
    for(int i = 0; i < 10; i++)
        printf("%d ", *integers_ptr++);
    printf("\n");
    

    size_t value = 0;
    size_t *test = &value;
    printf("sizeof: %lu\n", sizeof(*test));
    printf("val: %p\n", test);
    test++;
    printf("sizeof: %lu\n", sizeof(*test));
    printf("val: %p\n", test);

    int *test2 = NULL;
    printf("sizeof: %lu\n", sizeof(*test2));
    printf("val: %p\n", test2);
    test2++;
    printf("sizeof: %lu\n", sizeof(*test2));
    printf("val: %p\n", test2);


    struct foo {
        int bar;
    };

    struct foo* test_struct = NULL;
    test_struct++;
    printf("sizeof: %lu\n", sizeof(*test_struct));
    printf("val: %p\n", test_struct);

    return 0;
}