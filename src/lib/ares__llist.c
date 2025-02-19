/* Copyright (C) 2023 by Brad House
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SPDX-License-Identifier: MIT
 */
#include "ares_setup.h"
#include "ares.h"
#include "ares_private.h"
#include "ares__llist.h"

struct ares__llist {
  ares__llist_node_t      *head;
  ares__llist_node_t      *tail;
  ares__llist_destructor_t destruct;
  size_t                   cnt;
};


struct ares__llist_node {
  void               *data;
  ares__llist_node_t *prev;
  ares__llist_node_t *next;
  ares__llist_t      *parent;
};


ares__llist_t *ares__llist_create(ares__llist_destructor_t destruct)
{
  ares__llist_t *list = ares_malloc(sizeof(*list));

  if (list == NULL)
    return NULL;

  memset(list, 0, sizeof(*list));

  list->destruct = destruct;

  return list;
}


void ares__llist_replace_destructor(ares__llist_t *list,
                                    ares__llist_destructor_t destruct)
{
  if (list == NULL)
    return;

  list->destruct = destruct;
}


typedef enum {
  ARES__LLIST_INSERT_HEAD,
  ARES__LLIST_INSERT_TAIL,
  ARES__LLIST_INSERT_BEFORE
} ares__llist_insert_type_t;


static ares__llist_node_t *ares__llist_insert_at(ares__llist_t *list, 
                                                 ares__llist_insert_type_t type,
                                                 ares__llist_node_t *at,
                                                 void *val)
{
  ares__llist_node_t *node = NULL;

  if (list == NULL || val == NULL)
    return NULL;

  node = ares_malloc(sizeof(*node));

  if (node == NULL)
    return NULL;

  memset(node, 0, sizeof(*node));
  node->data   = val;
  node->parent = list;

  if (type == ARES__LLIST_INSERT_BEFORE && (at == list->head || at == NULL)) {
    type = ARES__LLIST_INSERT_HEAD;
  }

  switch (type) {
    case ARES__LLIST_INSERT_HEAD:
      node->next = list->head;
      node->prev = NULL;
      if (list->head)
        list->head->prev = node;
      list->head = node;
      break;
    case ARES__LLIST_INSERT_TAIL:
      node->next = NULL;
      node->prev = list->tail;
      if (list->tail)
        list->tail->next = node;
      list->tail = node;
      break;
    case ARES__LLIST_INSERT_BEFORE:
      node->next = at;
      node->prev = at->prev;
      at->prev   = node;
      break;
  }
  if (list->tail == NULL)
    list->tail = node;
  if (list->head == NULL)
    list->head = node;

  list->cnt++;

  return node;
}


ares__llist_node_t *ares__llist_insert_first(ares__llist_t *list, void *val)
{
  return ares__llist_insert_at(list, ARES__LLIST_INSERT_HEAD, NULL, val);
}


ares__llist_node_t *ares__llist_insert_last(ares__llist_t *list, void *val)
{
  return ares__llist_insert_at(list, ARES__LLIST_INSERT_TAIL, NULL, val);
}


ares__llist_node_t *ares__llist_insert_before(ares__llist_node_t *node,
                                              void *val)
{
  if (node == NULL)
    return NULL;

  return ares__llist_insert_at(node->parent, ARES__LLIST_INSERT_BEFORE, node,
                               val);
}


ares__llist_node_t *ares__llist_insert_after(ares__llist_node_t *node,
                                             void *val)
{
  if (node == NULL)
    return NULL;

  if (node->next == NULL)
    return ares__llist_insert_last(node->parent, val);

  return ares__llist_insert_at(node->parent, ARES__LLIST_INSERT_BEFORE,
                               node->next, val);
}


ares__llist_node_t *ares__llist_node_first(ares__llist_t *list)
{
  if (list == NULL)
    return NULL;
  return list->head;
}


ares__llist_node_t *ares__llist_node_last(ares__llist_t *list)
{
  if (list == NULL)
    return NULL;
  return list->tail;
}


ares__llist_node_t *ares__llist_node_next(ares__llist_node_t *node)
{
  if (node == NULL)
    return NULL;
  return node->next;
}


ares__llist_node_t *ares__llist_node_prev(ares__llist_node_t *node)
{
  if (node == NULL)
    return NULL;
  return node->prev;
}


void *ares__llist_node_val(ares__llist_node_t *node)
{
  if (node == NULL)
    return NULL;

  return node->data;
}


size_t ares__llist_len(ares__llist_t *list)
{
  if (list == NULL)
    return 0;
  return list->cnt;
}


ares__llist_t *ares__llist_node_parent(ares__llist_node_t *node)
{
  if (node == NULL)
    return NULL;
  return node->parent;
}


void *ares__llist_first_val(ares__llist_t *list)
{
  return ares__llist_node_val(ares__llist_node_first(list));
}


void *ares__llist_last_val(ares__llist_t *list)
{
  return ares__llist_node_val(ares__llist_node_last(list));
}


void *ares__llist_node_claim(ares__llist_node_t *node)
{
  void          *val;
  ares__llist_t *list;

  if (node == NULL)
    return NULL;

  list = node->parent;
  val  = node->data;

  if (node->prev) {
    node->prev->next = node->next;
  }

  if (node->next) {
    node->next->prev = node->prev;
  }

  if (node == list->head) {
    list->head = node->next;
  }

  if (node == list->tail) {
    list->tail = node->prev;
  }
  ares_free(node);

  list->cnt--;

  return val;
}


void ares__llist_node_destroy(ares__llist_node_t *node)
{
  ares__llist_destructor_t destruct;
  void                    *val;

  if (node == NULL)
    return;

  destruct = node->parent->destruct;

  val = ares__llist_node_claim(node);
  if (val != NULL && destruct != NULL)
    destruct(val);
}


void ares__llist_node_replace(ares__llist_node_t *node, void *val)
{
  ares__llist_destructor_t destruct;
  
  if (node == NULL)
    return;

  destruct = node->parent->destruct;
  if (destruct != NULL)
    destruct(node->data);

  node->data = val;
}


void ares__llist_destroy(ares__llist_t *list)
{
  ares__llist_node_t *node;

  if (list == NULL)
    return;

  while ((node = ares__llist_node_first(list)) != NULL) {
    ares__llist_node_destroy(node);
  }
  ares_free(list);
}
