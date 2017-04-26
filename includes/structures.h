
#ifndef STRUCTURES_H
# define STRUCTURES_H

# include "rt.h"

typedef struct		s_args
{
	struct s_env	*env;
	int				start;
	int				end;
	int				done;
}					t_args;

typedef struct		s_argb
{
	unsigned char	b;
	unsigned char	g;
	unsigned char	r;
	unsigned char	a;
}					t_argb;

typedef struct		s_player
{
	double			axe;
	double			cam_x;
	double			pos_x;
	double			pos_y;
	double			ray_x;
	double			ray_y;
}					t_player;

typedef struct		s_env
{
    SDL_Window		*win;
    SDL_Event		event;
    SDL_Cursor		*cursor;
	SDL_Surface		*bmp;
	SDL_Renderer	*render;
	SDL_Texture		*texture;
	SDL_Surface		*sprite;
	SDL_Surface		*screen;
	int				*pixels;
	pthread_t		**thread;
	t_args			*args;
	short			thread_cnt;
	short			pal;
	short			text;
	short			blur;
	short			**map;
	double			dir_x;
	double			old_dir_x;
	double			dir_y;
	double			mid_dir_x;
	double			mid_dir_y;
	double			plane_x;
	double			old_plane_x;
	double			plane_y;
	t_player		cam;
    short			redraw;
    char			run;
	const Uint8		*key;
	int				x;
	int				y;
	int				color[1][6];
	time_t			time;
	time_t			oldtime;
	struct tm		mytime;
}                   t_env;

#endif /* structures_h */
