/*!
   \file rfcomm-client.c
   \brief "Description"
   \author "Your name"
   \date "DD"/"Month"/"Year"
 */
/** As with Internet programming, first allocate a socket with the socket system call.
*Instead of AF_INET, use AF_BLUETOOTH, and instead of IPPROTO_TCP, use BTPROTO_RFCOMM.
*Since RFCOMM provides the same delivery semantics as TCP, SOCK_STREAM can still be used
for the socket type.
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv)
{
	struct sockaddr_rc addr = { 0 };
	int s, status;
	char dest[18] = "B8:27:EB:00:3D:BC";

	// allocate a socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t)1;
	str2ba( dest, &addr.rc_bdaddr );

	// connect to server
	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

	// send a message
	if ( status == 0 ) {
		status = write(s, "hello!", 6);
	}

	if ( status < 0 ) {
		perror("uh oh");
	}

	close(s);
	return 0;
}
