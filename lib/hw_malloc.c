#include "hw_malloc.h"

void *hw_malloc(size_t bytes)
{
	fixed_bytes(&bytes);
	if(HEAP == NULL)
		heap_init(&HEAP);
	struct chunk_header* org_chunk = fixed_bin(HEAP,bytes);
	if(org_chunk == NULL) {
		printf("no fixed_bin.\n");
		return NULL;
	} else {
		list_del(org_chunk);
		if(org_chunk->chunk_size >= bytes+sizeof(struct chunk_header)+48)
			split_chunk(org_chunk, bytes);
		else {
			struct chunk_header* upper = get_uchunk_header(org_chunk,org_chunk->chunk_size);
			upper->prev_free_flag = false;
		}
		return recul_chunk_header(org_chunk);
	}
}
int hw_free(void *mem)
{
//   puts("*****************bin **************");
//   print_bin(HEAP,0);
//   print_bin(HEAP,1);
//   print_bin(HEAP,2);
//   print_bin(HEAP,3);
//   print_bin(HEAP,4);
//   print_bin(HEAP,5);
//   print_bin(HEAP,6);
	mem+=(long long int)HEAP->start_brk;
	if(mem>=(void*)HEAP->start_brk+HEAP_SIZE || mem<(void*)HEAP->start_brk) {
		puts("no in range");
		return 0;
	}
	struct chunk_header* free_chunk = mem_to_chunk(mem);
	if(is_valid(mem)==true && is_free(free_chunk) == false) {
		put_into_bin(free_chunk);
		return 1;
	}
	return 0;
}
void *get_start_sbrk(void)
{
	return HEAP->start_brk;
}
void heap_init(struct heap_t** HEAP)
{
	*HEAP = (struct heap_t*)malloc(sizeof(struct heap_t));
	assert(*HEAP!=NULL);
	(*HEAP)->next = NULL;
	(*HEAP)->start_brk = (void*)sbrk(HEAP_SIZE);
	assert((*HEAP)->start_brk != (void*)-1);
	for(int i=0; i<BIN_NUM ; ++i) {
		bin_init((&(*HEAP)->BIN[i]));
	}
	init_chunk_header((struct chunk_header*)(*HEAP)->start_brk,HEAP_SIZE,1, 1);
	list_add(*HEAP, (struct chunk_header*)(*HEAP)->start_brk);
}
void bin_init(struct chunk_header** bin)
{
	*bin = (struct chunk_header*)malloc(sizeof(struct chunk_header));
	assert(*bin!=NULL);
	(*bin)->prev = (*bin)->next = (*bin);
	(*bin)->chunk_size = sizeof(struct chunk_header);
	(*bin)->prev_chunk_size = sizeof(struct chunk_header);
	(*bin)->prev_free_flag = true;
}
struct chunk_header* get_uchunk_header(struct chunk_header* add_pos,
                                       size_t bytes)
{
	assert(add_pos != NULL);
	struct chunk_header* re_header = (struct chunk_header*)((void*)add_pos+bytes);
	if((void*)re_header >= (void*)HEAP->start_brk+HEAP_SIZE)
		re_header = (void*)re_header-HEAP_SIZE;
	return re_header;
}
struct chunk_header* get_lchunk_header(struct chunk_header* add_pos,
                                       size_t bytes)
{
	assert(add_pos != NULL);
	struct chunk_header* re_header = (struct chunk_header*)((
	                                     void*)add_pos-add_pos->prev_chunk_size);
	if((void*)re_header < (void*)HEAP->start_brk)
		re_header = (void*)re_header+HEAP_SIZE;
	return re_header;
}
void init_chunk_header(struct chunk_header* entry, size_t bytes,
                       chunk_size_t prev_chunk_size, chunk_flag_t prev_free_flag)
{
	assert(entry!=NULL);
	entry->prev = NULL;
	entry->next = NULL;
	entry->chunk_size = bytes;
	entry->prev_chunk_size = prev_chunk_size;
	entry->prev_free_flag = prev_free_flag;
}
void split_chunk(struct chunk_header* org_chunk,
                 size_t bytes) //TODO smaller than 48
{
	assert(org_chunk!=NULL);
	bytes += sizeof(struct chunk_header);
//    printf("split bytes = %lu\n", bytes);
	struct chunk_header* rest_chunk = get_uchunk_header(org_chunk, bytes);
	if(rest_chunk->chunk_size == HEAP_SIZE) {
		init_chunk_header(rest_chunk,org_chunk->chunk_size-bytes,bytes,0);
		init_chunk_header(org_chunk,bytes,org_chunk->chunk_size-bytes,1);
	} else {
		init_chunk_header(rest_chunk,org_chunk->chunk_size-bytes,bytes,0);
		init_chunk_header(org_chunk,bytes,org_chunk->prev_chunk_size,
		                  org_chunk->prev_free_flag);
		renew_upper_chunk(rest_chunk);
	}
	list_add(HEAP, rest_chunk);
}
int get_bin_num(chunk_size_t chunk_size)
{
//	printf("get func, chunk_size =%lld\n",chunk_size);
	assert(chunk_size>=40 && chunk_size%8==0);
	return (chunk_size-48)/8>5 ?6 :(chunk_size-48)/8;
}
void __list_add(struct chunk_header* new_lst,
                struct chunk_header* prev,
                struct chunk_header* next)
{
	assert(next!=NULL && prev!=NULL);
	assert(new_lst!=NULL && new_lst->prev==NULL && new_lst->next==NULL);
	next->prev = new_lst;
	new_lst->next = next;
	new_lst->prev = prev;
	prev->next = new_lst;
}
void list_add(struct heap_t* HEAP, struct chunk_header* entry)
{
	assert(HEAP!=NULL && entry!=NULL);
	int bin_num = get_bin_num(entry->chunk_size);
	renew_upper_chunk(entry);
	struct chunk_header* listptr;
	struct chunk_header* bin_head = HEAP->BIN[bin_num];
	for(listptr = bin_head->next; ; listptr=listptr->next) {
		//    printf("%lld, %lld\n", entry->chunk_size, listptr->chunk_size);
		if(entry->chunk_size > listptr->chunk_size) {
			//    puts("list_add");
			__list_add(entry,listptr->prev,listptr);
			break;
		}
	}
}
void __list_del(struct chunk_header* prev, struct chunk_header* next)
{
	next->prev = prev;
	prev->next = next;
}
void list_del(struct chunk_header* del)
{
	assert(del!=NULL);
	__list_del(del->prev,del->next);
	del->next = NULL;
	del->prev = NULL;
}
void fixed_bytes(size_t* bytes)
{
	*bytes = ((*bytes-1)/8+1)*8;
}
struct chunk_header* find_space(struct heap_t* HEAP, size_t bytes, int bin_idx)
{
	assert(HEAP!=NULL);
	// puts("in find space");
	//printf("bin idx= %d\n",bin_idx);
	struct chunk_header* bin_head = HEAP->BIN[bin_idx];
	struct chunk_header* listptr = bin_head->next;
	while(listptr != bin_head) {
		if(listptr->chunk_size == bytes)
			return listptr;
		else if(listptr->chunk_size < bytes)
			break;
		listptr = listptr->next;
	}
	if(bin_idx==6 && listptr->prev == bin_head ) {
		return NULL;
	}
	return listptr->prev;
}
struct chunk_header* fixed_bin(struct heap_t* HEAP, size_t bytes)
{
	assert(HEAP!=NULL && bytes%8==0);
	//puts("in fixed bin");
	bytes += sizeof(struct chunk_header);
	int bin_num = get_bin_num(bytes);
	for(; bin_num <BIN_NUM; ++bin_num) {
		if(HEAP->BIN[bin_num]->next != HEAP->BIN[bin_num])
			break;
	}
	if(bin_num>6) return NULL;
	return(find_space(HEAP, bytes, bin_num));
}
void print_bin(struct heap_t* HEAP, int bin_idx)
{
	struct chunk_header* bin_head = HEAP->BIN[bin_idx];
	struct chunk_header* listptr = bin_head->next;
	while(listptr != bin_head) {
		printf("0x%08llx--------%lld\n",get_relative_addr(HEAP,listptr),
		       listptr->chunk_size);
		listptr = listptr->next;
	}
}
void renew_upper_chunk(struct chunk_header* entry)
{
	struct chunk_header* upper_chunk = get_uchunk_header(entry,entry->chunk_size);
	assert(upper_chunk!=NULL);
	upper_chunk->prev_chunk_size = entry->chunk_size;
	upper_chunk->prev_free_flag = true;
}
long long int get_relative_addr(struct heap_t* HEAP, struct chunk_header* entry)
{
	return (void*)entry - HEAP->start_brk;
}
struct chunk_header* mem_to_chunk(void* mem)
{
	return (struct chunk_header*)mem-1;
}
bool is_valid(void* mem)
{
	struct chunk_header* free_chunk = mem_to_chunk(mem);
	struct chunk_header* ptr = (void*)HEAP->start_brk;
//    printf("heap %p\n", HEAP->start_brk);
	//      printf("ptr= %p, fc =%p\n", ptr,free_chunk);
	while((void*)ptr <= (void*)free_chunk) {
		//    printf("ptr= %p, fc =%p\n", ptr,free_chunk);
		if((void*)ptr == (void*)free_chunk)
			return true;
		ptr = get_uchunk_header(ptr, ptr->chunk_size);
	}
	return false;
}
void put_into_bin(struct chunk_header* free_chunk)
{
	renew_upper_chunk(free_chunk);
	list_add(HEAP, free_chunk);
	struct chunk_header* upper = get_uchunk_header(free_chunk,
	                             free_chunk->chunk_size);
	struct chunk_header* upper2 = get_uchunk_header(upper, upper->chunk_size);
	struct chunk_header* lower = get_lchunk_header(free_chunk,
	                             free_chunk->prev_chunk_size);
	if(upper2->prev_free_flag == true && upper > free_chunk) {
		list_del(upper);
		list_del(free_chunk);
		merge(upper, free_chunk);
	} else if(free_chunk->prev_free_flag == true && lower < free_chunk) {
		list_del(free_chunk);
		list_del(lower);
		merge(free_chunk, lower);
	}
}
void merge(struct chunk_header* upper, struct chunk_header* lower)
{
	renew_upper_chunk(upper);
	lower->chunk_size += upper->chunk_size;
	renew_upper_chunk(lower);
	list_add(HEAP, lower);
}
void * recul_chunk_header(struct chunk_header* re_chunk)
{
	return (void*)re_chunk + sizeof(struct chunk_header) - (unsigned long long int)
	       HEAP->start_brk;
}
bool is_free(struct chunk_header* entry)
{
	struct chunk_header* upper = get_uchunk_header(entry, entry->chunk_size);
	return upper->prev_free_flag;
}
