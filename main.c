#include "get_ip.h"

char	*ft_strtoc(char *str, char c)
{
	int		i;

	i = 0;
	while (*str && *str < 33)
		str++;
	while (str[i] && str[i] != c)
		i++;
	if (i > 0)
		return (ft_strndup(str, (size_t)i));
	return (NULL);
}

char	*get_dns_server()
{
	int		fd;
	char	*line;
	char	*res;

	if ((fd = open(LOG_FILE, O_RDONLY)) < 0)
	{
		ft_putendl_fd("Error opening\n", STDERR_FILENO);
		return (NULL);
	}
	while (get_next_line(fd, &line))
	{
		if (line[0] == '#')
		{
			ft_strdel(&line);
			continue;
		}
		if (!ft_strncmp(line, "nameserver", 10))
		{
			res = ft_strtoc(line + 10, ' ');
			ft_strdel(&line);
			close(fd);
			return (res);
		}
		ft_strdel(&line);
	}
	close(fd);
	return (NULL);
}

void	init_dnshdr(struct DNS_HDR **dns_h)
{
	struct DNS_HDR	*dns = (*dns_h);

	dns->id = (unsigned short) htons(getpid());
	dns->qr = 0;
	dns->opcode = 0;
	dns->aa = 0;
	dns->tc = 0;
	dns->rd = 1;
	dns->ra = 0;
	dns->z = 0;
	dns->ad = 0;
	dns->cd = 0;
	dns->rcode = 0;
	dns->q_count = htons(1);
	dns->ans_count = 0;
	dns->auth_count = 0;
	dns->add_count = 0;
}

void name_format(unsigned char **src, char *host)
{
	int		lock = 0;
	int		len;
	char	*dns = (*src);

	strcat(host,".");
	len = (int)ft_strlen(host);
	for(int i = 0 ; i < len; i++)
	{
		if(host[i]=='.')
		{
			*dns++ = i - lock;
			for(;lock<i;lock++)
			{
				*dns++= host[lock];
			}
			lock++;
		}
	}
	*dns++='\0';
}

int	main(int ac, char **av)
{
	char	*dns_serv;

	if (ac != 2)
	{
		ft_putendl_fd("Error count argument", STDERR_FILENO);
		return (1);
	}
	if (!(dns_serv = get_dns_server()))
		dns_serv = OPEN_DNS;
	ft_get_host(av[1], dns_serv, T_A);
	return (0);
}
