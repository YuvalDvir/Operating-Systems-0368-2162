#include <stdio.h>
#include <sys/param.h>
#include <signal.h>
#include <string.h>
#include <zconf.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

/****************************************************
*                                                   *
*           Operating Systems (0368-2162)           *
*               Mini Shell Assignment               *
*                                                   *
*****************************************************/

int perform_piping(char** arglist, int seperator);
int perform_foreground_process(char** arglist);
int perform_background_process(char** arglist);
void zombies_handler();
void allow_SIGINT();
void ignore_SIGINT();
void SIGCHLD_handler(int sig);

/* After finishes, the parent (shell) should not terminate upon SIGINT *
 * prevent zombies and remove them as fast as possible */
int prepare(void){
    zombies_handler();
    ignore_SIGINT();

    return 0;
}

/* Changing the action taken by the process on receipt of a SIGINT signal */
void ignore_SIGINT(){
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, 0) == -1) {
        fprintf(stderr, "An error occurred while using my shell: %s\n", strerror( errno ));
        exit(1);
    }
}

/* Changing the action taken by the process on receipt of a SIGINT signal */
void allow_SIGINT(){
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGINT, &sa, 0) == -1) {
        perror(0);
        exit(1);
    }
}

/* Changing the action taken by the process on receipt of a SIGCHLD signal */
void zombies_handler(){
    struct sigaction sa;
    sa.sa_handler = &SIGCHLD_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        fprintf(stderr, "An error occurred while using my shell: %s\n", strerror( errno ));
        exit(1);
    }
}

/* Defining an action to be taken by the process on receipt of a SIGCHLD signal */
void SIGCHLD_handler(int sig) {
    int saved_errno = errno;
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
    errno = saved_errno;
}

/* Commands specified in the arglist executed via execvp() as a child process using fork() */
int process_arglist(int count, char** arglist){
    /* If the last non-NULL word in arglist is "&" (a single ampersand), run the child process in the background */
    if (!strcmp(arglist[count-1],"&")){
        arglist[count-1] = NULL;
        perform_background_process(arglist);
        return 1;
    }

    /* If arglist contains the word "|" (a single pipe symbol), run two child processes *
     * with the output (stdout) of the first process piped to the input (stdin) of the second process */
    for (int i = 0; i < count; ++i) {
        if (!strcmp(arglist[i],"|")){
            perform_piping(arglist,i);
            return 1;
        }
    }

    /* Run the child process in the Foreground */
    perform_foreground_process(arglist);

    return 1;
}

/* The shell executes the command and waits until it completes before accepting another command */
int perform_foreground_process(char** arglist){
    int pid = fork();
    if (pid < 0){
        fprintf(stderr, "An error occurred while using my shell: %s\n", strerror( errno ));
        exit(1);
    } else if (0 == pid) {
        allow_SIGINT();                     /* Foreground child processes (regular commands or parts of a pipe) should terminate upon SIGINT */
        execvp(arglist[0], arglist);
        exit(1);
    }
    waitpid(pid, NULL, 0);
    return 1;
}

/* The shell executes the command but does not wait for its completion before accepting another command */
int perform_background_process(char** arglist){
    int pid = fork();
    if (pid < 0){
        fprintf(stderr, "An error occurred while using my shell: %s\n", strerror( errno ));
        exit(1);
    } else if (0 == pid) {
        execvp(arglist[0], arglist);
        exit(1);
    }

    return 1;
}

/* The shell executes both commands concurrently, piping the standard output of the first command to the standard input of the second command *
 * The shell waits until both commands complete before accepting another command */
int perform_piping(char** arglist, int seperator){
    int fd[2];
    /* Creating the pipe */
    if (pipe(fd) == -1){
        fprintf(stderr, "An error occurred while using my shell: %s\n", strerror( errno ));
        exit(1);
    }

    /* First child process */
    int pid1 = fork();
    if (pid1 < 0){
        fprintf(stderr, "An error occurred while using my shell: %s\n", strerror( errno ));
        exit(1);
    }

    else if (0 == pid1){
        arglist[seperator] = NULL;
        allow_SIGINT();                     /* Foreground child processes (regular commands or parts of a pipe) should terminate upon SIGINT */
        dup2(fd[1],STDOUT_FILENO);          /* Writing STDOUT to the pipe */
        close(fd[0]);
        close(fd[1]);
        execvp(arglist[0],arglist);
    }

    /* Second child process */
    int pid2 = fork();
    if(pid2 < 0){
        fprintf(stderr, "An error occurred while using my shell: %s\n", strerror( errno ));
        exit(1);
    }
    else if (0 == pid2){
        allow_SIGINT();                     /* Foreground child processes (regular commands or parts of a pipe) should terminate upon SIGINT */
        dup2(fd[0],STDIN_FILENO);           /* Reading STDIN from the pipe */
        close(fd[0]);
        close(fd[1]);
        execvp(arglist[seperator + 1],arglist + seperator + 1);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 1;
}

/* The skeleton calls this function before exiting */
int finalize(void){
    return 0;
}
