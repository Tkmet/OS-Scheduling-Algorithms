#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>



// TODO: Add more fields to this struct
struct job {
  int id;
  int arrival;
  int length;
  int duration;
  int tickets;
  int start_time;
  int remaining;
  int bool_finished;
  int complete_time;
  struct job *next;
};

/*** Globals ***/ 
int seed = 100;

//This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;
struct job *queue = NULL;
/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length, int tickets){
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job*) malloc(sizeof(struct job));

  //tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;
  tmp->tickets = tickets;
  tmp->bool_finished = 0;
  tmp->duration = length;
  tmp->remaining = length;
  tmp->complete_time = 0; 
  tmp->start_time = -1;


  // the new job is the last job
  tmp->next = NULL;

  // Case: job is first to be added, linked list is empty 
  if (head == NULL){
    head = tmp;
    return;
  }

  struct job *prev = head;

  //Find end of list 
  while (prev->next != NULL){
    prev = prev->next;
  }

  //Add job to end of list 
  prev->next = tmp;
  return;
}

struct job* createJob(int id, int length, int arrival, int time, int remaining, int start) {
    struct job* tmp = (struct job*)malloc(sizeof(struct job));
    tmp->id = id;
    tmp->length = length;
    tmp->arrival = arrival;
    //tmp->tickets = tickets;
    tmp->remaining = remaining;
    tmp->complete_time = time; 
    tmp->next = NULL;
    tmp->start_time = start;
    return tmp;
}

// Function to insert a new node at the end of the list
void insertJob(struct job** head, int id, int length, int arrival, int time, int remaining, int start_time) {
    struct job* NewNode = createJob(id, length, arrival, time, remaining, start_time);
    if (*head == NULL) {
        *head = NewNode;
    } else {
        struct job* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = NewNode;
    }
}


/*Function to read in the workload file and create job list*/
void read_workload_file(char* filename) {
  int id = 0;
  FILE *fp;
  size_t len = 0;
  ssize_t read;
  char *line = NULL,
       *arrival = NULL, 
       *length = NULL;
  int tickets = 0;

  struct job **head_ptr = malloc(sizeof(struct job*));

  if( (fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) > 1) {
    arrival = strtok(line, ",\n");
    length = strtok(NULL, ",\n");
    tickets += 100;
       
    // Make sure neither arrival nor length are null. 
    assert(arrival != NULL && length != NULL);
        
    append(id++, atoi(arrival), atoi(length), tickets);
  }

  fclose(fp);

  // Make sure we read in at least one job
  assert(id > 0);

  return;
}

void printList(struct job *node){
  //printList
  //utility function
  while(node != NULL){
    printf("id: %d, arrival: %d, duration: %d, remaining: %d, completion time: %d, start time %d\n", node->id, node->arrival, node->length, node->remaining, node->complete_time, node->start_time);
    node = node->next;
  }
  return;
}

// Function to find the job with the shortest remaining duration
struct job* findShortestJob(struct job* head) {
    struct job* shortestJob = head;
    struct job* temp = head->next;
    while (temp != NULL) {
        if (temp->remaining < shortestJob->remaining) {
            shortestJob = temp;
        }
        temp = temp->next;
    }
    return shortestJob;
}

int findShortestArrival(struct job* head) {
    struct job* shortestJob = head;
    struct job* temp = head->next;
    while (temp != NULL) {
        if (temp->arrival < shortestJob->arrival) {
            shortestJob = temp;
        }
        temp = temp->next;
    }
    return shortestJob->arrival;
}

void queuedJobs(struct job* head, int time){
  struct job* tmp = head;
  while(tmp != NULL){
    if(tmp->arrival <= time && tmp->remaining != 0){
      //printf("found a job id: %d\n", tmp->id);
      insertJob(&queue, tmp->id, tmp->length, tmp->arrival, tmp->complete_time, tmp->remaining, tmp->start_time);
    }
    tmp = tmp -> next;
  }
}

int totalJobs(struct job* head){
  int total = 0;
  struct job* tmp = head;
  while(tmp != NULL){
    total ++;
    tmp = tmp -> next;
  }
  return total;
}

