#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>

#include "general.h"
#include "graph.h"
#include "pq.h"


/**
 * Implement the interface given in pq.h.
 *
 * We make a few proposals here, but you are welcome to deviate from that, as long as you
 * correctly implement the interface with the specified worst-case complexities!
 *
 */



/**
 * First, we propose a 1-based indexing into the heap, using 0 as "invalid index".
 *
 * Wrapping a heap-index into a structure prevents its accidental confusion with element indexes (Which might save you from a lot of hard-to-find bugs!).
 *
 */

typedef struct {size_t i;} hidx_t;


const hidx_t INVALID_HIDX = {0};
const hidx_t hidx_first = {1};

size_t idx_of(hidx_t i) {return i.i - 1;} /// Convert to zero-based index
bool hidx_is_invalid(hidx_t i) {return i.i == 0;}

// Index operations
hidx_t parent(hidx_t i) {return (hidx_t){i.i / 2}; }
hidx_t left(hidx_t i) {return (hidx_t){i.i * 2}; }
hidx_t right(hidx_t i) {return (hidx_t){i.i * 2 + 1}; }
bool has_parent(hidx_t i) {return parent(i).i>0; }
bool has_left(size_t n, hidx_t i) {return left(i).i<=n; }
bool has_right(size_t n, hidx_t i) {return right(i).i<=n; }

/**
 * The priority queue has 3 main components:
 *   1. An array D that maps nodes to priorities. It is independent from the content of the actual queue.
 *   2. An array H of nodes that encodes the heap of size heap_size.
 *   3. An array I, that maps each node to its current index in H. If a node u is not in the queue, we set H[u] = INVALID_HIDX.
 *
 */
struct _DPQ_t {
    size_t N;     // Number of elements
    size_t heap_size;    // Current size of heap
    weight_t *D;  // Priorities
    node_t *H;    // The heap
    hidx_t *I;    // Index of element in heap.
};


DPQ_t *DPQ_new(size_t N) 
{
    // Initialize the structure. Allocate enough space for all arrays to hold N elements.
 
    DPQ_t* pq = (DPQ_t*) malloc(sizeof(DPQ_t));
    pq->N = N;
    pq->heap_size = 0;
    pq->D = (weight_t*) malloc(sizeof(weight_t)*N);
    pq->H = (node_t*) malloc(sizeof(node_t)*N);
    pq->I = (hidx_t*) malloc(sizeof(hidx_t)*N);

    for(int i = 0; i < N; i++)
    {
      pq->D[i] = weight_inf();
      pq->I[i] = INVALID_HIDX;
    }//for

    
    return pq;
}

void DPQ_delete(DPQ_t *pq)
{
    free(pq->D);
    free(pq->H);
    free(pq->I);
    // Free everything in pq

    // and pq itself
    free(pq);
}

weight_t *DPQ_dist_free(DPQ_t *pq)
{
  // Free everything but the D-array, which is returned
  weight_t *res=pq->D;

  free(pq->H);
  free(pq->I);
 
  free(pq);
  return res;
}


weight_t DPQ_prio(DPQ_t const *pq, node_t u)
{

  // Its a good idea to use assertions to ensure the preconditions!
  // This makes programs more likely to fail close to the reason of the error, rarher than later, when the (indirect) consequences become apparent.
   assert(u<pq->N);
   return pq->D[u]; /// *** return priority of node u
}

/**
 * It is a good idea to wrap repetitive patterns into an own function (though, in C, it can be a pain).
 * This reduces the redundancy, and the chance of accidentally getting one of the repeated versions wrong.
 *
 * In this case, we get the priority associated with a heap index.
 */
weight_t _DPQ_hprio(DPQ_t const *pq, hidx_t i)
{
    return DPQ_prio(pq,pq->H[idx_of(i)]);
}

/**
 * swapping the nodes
 */
void _DPQ_swap(DPQ_t *pq, hidx_t i, hidx_t j)
{
  node_t tempNode = pq->H[idx_of(i)];
  hidx_t tempIndex = pq->I[pq->H[idx_of(i)]];

  pq->H[idx_of(i)] = pq->H[idx_of(j)];
  pq->I[pq->H[idx_of(i)]] = pq->I[pq->H[idx_of(j)]];

  pq->H[idx_of(j)] = tempNode;
  pq->I[pq->H[idx_of(j)]] = tempIndex;


  // Swap the nodes
}

