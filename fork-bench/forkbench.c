#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

char *cmnd;

void usage(void) {
        fprintf(stderr, "Usage: %s -p maxproc -w maxwork\n", cmnd);
        exit(1);
}

int main(int argc, char **argv) {
        int i;
        int max_proc = 10;
        int max_work = 0;
        struct {
                int type;
                int pid;
                int outstanding;
        } *event;
        int ec = 0;
        int pid;
        int outstanding = 0;
        int c;
        int max_outstanding = 0;
        int sum_outstanding = 0;
        int failed_forks = 0;

        while ((c = getopt(argc, argv, "p:w:")) != EOF) {
                switch (c) {
                        char *p;
                case 'p':
                        max_proc = strtol(optarg, &p, 0);
                        if (p == optarg || *p) usage();
                        break;
                case 'w':
                        max_work = strtol(optarg, &p, 0);
                        if (p == optarg || *p) usage();
                        break;
                case '?':
                        usage();
                default:
                        assert(0);
                }

        }
        event = malloc(max_proc * 2 * sizeof(*event));
        if (!event) {
                fprintf(stderr,
                        "%s: cannot malloc event log: %s\n",
                        argv[0], strerror(errno));
                exit(1);
        }

        for (i = 0; i < max_proc; i++) {
                switch (pid = fork()) {
                case -1:
                        assert(ec < 2*max_proc);
                        event[ec].type = 0;
                        event[ec].pid = pid;
                        event[ec].outstanding = outstanding;
                        ec++;
                        break;
                case 0:
                        for (i = 0; i < max_work; i++) ;
                        exit(0);
                        break;
                default:
                        outstanding++;
                        assert(ec < 2*max_proc);
                        event[ec].type = 0;
                        event[ec].pid = pid;
                        event[ec].outstanding = outstanding;
                        ec++;
                        while ((pid = wait3(NULL, WNOHANG, NULL)) > 0) {
                                outstanding--;
                                assert(ec < 2*max_proc);
                                event[ec].type = 1;
                                event[ec].pid = pid;
                                event[ec].outstanding = outstanding;
                                ec++;
                        }
                }
        }
        while ((pid = wait3(NULL, 0, NULL)) > 0) {
                outstanding--;
                assert(ec < 2*max_proc);
                event[ec].type = 1;
                event[ec].pid = pid;
                event[ec].outstanding = outstanding;
                ec++;
        }
        for (i = 0; i < ec; i++) {
                printf("%d: %s %d (%d outstanding)\n",
                        i,
                        event[i].type ? "exit" : "fork",
                        event[i].pid,
                        event[i].outstanding);
                if (event[i].outstanding > max_outstanding) {
                        max_outstanding = event[i].outstanding;
                }
                sum_outstanding += event[i].outstanding;
                if (event[i].type == 0 && event[i].pid == -1) {
                        failed_forks ++;
                }
        }
        printf ("\n");
        printf ("max outstanding: %d, avg outstanding: %g\n",
                max_outstanding, (double)sum_outstanding / ec);
        printf ("failed forks: %d\n", failed_forks);
        exit(0);
}



