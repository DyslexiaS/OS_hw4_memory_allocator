#include "hw_malloc.h"

void *hw_malloc(size_t bytes)
{
	fixed_bytes(&bytes);
	if(HEAP == NULL)
		heap_init(&HEAP);
	struct chunk_header* org_chunk = fixed_bin(HEAP,bytes);
	if(org_chunk == NULL)
		return NULL;
	else {
		struct chunk_header* upper_chunk = get_chunk_header(org_chunk, bytes);
		upper_chunk->prev_free_flag = false;
		list_del(org_chunk);
		split_chunk(org_chunk, bytes);
		list_add(HEAP, upper_chunk);
		return (void*)org_chunk+40;
	}
}
int hw_free(void *mem)
{
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
struct chunk_header* get_chunk_header(struct chunk_header* add_pos,
                                      size_t bytes)
{
	assert(add_pos != NULL);
	struct chunk_header* re_header = (struct chunk_header*)((void*)add_pos+bytes);
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
	bytes += 40;
	struct chunk_header* rest_chunk = get_chunk_header(org_chunk, bytes);
	if(rest_chunk->chunk_size == HEAP_SIZE) {
		init_chunk_header(rest_chunk,org_chunk->chunk_size-bytes,bytes,0);
		init_chunk_header(org_chunk,bytes,org_chunk->chunk_size-bytes,1);
	} else {
		init_chunk_header(rest_chunk,org_chunk->chunk_size-bytes,bytes,0);
		init_chunk_header(org_chunk,bytes,org_chunk->prev_chunk_size,
		                  org_chunk->prev_free_flag);
	}
}
int get_bin_num(chunk_size_t chunk_size)
{
	assert(chunk_size>=40 && chunk_size%8==0);
	return (chunk_size-48)/8>5 ?6 :(chunk_size-48)/8;
}
static void __list_add(struct chunk_header* new_lst,
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
	struct chunk_header* listptr;
	struct chunk_header* bin_head = HEAP->BIN[bin_num];
	for(listptr = bin_head->next; ; listptr=listptr->next) {
		if(entry->chunk_size < listptr->chunk_size) {
			__list_add(entry,listptr->prev,listptr);
			break;
		}
	}
}
static void __list_del(struct chunk_header* prev, struct chunk_header* next)
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
	struct chunk_header* bin_head = HEAP->BIN[bin_idx];
	struct chunk_header* listptr = bin_head->next;
	while(listptr != bin_head) {
		if(listptr->chunk_size == bytes)
			return listptr;
		else if(listptr->chunk_size < bytes) {
			return listptr->prev;
		}
	}
	return NULL;
}
struct chunk_header* fixed_bin(struct heap_t* HEAP, size_t bytes)
{
	assert(HEAP!=NULL);
	int bin_num = get_bin_num(bytes);
	for(int i = bin_num; i<BIN_NUM; ++i) {
		if(HEAP->BIN[bin_num]->next == HEAP->BIN[bin_num])
			continue;
	}
	return(find_space(HEAP, bytes, bin_num));
}
