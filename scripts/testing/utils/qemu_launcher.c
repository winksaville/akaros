#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char* prog_name = "qemu-system-x86_64";
	char* params[argc];
	int i;

	params[0] = prog_name;
	for (i = 1; i < argc; ++i)
	{
		params[i] = argv[i];
	}
	params[argc] = NULL;

	execvp(prog_name, params);

	return 0;
}
