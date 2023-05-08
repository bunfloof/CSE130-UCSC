#pragma once

/**
 * `kvpair_t` stores a pair strings: key and value.
 */
typedef struct kvpair_t {
  char *key;
  char *value;
} kvpair_t;

/**
 * `kvpair_new` creates a new `kvpair_t` by copying the provided `key` and
 * `value`.
 */
kvpair_t *kvpair_new(char *key, char *value);

/**
 * `kvpair_clone` creates a copy of `kv`.
 */
kvpair_t *kvpair_clone(kvpair_t *kv);

/**
 * `kvpair_free` frees `kvpair_t`.
 */
void kvpair_free(kvpair_t **kv);

/**
 * `kvpair_update_value` updates the value of the pair.
 */
void kvpair_update_value(kvpair_t *pair, char *new_value);

struct kvlist_node_t;
typedef struct kvlist_node_t kvlist_node_t;

/**
 * `kvlist_t` is a list of `kvpair_t`s.
 */
typedef struct kvlist_t kvlist_t;

/**
 * `kvlist_new` creates a new `kvlist_t`.
 */
kvlist_t *kvlist_new(void);

/**
 * `kvlist_free` frees `kvlist_t`.
 * It also frees all pairs inside the list.
 */
void kvlist_free(kvlist_t **lst);

/**
 * `kvlist_append` appends the pair `kv` to the list `lst`.
 */
void kvlist_append(kvlist_t *lst, kvpair_t *kv);

/**
 * `kvlist_extend` concatenates two lists `lst` and `lst2`.
 * After this function returns, all pairs in `lst2` are moved to `lst` and
 * `lst2` becomes empty.
 */
void kvlist_extend(kvlist_t *lst, kvlist_t *lst2);

/**
 * `kvlist_sort` sorts the list by keys.
 */
void kvlist_sort(kvlist_t *lst);

/**
 * `kvlist_print` prints the contents of `lst` to the file descriptor `fd`.
 */
void kvlist_print(int fd, kvlist_t *lst);

/**
 * `kvlist_iterator_t` is used to iterate over `kvlist_t`.
 */
struct kvlist_iterator_t;
typedef struct kvlist_iterator_t kvlist_iterator_t;

/**
 * `kvlist_iterator_new` creates a new iterator.
 */
kvlist_iterator_t *kvlist_iterator_new(kvlist_t *lst);

/**
 * `kvlist_iterator_next` returns the next `kvpair_t`.
 * It returns `NULL` if there is no more pair.
 */
kvpair_t *kvlist_iterator_next(kvlist_iterator_t *it);

/**
 * `kvlist_iterator_free` frees `kvlist_iterator_t`.
 */
void kvlist_iterator_free(kvlist_iterator_t **it);
