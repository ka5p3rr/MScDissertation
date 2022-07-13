#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "cheriintrin.h"


void print(char *str, void *ptr) {
    printf("%s %lu %lu %lu %lu %u %u\n", str, cheri_address_get(ptr), cheri_base_get(ptr), cheri_length_get(ptr), cheri_offset_get(ptr), cheri_perms_get(ptr), cheri_tag_get(ptr));
}

int main() {
    char ptr[] = "Hello world!";
    char *ptr1;

    /* Initial memory allocation */
    print("ptr: ", ptr);

    ptr1 = ptr;
    print("ptr1:", ptr);

    printf("ptr:  %s\n", ptr);
    printf("ptr1: %s\n", ptr1);
    return 0;
}