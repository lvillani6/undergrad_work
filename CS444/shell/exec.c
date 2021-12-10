#include "shtypes.h"
#include "syscalls.h"
#include "io.h"
#include "jobs.h"

extern char *history[20];
struct Node* jobs = NULL;

int handle_builtin(struct cmd *c) // c is the pointer to a cmd struct */
{
	// already written : make sure c parsed properly and if not return 0 */
	if (!c)
		return 0;
	// already written : tp is the type of the command, like Redir, background,
	// exec, list, or pipe (shtypes.h) */
	// Note: tp is the field out of the struct that we are pointing to (using the ->)
	switch (c->tp)
	{
		// Note : only if c is an exec then it could be a built-in */
		case EXEC: // Note : EXEC is an enum (shtypes.h)
			// Already written: Echo builtin command
			// If comparing c->exec.argv[0] and "echo" gives 0 then they are
			// equal and you loop over the other arguments and print them out.
			if (strcmp(c->exec.argv[0], "echo") == 0)
			{
				for (int i = 1; ; i++)
					if (c->exec.argv[i])
						printf("%s ", c->exec.argv[i]);
					else
						break;
				printf("\n");
				return 1;
			}
			// Already written: Exit builtin command
			else if (strcmp(c->exec.argv[0], "exit") == 0)
			{
				int code = 0;
				if(jobs != NULL) {
					printf("ERROR: Must close all background jobs before exiting.\n");
					return 1;
				}
				if(c->exec.argv[1]) {
					code = atoi(c->exec.argv[1]);
				}
				exit(code); // Kills your process and returns code
			}
			// TODO: The following are builtins you need to implement for your shell.
			// TODO for students: implement cd
			else if (strcmp(c->exec.argv[0], "cd") == 0)
			{
				if(c->exec.argv[1])
					chdir(c->exec.argv[1]);
				return 1;
			}
			// TODO for students: implement pwd
			else if (strcmp(c->exec.argv[0], "pwd") == 0)
			{
				char cwd[255];
				getcwd(cwd, 254);
				printf("%s\n", cwd);
				return 1;
			}
			// TODO for students: implement jobs
			else if (strcmp(c->exec.argv[0], "jobs") == 0)
			{
				printList(jobs);
				// pid_t id= waitpid(-1, NULL, WNOHANG);
				// printf("%d\n", id);
				return 1;
			}
			// TODO for students: implement history
			else if (strcmp(c->exec.argv[0], "history") == 0)
			{
				for(int i = 19; i >= 0; i--)
				if(history[i] != NULL)
					printf("%d: %s", i+1, history[i]);
				printf("\n");
				return 1;
			}
			// TODO for students: implement kill. Note: You should be able to run 'kill 20' to end the process with id 20.
			else if (strcmp(c->exec.argv[0], "kill") == 0)
			{
				if (c->exec.argv[1]) {
					int num = atoi(c->exec.argv[1]);
					pid_t j_pid = searchNode(&jobs, num);
					if(j_pid != 0){
						kill(j_pid, SIGKILL);
						deleteNode(&jobs, j_pid);
					}
					return 1;
				}
				else
					printf("USAGE: kill %s\n", "%num"); break;
				return 1;
			}
			// TODO for students: implement help
			else if (strcmp(c->exec.argv[0], "help") == 0)
			{
				char *commands[10] = { "cd\t[dir]", "clear", "echo\t[arg ...]", "exit\t[n]", "help", "history", "jobs", "kill\t[n]", "ls", "pwd"};
				for(int i = 0; i < 10; i++)	{
					printf("%s\n", commands[i]);
				}
				return 1;
			}
			// TODO for students: implement clear
			else if (strcmp(c->exec.argv[0], "clear") == 0)
			{
				system("clear");
				return 1;
			}

			else {
				return 0;
			}
		default:
			return 0; //Note : returns 0 when you call handle_builtin in main
	}
}

/* Most of your implementation needs to be in here, so a description of this
 * function is in order:
 *
 * int exec_cmd(struct cmd *c)
 *
 * Executes a command structure. See shtypes.h for the definition of the
 * `struct cmd` structure.
 *
 * The return value is the exit code of a subprocess, if a subprocess was
 * executed and waited for. (Id est, it should be the low byte of the return
 * value from that program's main() function.) If no return code is collected,
 * the return value is 0.
 *
 * For pipes and lists, the return value is the exit code of the rightmost
 * process.
 *
 * The function does not change the assignment of file descriptors of the
 * current process--but it may fork new processes and change their file
 * descriptors. On return, the shell is expected to remain connected to
 * its usual controlling terminal (via stdin, stdout, and stderr).
 *
 * This will not be called for builtins (values for which the function above
 * returns a nonzero value).
 */
int exec_cmd(struct cmd *c)
{
	// Don't try to execute a command if parsing failed.
	if (!c)
		return -1;

	int child;

	switch (c->tp)
	{
		case EXEC:
			// TODO for students: What will run a command? (Hint: use execvp syscall)

			if((child = fork1()))
			{
				int code;
				while(wait(&code) != child);
				exit(WEXITSTATUS(code));
			}
			else
				execvp(c->exec.argv[0], c->exec.argv);
			break;

		case PIPE:
			// TODO for students: Run *two* commands, but with the stdout chained
			// to the stdin of the next command. How? Use the syscall pipe like so:
			{
				int pipefds[2];
				pipe(pipefds);
				// NOTE: At this point, pipefds[0] is a read-only FD, and pipefds[1] is a
				// write-only FD. Make *absolutely sure* you close all instances of
				// pipefds[1] in processes that aren't using it, or the reading child
				// will never know when it needs to terminate! */
			}
			break;

		case LIST:
			// The LIST case is already written */
			exec_cmd(c->list.first);
			return exec_cmd(c->list.next);
			break;

		case REDIR:
			// You need to use the fields in the c->redir struct here; fd is the file
			// descriptor you are going to redirect with (STDIN or STDOUR), the path
			// is the file you need to open for redirecting, the mode is how to open
			// the file you are going to use, and the cmd field is what command to run

			dup2(open(c->redir.path, c->redir.mode),c->redir.fd);
			exec_cmd(c->redir.cmd);

			break;

		case BACK:
			// TODO for students: This should be easy--what's something you can
			// remove from EXEC to have this function return before the child exits? */

			if((child = fork1()))
				exit(0);
			else
				execvp(c->back.cmd->exec.argv[0], c->back.cmd->exec.argv);
			break;

		default:
			fatal("BUG: exec_cmd unknown command type\n");
	}
}
