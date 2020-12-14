#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * 0                                                              63
 * ================================================================
 * SSSSSSSSaaaaSSSSSSSSbbbb
 * ^
 * base ptr a
 * ^
 * size ptr a
 *         ^
 *         user ptr a
 *
 *             ^
 *             base ptr b
 *             ^
 *             size ptr b
 *                     ^
 *                     user ptr b
 *
 * ^
 * MEMORY_POOL
 */

static uint8_t MEMORY_POOL[64];

struct free_entry {
  void *ptr;
  uint64_t size;
};
typedef struct free_entry free_entry_t;

static free_entry_t FREE_LIST[64] = {
  (free_entry_t){
    .ptr = MEMORY_POOL,
    .size = 64,
  },
};
static uint64_t FREE_LIST_USED = 1;

void print_free_list() {
  printf("FREE LIST:\n");
  for (uint64_t i=0; i<FREE_LIST_USED; i++) {
    free_entry_t *entry;
    entry = &FREE_LIST[i];
    printf("  %p (%llu)\n", entry->ptr, entry->size);
  }
}

free_entry_t *find_free_entry(size_t size) {
  free_entry_t *best_entry = FREE_LIST;
    
  for (uint64_t i=0; i<FREE_LIST_USED; i++) {
    free_entry_t *entry;
    entry = &FREE_LIST[i]; // & - pretzel operator - reads like 'address of'. returns a pointer

    if (entry->size >= size && entry->size < best_entry->size) {
      best_entry = entry;
    }
  }

  return best_entry;
}

void *malloc(size_t size) {
  size += 8;

  free_entry_t *entry;
  entry = find_free_entry(size);

  void *base_ptr;
  uint64_t *size_ptr;
  void *user_ptr;

  base_ptr = entry->ptr;
  size_ptr = base_ptr;
  user_ptr = base_ptr + 8;

  *size_ptr = size; // * - splat operator - on the left-hand side of the assigment to write the value to that memory address

  entry->ptr += size;
  entry->size -= size;

  print_free_list();

  return user_ptr;
}

void free(void *user_ptr) {
  free_entry_t *entry;
  entry = &FREE_LIST[FREE_LIST_USED];

  void *base_ptr;
  uint64_t *size_ptr;
  uint64_t size;

  base_ptr = user_ptr - 8;
  size_ptr = base_ptr;

  size = *size_ptr;

  entry->ptr = base_ptr;
  entry->size = size;

  FREE_LIST_USED++;

  printf("FREE\n");
  print_free_list();
}

int main() {
  print_free_list();

  char *a;
  char *b;
  char *c;
  char *d;

  a = malloc(4);
  b = malloc(4);
  c = malloc(4);

  strcpy(a, "aaa\0");
  strcpy(b, "bbb\0");
  strcpy(c, "ccc\0");

  printf("a: %p\n", a);
  printf("b: %p\n", b);
  printf("c: %p\n", c);

  free(b);
  d = malloc(4);

  printf("d: %p\n", d);
}

