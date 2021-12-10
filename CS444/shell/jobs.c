#include "jobs.h"
#include "syscalls.h"
#include <stdio.h>
#include <stdlib.h>

void push(struct Node** head_ref, int new_job_num, pid_t new_job)
{
	struct Node* new_node
		= (struct Node*)malloc(sizeof(struct Node));
	new_node->job_num = new_job_num;
	new_node->job = new_job;
	new_node->next = (*head_ref);
	(*head_ref) = new_node;
}
pid_t searchNode(struct Node** head_ref, int job_num)
{
	struct Node *temp = *head_ref, *prev;
	if (temp != NULL && temp->job_num == job_num) {
		pid_t job = temp->job;
		*head_ref = temp->next;
		free(temp);
		return job;
	}
	while (temp != NULL && temp->job != job_num) { // Search
		prev = temp;
		temp = temp->next;
	}
	if (temp == NULL) {
		free(temp);
		return 0;
	}
	prev->next = temp->next;
	free(temp);
}

void deleteNode(struct Node** head_ref, int job_num)
{
	struct Node *temp = *head_ref, *prev;
	if (temp != NULL && temp->job_num == job_num) {
		*head_ref = temp->next;
		free(temp);
		return;
	}
	while (temp != NULL && temp->job != job_num) { // Search
		prev = temp;
		temp = temp->next;
	}
	if (temp == NULL)
		return;
	prev->next = temp->next;
	free(temp);
}

void printList(struct Node* node)
{
	while (node != NULL) {
		printf("%d: %d ", node->job_num, node->job);
		node = node->next;
	}
}
