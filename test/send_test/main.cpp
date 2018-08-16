#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


#define SOCK_NAME    "/tmp/imm_socket_sample"

pthread_mutex_t g_mutex;

static void sigHandler (int nSigno)
{
	printf ("%d\n", nSigno);
}

static void *threadHandler (void *args)
{
	int fd = (int)(*(int*)args);
	printf ("%s fd %d\n", __func__, fd);

	char buf[]   = {0x01, 0x00, 0x00 ,0x00, 0x04}; //null packet
	char buf2[]  = {0x01, 0x00, 0x00, 0x02, 'a', 'b', 0x04}; // 1packet
	char buf3[]  = {0x01, 0x00, 0x00, 0x02, 'c', 'd', 0x04, 0x01, 0x00, 0x00, 0x03, 'e', 'f', 'g', 0x04}; // 2packet set

	// divide test
//	char buf3[]  = {0x01, 0x00, 0x00, 0x02, 'c', 'd', 0x04, 0x01, 0x00, 0x00, 0x03, 'e', 'f', 'g', 0x04};
	char buf4[]  = {0x01, 0x00, 0x00                                                                   };
	char buf5[]  = {                  0x02, 'c'                                                        };
	char buf6[]  = {                             'd', 0x04, 0x01, 0x00                                 };
	char buf7[]  = {                                                    0x00, 0x03, 'e'                };
	char buf8[]  = {                                                                     'f'           };
	char buf9[]  = {                                                                          'g'      };
	char buf10[] = {                                                                               0x04};


	while (1) {
		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf, sizeof(buf)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);

		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf2, sizeof(buf2)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);
		
		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf3, sizeof(buf3)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);
		
		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf4, sizeof(buf4)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);
		
		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf5, sizeof(buf5)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);
		
		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf6, sizeof(buf6)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);
		
		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf7, sizeof(buf7)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);
		
		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf8, sizeof(buf8)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);

		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf9, sizeof(buf9)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);

		sleep (5);

		pthread_mutex_lock (&g_mutex);
		if (write (fd, buf10, sizeof(buf10)) < 0) {
			perror("write");
		}
		pthread_mutex_unlock (&g_mutex);
	}

    return NULL;
}

int main (void)
{
	pthread_mutex_init (&g_mutex, NULL);

	struct sigaction stSigact;
	memset (&stSigact, 0x00, sizeof(stSigact));
	stSigact.sa_handler = sigHandler;
	if (sigaction (SIGPIPE, &stSigact, NULL) < 0) {
		perror ("sigaction()");
		exit (EXIT_FAILURE);
	}


	struct sockaddr_un addr;
	int fd;
	char buf[1024] = {0};

	if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit( EXIT_FAILURE );
	}
	printf ("fd=%d\n", fd);


	memset( &addr, 0x00, sizeof(addr) );
	addr.sun_family = AF_UNIX;
	strcpy( addr.sun_path, SOCK_NAME );

	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr.sun_family)+strlen(SOCK_NAME)) < 0) {
		close (fd);
		perror("connect");
		exit( EXIT_FAILURE );
	}


	pthread_t id;
	if (pthread_create (&id, NULL, threadHandler, &fd) != 0) {
		close (fd);
		perror ("pthread_create");
		exit( EXIT_FAILURE );
	}

	while (1) {
		memset (buf, 0x00, sizeof(buf));

		fgets(buf, sizeof(buf)-1, stdin);

		// delete LF
		*(buf + strlen(buf) - 1) = 0x00;

		if (
			(*(buf+0) == 'S') &&
			(*(buf+1) == 'O') &&
			(*(buf+2) == 'H')
		) {
			pthread_mutex_lock (&g_mutex);
			memset (buf, 0x00, sizeof(buf));
			*(buf+0) = 0x01;

		} else if (
			(*(buf+0) == 'E') &&
			(*(buf+1) == 'O') &&
			(*(buf+2) == 'T')
		) {
			memset (buf, 0x00, sizeof(buf));
			*(buf+0) = 0x04;
			pthread_mutex_unlock (&g_mutex);
		}


		if (write (fd, buf, strlen(buf)) < 0) {
			perror("write");
		}

//		if (read (fd, buf, sizeof(buf)) < 0) {
//			perror("read");
//		}
//		printf ("echo [%s]\n", buf);
	}

	close (fd);
	pthread_mutex_destroy (&g_mutex);


	exit( EXIT_SUCCESS );
}
