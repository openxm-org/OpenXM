#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* In some distributions of linux you need to include the following file
#include <linux/sched.h>
*/
#include <unistd.h> 

/* oxlog /usr/X11R6/bin/xterm -icon -e /home/nobuki/kxx/ox
    0     1                    2     3  4
         -ox /home/nobuki/kxx/ox_asir
          5   6
*/
/* oxlog /home/nobuki/kxx/ox
    0     1     
         -ox /home/nobuki/kxx/ox_asir  >& /dev/null
          2     3
*/
main(int argc, char *argv[]) {
  sigset_t sss;
  int i;
  /* char *env[2];
  env[0] = NULL;*/
  sigemptyset(&sss);
  sigaddset(&sss,SIGINT);
  sigprocmask(SIG_BLOCK,&sss,NULL);

  if (argc == 4) {
    execl(argv[1],argv[1],argv[2],argv[3],NULL);
  }else if (argc == 5) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],NULL);
  }else if (argc == 6) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],NULL);
  }else if (argc == 7) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],NULL);
  }else if (argc == 8) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],NULL);
  }else if (argc == 9) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],NULL);
  }else if (argc == 10) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],NULL);
  }else if (argc == 11) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],NULL);
  }else if (argc == 12) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11],NULL);
  }else if (argc == 13) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],NULL);
  }else if (argc == 14) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],NULL);
  }else if (argc == 15) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],NULL);
  }else if (argc == 16) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],argv[15],NULL);
  }else if (argc == 17) {
    execl(argv[1],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
	  argv[7],argv[8],argv[9],argv[10],argv[11], argv[12],argv[13],argv[14],argv[15],argv[16],NULL);
  }else {
    fprintf(stderr,"Error in oxlog: cannot handle argc=%d\n",argc);
    for (i=0; i<argc; i++) {
      fprintf(stderr,"argv[%d]=%s ",i,argv[i]);
    }
    fprintf(stderr,"\n");
    fprintf(stderr,"Usage example 1: oxlog /usr/X11R6/bin/xterm -icon -e /home/nobuki/kxx/ox -ox /home/nobuki/kxx/ox_asir\n");
    fprintf(stderr,"      example 2: oxlog /home/nobuki/kxx/ox -ox /home/nobuki/kxx/ox_asir >& /dev/null\n");
    exit(10);
  }
  fprintf(stderr,"\nError in oxlog: Failed to start the process.\n");
  for (i=0; i<argc; i++) {
    fprintf(stderr,"argv[%d]=%s ",i,argv[i]);
  }
  fprintf(stderr,"\n");
  exit(20);
}
mainold1() {
  sigset_t sss;
  int i;
  /* char *env[2];
  env[0] = NULL;*/
  sigemptyset(&sss);
  sigaddset(&sss,SIGINT);
  sigprocmask(SIG_BLOCK,&sss,NULL);
  /* for (i=0; i<1000; i++) {
     sleep(1);
     printf("Hello\n");
     }
     exit(0); */
  execl("/usr/X11R6/bin/xterm","/usr/X11R6/bin/xterm",
	"-e","/home/nobuki/kxx/ox","-ox","/home/nobuki/kxx/ox_asir",NULL);
/*  execl("/home/nobuki/kxx/ox","/home/nobuki/kxx/ox","-ox","/home/nobuki/kxx/ox_asir",NULL);*/
}

