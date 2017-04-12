
#ifndef STRUCTURES_H
# define STRUCTURES_H

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
	float			axe;
	float			cam_x;
	float			pos_x;
	float			pos_y;
	float			ray_x;
	float			ray_y;
}					t_player;

typedef struct		s_env
{
    SDL_Window		*win;
    SDL_Event		event;
    SDL_Cursor		*cursor;
	SDL_Surface		*s_back;
	SDL_Surface		*bmp;
	SDL_Renderer	*render;
	SDL_Texture		*texture;
	pthread_t		**thread;
	t_args			*args;
	int				thread_cnt;
	int				pal;
	int				text;
	int				**map;
	float			dir_x;
	float			old_dir_x;
	float			dir_y;
	float			mid_dir_x;
	float			mid_dir_y;
	float			plane_x;
	float			old_plane_x;
	float			plane_y;
	t_player		cam;
    short			redraw;
    char			run;
	const Uint8		*key;
	int				x;
	int				y;
	int				color[1][6];
}                   t_env;

#endif /* structures_h */
