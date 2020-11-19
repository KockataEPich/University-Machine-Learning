#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>

#include "graph.h"
#include "pq.h"
#include "sp_algorithms.h"

sssp_result_t *bfs(graph_t const *g, node_t src, node_t dst)
{
  unsigned long long stat_edges_explored = 0;

  size_t N = graph_get_num_nodes(g);
  node_t *pred = CALLOC(node_t,N);
  weight_t *dist = CALLOC(weight_t,N);

  // Initialise
  for(node_t i = 0; i < N; i++)
  {
    dist[i] = weight_inf();
  }//for

  // Input the source node
  pred[src] = src;
  dist[src] = weight_zero();

  // Put it in the priority queue
  DPQ_t *pq = DPQ_new(N);
  DPQ_insert(pq, src, weight_zero());

  // Create the dis map and pred map
  while(!DPQ_is_empty(pq))
  {
    node_t nextNode = DPQ_pop_min(pq);
    for (edge_tgt_t const *tgt=graph_succs_begin(g,nextNode);tgt!=graph_succs_end(g,nextNode);++tgt)
    {
      // If the node is unexplored, update it
      if(weight_is_inf(dist[tgt->v]))
      {
        dist[tgt->v] = weight_add(dist[nextNode], weight_of_int(1));
        pred[tgt->v] = nextNode;
        DPQ_insert(pq, tgt->v, dist[tgt->v]);

        // If we find the node we are done.
        if(nextNode == dst)
        {
          DPQ_delete(pq);
          return sssp_result_new(N,src,dst,false,pred,dist,stat_edges_explored);
        }//if
      }

     // Increment edges_explored
     stat_edges_explored++;

    }//for

  }//while

  DPQ_delete(pq);
  return sssp_result_new(N,src,dst,false,pred,dist,stat_edges_explored);
}//BFS


sssp_result_t *bellman_ford(graph_t const *g, node_t src)
{

  unsigned long long edgesRelaxed = 0;
  bool negative_cycle = false;    // Used to detect negative cycles

  // We have no dst
  node_t dst = INVALID_NODE;

  size_t N = graph_get_num_nodes(g);

  node_t *pred = CALLOC(node_t,N);
  weight_t *dist = CALLOC(weight_t,N);

  // Initialise
  for(node_t i = 0; i < N; i++)
  {
    dist[i] = weight_inf();
    pred[i] = INVALID_NODE;
  }//for

  pred[src] = src;
  dist[src] = weight_zero();

 // For every direction, for every node, for every edge
 bool is_there_change;
 for(node_t j = 0; j < N - 1; j++)
 {
   is_there_change = false;
   for(node_t i = 0; i < N; i++)
   {
     for (edge_tgt_t const *tgt=graph_succs_begin(g,i);tgt!=graph_succs_end(g,i);++tgt)
     {
       // Check if edge needs relaxing
       if(weight_is_inf(dist[tgt->v]) || weight_less(weight_add(dist[i], tgt->w), dist[tgt->v]))
       {
         dist[tgt->v] = weight_add(dist[i], tgt->w);
         pred[tgt->v] = i;
         edgesRelaxed++;
         is_there_change = true;
       }//if
     }//for
   }//for
   if(!is_there_change)
    break;
 }//for

 // Loops that check for negative cycles
 is_there_change = false;
 for(node_t j = 0; j < N - 1; j++)
 {
   for(node_t i = 0; i < N; i++)
   {

     for (edge_tgt_t const *tgt=graph_succs_begin(g,i);tgt!=graph_succs_end(g,i);++tgt)
     {
        if(weight_less(weight_add(dist[i], tgt->w), dist[tgt->v]))
        {
          // If a node has been relaxed -> negative cycle
          is_there_change = true;
          negative_cycle = true;
          dist[tgt->v] = weight_neg_inf();
          edgesRelaxed++;
        }//if
      }//for
    }//for
    if(!is_there_change)
     break;
  }//for
  return sssp_result_new(N,src,dst,negative_cycle,pred,dist,edgesRelaxed);

}//bellman_ford

sssp_result_t *dijkstra(graph_t const *g, node_t src, node_t dst)
{
  unsigned long long edgesRelaxed = 0;

  size_t N = graph_get_num_nodes(g);

  node_t *pred = CALLOC(node_t,N);
  weight_t *dist = CALLOC(weight_t,N);

  // Initialise
  for(node_t i = 0; i < N; i++)
  {
    dist[i] = weight_inf();
    pred[i] = INVALID_NODE;
  }//for

  pred[src] = src;
  dist[src] = weight_zero();

  DPQ_t *pq = DPQ_new(N);

  DPQ_insert(pq, src, dist[src]);


  while(!DPQ_is_empty(pq))
  {
    node_t nextNode = DPQ_pop_min(pq);

    // If we find the node we are done
    if(nextNode == dst)
    {
      DPQ_delete(pq);
      return sssp_result_new(N,src,dst,false,pred,dist,edgesRelaxed);
    }//if

    // Check the adjacent nodes.
    for (edge_tgt_t const *tgt=graph_succs_begin(g,nextNode);tgt!=graph_succs_end(g,nextNode);++tgt)
    {
      // Update a node if it needs relaxing
      if(weight_is_inf(dist[nextNode]) || weight_less(weight_add(dist[nextNode], tgt->w), dist[tgt->v]))
      {
        dist[tgt->v] = weight_add(dist[nextNode], tgt->w);
        pred[tgt->v] = nextNode;
        DPQ_insert(pq, tgt->v, dist[tgt->v]);
        edgesRelaxed++;
      }//if
    }//for
  }//while

  DPQ_delete(pq);
  return sssp_result_new(N,src,dst,false,pred,dist,edgesRelaxed);
}

sp_result_t *astar_search(graph_t const *g, node_t src, node_t dst, weight_t const* h)
{
  unsigned long long edgesRelaxed = 0;

  size_t N = graph_get_num_nodes(g);

  node_t *pred = CALLOC(node_t,N);
  weight_t *dist = CALLOC(weight_t,N);

  // Initialise
  for(node_t i = 0; i < N; i++)
  {
    dist[i] = weight_inf();
    pred[i] = INVALID_NODE;
  }//for

  pred[src] = src;
  dist[src] = weight_zero();

  DPQ_t *pq = DPQ_new(N);

  DPQ_insert(pq, src, dist[src]);


  // Do the same as in Dijkstra, except add the h weight to the normal weights
  while(!DPQ_is_empty(pq))
  {
    node_t nextNode = DPQ_pop_min(pq);

    if(nextNode == dst)
    {
      DPQ_delete(pq);
      return sssp_to_sp_result(sssp_result_new(N,src,dst,false,pred,dist,edgesRelaxed), dst);
    }//if

    for (edge_tgt_t const *tgt=graph_succs_begin(g,nextNode);tgt!=graph_succs_end(g,nextNode);++tgt)
    {
      if(weight_is_inf(dist[nextNode]) ||  weight_less(weight_add(weight_add(dist[nextNode], tgt->w), h[nextNode]), dist[tgt->v]))
      {
        dist[tgt->v] = weight_add(dist[nextNode], tgt->w);
        pred[tgt->v] = nextNode;
        DPQ_insert(pq, tgt->v, weight_add(dist[tgt->v], h[tgt->v]));
        edgesRelaxed++;
      }//if

    }//for
  }//while

  DPQ_delete(pq);
  return sssp_to_sp_result(sssp_result_new(N,src,dst,false,pred,dist,edgesRelaxed), dst);
}
