#ifndef JOBS_H
#define JOBS_H

#include "syscalls.h"

typedef struct Node {
	int job_num;
	pid_t job;
	struct Node* next;
}
job_list;

void push(struct Node** head_ref, int new_job_num, pid_t new_job);
pid_t searchNode(struct Node** head_ref, int job_num);
void deleteNode(struct Node** head_ref, int job_num);
void printList(struct Node* node);

#endif