int updateRemaining(struct job* head, int id, int newTime, int doneTime, int startTime){
  struct job* tmp = head;
  while(tmp != NULL){
    //find element
    if(tmp->id == id){
      //modify remaining time
      tmp->remaining = newTime;
      tmp->complete_time = doneTime;
      tmp->start_time = startTime;

    }
    tmp = tmp->next;
  }
}

void freeQueue(struct job **head) {
    struct job *current = *head;
    struct job *next;

    while (current != NULL) {
        next = current->next; // Save the next node
        free(current); // Free the current node
        current = next; // Move to the next node
    }

    // After all nodes are freed, set the head pointer to NULL
    *head = NULL;
}



void policy_STCF(struct job *head, int slice) {
  // TODO: Fill this in

  printf("Execution trace with STCF:\n");

  int currentTime = 0;
  int jobCount = 0;
  int jobTotal = totalJobs(head);
  int intervalTime = slice; 
  int start = findShortestArrival(head);
  int idleFlag = 0;
  struct job* runningJob = NULL;

  //find and queue initial jobs
  queuedJobs(head, start);

  while(1){
    //find shortest job in queue
    runningJob = findShortestJob(queue);

    //consider idle time
    while(currentTime < runningJob->arrival){

      currentTime ++;
      intervalTime --;
      //reset time if computer is idle for more than slice time
      if (intervalTime <= 0){
        intervalTime = slice; //reset interval time;
      }
    }  

    //running the job
    if (runningJob->remaining < slice){
      printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", currentTime, runningJob->id, runningJob->arrival, runningJob->remaining);
    } else {
      printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", currentTime, runningJob->id, runningJob->arrival, slice);
    }
    // Record start time when a job starts executing
    if (runningJob->start_time == -1) {
      runningJob->start_time = currentTime;
    }
    while(intervalTime > 0){
      runningJob->remaining = runningJob->remaining - 1;
      intervalTime --;
      currentTime ++;

      if(runningJob->remaining == 0){

        jobCount ++;

        runningJob->complete_time = currentTime;
        break;
      }
    }
    intervalTime = slice; //reset interval time

    //need to update head
    updateRemaining(head, runningJob->id, runningJob->remaining, runningJob->complete_time, runningJob->start_time);
    //need to clear queue
    freeQueue(&queue);

    queuedJobs(head, currentTime);

    struct job* tmp = head;

    if(tmp->arrival < currentTime && tmp->remaining !=0){

    }else{
        while (tmp != NULL){

          while(tmp->arrival > currentTime && tmp->remaining != 0){

            currentTime++;
            intervalTime--;
            if (intervalTime <= 0){
              intervalTime = slice; //reset interval time;
            }
            tmp == NULL;
          }
          tmp = tmp -> next; 
        }
    }

    queuedJobs(head, currentTime);

    intervalTime = slice; //reset interval time

    if(jobCount == jobTotal){
      break;
    } 

  }

  printf("End analyzing STCF.\n");

  return;
}

void analyze_STCF(struct job *head) {
  struct job* temp = head;
    int response_sum = 0;
    int turnaround_sum = 0;
    int wait_sum = 0;
    int jobCount = 0;

    while (temp != NULL) {
        int response = temp->start_time - temp->arrival;
        int turnaround = temp->complete_time - temp->arrival;
        int wait = turnaround - temp->length;
        printf("Job %d -- Response time: %d Turnaround: %d Wait: %d\n", temp->id, response, turnaround, wait);
        response_sum += response;
        turnaround_sum += turnaround;
        wait_sum += wait;
        jobCount++;
        temp = temp->next;
    }

    double response_avg = (double)response_sum / jobCount;
    double turnaround_avg = (double)turnaround_sum / jobCount;
    double wait_avg = (double)wait_sum / jobCount;

    printf("Average -- Response: %.2lf Turnaround: %.2lf Wait: %.2lf\n", response_avg, turnaround_avg, wait_avg);
  return;
}

void insertAtBeginning(struct job** head, struct job* newJob) {
    newJob->next = *head;
    *head = newJob;
}

