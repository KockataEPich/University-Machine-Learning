// Giles Reger, 2019

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "global.h"
#include "pq.h"

// Some helper functions for checking the results of calls in tests
void check_result(int testno, char* expected, char* found){
 if(found == NULL){
   fprintf(stderr,"Test %d Failed when %s expected and NULL found\n",testno,expected);
   exit(-1);
 }
 if(strcmp(expected,found) != 0){
   fprintf(stderr,"Test %d Failed when %s expected and %s found\n",testno,expected,found);
   exit(-1);
 }
}
void assert(int testno, bool value, char* reason){
  if(!value){
   fprintf(stderr,"Test %d Failed as %s\n",testno,reason);
  }
}

// Check that hello and goodbye are poped in the right order
void run_test0(){
  printf("TEST 0\n");
  PriorityQueue queue = initialize_pq(10);
  printf("Initialised...\n");
  insert(queue,"hello",1);
  printf("Inserted hello with priority 1...\n");
  insert(queue,"goodbye",2);
  printf("Inserted goodbye with priority 2...\n");
  check_result(0,"hello",pop_min(queue));
  printf("Popped hello...\n");
  check_result(0,"goodbye",pop_min(queue));
  printf("Popped goodbye...\n");
  assert(0,is_empty(queue)," queue is meant to be empty");
  printf("Queue now empty\n\n\n");
  tidy(queue);
}

void run_test1(){
  printf("TEST 1\n");
  PriorityQueue queue = initialize_pq(40);
  printf("At this point the queue is initialised... \n");
  insert(queue,"JOB I", 20);
  printf("Inserted JOB I with priority 20... \n");
  insert(queue, "JOB IX", 10);
  printf("Inserted JOB IX with priority 10... \n");
  insert(queue, "JOB CM", 30);
  printf("Inserted JOB CM with priority 30... \n");
  check_result(1,"JOB IX", pop_min(queue));
  printf("Popped JOB IX...\n");
  check_result(1,"JOB I", pop_min(queue));
  printf("Popped JOB I...\n");
  check_result(1,"JOB CM", pop_min(queue));
  printf("Popped JOB CM...\n");
  assert(1,is_empty(queue), "queue should be empty");
  printf("Queue now empty\n\n\n");
  tidy(queue);
}

void run_test2(){
  printf("TEST 2\n");
  PriorityQueue queue = initialize_pq(40);
  printf("At this point the queue is initialised... \n");
  insert(queue,"Hello", 6);
  printf("Inserted Hello with priority 6... \n");
  insert(queue, "There", 10);
  printf("Inserted There with priority 10... \n");
  insert(queue, "Going", 30);
  printf("Inserted Going with priority 30... \n");
  insert(queue, "Is", 19);
  printf("Inserted Is with priority 19... \n");
  insert(queue, "How", 15);
  printf("Inserted How with priority 15... \n");
  insert(queue, "It", 23);
  printf("Inserted It with priority 23... \n");
  check_result(2,"Hello", pop_min(queue));
  printf("Popped Hello...\n");
  check_result(2,"There", pop_min(queue));
  printf("Popped There...\n");
  check_result(2,"How", pop_min(queue));
  printf("Popped How...\n");
  check_result(2,"Is", pop_min(queue));
  printf("Popped Is...\n");
  check_result(2,"It", pop_min(queue));
  printf("Popped It...\n");
  check_result(2,"Going", pop_min(queue));
  printf("Popped Going...\n");
  assert(2,is_empty(queue), "queue should be empty");
  printf("Queue now empty\n\n\n");
  tidy(queue);
}

void run_test3(){
  printf("TEST 3\n");
  PriorityQueue queue = initialize_pq(40);
  printf("At this point the queue is initialised... \n");
  insert(queue,"Thank", 5);
  printf("Inserted Thank with priority 5... \n");
  insert(queue, "Fine,", 2);
  printf("Inserted Fine, with priority 2... \n");
  insert(queue, "you!", 6);
  printf("Inserted you! with priority 6... \n");
  check_result(3,"Fine,", pop_min(queue));
  printf("Fine,...\n");
  check_result(3,"Thank", pop_min(queue));
  printf("Thank...\n");
  check_result(3,"you!", pop_min(queue));
  printf("you!...\n");
  assert(3,is_empty(queue), "queue should be empty");
  printf("Queue now empty\n\n\n");
  tidy(queue);
}

 void run_test4(){
  printf("TEST 4\n");
  PriorityQueue queue = initialize_pq(40);
  printf("At this point the queue is initialised... \n");
  insert(queue,"fail", 11);
  printf("Inserted fail with priority 11... \n");
  insert(queue, "Systems.", 21);
  printf("Inserted Systems with priority 21... \n");
  insert(queue, "Operating", 16);
  printf("Inserted Operating with priority 16... \n");
  insert(queue, "I", 2);
  printf("Inserted I with priority 2... \n");
  check_result(4,"I", pop_min(queue));
  printf("Popped I...\n");
  check_result(4,"fail", pop_min(queue));
  printf("Popped fail...\n");
  check_result(4,"Operating", pop_min(queue));
  printf("Popped Operating...\n");
  check_result(4,"Systems.", pop_min(queue));
  printf("Popped Systems. ...\n");
  assert(4,is_empty(queue), "queue should be empty");
  printf("Queue now empty\n\n\n");
  tidy(queue);
}


void run_test5(){
  printf("TEST 5\n");
  PriorityQueue queue = initialize_pq(20);
  printf("At this point the queue is initialised... \n");
  insert(queue,"didn´t", 14);
  printf("Inserted didn´t with priority 14... \n");
  insert(queue, "life.", 18);
  printf("Inserted life. with priority 18... \n");
  insert(queue, "least", 8);
  printf("Inserted least with priority 8... \n");
  insert(queue, "fail", 16);
  printf("Inserted fail with priority 16... \n");
  insert(queue, "At", 5);
  printf("Inserted At with priority 5... \n");
  insert(queue, "you", 10);
  printf("Inserted you with priority 10... \n");
  insert(queue, "in", 17);
  printf("Inserted in with priority 17... \n");
  check_result(5,"At", pop_min(queue));
  printf("Popped At...\n");
  check_result(5,"least", pop_min(queue));
  printf("Popped least...\n");
  check_result(5,"you", pop_min(queue));
  printf("Popped you...\n");
  check_result(5,"didn´t", pop_min(queue));
  printf("Popped didn´t...\n");
  check_result(5,"fail", pop_min(queue));
  printf("Popped fail...\n");
  check_result(5,"in", pop_min(queue));
  printf("Popped in...\n");
  check_result(5,"life.", pop_min(queue));
  printf("Popped life...\n");
  assert(5,is_empty(queue), "queue should be empty");
  printf("Queue now empty\n\n\n");
  tidy(queue);
}


char* prog_name = 0;
int main (int argc, char *argv[])
{
 prog_name=argv[0];
 if(argc != 2){
   fprintf(stderr,"Supply test number\n");
   return -1;
 }
 char* tmp;
 int test_number = strtol(argv[1],&tmp,0);
 if(*tmp != '\0'){
   fprintf(stderr,"Supply test number as an integer\n");
   return -1;
 }
 switch(test_number){
   case 0: run_test0(); break;
   case 1: run_test1(); break;
   case 2: run_test2(); break;
   case 3: run_test3(); break;
   case 4: run_test4(); break;
   case 5: run_test5(); break;


   default:
     fprintf(stderr,"Test number %d not recognised\n",test_number);
 }
 return 0;
}
