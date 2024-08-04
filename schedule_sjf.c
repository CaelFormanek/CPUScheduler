#include "schedulers.h"
#include "list.h"
#include "cpu.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


// global variables
struct node* list;
int counter = 0;
int total_waiting_time = 0;
int total_turnaround_time = 0;
int total_response_time = 0;
// atomic variable for tid
int value = 0;

// add a task to the list 
void add(char *name, int priority, int burst)
{
  Task* newTask = malloc(sizeof(Task));
  newTask->name = name;
  newTask->priority = priority;
  newTask->burst = burst;
  newTask->tid = __sync_fetch_and_add(&value, 1);
  
  insert(&list, newTask);
  counter++;
}

// pickNextTask() will help the scheduler choose a task to execute
Task* pickNextTask()
{
  Task* shortest_task = list->task;
  struct node* temp = list;
  
  while (temp != NULL)
  {
    if (temp->task->burst < shortest_task->burst)
    {
      shortest_task = temp->task;
    }
    temp = temp->next;
  }

  delete(&list, shortest_task);
  return shortest_task;
}

// invoke the scheduler
void schedule()
{
  int time_tracker = 0;

  Task* nextTask = malloc(sizeof(Task));
  int count = 0;
 
  for (count = 0; count < counter; count++) 
  {
    nextTask = pickNextTask();

    // calculate waiting time
    total_waiting_time += time_tracker;
    
    // calculate turnaround time
    total_turnaround_time += nextTask->burst;
    total_turnaround_time += time_tracker;
    
    // response time is the same as waiting time because
    // all tasks arrived at the same time
    total_response_time = total_waiting_time;

    time_tracker += nextTask->burst;

    run(nextTask, nextTask->burst);
  }
  printf("Average waiting time: %f\n", total_waiting_time*(1.0)/counter);
  printf("Average turnaround time: %f\n", total_turnaround_time*(1.0)/counter);
  printf("Average response time: %f\n", total_response_time*(1.0)/counter);
}