void sortLinkedListByArrival(struct job** head) {
    if (*head == NULL || (*head)->next == NULL) {
        return;  // List is empty or has only one element, it's already sorted.
    }

    struct job* sorted = NULL;
    struct job* current = *head;

    while (current != NULL) {
        struct job* next = current->next;
        if (sorted == NULL || sorted->arrival >= current->arrival) {
            current->next = sorted;
            sorted = current;
        } else {
            struct job* search = sorted;
            while (search->next != NULL && search->next->arrival < current->arrival) {
                search = search->next;
            }
            current->next = search->next;
            search->next = current;
        }
        current = next;
    }
    *head = sorted;
}

void printLinkedList(struct job* head) {
    while (head != NULL) {
        printf("id: %d, arrival: %d, length: %d\n", head->id, head->arrival, head->length);
        head = head->next;
    }
}

void policy_RR(struct job *head, int slice) {
    printf("Execution trace with RR:\n");

    struct job *current_job = head;
    int current_time = 0;
    int remaining_jobs = 0;

    while (1) {
        int jobs_executed = 0;

        current_job = head;
        while (current_job != NULL) {
            if (current_job->bool_finished == 0 && current_job->arrival <= current_time) {
                if (current_job->start_time == -1) {
                    current_job->start_time = current_time;
                }

                printf("t=%d: [Job %d] arrived at [%d], ran for: ", current_time, current_job->id, current_job->arrival);

                if (current_job->duration <= slice) {
                    printf("[%d]\n", current_job->duration);
                    current_time += current_job->duration;
                    current_job->bool_finished = 1;
                    current_job->complete_time = current_time;
                } else {
                    printf("[%d]\n", slice);
                    current_time += slice;
                    current_job->duration -= slice;
                }

                jobs_executed++;
            }

            current_job = current_job->next;
        }

        if (jobs_executed == 0) {
            // No job executed in this time slice, move time forward
            current_time++;
        }

        // Check if all jobs have finished
        current_job = head;
        remaining_jobs = 0;
        while (current_job != NULL) {
            if (current_job->bool_finished == 0) {
                remaining_jobs = 1;
                break;
            }
            current_job = current_job->next;
        }

        if (remaining_jobs == 0) {
            break;
        }
    }

    printf("End of execution with RR.\n");
}



void analyze_RR(struct job * head){
    struct job* temp = head;
    int response_sum = 0;
    int turnaround_sum = 0;
    int wait_sum = 0;
    int jobCount = 0;

    while (temp != NULL) {
        int response = temp->start_time - temp->arrival;
        int turnaround = temp->complete_time - temp->arrival;
        int wait = turnaround - temp->length;
        printf("Job %d -- Response time: %d Turnaround: %d Wait: %d\n", temp->id, response, turnaround, wait);
        response_sum += response;
        turnaround_sum += turnaround;
        wait_sum += wait;
        jobCount++;
        temp = temp->next;
    }

    double response_avg = (double)response_sum / jobCount;
    double turnaround_avg = (double)turnaround_sum / jobCount;
    double wait_avg = (double)wait_sum / jobCount;

    printf("Average -- Response: %.2lf Turnaround: %.2lf Wait: %.2lf\n", response_avg, turnaround_avg, wait_avg);

  return;
}

int listSize(struct job *node){
  //gets size of list
  int count = 0; 
  while(node != NULL){
    count ++;
    node = node->next;
  }
  return count; 
}

