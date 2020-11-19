#include "graph.h"
#include "string.h"

int initialize_graph (Graph *mygraph, int MaxSize) 
{
  if(mygraph == NULL){ return -1; }
  mygraph->MaxSize = MaxSize;
  mygraph->table = (Node*) malloc(sizeof(Node)*MaxSize); 
  check(mygraph->table,"mygraph->table allocation failed in initialize_graph");
  for(int i=0;i<MaxSize;i++){ 
    mygraph->table[i].name=NULL; 
    mygraph->table[i].outlist=NULL;
    mygraph->table[i].outdegree = 0;
  }
  return 1;
}
int insert_graph_node (Graph *mygraph, int n, char *name) 
{
  if(mygraph == NULL){ return -1; }
  mygraph->table[n].name = strdup(name); 
  return 1;
}
int insert_graph_link (Graph *mygraph, int source, int target) 
{
  if(mygraph == NULL){ return -1; }
  List* list = malloc(sizeof(List));
  check(list,"list allocation failed in insert_graph_link");
  list->index = target;
  list->next = mygraph->table[source].outlist; 
  mygraph->table[source].outlist = list; 
  mygraph->table[source].outdegree++;
  return 1;
}
// use to check result of strdup, malloc etc.
void check (void *memory, char *message) 
{
  if (memory == NULL) 
  {
    fprintf (stderr, "Can't allocate memory for %s\n", message);
    exit (3);
  }
}
int read_graph (Graph *mygraph, char *filename)
/* 
 * Reads in graph from FILE *filename which is of .gx format.
 * Stores it as Graph in *mygraph. 
 * Returns an error if file does not start with MAX command,
 * or if any subsequent line is not a NODE or EDGE command. 
 * Does not check that node numbers do not exceed the maximum number
 * Defined by the MAX command. 
 * 8/2/2010 - JLS
 */
{
  FILE *fp;
  char command[80], name[80];
  int i, s, t;
  fp= fopen (filename, "r");
  if (fp==NULL) 
  {
    fprintf(stderr,"cannot open file %s\n", filename);
    return -1;
  }
  printf ("Reading graph from %s\n", filename);
  fscanf (fp,"%s", command);
  if (strcmp (command, "MAX")!=0) 
  {
    fprintf (stderr, "Error in graphics file format\n");
    fclose (fp);
    return -1;
  } 
  else 
  {
    fscanf (fp, "%d", &i);
    initialize_graph (mygraph, i+1); // +1 so nodes can be numbered 1..MAX
    while (fscanf (fp, "%s", command)!=EOF) 
    {
      if (strcmp (command, "NODE")==0) 
      {
        fscanf (fp, "%d %s", &i, name);
        insert_graph_node (mygraph, i, name);
      } 
      else 
      {
        if (strcmp (command, "EDGE")==0) 
        {
          fscanf (fp, "%d %d", &s, &t);
          insert_graph_link (mygraph, s, t);
        } 
        else 
        {
	  fclose (fp);
          return -1;
        }
      }
    }
  }
  fclose (fp);
  return 0;
}
void print_graph (Graph *mygraph)
/* 
 * Prints out Graph *mygraph to the stdout in .gx format - JLS
 */
{
  int i;
  List *current;
  printf ("MAX %d\n", mygraph->MaxSize);
  for (i=0; i<mygraph->MaxSize; i++)
    if (mygraph->table[i].name!=NULL) 
    {
      printf ("NODE %d %s\n", i, mygraph->table[i].name);
      current= mygraph->table[i].outlist;
      while (current!=NULL) 
      {
        printf ("EDGE %d %d\n", i, current->index);
        current= current->next;
      }
    }
}

void tidyGraph(Graph* mygraph)
{
  for(int i=0;i<mygraph->MaxSize;i++){
    Node n = mygraph->table[i];
    free(n.name);
    tidyList(n.outlist);
  }
  free(mygraph->table);
}
void tidyList(List* list)
{
  if(list==NULL){ return;}
  List* next = list->next;
  while(list!=NULL){
    free(list);
    list = next;
    if(list!=NULL){ next = list->next;}
  }
}
