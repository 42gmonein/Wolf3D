NAME = Wolf3D

CPP = gcc
CFLAGS = -O3 -fsanitize=address
SDLFLAGS = -lSDL2-2.0
HEADER = includes
SOURCES = main.c \
		parsing.c \
		raycast.c \
		filler.c \
		ft_fill_area.c
DIR_O = obj
DIR_S = srcs

SRCS = $(addprefix $(DIR_S)/,$(SOURCES))
OBJS = $(addprefix $(DIR_O)/,$(SOURCES:.c=.o))

all: obj $(NAME)

${NAME}: $(OBJS)
	${CPP} $(CFLAGS) -o $@ $^ $(SDLFLAGS) libft/libft.a -lm

$(DIR_O)/%.o: $(DIR_S)/%.c
	$(CPP) $(CFLAGS) -I $(HEADER) -c -o $@ $< $(SDLFLAGS) libft/libft.a -lm

obj:
	mkdir -p obj

clean:	
	rm -rf obj

.PHONY: all $(NAME)