void policy_LT(struct job *head, int slice) {
    int task_left = listSize(head);
    if (head == NULL)
        return;

    int current_time = 0;
    int total_ticket = 1;

    while (total_ticket || task_left != 0) {
        struct job* temp = head;
        total_ticket = 0;
        task_left = listSize(head);

        do {
            if (temp->duration > 0 && temp->arrival <= current_time) {
                temp->tickets = 100;
                total_ticket += 100;

                
            } else if (temp->duration <= 0 && temp->arrival <= current_time &&!temp->bool_finished) {
                temp->tickets = 0;
                temp->bool_finished = 1;
                
                //temp->complete_time = current_time;
                task_left--;
            } else if (temp->bool_finished) {
                task_left--;
            }
            temp = temp->next;
        } while (temp != NULL);

        if (total_ticket == 0 && task_left == 0) {
            //printf("Finish policy_LT\n");
            return;
        } 
        if (total_ticket == 0 && task_left != 0) {
            current_time++;
            //printf("+1\n");
        }

        float rand_num = ((float)rand() / (float)(RAND_MAX)) * total_ticket;
        int ticket_check = 0;
        temp = head;

        do {
            ticket_check += temp->tickets;

            if (ticket_check <= rand_num)
                temp = temp->next;
        } while ((float)ticket_check <= rand_num);

        if (temp != NULL && temp->duration > slice ) {
            if(temp->arrival > current_time)
              current_time = temp->arrival;
            if (temp->length == temp->duration){//this means task is executed for the first time.
              temp->start_time = current_time;
            }
            
            temp->duration -= slice;
            printf("t=%d [Job %d] arrived at [%d], ran for: [%d]\n", current_time, temp->id, temp->arrival, slice);
            current_time += slice;
        } else if (temp != NULL && temp->duration <= slice && temp->duration > 0 ) {
            if(temp->arrival > current_time)
              current_time = temp->arrival;
            if (temp->length == temp->duration){//this means task is executed for the first time.
              temp->start_time = current_time;
            }
            
            int tmp_duration = temp->duration;
            temp->duration = 0;
            printf("t=%d [Job %d] arrived at [%d], ran for: [%d]\n", current_time, temp->id, temp->arrival, tmp_duration);
            current_time += tmp_duration;
            temp->complete_time = current_time;
        } else {
            // No job to execute, introduce idle time
            int next_arrival = findShortestArrival(head);
            int idle_time = next_arrival - current_time;
            current_time = next_arrival;
            
            //printf("t=%d [Idle] ran for: [%d]\n", current_time, idle_time);
        }
        //printf("current time: %d\n\n", current_time);
        
    }
    //printf("Finish policy_LT\n");
    return;
}


void analyze_LT(struct job * head){
  struct job *temp = head;
  int response;
  int turnaround;
  int wait;

  double response_avg;
  double turnaround_avg;
  double wait_avg;
  do
  {
    response = temp->start_time - temp->arrival;
    turnaround = temp->complete_time - temp->arrival;
    wait = temp->complete_time - temp->arrival - temp->length;

    response_avg += response;
    turnaround_avg += turnaround;
    wait_avg += wait;

    printf("Job %d -- Response time: %d Turnaround: %d Wait: %d\n", temp->id, response, turnaround, wait);
    temp = temp->next;
  } while (temp != 0);

  double task_size = listSize(head);
  response_avg = response_avg / task_size;
  turnaround_avg = turnaround_avg / task_size;
  wait_avg = wait_avg / task_size;
  printf("Average -- Response time: %.2f Turnaround: %.2f Wait: %.2f\n", response_avg, turnaround_avg, wait_avg);

  return;
}



int main(int argc, char **argv) {

 if (argc < 5) {
    fprintf(stderr, "missing variables\n");
    fprintf(stderr, "usage: %s analysis-flag policy workload-file slice-length\n", argv[0]);
		exit(EXIT_FAILURE);
  }

  int analysis = atoi(argv[1]);
  char *policy = argv[2],
       *workload = argv[3];
  int slice = atoi(argv[4]);

  // Note: we use a global variable to point to 
  // the start of a linked-list of jobs, i.e., the job list 
  read_workload_file(workload);

  if (strcmp(policy, "STCF") == 0 ) {
    policy_STCF(head, slice);
    if (analysis) {
      printf("Begin analyzing STCF:\n");
      analyze_STCF(head);
      printf("End analyzing STCF.\n");
    }

    exit(EXIT_SUCCESS);
  }

  if (strcmp(policy, "RR") == 0 ) {
    policy_RR(head, slice);
    if (analysis) {
      printf("Begin analyzing RR:\n");
      analyze_RR(head);
      printf("End analyzing RR.\n");
    }

    exit(EXIT_SUCCESS);
  }

  if (strcmp(policy, "LT") == 0 ) {
    policy_LT(head, slice);
    if (analysis) {
      printf("Begin analyzing LT:\n");
      analyze_LT(head);
      printf("End analyzing LT.\n");
    }

    exit(EXIT_SUCCESS);
  }

	exit(EXIT_SUCCESS);
}