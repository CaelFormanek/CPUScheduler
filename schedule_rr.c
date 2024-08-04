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
int index_tracker = 0;
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
  
  if (index_tracker == counter)
  {
    index_tracker = 0;
  }
  
  Task* nextTask = getTaskAtIndex(list, counter-1-index_tracker);
  index_tracker++;
  return nextTask; 
}

// invoke the scheduler
void schedule()
{
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

