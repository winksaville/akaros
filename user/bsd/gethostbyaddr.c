/* posix */
#include <sys/types.h>
#include <unistd.h>

/* bsd extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int h_errno;

struct hostent *gethostbyaddr (__const void *addr, __socklen_t len,
			       int type)
{
	unsigned long a, y;
	struct in_addr x;
	__const unsigned char *p = addr;

	if(type != AF_INET || len != 4){
		h_errno = NO_RECOVERY;
		return 0;
	}

	y = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
	x.s_addr = htonl(y);

	return gethostbyname(inet_ntoa(x));
}
