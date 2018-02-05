
NAME = get_ip

G = gcc

SRC = get_host.c \
	main.c

OBJ = $(SRC:.c=.o)

LIBFT = libft/libft.a

all: $(NAME)

$(NAME): $(OBJ)
	@make -C libft/
	$(G)  -o $@ $(OBJ) $(LIBFT)

%.o: %.c
	$(G) -c $< -o $@

clean:
	@make -C libft/ clean
	@rm -f $(OBJ)


fclean: clean
	@make -C libft/ fclean
	@rm -f $(NAME)
re: fclean all
	@make -C libft/ re
