#include "kvlist.h"

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

kvpair_t *kvpair_new(char *key, char *value) {
  kvpair_t *pair = (kvpair_t *)malloc(sizeof(kvpair_t));
  pair->key = (char *)malloc(strlen(key) + 1);
  strcpy(pair->key, key);
  pair->value = (char *)malloc(strlen(value) + 1);
  strcpy(pair->value, value);
  return pair;
}

kvpair_t *kvpair_clone(kvpair_t *kv) { return kvpair_new(kv->key, kv->value); }

void kvpair_free(kvpair_t **kv) {
  free((*kv)->key);
  free((*kv)->value);
  free(*kv);
  *kv = NULL;
}

void kvpair_update_value(kvpair_t *pair, char *new_value) {
  free(pair->value);
  pair->value = (char *)malloc(strlen(new_value) + 1);
  strcpy(pair->value, new_value);
}

struct kvlist_node_t {
  kvpair_t *kv;
  struct kvlist_node_t *next;
};

kvlist_node_t *kvlist_node_new(kvpair_t *kv) {
  kvlist_node_t *node = (kvlist_node_t *)malloc(sizeof(kvlist_node_t));
  node->kv = kv;
  node->next = NULL;
  return node;
}

void kvlist_node_free(kvlist_node_t **node) {
  kvpair_free(&(*node)->kv);
  free(*node);
  *node = NULL;
}

struct kvlist_t {
  kvlist_node_t *head;
  kvlist_node_t *tail;
};

kvlist_t *kvlist_new(void) {
  kvlist_t *list = (kvlist_t *)malloc(sizeof(kvlist_t));
  list->head = NULL;
  list->tail = NULL;
  return list;
}

void kvlist_free(kvlist_t **list) {
  kvlist_node_t *node = (*list)->head;
  while (node != NULL) {
    kvlist_node_t *next = node->next;
    kvlist_node_free(&node);
    node = next;
  }
  free(*list);
  *list = NULL;
}

void kvlist_append(kvlist_t *list, kvpair_t *pair) {
  kvlist_node_t *node = (kvlist_node_t *)malloc(sizeof(kvlist_node_t));
  node->kv = pair;
  node->next = NULL;
  if (list->head == NULL) {
    list->head = node;
  } else {
    list->tail->next = node;
  }
  list->tail = node;
}

void kvlist_extend(kvlist_t *list, kvlist_t *other) {
  if (other->head == NULL) {
    return;
  }
  if (list->head == NULL) {
    list->head = other->head;
    list->tail = other->tail;
  } else {
    list->tail->next = other->head;
    list->tail = other->tail;
  }
  other->head = NULL;
  other->tail = NULL;
}

void kvlist_node_split(kvlist_node_t *source, kvlist_node_t **front,
                       kvlist_node_t **back) {
  kvlist_node_t *p1 = source->next;
  kvlist_node_t *p2 = source;
  while (p1 != NULL) {
    p1 = p1->next;
    if (p1 != NULL) {
      p2 = p2->next;
      p1 = p1->next;
    }
  }
  *front = source;
  *back = p2->next;
  p2->next = NULL;
}

kvlist_node_t *kvlist_node_merge(kvlist_node_t *a, kvlist_node_t *b) {
  kvlist_node_t *result = NULL;
  kvlist_node_t *current = NULL;

  if (a == NULL) {
    return b;
  }
  if (b == NULL) {
    return a;
  }

  while (a != NULL && b != NULL) {
    if (strcmp(a->kv->key, b->kv->key) <= 0) {
      if (result == NULL) {
        result = a;
        current = a;
      } else {
        current->next = a;
        current = current->next;
      }
      a = a->next;
    } else {
      if (result == NULL) {
        result = b;
        current = b;
      } else {
        current->next = b;
        current = current->next;
      }
      b = b->next;
    }
  }

  if (a != NULL) {
    current->next = a;
  } else {
    current->next = b;
  }

  return result;
}

void kvlist_node_mergesort(kvlist_node_t **l) {
  kvlist_node_t *head = *l;
  kvlist_node_t *p1;
  kvlist_node_t *p2;
  if (head == NULL || head->next == NULL) {
    return;
  }
  kvlist_node_split(head, &p1, &p2);
  kvlist_node_mergesort(&p1);
  kvlist_node_mergesort(&p2);
  *l = kvlist_node_merge(p1, p2);
}

void kvlist_sort(kvlist_t *lst) {
  kvlist_node_mergesort(&lst->head);

  kvlist_node_t *c = lst->head;
  while (c != NULL && c->next != NULL) {
    c = c->next;
  }
  lst->tail = c;
}

void kvlist_print(int fd, kvlist_t *lst) {
  kvlist_iterator_t *itor = kvlist_iterator_new(lst);
  for (;;) {
    kvpair_t *pair = kvlist_iterator_next(itor);
    if (pair == NULL) {
      break;
    }
    dprintf(fd, "%s,%s\n", pair->key, pair->value);
  }
  kvlist_iterator_free(&itor);
}

struct kvlist_iterator_t {
  kvlist_node_t *node;
};

kvlist_iterator_t *kvlist_iterator_new(kvlist_t *lst) {
  kvlist_iterator_t *iter =
      (kvlist_iterator_t *)malloc(sizeof(kvlist_iterator_t));
  iter->node = lst->head;
  return iter;
}

kvpair_t *kvlist_iterator_next(kvlist_iterator_t *iter) {
  if (iter->node == NULL) {
    return NULL;
  }
  kvpair_t *pair = iter->node->kv;
  iter->node = iter->node->next;
  return pair;
}

void kvlist_iterator_free(kvlist_iterator_t **iter) {
  free(*iter);
  *iter = NULL;
}
