#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "general.h"
#include "graph.h"
#include "pq.h"
#include "hashmap.h"
#include "sp_algorithms.h"


size_t randsize(size_t l, size_t h) {
  assert (l<h);
  size_t res = round(drand48() * (h-l-1)) + l;
  assert(l<=res && res < h);
  return res;
}

raw_weight_t randrawweight(raw_weight_t mn, raw_weight_t mx) {
  assert (WEIGHT_MIN <= mn && mn<=mx && mx<=WEIGHT_MAX);
  raw_weight_t res = round(drand48() * (mx - mn)) + mn;
  assert(mn <= res && res <= mx);
  return res;
}

weight_t randweight(double p_inf,raw_weight_t mn, raw_weight_t mx) {
  if (drand48()<p_inf) return weight_inf();
  else return weight_of_int(randrawweight(mn,mx));
}

char *randname(size_t len) {
    static const char alpha[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    char *s = CALLOC(char,len+1);

    for (int i = 0; i < len; ++i) {
        s[i] = alpha[rand() % (sizeof(alpha) - 1)];
    }

    s[len] = 0;

    return s;
}


graph_t *rand_graph(size_t N, double p_edge, raw_weight_t min_weight, raw_weight_t max_weight) {


  graph_t *g = graph_new(N);

  for (node_t u=0; u<N; ++u) {
    for (node_t v=0; v<N; ++v) {
      if (u!=v && drand48() < p_edge) {
        weight_t w = weight_of_int(round(drand48() * (max_weight - min_weight)) + min_weight);
        graph_add_edge(g,u,w,v);
      }
    }
  }

  return g;
}

typedef struct _point_t {
  double x;
  double y;
} point_t;

typedef struct _map_graph_t {
  graph_t *g;
  point_t *coords;
} map_graph_t;

void map_graph_delete(map_graph_t mg) {
  graph_delete(mg.g);
  free(mg.coords);
}

weight_t coords_dist(point_t *coords, node_t u, node_t v) {
  double dx = coords[u].x - coords[v].x;
  double dy = coords[u].y - coords[v].y;

  return (weight_of_int(round(sqrt(dx*dx + dy*dy))));
}


map_graph_t rand_map_graph(size_t N, double dim, double p_edge) {

  graph_t *g = graph_new(N);
  point_t *coords = CALLOC(point_t,N);


  for (size_t u=0;u<N;++u) {
    coords[u].x = drand48() * dim;
    coords[u].y = drand48() * dim;
  }

  for (node_t u=0; u<N; ++u) {
    for (node_t v=0; v<N; ++v) {
      if (u!=v && drand48() < p_edge) {
        double dx = coords[u].x - coords[v].x;
        double dy = coords[u].y - coords[v].y;

        weight_t w = weight_of_int(round(sqrt(dx*dx + dy*dy)));
        graph_add_edge(g,u,w,v);
      }
    }
  }

  return (map_graph_t){g,coords};
}







void testDPQ() {

  size_t N=1000;
  DPQ_t *pq = DPQ_new(N);

  // Fill queue
  weight_t total = weight_of_int(0);

  for (size_t i=0;i<N;++i)
  {

    assertmsg(weight_is_inf(DPQ_prio(pq,i)),"Initially, all priorities must be infinite");

    if (i%10 != 0) {
      weight_t w = randweight(.01,-10000,10000);
      if (!weight_is_inf(w)) total = weight_add(total,w);

      DPQ_insert(pq,i,w);

    }
  }

  // Check membership, and total value
  weight_t total2 = weight_of_int(0);
  for (size_t i=0;i<N;++i) {
    assert(DPQ_contains(pq,i) == (i%10 != 0));
    if (DPQ_contains(pq,i)) {
      weight_t w = DPQ_prio(pq,i);
      if (!weight_is_inf(w)) total2 = weight_add(total2,w);

    }
  }
  assert(weight_eq(total2, total));

  // Decrease some keys
  total = weight_of_int(0);
  for (size_t i=1;i<N;++i)
  {
    if (DPQ_contains(pq,i))
    {
      weight_t w = DPQ_prio(pq,i);

      bool ch=false;

      if (drand48() < .5)
      {
        if (weight_is_inf(w)) w = randweight(0,-10000,10000);
        else w = randweight(0,-11000,weight_to_int(w) - 1);
        ch=true;

      }

      if (!weight_is_inf(w)) total = weight_add(total,w);

      if (ch) DPQ_decrease_key(pq,i,w);

      assert(weight_eq(DPQ_prio(pq,i),w));
    }
  }

  // Check membership and total value

  total2 = weight_of_int(0);
  for (size_t i=0;i<N;++i)
  {

    if (DPQ_contains(pq,i)) {
      weight_t w = DPQ_prio(pq,i);
      if (!weight_is_inf(w))
      {
       total2 = weight_add(total2,w);
      }
    }
  }

  assert(weight_eq(total2, total));

  // Pop keys
  total2 = weight_of_int(0);
  assert(!DPQ_is_empty(pq));
  node_t u = DPQ_pop_min(pq);

  weight_t last = DPQ_prio(pq,u);
  if (!weight_is_inf(last)) total2 = weight_add(total2,last);

  while (!DPQ_is_empty(pq))
  {
    u = DPQ_pop_min(pq);
    weight_t w = DPQ_prio(pq,u);
    assert(!weight_less(w,last));
    last = w;
    if (!weight_is_inf(last)) total2 = weight_add(total2,last);
  }

  assert(weight_eq(total2, total));

  DPQ_delete(pq);

}



void test_hashmap(hashing_mode_t mode) {
  size_t N = 1000;
  char *keys[N];

  hashmap_t *m = hashmap_new(2,mode);

  for (size_t i=0;i<N;++i) {
    char *name = randname(randsize(1,50));
    char k[strlen(name) + 20];
    sprintf(k,"%s%zu",name,i);
    free(name);
    keys[i] = strdup(k);
    m = hashmap_insert(keys[i],i,m);

    if (!(hashmap_contains(keys[i],m))) {
      printf("CONTAINS-KEY: %s\n", keys[i]);
      hashmap_print_set(m);
      assert(false);
    };
  }

  assert(hashmap_get_size(m) == N);

  for (size_t i=0;i<N;++i) {
    assert(hashmap_contains(keys[i],m));
    size_t v = SIZE_MAX;
    if (!hashmap_lookup(keys[i],&v,m)) assert(false);
    assert(v == i);
  }

  for (size_t i=0;i<N;++i) {
    char *name = randname(randsize(1,50));
    assert(!hashmap_contains(name,m));
    free(name);
  }

  hashmap_delete(m);

  for (size_t i=0;i<N;++i) free(keys[i]);
}


void compute_weight_matrix(graph_t const *g, size_t N, weight_t w[N][N]) {
  for (size_t u=0;u<N;++u) {
    for (size_t v=0;v<N;++v)
      w[u][v]=weight_inf();

    for (edge_tgt_t const *tgt = graph_succs_begin(g,u); tgt !=graph_succs_end(g,u); ++tgt) {
      w[u][tgt->v] = tgt->w;
    }
  }
}

typedef struct _path_info_t {
  weight_t weight;
  size_t len;
  node_t src;
  node_t dst;
} path_info_t;


path_info_t compute_path_info(graph_t const *g, path_t const *p) {
  path_info_t res = {weight_zero(),0,INVALID_NODE,INVALID_NODE};
  size_t N = graph_get_num_nodes(g);

  if (path_len(p) == 0) return res;


  // Build weight matrix
  weight_t w[N][N];

  compute_weight_matrix(g,N,w);

//   for (size_t u=0;u<N;++u) {
//     for (size_t v=0;v<N;++v)
//       w[u][v]=weight_inf();
//
//     for (edge_tgt_t const *tgt = graph_succs_begin(g,u); tgt !=graph_succs_end(g,u); ++tgt) {
//       w[u][tgt->v] = tgt->w;
//     }
//   }

  node_t u = path_get(p,0);

  res.src = u;

  for (size_t i=1;i<path_len(p);++i) {
    node_t v = path_get(p,i);
    ++res.len;
    res.weight = weight_add(res.weight,w[u][v]);
    u=v;
  }

  res.dst = u;

  return res;
}


void check_sp_result(graph_t const *g, node_t src, node_t dst, weight_t real_dist, sp_result_t const *r) {

  assert(weight_eq(r->dist, real_dist));

  assert(src == r->src && dst == r->dst);


  if (r->path == NULL && weight_is_inf(r->dist)) {
    assert(src != dst);
    // No check for actual unreachability
    return;
  }

  // If reachable, we expect a path and a finite distance
  assert(r->path && !weight_is_inf(r->dist));

  path_info_t pi = compute_path_info(g,r->path);

  // Path must go from src to dst
  assert(pi.src == src && pi.dst == dst);

  // Its weight must match the claimed distance
  assertmsg(weight_eq(pi.weight, r->dist),"Path weight must match claimed distance: %ld != %ld",pi.weight.__w,r->dist.__w);
}


void check_sssp_result(graph_t const *g, node_t src, sssp_result_t const *r) {
  size_t N = graph_get_num_nodes(g);
  assert (r->N == N);

  msg1("check-apsp");

  assert(r->dist && r->pred);

  assert(src == r->src && r->dst==INVALID_NODE);


  /*
    Certification Algorithm

      === Reachability ===
        A node is reachable, iff its distance is not +inf.
        To verify reachability, we assert that there is no edge from a reachable to an unreachable node, and that src is reachable.

      === Distance not too long ===
        The distance of the start node must be 0, or -inf.
        We check that no edge can be relaxed, i.e., that for every edge (u,v), we have dist[u] + w(u,v) >= dist[v].
        This property guarantees that the specified distances are not too long.
        NOTE: This also works for nodes on negative cycles: Setting their distance to -inf is the only
              way that every node on the negative cycle can be stable.

      === Pred map correct ===
        pred[u] must be INVALID_NODE for unreachable nodes.
        for reachable nodes, (pred[u],u) must be an edge.
          EXCEPTION: Start node, if finite distance: Here, pred[src]=src

      === Distances not too short (finite) ===
        For nodes u with finite distance, we check that dist[pred[u]] + w(pred[u],u) == dist[u].
          EXCEPTION: Start node with finite distance


      === Distances not too short (negative weight cycles) ===
        For nodes with -inf distance, we actually follow the predecessor map to find a negative weight cycle

  */

  weight_t w[N][N];
  compute_weight_matrix(g,N,w);

  if (r->has_negative_cycle) msg1("Negative cycle reported");

  assertmsg(weight_is_neg_inf(r->dist[src]) || weight_eq(r->dist[src], weight_zero()), "Distance of start node can only be 0, or -inf");

  bool has_neg_inf_dist=false; // Will be set if we encounter node with -inf distance. We'll compare that to result's negative weight cycle.

  for (node_t u=0;u<N;++u) {
    // Check that edges cannot be relaxed, and do not go from reachable to unreachable
    for (edge_tgt_t const *tgt = graph_succs_begin(g,u); tgt!=graph_succs_end(g,u); ++tgt) {
      node_t v = tgt->v;
      weight_t ew = tgt->w;

      assertmsg(weight_is_inf(r->dist[u]) || !weight_is_inf(r->dist[v]), "Edge from reachable to unreachable node");

      assert(!weight_is_inf(ew)); // If this fails, something is fishy with the graph: Edges shouldn't have infinite weight!

      weight_t rd = weight_add(r->dist[u],ew); // Relax distance for node v

      assertmsg(!weight_less(rd,r->dist[v]),"Edge %zu -(%ld)-> %zu still relaxable (dist[u]=%ld) %ld < %ld",u,weight_to_int(ew),v,r->dist[u].__w,rd.__w,r->dist[v].__w); // If edge can still be relaxed, dist-map is fishy!

    }

    // Check for pred-map

    if (weight_is_inf(r->dist[u])) {
      // Unreachable node
      assert(r->pred[u] == INVALID_NODE);
    } else if (!weight_is_neg_inf(r->dist[u])) {
      // Reachable node, not over negative-weight cycle
      node_t pu = r->pred[u];
      assert(pu < N);

      if (u==src) assert(pu==src); // Special case: pred of source points to itself.
      else {
        weight_t ew = w[pu][u];
        assertmsg(weight_is_finite(ew),"Pred over non-edge: %zu -> %zu (w=%ld)",pu,u,w[pu][u].__w);
        weight_t rd = weight_add(r->dist[pu],ew); // Relax distance for node u

        assert(weight_eq( rd, r->dist[u])); // Estimates for edges on shortest path must be precise

      }
    } else {
      // Reachable node, over negative-weight cycle
      has_neg_inf_dist=true;

      node_t pu = r->pred[u];
      weight_t ew = w[pu][u];
      assert(pu < N);

      assertmsg(weight_is_finite(ew),"Pred over non-edge: %zu -> %zu (w=%ld)",pu,u,w[pu][u].__w);

      // Follow the cycle.
      // Take N steps to make sure we are actually on the cycle
      for (size_t i=0; i<N; ++i) {
        pu = r->pred[pu];
        assert(pu<N);
      }

      // Now do another round through the cycle, computing its weight
      weight_t pw = weight_zero();
      node_t v = pu;
      do {
        pw = weight_add(pw,w[r->pred[v]][v]);
        v = r->pred[v];
      } while (v != pu);

      assert(weight_less(pw,weight_zero()));
    }
  }

  assert(has_neg_inf_dist == r->has_negative_cycle);


}

void check_sssp_result_compat(sssp_result_t const *r1, sssp_result_t const *r2) {
  assert(r1->N == r2->N);
  size_t N = r1->N;
  for (node_t u = 0; u<N; ++u) {
    assert(weight_eq(r1->dist[u], r2->dist[u]));
  }
}


typedef enum _tests_t {
  TEST_HASHMAP        = 0x01,
  TEST_PQ             = 0x02,
  TEST_BFS            = 0x04,
  TEST_BELLMAN_FORD   = 0x08,
  TEST_BELLMAN_FORD_NEG = 0x10,
  TEST_DIJKSTRA       = 0x20,
  TEST_ASTAR          = 0x40
} tests_t;

tests_t do_tests = 0xFF;


// Returns distance to node dst
weight_t check_sssp_algos(graph_t const *g, node_t src, node_t dst, bool negative) {
  sssp_result_t *r1 = NULL;
  sssp_result_t *r2 = NULL;

  weight_t d = weight_inf();

  if (!negative && (do_tests & TEST_DIJKSTRA)) {
    msg1("Dijkstra");
    r1 = dijkstra(g,src,INVALID_NODE);
    check_sssp_result(g,src,r1);
    d = r1->dist[dst];
  }

  if (do_tests & TEST_BELLMAN_FORD) {
    msg1("Bellman-Ford");
    r2 = bellman_ford(g,src);
    check_sssp_result(g,src,r2);
    d = r2->dist[dst];
  }

  if (r1 && r2) {
    msg1("Results-Compat-Check");
    check_sssp_result_compat(r1,r2);
  }

  if (r1) sssp_result_delete(r1);
  if (r2) sssp_result_delete(r2);

  return d;
}

graph_t *graph_unweighted_of(graph_t const *g) {
  size_t N = graph_get_num_nodes(g);
  graph_t *gg = graph_new(N);

  for (node_t u=0;u<N;++u) {
    for (edge_tgt_t const *tgt=graph_succs_begin(g,u); tgt!=graph_succs_end(g,u);++tgt) {
      graph_add_edge(gg,u,weight_of_int(1),tgt->v);
    }
  }

  return gg;
}



void do_graph_check(size_t N, double p, raw_weight_t mn, raw_weight_t mx) {

  msg1("graph check %zu %lf [%ld..< %ld]",N,p,mn,mx);

  graph_t *g = rand_graph(N,p,mn,mx);

  check_sssp_algos(g,0,0,mn<0);

  if (do_tests & TEST_BFS) {
    msg1("BFS");
    graph_t *ug = graph_unweighted_of(g);
    sssp_result_t *r = bfs(ug,0,INVALID_NODE);
    check_sssp_result(ug,0,r);
    sssp_result_delete(r);
    graph_delete(ug);
  }

  graph_delete(g);
}

void do_map_graph_check(size_t N, double p, size_t src, size_t dst) {
  assert(src<N && dst<N);

  if (!(do_tests & TEST_ASTAR)) return;
  if (!(do_tests & (TEST_DIJKSTRA | TEST_BELLMAN_FORD))) error("For testing A*, you must also test Dijkstra and/or Bellman-Ford!");

  msg1("map graph check %zu %lf %zu ->* %zu",N,p,src,dst);


  map_graph_t mg = rand_map_graph(N,10000,p);

  weight_t real_dist = check_sssp_algos(mg.g,src,dst,false);

  weight_t h[N];
  for (size_t u=0; u<N; ++u) h[u] = coords_dist(mg.coords,u,dst);

  msg1("A*");
  sp_result_t *r = astar_search(mg.g,src,dst,h);

  check_sp_result(mg.g,src,dst,real_dist,r);

  sp_result_delete(r);

  map_graph_delete(mg);
}

void reseed_rand() {
  srand48(789231);
}

int main(int argc, char **argv) {

  do_tests=0;

  set_msg_verb(-1);

  for (int i=1;i<argc;++i) {
    if (strcmp(argv[i],"hashmap")==0) do_tests|=TEST_HASHMAP;
    else if (strcmp(argv[i],"pq")==0) do_tests|=TEST_PQ;
    else if (strcmp(argv[i],"bfs")==0) do_tests|=TEST_BFS;
    else if (strcmp(argv[i],"bellman-ford")==0) do_tests|=TEST_BELLMAN_FORD;
    else if (strcmp(argv[i],"bellman-ford-neg")==0) do_tests|=TEST_BELLMAN_FORD|TEST_BELLMAN_FORD_NEG;
    else if (strcmp(argv[i],"dijkstra")==0) do_tests|=TEST_DIJKSTRA;
    else if (strcmp(argv[i],"astar")==0) do_tests|=TEST_ASTAR;
    else if (strcmp(argv[i],"-v")==0) set_msg_verb(0);
    else if (strcmp(argv[i],"-vv")==0) set_msg_verb(1);
    else if (strcmp(argv[i],"-vvv")==0) set_msg_verb(2);
    else error("Unknown command line option: %s", argv[i]);
  }

  if (!do_tests) do_tests = 0xFF;



  if (do_tests & TEST_HASHMAP) {
    msg0("Testing Hashmap");
    reseed_rand();
    for (size_t i=0;i<200;++i) {
      test_hashmap(HASH_1_LINEAR_PROBING);
      test_hashmap(HASH_1_DOUBLE_HASHING);
      test_hashmap(HASH_1_QUADRATIC_PROBING);
    }
  }

  if (do_tests & TEST_PQ) {
    msg0("Testing PQ");
    reseed_rand();
    for (size_t i=0;i<1000;++i) testDPQ();
  }

  tests_t const GRAPH_TESTS = TEST_ASTAR | TEST_BELLMAN_FORD | TEST_BFS | TEST_DIJKSTRA;

  if (do_tests & GRAPH_TESTS) {
    msg0("Graph checks");

    double eprob = 1.0;

    msg0("Positive weights");
    reseed_rand();
    eprob = 1.0;
    for (int i=0;i<8;++i,eprob/=2) {
      do_graph_check(100,eprob,0,10000);
    }

    if (do_tests & TEST_BELLMAN_FORD_NEG) {
      msg0("Negative weights");
      reseed_rand();
      eprob = 1.0;
      for (int i=0;i<8;++i,eprob/=2) {
        do_graph_check(100,eprob,-10000,10000);
      }

      msg0("Corner case: All negative");
      reseed_rand();
      eprob = 1.0;
      for (int i=0;i<8;++i,eprob/=2) {
        do_graph_check(100,eprob,-10000,0);
      }
    }

    msg0("Corner case: All zero");
    reseed_rand();
    eprob = 1.0;
    for (int i=0;i<8;++i,eprob/=2) {
      do_graph_check(100,eprob,0,0);
    }

    msg0("Corner case: Singleton graph");
    reseed_rand();
    do_graph_check(1,0,0,0);
    do_map_graph_check(1,0,0,0);


    if (do_tests & TEST_ASTAR) {
      msg0("A*");
      reseed_rand();
      eprob = 1.0;
      for (int i=0;i<8;++i,eprob/=2) {
        do_map_graph_check(100,eprob,0,1);
      }
    }
  }

  return 0;
}
