#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "general.h"
#include "sp_algorithms.h"
#include "hashmap.h"
#include "airports.h"


void count_reachable(char const *code) {
  /**
   * Print out the number of airports reachable from airport code.
   * Also count the start airport itself!
   */

  graph_t const *g = ap_get_graph();
  node_t src = ap_get_id(code);

  sssp_result_t *resultSP;
  node_t dst = -1;
  resultSP = bfs(g, src, dst);

  size_t count = 0;

  for(node_t i = 0; i < graph_get_num_nodes(g); i++) 
  {
    if(!weight_is_inf(resultSP->dist[i]))
    {
      count++;
    }
  }
  // Use this format!
  sssp_result_delete(resultSP);
  printf("%zu airports reachable from %s\n",count,code);


}

void compute_route(char const *algo, char const *scode, char const *dcode) {
  node_t s=ap_get_id(scode);
  node_t d=ap_get_id(dcode);
  graph_t const *g = ap_get_graph();
 
  unsigned long long relaxedCount; // Used for outputing the relaxed nodes 
  bool is_astar = false;           // Checks if the alg is A*

  sssp_result_t *resultSP;         // results for the appropriate algorithms
  sp_result_t *sp_resultSP;         
 

  /** Compute a shortest route between s and d, using the specified algorithm!
   *  "bfs" should compute a route with minimal hops, all other algorithms compute a route with minimal mileage
   */

  if (strcmp(algo,"bellman-ford")==0)
  {
    printf("BELLMAN FORD\n");
    resultSP = bellman_ford(g, s);
  }
  else if (strcmp(algo,"dijkstra")==0)
  {
    resultSP = dijkstra(g, s, d);
    printf("DIJKSTRA\n");
  }
  else if (strcmp(algo,"astar")==0)
  {
    size_t N = graph_get_num_nodes(g);
    
    is_astar = true;

    // Compute the h
    weight_t *h = malloc(sizeof(weight_t) * N);

    for(node_t index; index < N; index++)
    {
      if(ap_is_valid_id(index))
       h[index] = ap_get_dist(index, d);
    }//for

    sp_resultSP = astar_search(g, s, d, h);
    
    free(h);
    printf("ASTAR\n");
  }
  else if (strcmp(algo,"bfs")==0)
  {
    resultSP = bfs(g, s, d);
    printf("BFS\n");
  }
  else error("Invalid algorithm name: %s", algo);

  // Check if it is astar or others 
  bool no_route = false;
  if(!is_astar)
  { 
    // Check if the target is reachable
    relaxedCount = resultSP->relax_count;
    if(weight_is_inf(resultSP->dist[d]))
      no_route = true;
  }//if
  else
  {
   // Check if the target is reachable
   relaxedCount = sp_resultSP->relax_count;
   if(sp_resultSP->path == NULL)
    no_route = true;
  }//else




  // If there is a route
  if(!no_route)
  {
    // Get the path from the src to the dst depending on the algorithm used
    path_t *shortestPath;
    if(!is_astar)
    {
     shortestPath = path_from_pred(resultSP->pred, d);
     sssp_result_delete(resultSP);
    } 
    else
      shortestPath = sp_resultSP->path;

    weight_t totalDistance = weight_zero();

    // Output one line per hop with its distance
    for(size_t i = 0; i < path_len(shortestPath) - 1;i++)
    {

     node_t source = path_get(shortestPath, i);
     node_t destination = path_get(shortestPath, i+1);
     char*  sourceCode = ap_get_code(source);
     char*  destinationCode = ap_get_code(destination);

     weight_t distanceSrcToDst = ap_get_dist(source, destination);

     totalDistance = weight_add(totalDistance, distanceSrcToDst);
     printf("%s to %s (%ldkm)\n",sourceCode, destinationCode,
                                               weight_to_int(distanceSrcToDst));


    }
    path_delete(shortestPath);
    // Output the total distance
    printf("Total = %ldkm\n",weight_to_int(totalDistance));
  }
  else
   printf("No route from %s to %s \n", scode,dcode);

  // Output the relaxed edges
  msg0("relaxed %llu edges\n", relaxedCount);

 


  /// And, in any case, log the number of relaxed/explored edges

}


int main(int argc, char **argv)
{

  ap_std_init();

  if (argc==5 && strcmp(argv[1],"route")==0) compute_route(argv[2],argv[3],argv[4]);
  else if (argc==3 && strcmp(argv[1],"count")==0) count_reachable(argv[2]);
  else error("Invalid command line");

  ap_delete();

  return 0;
}
