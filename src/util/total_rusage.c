/* $OpenXM$ */

/* outputs total virtual memory size and total cputime consumed by
   whole processes which have the specified process as their ancestor */

#include <sys/param.h>
#include <sys/user.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>

#include <fcntl.h>
#include <kvm.h>
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>

struct proc_info {
	int pid,ppid;
	int vsize;    /* kbytes */
	double time;  /* second */
};

int compare_pi(struct proc_info *a, struct proc_info *b)
{
	 if ( a->pid < b->pid ) return 1;
	 else if ( a->pid >= b->pid ) return -1;
	 else return 0;
}

main(int argc, char **argv)
{
	char *nlistf,*memf;
	char errbuf[BUFSIZ];
	int rootid,n,i,j,pid,ppid;
	kvm_t *kd;
	struct kinfo_proc *kp;
	struct proc_info *pi;

	if ( argc != 2 ) {
		fprintf(stderr,"total_rusage <process id>\n");
		exit(0);
	}
	rootid = atoi(argv[1]);
 	nlistf = memf = _PATH_DEVNULL;	
	kd = kvm_openfiles(nlistf,memf,NULL,O_RDONLY,errbuf);
	if ((kp = kvm_getprocs(kd, KERN_PROC_ALL, 0, &n)) == 0) {
		fprintf(stderr,"%s\n",kvm_geterr(kd));
		exit(0);
	}
	pi = (struct proc_info *)calloc(n,sizeof(struct proc_info));
	for ( i = 0; i < n; i++, kp++ ) {
		pi[i].pid = kp->kp_proc.p_pid;
		pi[i].ppid = kp->kp_eproc.e_ppid;
		pi[i].vsize = kp->kp_eproc.e_vm.vm_map.size/1024;
		pi[i].time = (double) (kp->kp_proc.p_uu+kp->kp_proc.p_su)/(double)1e6;
	}
	qsort(pi,n,sizeof(struct proc_info),
		(int (*)(const void *,const void *))compare_pi);
	for ( i = 0; i < n; i++ ) {
		pid = pi[i].pid;	
		if ( pid == rootid ) {
			printf("%d %f\n",pi[i].vsize,pi[i].time);
			exit(0);
		} else {
			/* find the parent */
			ppid = pi[i].ppid;	
			for ( j = i+1; j < n; j++ )
				if ( pi[j].pid == ppid )
					break;
			if ( j < n ) {
				pi[j].vsize += pi[i].vsize;
				pi[j].time += pi[i].time;
			}
		}
	}
}
