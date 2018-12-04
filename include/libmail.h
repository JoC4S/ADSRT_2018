
#ifndef mail_H
#define mail_H

#define REQUEST_MSG_SIZE        1024
#define REPLY_MSG_SIZE          500
#define SERVER_PORT_NUM         25

#define WAITFOR_ACK		0
#define NOACKWAIT		1

#define ehlo            "ehlo localhost\n"
#define authlogin       "auth login\n"
#define mailuser64      "am1jc2FlekBnbWFpbC5jb20K\n"                            /** usuario gmail encriptado*/
#define userpass64      "RXJydlQzVTMK\n"                                        /** password usuario gmail encriptado*/
//#define mailto        "RCPT TO:: <user@euss.es>\n"                           /** Remitente*/
#define mailfrom       "mail from: <user@euss.es>\n"                           /** Remitente*/
#define DATA            "DATA\n"                                                /** Comando de inicio de cuerpo del correo*/
#define mailSubject	    "Subject: Informe diario.\nFrom: user@euss.es\nTo: <user@euss.es>\n" /** Asunto del correo*/
#define endOfmail       "\r\n.\r\n"                                             /** Comando de finalización de correo.*/


void sendTCPData (int opcion, char *msg);
int sendmail(char* mailto,char *texto_a_enviar);
#endif
