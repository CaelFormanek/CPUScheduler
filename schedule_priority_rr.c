#include "schedulers.h"
#include "list.h"
#include "cpu.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


// global variables
struct node* list;
struct node* backuplist;
int backuplistnumitems = 0;
int counter = 0;
int counter_tracker = 0;
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

// delete from list, add to backuplist
Task* deleteHelper()
{
  Task* deleted_task = list->task;
  struct node* temp = list;

  while (temp != NULL)
  {
    if (temp->task->priority < deleted_task->priority)
    {
      deleted_task = temp->task;
    }
    temp = temp->next;
  }

  delete(&list, deleted_task);
  return deleted_task;
}

// sort the list for by priority
void sortList()
{
  Task* nextTask = malloc(sizeof(Task));
  int count = 0;
 
  for (count = 0; count < counter; count++) 
  {
    nextTask = deleteHelper();
    insert(&backuplist, nextTask);
  }
}

// pickNextTask() will help the scheduler choose a task to execute
Task* pickNextTask()
{
  Task* t = malloc(sizeof(Task));
  t = getTaskAtIndex(backuplist, counter_tracker);
  counter_tracker++;
  if (counter_tracker == counter)
  {
    counter_tracker = 0;
  }
  return t;
}

// invoke the scheduler
void schedule()
{
  sortList();
  int prev_waiting_time = 0;
  int tasks_counter = 0;

  int actual_waiting_time = 0;

  Task* nextTask = malloc(sizeof(Task));
  int zerocounter = 0;
  while (zerocounter < counter)
  {
    nextTask = pickNextTask();
    if (nextTask->burst == 0)
    {
      zerocounter++;
    }
     if (tasks_counter == counter)
    {
      actual_waiting_time = total_waiting_time;
    }

    // calculate turnaround time
    total_turnaround_time += nextTask->burst;

    // response time will be the same as waiting time
    // since tasks arrive at same time
    tasks_counter++;

    int temp_burst = nextTask->burst;
    if (temp_burst > 0  && temp_burst < 10)
    {
      // calculate waiting time
      total_waiting_time += prev_waiting_time;
      prev_waiting_time += temp_burst;
      run(nextTask, temp_burst);
    }
    else if (temp_burst >= 10)
    {
      // calculate waiting time
      total_waiting_time += prev_waiting_time;
      prev_waiting_time += 10;
      run(nextTask, 10);
    }
    else
    {
      // do nothing
    }
    
    if (temp_burst <= 10)
    {
      nextTask->burst = 0;
    }
    else
    {
      nextTask->burst = nextTask->burst - 10;
    }
  }
  total_response_time = actual_waiting_time;
  printf("Average waiting time: %f\n", actual_waiting_time*(1.0)/counter);
  printf("Average turnaround time: %f\n", total_turnaround_time*(1.0)/counter);
  printf("Average response time: %f\n", total_response_time*(1.0)/counter);
}

