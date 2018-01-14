#ifndef HW_MALLOC_H
#define HW_MALLOC_H

#include <stddef.h>
#include <inttypes.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define chunk_size_t long long
#define chunk_flag_t long long
#define HEAP_SIZE 64*1024
#define BIN_NUM 7

struct chunk_header {
	struct chunk_header* prev;
	struct chunk_header* next;
	chunk_size_t chunk_size;
	chunk_size_t prev_chunk_size;
	chunk_flag_t prev_free_flag;
};
struct heap_t {
	struct chunk_header* BIN[7];
	void* start_brk;
	struct heap_t* next;
};
struct heap_t *HEAP;
extern void *hw_malloc(size_t bytes);
extern int hw_free(void *mem);
extern void *get_start_sbrk(void);
void heap_init();
void bin_init();
struct chunk_header* get_uchunk_header(struct chunk_header* add_pos,
                                       size_t bytes);
struct chunk_header* get_lchunk_header(struct chunk_header* add_pos,
                                       size_t bytes);
void init_chunk_header(struct chunk_header* entry, size_t bytes,
                       chunk_size_t prev_chunk_size, chunk_flag_t prev_free_flag);
void split_chunk(struct chunk_header* org_chunk, size_t bytes);
int get_bin_num(chunk_size_t chunk_size);
void __list_add(struct chunk_header* new_lst,
                struct chunk_header* prev,
                struct chunk_header* next);
void list_add(struct heap_t* HEAP, struct chunk_header* entry);
void __list_del(struct chunk_header* prev, struct chunk_header* next);
void list_del(struct chunk_header* del);
void fixed_bytes(size_t* bytes);
struct chunk_header* find_space(struct heap_t* HEAP, size_t bytes, int bin_idx);
struct chunk_header* fixed_bin(struct heap_t* HEAP, size_t bytes);
void print_bin(struct heap_t* HEAP, int bin_idx);
void renew_upper_chunk(struct chunk_header* entry);
long long int get_relative_addr(struct heap_t* HEAP,
                                struct chunk_header* entry);
bool is_valid(void* mem);
void put_into_bin(struct chunk_header* free_chunk);
struct chunk_header* mem_to_chunk(void* mem);
void merge(struct chunk_header* upper, struct chunk_header* lower);
void* recul_chunk_header(struct chunk_header* re_chunk);
bool is_free(struct chunk_header* entry);
#endif
