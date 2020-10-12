#include <fcntl.h>
#include <stdio.h>

void check_mode(void)
{
	int mode = 0x1b0;

	printf("Mode:\n");
	if(mode & S_IRWXU)
		printf("%s is on\n", "S_IRWXU");

	if(mode & S_IRUSR)
		printf("%s is on\n", "S_IRUSR");

	if(mode & S_IWUSR)
		printf("%s is on\n", "S_IWUSR");

	if(mode & S_IXUSR)
		printf("%s is on\n", "S_IXUSR");

	if(mode & S_IRWXG)
		printf("%s is on\n", "S_IRWXG");

	if(mode & S_IRGRP)
		printf("%s is on\n", "S_IRGRP");

	if(mode & S_IWGRP)
		printf("%s is on\n", "S_IWGRP");

	if(mode & S_IXGRP)
		printf("%s is on\n", "S_IXGRP");

	if(mode & S_IRWXO)
		printf("%s is on\n", "S_IRWXO");

	if(mode & S_IROTH)
		printf("%s is on\n", "S_IROTH");

	if(mode & S_IWOTH)
		printf("%s is on\n", "S_IWOTH");

	if(mode & S_IXOTH)
		printf("%s is on\n", "S_IXOTH");

	if(mode & S_ISUID)
		printf("%s is on\n", "S_ISUID");

	if(mode & S_ISGID)
		printf("%s is on\n", "S_ISGID");

	if(mode & S_ISVTX)
		printf("%s is on\n", "S_ISVTX");
}

void check_flags(void)
{
	int flags = 0xc1;

	printf("Flags:\n");
	if(flags & O_RDONLY)
		printf("%s is on\n", "O_RDONLY");
	if(flags & O_WRONLY)
		printf("%s is on\n", "O_WRONLY");
	if(flags & O_RDWR)
		printf("%s is on\n", "O_RDWR");
	if(flags & O_NONBLOCK)
		printf("%s is on\n", "O_NONBLOCK");
	if(flags & O_APPEND)
		printf("%s is on\n", "O_APPEND");
	if(flags & O_CREAT)
		printf("%s is on\n", "O_CREAT");
	if(flags & O_TRUNC)
		printf("%s is on\n", "O_TRUNC");
	if(flags & O_EXCL)
		printf("%s is on\n", "O_EXCL");
	if(flags & O_SHLOCK)
		printf("%s is on\n", "O_SHLOCK");
	if(flags & O_EXLOCK)
		printf("%s is on\n", "O_EXLOCK");
	if(flags & O_NOFOLLOW)
		printf("%s is on\n", "O_NOFOLLOW");
	if(flags & O_SYMLINK)
		printf("%s is on\n", "O_SYMLINK");
	if(flags & O_EVTONLY)
		printf("%s is on\n", "O_EVTONLY");
	if(flags & O_CLOEXEC)
		printf("%s is on\n", "O_CLOEXEC");
}

int main(void)
{
	check_mode();
	check_flags();
}
