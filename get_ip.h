
#ifndef GET_IP_H
#define GET_IP_H

#include "libft/libft.h"
#include <fcntl.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <stdbool.h>

#define LOG_FILE "/etc/resolv.conf"
#define OPEN_DNS "208.67.222.222"
#define BUFF_LEN 65536
#define OFFSET 49152
#define T_A 1 //Ipv4 address
#define T_CNAME 5 // canonical name

struct				DNS_HDR
{
	unsigned short	id; // identification number

	unsigned char	rd :1; // recursion desired
	unsigned char	tc :1; // truncated message
	unsigned char	aa :1; // authoritive answer
	unsigned char	opcode :4; // purpose of message
	unsigned char	qr :1; // query/response flag

	unsigned char	rcode :4; // response code
	unsigned char	cd :1; // checking disabled
	unsigned char	ad :1; // authenticated data
	unsigned char	z :1; // its z! reserved
	unsigned char	ra :1; // recursion available

	unsigned short	q_count; // number of question entries
	unsigned short	ans_count; // number of answer entries
	unsigned short	auth_count; // number of authority entries
	unsigned short	add_count; // number of resource entries
};

#pragma pack(push, 1)
struct				R_DATA
{
	unsigned short	type;
	unsigned short	class;
	unsigned int	ttl;
	unsigned short	data_len;
};
#pragma pack(pop)

struct				RES_RECORD
{
	unsigned char	*name;
	struct R_DATA	*resource;
	unsigned char	*rdata;
};

struct				QUESTION
{
	unsigned short	qtype;
	unsigned short	qclass;
};

typedef struct
{
	unsigned char	*name;
	struct QUESTION	*ques;
}					QUERY;

char				*get_dns_server(void);
/*
 * Get the DNS servers from /etc/resolv.conf file on Linux
 */
void				init_dnshdr(struct DNS_HDR **dns_h);
void				name_format(unsigned char **src, char *host);
void				ft_get_host(char *host, char *dns_serc, int query_type);

#endif
