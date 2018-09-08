/* $OpenXM$
 */
int unblock_signal(int sigset[]);
int block_signal(int sigset[]);
int set_signal(int sig,void (*handler)(int m));
void *mysignal(int sig,void (*handler)(int m));


