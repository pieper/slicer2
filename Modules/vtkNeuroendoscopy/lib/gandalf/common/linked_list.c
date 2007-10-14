/**
 * File:          $RCSfile: linked_list.c,v $
 * Module:        Linked list
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:11:45 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <gandalf/common/linked_list.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonList Linked Lists
 * \{
 */

/* setup initial fields of list */
static void init_list ( Gan_List *l )
{
   l->p_first = NULL;
   l->p_current = NULL;
   l->p_last = NULL;

   l->node_count = 0;
   l->current_position = -1;
   l->is_straight = GAN_TRUE;
}

/**
 * \brief Makes a new singly linked list containing no elements.
 * \return The new list, or \c NULL on ailure.
 *
 * Makes a new singly linked list containing no elements. Memory for the list
 * is allocated using gan_malloc_object().
 */
Gan_List*
 gan_list_new(void)
{
   Gan_List *l;

   /* Initialise double list head */
   l = gan_malloc_object(struct Gan_List);

   if ( l == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_list_new", GAN_ERROR_MALLOC_FAILED, "list", sizeof(*l) );
      return NULL;
   }

   init_list(l);
   l->alloc = GAN_TRUE;
   return l;
}


/**
 * \brief Forms a new singly linked list containing no elements.
 * \return The newly formed list, or \c NULL on failure.
 *
 * Forms a new singly linked list containing no elements. Memory for the list
 * is provided in the given pointer list.
 */
Gan_List*
 gan_list_form(Gan_List *list)
{
   init_list(list);
   list->alloc = GAN_FALSE;
   return list;
}


/**
 * \brief Makes a new linked list containing nodes with data from array.
 * \return The new list, or \c NULL on failure.
 *
 * Makes a new singly linked list containing nodes with data from array.
 * The array and the number of elements in the array are passed as parameters.
 */
Gan_List*
 gan_list_new_from_array( void **array, unsigned n )
{
   unsigned i;
   Gan_List* list = gan_list_new();

   for ( i = 0; i < n; i++ )
      gan_list_insert_next(list, array[i]);

   return list;
}


/**
 * \brief Frees the list, nodes and data contained in the list.
 * \param list The list to free
 * \param free_func The function to free each node of the list
 * \return No value.
 *
 * Frees the list, nodes and data contained in the list. The \a list and the
 * function \a free_func to be used to deallocate the data contained in the
 * nodes are passed as parameters. Pass \c NULL for \a free_func if data
 * pointed to by the node should not to be freed.
 */
void
 gan_list_free        ( Gan_List* list, Gan_FreeFunc free_func)
{
   gan_list_delete_all(list, free_func);

   if(list->alloc) free(list);
}

/**
 * \brief Inserts a new node at the head of the list.
 * \param list The list
 * \param data The data pointer to insert in the list
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Inserts a new node at the head of the list that points to the object
 * pointed to by the data parameter.
 *
 * \sa gan_list_insert_next(), gan_list_insert_prev(), gan_list_insert_last().
 */
Gan_Bool
 gan_list_insert_first   ( Gan_List* list, void *data ) 
{
   Gan_List_Node *new_node;

   new_node = gan_malloc_object(struct Gan_List_Node);

   if ( new_node == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_list_insert_first", GAN_ERROR_MALLOC_FAILED, "list node", sizeof(*new_node) );
      return GAN_FALSE;
   }

   new_node->p_data = data;
        
   if(list->node_count != 0)
   {
      new_node->p_next = list->p_first;
      new_node->p_prev = list->p_last;
        
      list->p_last->p_next = new_node;                
      list->p_first->p_prev = new_node;
   }
   else
   {
      list->p_last = new_node;                
      new_node->p_prev = new_node->p_next = new_node;
   }

   list->p_first = new_node; 
        
   if(list->current_position == -1)
   {
      list->p_current = new_node;
   }
        
   list->current_position++;
   list->node_count++;

   /* success */
   return GAN_TRUE;
}


/**
 * \brief Inserts a new node in the position following the current one.
 * \param list The list
 * \param data The data pointer to insert in the list
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Inserts a new node in the position following the current position.
 * The current position can be queried by calling gan_list_get_pos().
 *
 * \sa gan_list_insert_first(), gan_list_insert_prev(), gan_list_insert_last().
 */
