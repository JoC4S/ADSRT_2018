/*!
   \file rfcomm - server.c
   \brief how to establish a connection using an RFCOMM socket, transfer some data,
   \and disconnect. For simplicity, the client is hard-coded to connect to B8:27:EB:00:3D:BC".
   \macbook bluetooth mac: B8:E8:56:2A:F5:43
   \author "Your name"
   \date "DD"/"Month"/"Year"
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv)
{
	struct sockaddr_rc loc_addr = {0}, rem_addr = {0};
	char buf[1024] = { 0 };
	int sd, client, bytes_read;
	socklen_t opt = sizeof(rem_addr);

	// allocate socket
	sd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// bind socket to port 1 of the first available
	// local bluetooth adapter
	loc_addr.rc_family = AF_BLUETOOTH;
	loc_addr.rc_bdaddr = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t)1;
	bind(sd, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

	// put socket into listening mode
	printf("Socket en modo escucha...\n");
	listen(sd, 1);

	// accept one connection
	client = accept(sd, (struct sockaddr *)&rem_addr, &opt);
	printf("Petición de conexion aceptada");

	ba2str( &rem_addr.rc_bdaddr, buf );
	fprintf(stderr, "accepted connection from %s\n", buf);
	memset(buf, 0, sizeof(buf));

	// read data from the client
	bytes_read = read(client, buf, sizeof(buf));
	if ( bytes_read > 0 ) {
		printf("received [%s]\n", buf);
	}

	// close connection
	close(client);
	close(sd);
	return 0;
}
