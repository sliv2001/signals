int fdt, a[256], pos=0;

void receive(int sig, siginfo_t info){
	a[pos]=info.si_value;
	pos++;
	if (pos>=256){
		write(fdt, a, 256);
		pos = 0;
	}
}

void stop(int sig, siginfo_t info){
	write(fdt, a, pos+1);
}

int main(int argc, char** argv){
	struct sigaction act;
	if (argc!=2)
		errx(-1, "wrong args");
	if ((fdt=open(argv[1], O_WRONLY|O_TRUNC|O_CREAT, 0666)<0)
		err(-1, "wrong fdt");
	act.sa_sigaction=&receive;
	act.sa_mask=0;
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