// Sift up
void _DPQ_sift_up(DPQ_t *pq, hidx_t i)
{
  // If we have a parent, check if the heap condition is satisfied.
  // If not, swap and try again.
  if(has_parent(i))
  {
    if(weight_less(pq->D[pq->H[idx_of(i)]], pq->D[pq->H[idx_of(parent(i))]]))
    {
      _DPQ_swap(pq, parent(i), i);
      _DPQ_sift_up(pq, parent(i));
    }//if
  }//if
}//DPQ_sift_up

void _DPQ_sift_down(DPQ_t *pq, hidx_t i)
{
   while(true)
   {

     // No children
     if(!has_left(pq->heap_size, i) && !has_right(pq->heap_size, i))
      return;
     // If only 1 left child
     if(has_left(pq->heap_size, i) && !has_right(pq->heap_size, i))
      if(weight_less(_DPQ_hprio(pq, left(i)), _DPQ_hprio(pq, i)))
      {
        _DPQ_swap(pq, left(i), i);
        return;
      }//if


     // Check which of the two children is the smallest
     hidx_t smallest;
     if(weight_less(_DPQ_hprio(pq, left(i)), _DPQ_hprio(pq, right(i))))
      smallest = left(i);
     else
      smallest = right(i);

     // Check if the heap property is not already in place.
     // If not swap the elements and continue from it downwards.
     if(weight_less(_DPQ_hprio(pq, smallest), _DPQ_hprio(pq, i)))
     {
       _DPQ_swap(pq, i, smallest);
       _DPQ_sift_down(pq, smallest);
     }//if
     else
      return;
  }//while
  // Don't forget to handle the case that you end up at an index that has a left, but no right child!

}//_DPQ_sift_up

 //  2. An array H of nodes that encodes the heap of size heap_size.
 //  3. An array I, that maps each node to its current index in H. If a node u is not in the queue, we set H[u] = INVALID_HIDX.

bool DPQ_contains(DPQ_t const *pq, node_t u) 
{
    assert(u<pq->N);
    return !hidx_is_invalid(pq->I[u]); // Use I to check if node is on heap
}//DPQ_contains


void DPQ_insert(DPQ_t *pq, node_t u, weight_t w)
{

  hidx_t heap_size_struct;
  // Insert at end, then sift up
  pq->D[u] = w;
  pq->H[pq->heap_size] = u;

  heap_size_struct.i = pq->heap_size + 1;
  pq->I[u] = heap_size_struct;

  pq->heap_size++;
  _DPQ_sift_up(pq,heap_size_struct);

  // Don't forget to update I!
}//DPQ_insert

bool DPQ_is_empty(DPQ_t const *pq) {return pq->heap_size==0;}



node_t DPQ_pop_min(DPQ_t *pq)
{
  assert(pq->heap_size>0);

  // Get the top most node
  node_t result = pq->H[idx_of(hidx_first)];
  hidx_t heap_size_struct;

  heap_size_struct.i = pq->heap_size;

  // Swap it with the last
  _DPQ_swap(pq,hidx_first,heap_size_struct);

  // Pretend it does not exist
  pq->heap_size--;

  // Sift down to find the location of the current node
  _DPQ_sift_down(pq,hidx_first);

  // Update I
  pq->I[result] = INVALID_HIDX;
  return result;

  // Swap first with last element, decrement heap size, then sift down

  // Return the node that was removed
}//DPQ_pop_min

void DPQ_decrease_key(DPQ_t *pq, node_t u, weight_t w) 
{
  assert(u<pq->N);
  assert(DPQ_contains(pq,u));
  assert(weight_less(w, pq->D[u]));
 
  // Update priority, then sift up. Use I to find element's index on the heap
 
  pq->D[u] = w;
  _DPQ_sift_up(pq, pq->I[u]);
 
}//DPQ_decrease_key