Gan_Bool
 gan_list_insert_next ( Gan_List* list, void *data ) 
{        
   Gan_List_Node* new_node;

   /* If the new node is the first */
   if(list->node_count == 0 || list->current_position == -1)
   {
      Gan_Bool result;

      result = gan_list_insert_first(list, data);        
      if ( !result )
         gan_err_register ( "gan_list_insert_next", GAN_ERROR_FAILURE,"");
                                 
      return result;
   }

   /* If the current node is the last */
   if(list->current_position + 1 == list->node_count)
   {
      Gan_Bool result;

      result = gan_list_insert_last(list, data);        
      if ( !result )
         gan_err_register ( "gan_list_insert_next", GAN_ERROR_FAILURE,"");

      list->p_current = list->p_last;
      
      return result;
   }

   new_node = gan_malloc_object(struct Gan_List_Node);
   if ( new_node == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_list_insert_next", GAN_ERROR_MALLOC_FAILED, "list node", sizeof(struct Gan_List_Node) );
      return GAN_FALSE;
   }

   new_node->p_data = data;

   new_node->p_next = list->p_current->p_next;
   new_node->p_prev = list->p_current;
        
   list->p_current->p_next->p_prev = new_node;
   list->p_current->p_next = new_node; 
   list->p_current = new_node;

   list->current_position++;
   list->node_count++;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Inserts a new node in the position before current one.
 * \param list The list
 * \param data The data pointer to insert in the list
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Inserts a new \a data node in the position before the current position.
 * The current position can be queried by calling gan_list_get_pos().
 *
 * \sa gan_list_insert_first(), gan_list_insert_next(), gan_list_insert_last().
 */
Gan_Bool
 gan_list_insert_prev( Gan_List* list, void *data ) 
{        
   Gan_List_Node* new_node;

   /* If the new node is the first */
   if(list->node_count == 0)
   {
      Gan_Bool result;

      result = gan_list_insert_first(list, data);        
      if ( !result )
         gan_err_register ( "gan_list_insert_prev", GAN_ERROR_FAILURE,"");

      return result;
   }

   if(list->current_position <= 0)
   {
      Gan_Bool result;

      gan_err_test_bool ( !list->is_straight, "gan_list_insert_prev",
                          GAN_ERROR_INCOMPATIBLE,
                          "Attempt to insert a node before the first in non-circular list" );
      result = gan_list_insert_last(list, data);
      if ( !result )
         gan_err_register ( "gan_list_insert_prev", GAN_ERROR_FAILURE,"");
      
      return result;
   }

   new_node = gan_malloc_object(struct Gan_List_Node);
   if ( new_node == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_list_insert_prev", GAN_ERROR_MALLOC_FAILED, "list node", sizeof(*new_node) );
      return GAN_FALSE;
   }

   new_node->p_data = data;
   new_node->p_next = list->p_current;
   new_node->p_prev = list->p_current->p_prev;
        
   list->p_current->p_prev->p_next = new_node;
   list->p_current->p_prev = new_node; 

   list->p_current = new_node;
   list->node_count++;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Inserts a new node at the tail of the list.
 * \param list The list
 * \param data The data pointer to insert in the list
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Inserts a new \a data node at the tail of the \a list.
 *
 * \sa gan_list_insert_first(), gan_list_insert_next(), gan_list_insert_prev().
 */
Gan_Bool
 gan_list_insert_last ( Gan_List *list, void *data )
{
   Gan_List_Node* new_node;

   /* If the new node is the first */
   if(list->node_count == 0)
   {
      Gan_Bool result;

      result = gan_list_insert_first(list, data);
      if ( !result )
         gan_err_register ( "gan_list_insert_last", GAN_ERROR_FAILURE, "" );

      return result;
   }

   new_node = gan_malloc_object(struct Gan_List_Node);
   if ( new_node == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_list_insert_last", GAN_ERROR_MALLOC_FAILED, "node structure", sizeof(*new_node) );
      return GAN_FALSE;
   }

   new_node->p_data = data;

   new_node->p_next = list->p_first;
   new_node->p_prev = list->p_last;

   list->p_last->p_next = new_node;
   list->p_first->p_prev = new_node;
                
   list->p_current = list->p_last = new_node;

   list->current_position = list->node_count;

   list->node_count++;
   

   /* success */
   return GAN_TRUE;
}


/**
 * \brief Inserts a new node at a specified position (0 = head).
 * \param list The linked list
 * \param data The data pointer to insert
 * \param pos The position at which to insert the data pointer
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Inserts a new \a data node at a specified position \a pos (0 = head).
 * If the specified position is larger than the current number of nodes the
 * new node is inserted at the tail of the list.
 */
Gan_Bool
 gan_list_insert_at ( Gan_List *list, void *data, int pos )
{
   Gan_List_Node *new_node, *tmp;
   int i;

   /* If the new node is to be inserted first */
   if(pos == 0)
   {
      Gan_Bool result;

      result = gan_list_insert_first(list, data);        
      if ( !result )
         gan_err_register ( "gan_list_insert_at", GAN_ERROR_FAILURE, "" );

      return result;
   }

   /* If the new node is to be inserted last */
   if(pos == list->node_count)
   {
      Gan_Bool result;

      result = gan_list_insert_last(list, data);        
      if ( !result )
         gan_err_register ( "gan_list_insert_at", GAN_ERROR_FAILURE, "" );

      return result;
   }

   gan_err_test_bool((0<= pos) && (pos <=list->node_count), "gan_list_insert_at",
                     GAN_ERROR_INCOMPATIBLE,
                     "Node insertion point not within allowable range");

   /* Walk the list untill the node before the insertion point is found */
   tmp = list->p_first;
   for(i=1; i < pos; i++)
   {
      tmp = tmp->p_next;
   }

   new_node = gan_malloc_object(struct Gan_List_Node);
   if ( new_node == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_list_insert_at", GAN_ERROR_MALLOC_FAILED, "list node", sizeof(*new_node) );
      return GAN_FALSE;
   }

   new_node->p_data = data;
   new_node->p_next = tmp->p_next;
   new_node->p_prev = tmp;
        
   tmp->p_next->p_prev = new_node;
   tmp->p_next = new_node; 
   
   if(pos < list->current_position + 1)
   {
      list->current_position++;
   }

   list->node_count++;

   /* success */
   return GAN_TRUE;
}


/**
 * \brief Deletes the node at the head of the list.
 * \param list The linked list
 * \param free_func The function to be called to free the node
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Deletes the node at the head of the list. Pass \c NULL if data pointed to
 * by the node should not to be freed
 */
Gan_Bool
 gan_list_delete_first( Gan_List* list, Gan_FreeFunc free_func)
{
   Gan_List_Node *tmp;

   gan_err_test_bool(list->node_count != 0, "gan_list_delete_first",
                     GAN_ERROR_INCOMPATIBLE,
                     "Attempt to delete from empty list");
        
   tmp = list->p_first;

   list->p_first = tmp->p_next;
   list->p_first->p_prev = tmp->p_prev;
   list->p_first->p_prev->p_next = list->p_first;

   if(free_func) free_func( tmp->p_data );
   free(tmp);
        
   if(list->current_position == 0)
   {
      list->p_current = list->p_first;
   }
   else
      if(list->current_position != -1)
         list->current_position--;

   if(list->node_count == 2)
   {
      list->p_last = list->p_first;
   }
        
   list->node_count--;

   if(list->node_count == 0)
   {
      list->p_first = list->p_last = list->p_current = NULL;
      list->current_position = -1;
   }

   /* success */
   return GAN_TRUE;
}


/**
 * \brief Deletes the node following the current position.
 * \param list The linked list
 * \param free_func The function to be called to free the node
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Deletes the node following the current position. The current position can
 * be queried by calling gan_list_get_pos(). If the current position is
 * -1 then the node at the head is deleted, if the current position is the
 * tail the list is a doubly linked circular list then the node at the head is
 * deleted. Pass \c NULL as the free_func if data pointed to by the node should
 * not to be freed.
 */
Gan_Bool
 gan_list_delete_next ( Gan_List* list, Gan_FreeFunc free_func)
{
   Gan_List_Node* tmp;

   gan_err_test_bool(list->node_count != 0, "gan_list_delete_next",
                     GAN_ERROR_INCOMPATIBLE,
                     "Attempt to delete from empty list");

   if(list->current_position == -1) 
   {
      Gan_Bool result;

      result = gan_list_delete_first(list, free_func);
      if ( !result )
         gan_err_register ( "gan_list_delete_next", GAN_ERROR_FAILURE,"");
      
      return result;
   }
   if(list->current_position + 1 == list->node_count)
   {
      Gan_Bool result;

      gan_err_test_bool(!list->is_straight, "gan_list_delete_next",
                        GAN_ERROR_INCOMPATIBLE,
                        "Attempt to delete non-existent node from list");
      result = gan_list_delete_first(list, free_func);        
      if ( !result )
         gan_err_register ( "gan_list_delete_next", GAN_ERROR_FAILURE,"");

      return result;
   }

   if(list->current_position + 1 == list->node_count - 1)
   {
      Gan_Bool result;

      result = gan_list_delete_last(list, free_func);
      if ( !result )
         gan_err_register ( "gan_list_delete_next", GAN_ERROR_FAILURE,"");

      return result;
   }

   tmp = list->p_current->p_next;

   tmp->p_next->p_prev = tmp->p_prev;
   tmp->p_prev->p_next = tmp->p_next;

   if(free_func) free_func( tmp->p_data );
   free(tmp);

   list->node_count--;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Deletes the node in the current position in a linked list.
 * \param list The linked list
 * \param free_func The function to be called to free the node
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Deletes the node in the current position in a doubly linked \a list.
 * Moves the current position to the next node in the list.
 */
Gan_Bool
 gan_list_delete_current ( Gan_List *list, Gan_FreeFunc free_func )
{
   Gan_List_Node *tmp;

   gan_err_test_bool(list->node_count > 0, "gan_list_delete_current",
                     GAN_ERROR_INCOMPATIBLE, "no elements to delete");
   gan_err_test_bool(list->current_position >= 0,
                     "gan_list_delete_current",
                     GAN_ERROR_INCOMPATIBLE, "illegal location");
   if(list->current_position == 0)
   {
      Gan_Bool result;

      result = gan_list_delete_first(list, free_func);        
      if ( !result )
         gan_err_register ( "gan_list_delete_current", GAN_ERROR_FAILURE,
                            "" );

      return result;
   }

   if(list->current_position == list->node_count - 1)
   {
      Gan_Bool result;

      result = gan_list_delete_last(list, free_func);
      if ( !result )
         gan_err_register ( "gan_list_delete_current", GAN_ERROR_FAILURE,
                                 "" );

      return result;
   }

   tmp = list->p_current;

   tmp->p_next->p_prev = tmp->p_prev;
   tmp->p_prev->p_next = tmp->p_next;

   list->p_current = tmp->p_next;

   if(free_func) free_func( tmp->p_data );
   free(tmp);

   list->node_count--;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Deletes the node in the position previous to the current one.
 * \param list The linked list
 * \param free_func The function to be called to free the node
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Deletes the node in the position previous to the current position in a
 * doubly linked \a list.
 */
Gan_Bool
 gan_list_delete_prev ( Gan_List *list, Gan_FreeFunc free_func )
{
   Gan_List_Node *tmp;

   gan_err_test_bool(list->node_count != 0, "gan_list_delete_prev",
                     GAN_ERROR_INCOMPATIBLE,
                     "Attempt to delete node from empty list");

   if(list->current_position <= 0)
   {
      Gan_Bool result;

      gan_err_test_bool(!list->is_straight, "gan_list_delete_prev",
                        GAN_ERROR_INCOMPATIBLE, "Attempt to delete non-existent node from non-circular list");
      result = gan_list_delete_last(list, free_func);        
      if ( !result )
         gan_err_register ( "gan_list_delete_prev", GAN_ERROR_FAILURE,
                            "" );
      return result;
   }

   if(list->current_position == 1)
   {
      Gan_Bool result;

      result = gan_list_delete_first(list, free_func);        
      if ( !result )
         gan_err_register ( "gan_list_delete_prev", GAN_ERROR_FAILURE,
                            "" );
      return result;
   }

   tmp = list->p_current->p_prev;

   tmp->p_next->p_prev = tmp->p_prev;
   tmp->p_prev->p_next = tmp->p_next;

   if(free_func) free_func( tmp->p_data );
   free(tmp);

   list->node_count--;
   list->current_position--;

   /* success */
   return GAN_TRUE;
}



/**
 * \brief Deletes the node with the specified position (0 = head).
 * \param list The linked list
 * \param free_func The function to be called to free the node
 * \param pos Position of node to delete
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Deletes the node with the specified position \a pos (0 = head).
 * Pass \c NULL for \a free_func if data pointed to by the node should
 * not to be freed.
 */
Gan_Bool
 gan_list_delete_at( Gan_List *list, Gan_FreeFunc free_func, int pos )
{
   Gan_List_Node *tmp;
   int i;
        
   /* If the first node is to be deleted */
   if(pos == 0)
   {
      Gan_Bool result;

      result = gan_list_delete_first(list, free_func);        
      if ( !result )
         gan_err_register ( "gan_list_delete_at", GAN_ERROR_FAILURE,
                                 "" );
      return result;
   }

   /* If the last node is to be deleted */
   if(pos == list->node_count-1)
   {
      Gan_Bool result;

      result = gan_list_delete_last(list, free_func);        
      if ( !result )
         gan_err_register ( "gan_list_delete_at", GAN_ERROR_FAILURE,
                            "" );
      return result;
   }

   gan_err_test_bool((0<= pos) && (pos<=list->node_count), "gan_list_delete_at",
                     GAN_ERROR_INCOMPATIBLE,
                     "Attempt to delete non-existent node");

   /* Walk the list untill the node before the deletion point is found */
   tmp = list->p_first;
   for(i=0; i < pos; i++)
   {
      tmp = tmp->p_next;
   }

   tmp->p_next->p_prev = tmp->p_prev;
   tmp->p_prev->p_next = tmp->p_next;


   if(list->p_current == tmp)
   {
      list->p_current = tmp->p_next;
   }

   if(free_func) free_func( tmp->p_data );
   free(tmp);

   list->node_count--;

   if(pos < list->current_position + 1)
   {
      list->current_position--;
   }        

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Deletes the node at the tail of the list.
 * \param list The linked list
 * \param free_func The function to be called to free the node
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Deletes the node at the tail of the list. Pass \c NULL if data pointed to
 * by the node should not to be freed.
 */
Gan_Bool
 gan_list_delete_last ( Gan_List *list, Gan_FreeFunc free_func)
{
   Gan_List_Node *tmp;

   if(list->node_count == 1)
   {
      Gan_Bool result;

      result = gan_list_delete_first(list, free_func);
      if ( !result )
         gan_err_register ( "gan_list_delete_last", GAN_ERROR_FAILURE,
                            "" );
      return result;
   }

   tmp = list->p_last;

   tmp->p_prev->p_next = tmp->p_next;
   tmp->p_next->p_prev = tmp->p_prev;        
        
   if(list->p_current == list->p_last)
   {
      list->p_current = list->p_last->p_prev;
      list->current_position--;
   }

   list->p_last = tmp->p_prev;

   if(free_func) free_func( tmp->p_data );
   free(tmp);
        
   list->node_count--;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Deletes all the nodes in the list and frees their data.
 * \param list The linked list
 * \param free_func The function to be called to free the nodes
 * \return No value.
 *
 * Deletes all the nodes in the \a list and frees the data they point to
 * using the function \a free_func. Pass \a NULL for free_func if the data
 * pointed to by the nodes should not to be freed
 */
void
 gan_list_delete_all ( Gan_List *list, Gan_FreeFunc free_func )
{
   Gan_List_Node *tmp, *tmp_del;
   int i;

   if(list->node_count == 0)
      return;

   tmp = list->p_first;

   for(i=0; i<list->node_count; i++)
   {
      if(free_func) free_func( tmp->p_data );

      tmp_del = tmp;
      tmp = tmp->p_next;

      free(tmp_del);
   }

   list->p_first = NULL;
   list->p_current = NULL;
   list->p_last = NULL;

   list->node_count = 0;
   list->current_position = -1;
}

/**
 * \brief Moves the current position to the position before the head
 * \param list The linked list
 * \return No value.
 *
 * Moves the current position to the position before the head. The current
 * position becomes -1 such that the first node in the list becomes the next
 * node and the gan_list_get_next() and gan_list_set_next() functions can be
 * used to iterate through the list.
 */
void
 gan_list_goto_head ( Gan_List *list)
{
   list->current_position = -1;
   list->p_current = NULL;
}

/**
 * \brief Moves the current position to the specified position
 * \param list The linked list
 * \param pos The position to move to
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Moves the current position to the specified position.
 *          - -1 = position before the first element
 *          - 0 = head of the list
 */
Gan_Bool
 gan_list_goto_pos ( Gan_List *list, int pos)
{
   int i;

   gan_err_test_bool(pos >= -1 , "gan_list_goto_pos",
                     GAN_ERROR_INCOMPATIBLE, "Goto position < -1");

   if(pos > list->node_count)
   {
      gan_list_goto_tail(list);
      return GAN_TRUE;
   }
        
   if(pos == -1)
   {
      gan_list_goto_head(list);
      return GAN_TRUE;
   }

   if(list->current_position == -1)
   {
      list->current_position = 0;
      list->p_current = list->p_first;
   }

   if(pos < list->current_position)
   {
      i = list->current_position - pos;
      while(i != 0)
      {
         list->p_current = list->p_current->p_prev;
         i--;
      }
      list->current_position = pos;
   }
   else if(pos > list->current_position)
   {
      i = pos - list->current_position;
      while(i != 0)
      {
         list->p_current = list->p_current->p_next;
         i--;
      }
      list->current_position = pos;        
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Moves the current position to the tail of the list.
 * \param list The linked list
 * \return No value.
 *
 * Moves the current position to the tail of the list.
 */
void
 gan_list_goto_tail ( Gan_List *list)
{
   list->current_position = list->node_count - 1;

   if(list->node_count != 0)
   {
      list->p_current = list->p_last;
   }
}

/**
 * \brief Not a user function.
 *
 * Use the gan_list_get_prev() macro instead.
 */
void *
 gan_list_get_prev_node ( Gan_List *list)
{
   void* data;

   if(list->current_position <= 0)
   {
      gan_assert(!list->is_straight, "ERROR: Attempt to get next node from end of non-circular list gan_list_get_prev_node()]");

      data = list->p_last->p_data;

      list->p_current = list->p_last;
      list->current_position = list->node_count - 1;
                
      return data;
   }

   data = list->p_current->p_prev->p_data;

   list->p_current = list->p_current->p_prev;
   
   if(list->current_position == 0) /*circular lists only*/
   {
      list->current_position = list->node_count - 1;
   }
   else
   {
      list->current_position--;
   }
   return data;
}

/**
 * \brief Not a user function.
 *
 * Use the gan_list_get_current() macro instead.
 */
void *
 gan_list_get_current_node( Gan_List *list )
{
   gan_assert(list->current_position != - 1, "ERROR: Attempt to get current node from at list position = -1 gan_list_get_current_node()");
   
   return list->p_current->p_data;
}

/**
 * \brief Not a user function.
 *
 * Use the gan_list_get_next() macro instead.
 */
void *
 gan_list_get_next_node                ( Gan_List *list)
{
   void* data;

   gan_assert(list->current_position != list->node_count - 1 || !list->is_straight, "ERROR: Attempt to get next node from end of non-circular list gan_list_get_next_node()");

   if(list->current_position == -1)
   {
      list->p_current = list->p_first;
      list->current_position = 0;
      return list->p_first->p_data;
   }

   data = list->p_current->p_next->p_data;

   list->p_current = list->p_current->p_next;

   if(list->current_position == list->node_count - 1) /*circular lists only*/
   {
      list->current_position = 0;
   }
   else
   {
      list->current_position++;
   }

   return data;
}

/**
 * \brief Sets the previous data pointer, decrements the current position.
 * \param list The linked list
 * \param new_data Data pointer
 * \return Data pointer.
 *
 * Sets the value of the data pointer in the previous node and decrements the
 * current position in a doubly linked list. A pointer to the old data is
 * returned.
 */
void *
 gan_list_set_prev (Gan_List *list, void *new_data)
{
   void* tmp;

   if(list->current_position <= 0)
   {
      gan_assert(!list->is_straight, "ERROR: Attempt to set data of next node at end of non-circular list gan_list_set_prev()");
        
      tmp = list->p_last->p_data;
      list->p_last->p_data = new_data;
      list->p_current = list->p_last;
      list->current_position = list->node_count-1;
      return tmp;                
   }

   tmp = list->p_current->p_prev->p_data;
   list->p_current->p_prev->p_data = new_data;
   list->p_current = list->p_current->p_prev;

   if(list->current_position == 0) /*circular lists only*/
   {
      list->current_position = list->node_count - 1;
   }
   else
   {
      list->current_position--;
   }

   return tmp;
}

/**
 * \brief Sets the value of the data pointer in the current node.
 * \param list The linked list
 * \param new_data Data pointer
 * \return Data pointer.
 *
 * Sets the value of the data pointer in the current node.
 * A pointer to the old data is returned.
 */
void *
 gan_list_set_current (Gan_List *list, void *new_data)
{
   void* tmp;
   gan_assert(list->current_position != - 1, "ERROR: Attempt to set data of node at position = -1 in list [dlist->set_current()]");
        
   tmp = list->p_current->p_data;
   list->p_current->p_data = new_data;

   return tmp;
}

/**
 * \brief Sets the next data pointer, increments the current position.
 * \param list The linked list
 * \param new_data Data pointer
 * \return Data pointer.
 *
 * Sets the value of the data pointer in the next node of a linked \a list and
 * increments the current position. A pointer to the old data is returned.
 */
void *
 gan_list_set_next (Gan_List *list, void *new_data)
{
   void* tmp;

   gan_assert(list->current_position != list->node_count - 1 || !list->is_straight, "ERROR: Attempt to set data of next node at end of non-circular list [dlist->set_next()]");
        

   if(list->current_position == -1)
   {
      tmp = list->p_first->p_data;
      list->p_first->p_data = new_data;
      list->p_current = list->p_first;
      list->current_position++;
      return tmp;        
   }

   tmp = list->p_current->p_next->p_data;

   list->p_current->p_next->p_data = new_data;
   list->p_current = list->p_current->p_next;

   if(list->current_position == list->node_count - 1) /*circular lists only*/
   {
      list->current_position = 0;
   }
   else
   {
      list->current_position++;
   }
   return tmp;
}


/**
 * \brief Pushes the current position of the data pointer onto the stack.
 * \param list The linked list
 * \return No value.
 *
 * Pushes the current position of the data pointer in the \a list onto the
 * stack.
 */
void
 gan_list_push_current ( Gan_List *list)
{
   list->current_position_stack = list->current_position;
   list->p_stack_current = list->p_current;
}

/**
 * \brief Pops the current position of the data pointer from the stack.
 * \param list The linked list
 * \return No value.
 *
 * Pops the current position of the data pointer in the \a list from the stack.
 */
void
 gan_list_pop_current ( Gan_List *list)
{
   list->current_position = list->current_position_stack;
   list->p_current = list->p_stack_current;
}



/**
 * \brief Returns the current position in the list.
 * \param list The linked list
 * \return Integer position.
 *
 * Returns the current position in the \a list.
 */
int
 gan_list_get_pos ( Gan_List *list )
{
   return list->current_position;
}

/**
 * \brief Returns the current number of nodes in the list.
 * \param list The linked list
 * \return Integer number of nodes.
 *
 * Returns the current number of nodes in the \a list.
 */
int
 gan_list_get_size ( Gan_List *list )
{
   return list->node_count;
}

/**
 * \brief Processes all elements in the list.
 * \param list The linked list
 * \param process_func Function to call on each element of the list
 * \param data Pointer passed to \a process_func
 * \return No value.
 *
 * Processes all elements in the \a list by calling the \a process_func.
 * As arguments to the function the pointer to a nodes data object and the
 * data pointer are passed.
 */
void
 gan_list_process_data ( Gan_List *list,
                         void (*process_func)(void *, void *), void *data )
{
   int i, old_pos;

   old_pos = gan_list_get_pos(list);
        
   gan_list_goto_head(list);
        
   for(i=0; i< list->node_count; i++)
      process_func ( gan_list_get_next_node(list), data );

   gan_list_goto_pos(list,old_pos);
}

/**
 * \brief Concatanates two lists, creating a new list.
 * \param list1 The first linked list
 * \param list2 The second linked list
 * \return The concatenated list as a new list.
 *
 * Concatanates lists \a list1 and \a list2, creating a new list that contains
 * the nodes of \a list1 followed by the nodes of \a list2.
 */
Gan_List*
 gan_list_concat( Gan_List *list1, Gan_List *list2 )
{
   int i;
   Gan_List *list = gan_list_new();

   gan_list_goto_head(list1);
   gan_list_goto_head(list2);

   for(i=0; i < list1->node_count; i++)
   {
      gan_list_insert_next(list, gan_list_get_next_node(list1));
   }

   for(i=0; i < list2->node_count; i++)
   {
      gan_list_insert_next(list, gan_list_get_next_node(list2));
   }
        
   return list;
}

/**
 * \brief Reverses the order of the elements in the list.
 * \param list The linked list
 * \return No value.
 *
 * Reverses the order of the elements in the \a list.
 */
void
 gan_list_reverse( Gan_List *list )
{
   int i;
   Gan_List_Node *tmp;

   if(list->node_count < 2)
      return;

   gan_list_goto_pos( list, 0 );

   for(i = 0; i < list->node_count; i++)
   {
      tmp = list->p_current->p_next; 

      list->p_current->p_next = list->p_current->p_prev;
      list->p_current->p_prev = tmp;

      list->p_current = tmp;
   } 

   tmp = list->p_last;

   list->p_last = list->p_first;
   list->p_first = tmp;

   gan_list_goto_head(list);
}

/**
 * \brief Creates an array of pointers to the objects in a list.
 * \param list The linked list
 * \param array_ptr Pointer to the allocated and filled array
 * \return \f$ \geq 0\f$ Number of nodes in the list, or -1 on failure.
 *
 * Creates an array of pointers to the objects pointed to by the nodes.
 */
int
 gan_list_to_array ( Gan_List *list, void ***array_ptr )
{
   int      i;
   void   **array;

   *array_ptr = array = gan_malloc_array ( void *, list->node_count );
   if ( list->node_count > 0 && *array_ptr == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_list_to_array", GAN_ERROR_MALLOC_FAILED, "", list->node_count*sizeof(void *) );
      return -1;      
   }

   gan_list_goto_head(list);

   for ( i = 0; i < list->node_count; i++ )
      array[i] = gan_list_get_next_node(list);        

   return list->node_count;
}

/**
 * \brief Copies all the nodes of a list, creating a new list.
 * \param list The linked list
 * \return The copy of the list, or \c NULL on failure.
 *
 * Copies all the nodes of a \a list, creating a new list. Data pointed to by
 * the nodes is not copied.
 */
Gan_List*
 gan_list_copy( Gan_List *list )
{
   int      i;
   Gan_List *list_new;
   Gan_List_Node* pNode;

   list_new = gan_list_new();
   if(list_new == NULL)
   {
      gan_err_register("gan_list_copy", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   pNode = list->p_first;
   for ( i = 0; i < list->node_count; i++ )
   {
      gan_list_insert_next( list_new, pNode->p_data );
      pNode = pNode->p_next;
   }

   return list_new;
}

/**
 * \brief Copies all the nodes of a list, creating a new list.
 * \param list The linked list
 * \param copy_func The function used to copy each node
 * \return The copy of the list, or \c NULL on failure.
 *
 * Copies all the nodes of a \a list, creating a new list, creating copies
 * of each node using the provided function \a copy_func.
 */
Gan_List*
 gan_list_copy_with_data( Gan_List *list, Gan_CopyFunc copy_func )
{
   int      i;
   Gan_List *list_new;
   Gan_List_Node* pNode;

   list_new = gan_list_new();
   if(list_new == NULL)
   {
      gan_err_register("gan_list_copy", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   pNode = list->p_first;
   for ( i = 0; i < list->node_count; i++ )
   {
      void *pNewData = copy_func(pNode->p_data);
      if(pNewData == NULL)
      {
         gan_err_flush_trace();
         gan_err_register("gan_list_copy", GAN_ERROR_FAILURE, "");
         return NULL;
      }

      gan_list_insert_next( list_new, pNewData );
      pNode = pNode->p_next;
   }

   return list_new;
}

/**
 * \brief Sorts the nodes in the list in ascending order.
 * \param list The linked list
 * \param compare Function to use to compare the list elements
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Sorts the nodes in the \a list in ascending order. A pointer to a comparison
 * function \a compare used to evaluate the order of two data objects must be
 * passed. See documentation for \c qsort().
 */
void
 gan_list_sort_asc ( Gan_List *list,
                     int (*compare)(const void *, const void *) )
{
   int no_nodes, i;
   void **arr;

   no_nodes = gan_list_get_size(list);

   if(no_nodes <= 1)
      return;
        
   gan_list_to_array(list, &arr);

   qsort ( arr, (size_t) no_nodes, sizeof(void *),
           (int (*)(const void *, const void *))compare );

   gan_list_goto_head(list);
 
   for (i = 0; i < no_nodes; i++)
      gan_list_set_next(list, arr[i]);

   free(arr);
}

/**
 * \brief Makes a doubly linked list behave like a straight linked list.
 * \param list The linked list
 * \return No value.
 *
 * Makes a doubly linked list behave like a straight linked list
 */
void gan_list_make_straight ( Gan_List *list )
{
   list->is_straight = GAN_TRUE;
}

/**
 * \brief Makes a doubly linked list behave like a circular linked list.
 * \param list The linked list
 * \return No value.
 *
 * Makes a doubly linked list behave like a circular linked list.
 */
void gan_list_make_circular ( Gan_List *list )
{
   list->is_straight = GAN_FALSE;
}

/**
 * \brief Returns #GAN_TRUE is the doubly linked list is circular.
 * \param list The linked list
 *
 * Returns #GAN_TRUE is the doubly linked list is circular, #GAN_FALSE
 * otherwise.
 */
Gan_Bool
 gan_list_is_circular ( Gan_List *list )
{
   return (Gan_Bool)!list->is_straight;
}

/**
 * \brief Returns #GAN_TRUE is the doubly linked list contains 'data'
 * \param list The linked list
 * \param data The element to look for
 *
 * Returns #GAN_TRUE is the doubly linked list contains 'data', #GAN_FALSE
 * otherwise.
 */
Gan_Bool 
 gan_list_contains  ( Gan_List *list, void *data )
{
   Gan_List_Node* pNode = list->p_first;
   int iCtr;

   for (iCtr = 0; iCtr < list->node_count; iCtr++)
   {
      if(pNode->p_data == data)   
         return GAN_TRUE;

      pNode = pNode->p_next;
   }

   return GAN_FALSE;
}

/**
 * \brief Returns #GAN_TRUE is the doubly linked lists are the same
 * \param List1 The first linked list
 * \param List2 The second linked list
 *
 * Returns #GAN_TRUE is the doubly linked list are the same, #GAN_FALSE otherwise.
 */
Gan_Bool gan_list_compare( Gan_List *List1, Gan_List *List2 )
{
   Gan_List_Node *pNode1, *pNode2;
   int iCtr;

   if(List1 == NULL && List2 == NULL) return GAN_TRUE;
   if(List1 == NULL || List2 == NULL ||
      List1->node_count != List2->node_count ||
      List1->is_straight != List2->is_straight)
      return GAN_FALSE;

   pNode1 = List1->p_first;
   pNode2 = List2->p_first;
   for (iCtr = 0; iCtr < List1->node_count; iCtr++)
   {
      if(pNode1->p_data != pNode2->p_data)
         return GAN_FALSE;

      pNode1 = pNode1->p_next;
      pNode2 = pNode2->p_next;
   }

   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */
