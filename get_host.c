#include "get_ip.h"
#include <stdio.h>

static u_char	*read_name(unsigned char *reader, unsigned char *buff, int *count)
{
	unsigned char	*name;
	unsigned int	offset, jumped = 0, indx = 0;
	int				len;

	name = (unsigned char*)malloc( 256);
	name[0] = 0;
	(*count) = 1;
	while (*reader)
	{
		if(*reader >= 192)
		{
			offset = (*reader)*256 + *(reader+1) - OFFSET; //49152 = 11000000 00000000 ;)
			reader = buff + offset - 1;
			jumped = 1;
		}
		else
			name[indx++] = *reader;
		reader++;
		if (!jumped)
			(*count)++;
	}
	name[indx] = '\0';
	if (jumped)
		(*count)++;
	len = (int)ft_strlen((char*)name);
	for(int i = 0; i < len; i++)
	{
		indx = name[i];
		for (int j = 0; j < indx; j++)
		{
			name[i] = name[i + 1];
			i++;
		}
		if (i + 1 < len)
			name[i] = '.';
		else
			name[i] = '\0';
	}
	return (name);
}

u_char* ReadName(unsigned char* reader,unsigned char* buffer,int* count)
{
	unsigned char *name;
	unsigned int p=0,jumped=0,offset;
	int i , j;

	*count = 1;
	name = (unsigned char*)malloc(256);

	name[0]='\0';

	//read the names in 3www6google3com format
	while(*reader!=0)
	{
		if(*reader>=192)
		{
			offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000 ;)
			reader = buffer + offset - 1;
			jumped = 1; //we have jumped to another location so counting wont go up!
		}
		else
		{
			name[p++]=*reader;
		}

		reader = reader+1;

		if(jumped==0)
		{
			*count = *count + 1; //if we havent jumped to another location then we can count up
		}
	}

	name[p]='\0'; //string complete
	if(jumped==1)
	{
		*count = *count + 1; //number of steps we actually moved forward in the packet
	}

	//now convert 3www6google3com0 to www.google.com
	for(i=0;i<(int)strlen((const char*)name);i++)
	{
		p=name[i];
		for(j=0;j<(int)p;j++)
		{
			name[i]=name[i+1];
			i=i+1;
		}
		name[i]='.';
	}
	name[i-1]='\0'; //remove the last dot
	return name;
}

static void				print_answer(struct DNS_HDR	*dns, struct RES_RECORD *answers)
{
	long 				*ip;
	struct sockaddr_in	res;

	for(int i = 0; i < ntohs(dns->ans_count); i++)
	{
		ft_putstr("Name : ");
		ft_putstr(answers->name);
		if( ntohs(answers[i].resource->type) == T_A)
		{
			ip = (long*)answers[i].rdata;
			res.sin_addr.s_addr = (in_addr_t)(*ip);
			ft_putstr(" has IPv4 address : ");
			ft_putendl(inet_ntoa(res.sin_addr));
		}
		else if(ntohs(answers[i].resource->type)== T_CNAME)
		{
			//Canonical name for an alias
			ft_putstr("has alias name : ");
			ft_putendl(answers[i].rdata);
		}
		else
			ft_putchar('\n');
	}
}

static void				ft_parc_answer(struct DNS_HDR *dns, unsigned char *reader, unsigned char *buf)
{
	struct RES_RECORD	answers[20];
	int 				stop = 0;


	for(int i = 0; i < ntohs(dns->ans_count); i++)
	{
		answers->name = read_name(reader, buf, &stop);
		reader += stop;
		answers[i].resource = (struct R_DATA*)(reader);
		reader = reader + sizeof(struct R_DATA);
		if(ntohs(answers[i].resource->type) == 1)
		{
			answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));
			for(int j = 0; j < ntohs(answers[i].resource->data_len) ; j++)
				answers[i].rdata[j]=reader[j];
			answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';
			reader = reader + ntohs(answers[i].resource->data_len);
		}
		else
		{
			answers[i].rdata = read_name(reader,buf,&stop);
			reader = reader + stop;
		}
	}
	print_answer(dns, answers);
}

static unsigned char	*send_recv(unsigned char *buff, size_t len, char *dns_serv)
{
	struct sockaddr_in dest;
	int					sock;
	int					buf_len;

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	dest.sin_family = AF_INET;
	dest.sin_port = htons(53);
	dest.sin_addr.s_addr = inet_addr(dns_serv);
	buf_len = sizeof(dest);
	if (sendto(sock, buff, len, 0, (struct sockaddr*)&dest, sizeof(dest)) < 0)
	{
		ft_putendl_fd("Error sendto dns server\n", STDERR_FILENO);
		exit(1);
	}
	if(recvfrom (sock, buff , BUFF_LEN , 0 , (struct sockaddr*)&dest , (socklen_t*)&buf_len ) < 0)
	{
		ft_putendl_fd("Error recvfrom dns server\n", STDERR_FILENO);
		exit(1);
	}
	return (buff);
}

void					ft_get_host(char *host, char *dns_serc, int query_type)
{
	unsigned char	buf[BUFF_LEN], *qname, *reader;
	struct DNS_HDR	*dns = NULL;
	struct QUESTION	*qinfo = NULL;

	ft_bzero(buf, BUFF_LEN);
	dns = (struct DNS_HDR *)&buf;
	init_dnshdr(&dns);
	qname = &buf[sizeof(struct DNS_HDR)];
	name_format(&qname, host);
	qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HDR) + (strlen((const char*)qname) + 1)];
	qinfo->qtype = htons(query_type);
	qinfo->qclass = htons(1);
	send_recv(buf, sizeof(struct DNS_HDR) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION), dns_serc);
	dns = (struct DNS_HDR*)buf;
	reader = &buf[sizeof(struct DNS_HDR) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];

	ft_parc_answer(dns, reader, buf);
}