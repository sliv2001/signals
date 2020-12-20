#include "header.h"

int fdt, a[1024/sizeof(int)], pos=0;

void receive(int sig, siginfo_t* info, void* stuff){
	a[pos]=info->si_value.sival_int;
	pos++;
	if (pos>=1024/sizeof(int)){
		write(fdt, &a, 1024);
		pos = 0;
	}
}

void stop(int sig, siginfo_t* info, void* stuff){
	char buf = 0, i = 0;
	write(fdt, &a, pos-1);
	while ((buf = ((char*)&a[pos-1])[i]) != 0){
		write(fdt, &buf, 1);
		i++;
		if (i = sizeof(int))
			break;
	}
	exit(0);
}

int main(int argc, char** argv){
	struct sigaction act;
	sigset_t set;
	pid_t pid = getpid();
	memset(&act, 0, sizeof(act));
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	printf("%d\n", pid);
	if (argc!=2)
		errx(-1, "wrong args");
	if ((fdt=open(argv[1], O_WRONLY|O_TRUNC|O_CREAT, 0666))<0)
		err(-1, "wrong fdt");
	act.sa_sigaction=&receive;
	act.sa_mask=set;
	act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGUSR1, &act, NULL)<0)
		err(-1, "couldnot make sigaction");
	act.sa_sigaction = &stop;
	act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGUSR2, &act, NULL)<0)
		err(-1, "couldnot make sigaction");
	while (1)
		sleep(10000);
	return 0;
}
