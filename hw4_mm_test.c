#include "hw4_mm_test.h"
#include <string.h>
#include <stdlib.h>
/*int main()
{
	int* a = hw_malloc(6);
	printf("a= %p\n",a);
	print_bin(HEAP,6);
	int* b = hw_malloc(6);
	printf("b= %p\n",b);
	print_bin(HEAP,6);
	struct chunk_header * chunk = recul_chunk_header(a);
	printf("chunk= %p\n",chunk);
	chunk = (void*)chunk + chunk->chunk_size;
	printf("chunk next %p\n",chunk);
	chunk = (void*)chunk + chunk->chunk_size;
	printf("chunk=%p\n",chunk);
	chunk = (void*)chunk + chunk->chunk_size;
	if((void*)chunk>=(void*)HEAP->start_brk+HEAP_SIZE)
		chunk=(void*)chunk-HEAP_SIZE;
	printf("chunk = %p\n",chunk);
	return 0;
}
*/
int main()
{
	char opt[100];
	void* mem;
	int num;
	while(scanf("%s",opt)!=EOF) {
		if(!strcmp(opt,"alloc")) {
			scanf("%d",&num);
			printf("0x%08llx\n",(long long int)hw_malloc(num));
		} else if(!strcmp(opt,"free")) {
			scanf("%p",&mem);
			printf("%s\n",hw_free(mem) == 1 ?"success" : "fail");
		} else if(!strcmp(opt,"print")) {
			scanf(" bin[%d]",&num);
			print_bin(HEAP, num);
		} else {
			fprintf(stderr,"input error: %s\n", opt);
			exit(EXIT_FAILURE);
		}
	}
	return 0;
	/*
	printf("chunk_header size: %ld\n", sizeof(struct chunk_header));
	printf("%p\n", hw_malloc(8u));
	printf("%s\n", hw_free(NULL) == 1 ? "success" : "fail");
	printf("start_brk: %p\n", get_start_sbrk());
	return 0;
	*/
}

