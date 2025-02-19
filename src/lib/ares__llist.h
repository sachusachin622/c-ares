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
#ifndef __ARES__LLIST_H
#define __ARES__LLIST_H

/*! \addtogroup ares__llist LinkedList Data Structure
 *
 * This is a doubly-linked list data structure.
 *
 * Average time complexity:
 *  - Insert: O(1)   -- head or tail
 *  - Search: O(n)
 *  - Delete: O(1)   -- delete assumes you hold a node pointer
 *
 * @{
 */

struct ares__llist;

/*! Opaque data structure for linked list */
typedef struct ares__llist ares__llist_t;

struct ares__llist_node;

/*! Opaque data structure for a node in a linked list */
typedef struct ares__llist_node ares__llist_node_t;

/*! Callback to free user-defined node data
 * 
 *  \param[in] data  user supplied data
 */
typedef void (*ares__llist_destructor_t)(void *data);

/*! Create a linked list object
 * 
 *  \param[in] destruct  Optional. Destructor to call on all removed nodes
 *  \return linked list object or NULL on out of memory
 */
ares__llist_t *ares__llist_create(ares__llist_destructor_t destruct);

/*! Replace destructor for linked list nodes.  Typically this is used
 *  when wanting to disable the destructor by using NULL.
 * 
 *  \param[in] list      Initialized linked list object
 *  \param[in] destruct  replacement destructor, NULL is allowed
 */
void ares__llist_replace_destructor(ares__llist_t *list,
	                                ares__llist_destructor_t destruct);

/*! Insert value as the first node in the linked list
 * 
 *  \param[in] list   Initialized linked list object
 *  \param[in] val    user-supplied value.
 *  \return node object referencing place in list, or null if out of memory or
 *   misuse
 */
ares__llist_node_t *ares__llist_insert_first(ares__llist_t *list, void *val);

/*! Insert value as the last node in the linked list
 * 
 *  \param[in] list   Initialized linked list object
 *  \param[in] val    user-supplied value.
 *  \return node object referencing place in list, or null if out of memory or
 *   misuse
 */
ares__llist_node_t *ares__llist_insert_last(ares__llist_t *list, void *val);

/*! Insert value before specified node in the linked list
 * 
 *  \param[in] node  node referenced to insert before
 *  \param[in] val   user-supplied value.
 *  \return node object referencing place in list, or null if out of memory or
 *   misuse
 */
ares__llist_node_t *ares__llist_insert_before(ares__llist_node_t *node,
	                                          void *val);

/*! Insert value after specified node in the linked list
 * 
 *  \param[in] node  node referenced to insert after
 *  \param[in] val   user-supplied value.
 *  \return node object referencing place in list, or null if out of memory or
 *   misuse
 */
ares__llist_node_t *ares__llist_insert_after(ares__llist_node_t *node,
	                                         void *val);

/*! Obtain first node in list
 * 
 *  \param[in] list  Initialized list object
 *  \return first node in list or NULL if none
 */
ares__llist_node_t *ares__llist_node_first(ares__llist_t *list);

/*! Obtain last node in list
 * 
 *  \param[in] list  Initialized list object
 *  \return last node in list or NULL if none
 */
ares__llist_node_t *ares__llist_node_last(ares__llist_t *list);

/*! Obtain next node in respect to specified node
 * 
 *  \param[in] node  Node referenced
 *  \return node or NULL if none
 */
ares__llist_node_t *ares__llist_node_next(ares__llist_node_t *node);

/*! Obtain previous node in respect to specified node
 * 
 *  \param[in] node  Node referenced
 *  \return node or NULL if none
 */
ares__llist_node_t *ares__llist_node_prev(ares__llist_node_t *node);

/*! Obtain value from node
 * 
 *  \param[in] node  Node referenced
 *  \return user provided value from node
 */
void *ares__llist_node_val(ares__llist_node_t *node);

/*! Obtain the number of entries in the list
 * 
 *  \param[in] list  Initialized list object
 *  \return count
 */
size_t ares__llist_len(ares__llist_t *list);

/*! Obtain list object from referenced node
 * 
 *  \param[in] node  Node referenced
 *  \return list object node belongs to
 */
ares__llist_t *ares__llist_node_parent(ares__llist_node_t *node);

/*! Obtain the first user-supplied value in the list
 *  
 *  \param[in] list Initialized list object
 *  \return first user supplied value or NULL if none
 */
void *ares__llist_first_val(ares__llist_t *list);

/*! Obtain the last user-supplied value in the list
 *  
 *  \param[in] list Initialized list object
 *  \return last user supplied value or NULL if none
 */
void *ares__llist_last_val(ares__llist_t *list);

/*! Take ownership of user-supplied value in list without calling destructor.
 *  Will unchain entry from list.
 *  
 *  \param[in] node Node referenced
 *  \return user supplied value
 */
void *ares__llist_node_claim(ares__llist_node_t *node);

/*! Replace user-supplied value for node
 * 
 *  \param[in] node Node referenced
 *  \param[in] val  new user-supplied value
 */
void ares__llist_node_replace(ares__llist_node_t *node, void *val);

/*! Destroy the node, removing it from the list and calling destructor.
 * 
 *  \param[in] node  Node referenced
 */
void ares__llist_node_destroy(ares__llist_node_t *node);

/*! Destroy the list object and all nodes in the list.
 * 
 *  \param[in] list Initialized list object
 */
void ares__llist_destroy(ares__llist_t *list);

/*! @} */

#endif /* __ARES__LLIST_H */
