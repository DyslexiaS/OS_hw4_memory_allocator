#include "hw4_mm_test.h"

int main()
{
	int* a = hw_malloc(6);
	print("a= %p\n",a);
	int* b = hw_malloc(6);
	print("b= %p\n",b);

}
/*
int main() {
    printf("chunk_header size: %ld\n", sizeof(struct chunk_header));
    printf("%p\n", hw_malloc(8u));
    printf("%s\n", hw_free(NULL) == 1 ? "success" : "fail");
    printf("start_brk: %p\n", get_start_sbrk());
    return 0;
}
*/
