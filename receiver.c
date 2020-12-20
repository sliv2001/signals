#include "header.h"

int first=1, fdt, a[1024/sizeof(int)], pos=0;
struct timespec Rec, Now;
char* Path;

void receive(int sig, siginfo_t* info, void* stuff){
	if (first){
		first=0;
		clock_gettime(CLOCK_REALTIME, &Rec);
	}
	a[pos]=info->si_value.sival_int;
	pos++;
	if (pos>=1024/sizeof(int)){
		write(fdt, &a, 1024);
		pos = 0;
	}
}

void stop(int sig, siginfo_t* info, void* stuff){
	char buf = 0, i = 0;
	struct stat s;
	double speed;
	write(fdt, &a, (pos-1)*sizeof(int));
	while ((buf = ((char*)&a[pos-1])[i]) != 0){
		write(fdt, &buf, 1);
		i++;
		if (i == sizeof(int))
			break;
	}
	close(fdt);
	clock_gettime(CLOCK_REALTIME, &Now);
	if (stat(Path, &s)<0)
		err(-1, "Couldnot check speed");
	speed = (double)s.st_size/(double)((Now.tv_sec-Rec.tv_sec)*1000000000+Now.tv_nsec-Rec.tv_nsec)*1000000000;
	printf("Speed: %lf bps\n", speed);
	exit(0);
}

int main(int argc, char** argv){
	struct sigaction act;
	sigset_t set;
	pid_t pid = getpid();
	memset(&act, 0, sizeof(act));
	sigemptyset(&set);
	sigaddset(&set, SIGRTMIN);
	sigaddset(&set, SIGRTMIN+1);
	printf("%d\n", pid);
	if (argc!=2)
		errx(-1, "wrong args");
	Path = argv[1];
	if ((fdt=open(argv[1], O_WRONLY|O_TRUNC|O_CREAT, 0666))<0)
		err(-1, "wrong fdt");
	act.sa_sigaction=&receive;
	act.sa_mask=set;
	act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGRTMIN, &act, NULL)<0)
		err(-1, "couldnot make sigaction");
	act.sa_sigaction = &stop;
	act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGRTMIN+1, &act, NULL)<0)
		err(-1, "couldnot make sigaction");
	while (1)
		sleep(10000);
	return 0;
}
