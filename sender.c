int fdf, pid, depth=0;
size_t file_size;
struct stat buf;

void Send(int a, int sig){
	union sigval value;
	value.sival_int = a;
	if (sigqueue(pid, sig, value)<0){
		if (errno == EAGAIN&& depth<=3){
			usleep(10<<depth);
			depth++;
			Send(a, sig);
			depth--;
		}
		else
			err(-1, "sending fault");
	}
}

void sendit(){
	int k, i;
	int a[256];
	if (sigqueue(pid, 0, 0)<0)
		err(-1, "no process exist with pid %d", pid);
	for (i=0; i<file_size/1024; i++){
		read(fdf, &a, 1024);
		for (k=0; k<256; k++){
			Send(a[k], SIGUSR1);
		}
	}
	Send(0, SIGUSR2);
}

int main(int argc, char** argv){
	if (argc!=3)
		errx(-1, "wrong args");
	pid = atoi(argv[2]);
	if (pid<0)
		err(-1, "wrong pid");
	if (stat(argv[1], &buf)<0)
		err(-1, "wrong file");
	if ((fdf = open(argv[1], O_RDONLY)<0)
		err(-1, "wrong file");
	if (!S_ISREG(buf.st_mode))
		errx(-1, "not reg file");
	if ((file_size = buf.st_size)<=0)
		errx(-1, "file empty");
	sendit();
	return 0;
}