
#include "rt.h"
#include <signal.h>
#define D_PAS 0.05f
#define R_PAS 0.025f

int	SDL_EnableKeyRepeat(int a, int b);

void	px2img(SDL_Renderer *render, int clr, int x, int y)
{
//	t_argb	*c;
//
//	c = (t_argb *)&clr;
	SDL_SetRenderDrawColor(render,
		((t_argb *)&clr)->r,
		((t_argb *)&clr)->g,
		((t_argb *)&clr)->b,
		((t_argb *)&clr)->a
//		c->g,
//		c->b,
//		c->a
		);
	SDL_RenderDrawPoint(render, x, y);
}

int		get_pixel(SDL_Surface *src, int x, int y)
{
	return (*((int *)(src->pixels + y * src->pitch + (x << 2))));
}

static int handle_events(t_env *env)
{
	int		act;

	act = 0;
	if (env->key[SDL_SCANCODE_W] == 1)
	{
		env->cam.pos_x +=
			(env->map
			[(int)env->cam.pos_y][(int)(env->cam.pos_x + env->mid_dir_x * D_PAS)]
			== 0 ? D_PAS * env->mid_dir_x : 0);
		env->cam.pos_y +=
			(env->map
			[(int)(env->cam.pos_y + env->mid_dir_y * D_PAS)][(int)env->cam.pos_x]
			== 0 ? D_PAS * env->mid_dir_y : 0);
			act = 1;
	}
	if (env->key[SDL_SCANCODE_S] == 1)
	{
		env->cam.pos_x -=
			(env->map
			[(int)env->cam.pos_y][(int)(env->cam.pos_x - env->mid_dir_x * D_PAS)]
			== 0 ? D_PAS * env->mid_dir_x : 0);
		env->cam.pos_y -=
			(env->map
			[(int)(env->cam.pos_y - env->mid_dir_y * D_PAS)][(int)env->cam.pos_x]
			== 0 ? D_PAS * env->mid_dir_y: 0);
			act = 1;
	}
	if (env->key[SDLK_a] == 1)
		env->cam.pos_y -= D_PAS;
	if (env->key[SDLK_d] == 1)
		env->cam.pos_y += D_PAS;
	if (env->key[SDL_SCANCODE_E] == 1)
	{
		env->old_dir_x = env->dir_x;
		env->dir_x = env->dir_x * cos(-R_PAS)
					- env->dir_y * sin(-R_PAS);
		env->dir_y = env->old_dir_x * sin(-R_PAS)
					+ env->dir_y * cos(-R_PAS);
		env->old_plane_x = env->plane_x;
		env->plane_x = env->plane_x * cos(-R_PAS)
						- env->plane_y * sin(-R_PAS);
		env->plane_y = env->old_plane_x * sin(-R_PAS)
						+ env->plane_y * cos(-R_PAS);
			act = 1;
	}
	if (env->key[SDL_SCANCODE_Q] == 1)
	{
		env->old_dir_x = env->dir_x;
		env->dir_x = env->dir_x * cos(R_PAS)
					- env->dir_y * sin(R_PAS);
		env->dir_y = env->old_dir_x * sin(R_PAS)
					+ env->dir_y * cos(R_PAS);
		env->old_plane_x = env->plane_x;
		env->plane_x = env->plane_x * cos(R_PAS)
						- env->plane_y * sin(R_PAS);
		env->plane_y = env->old_plane_x * sin(R_PAS)
						+ env->plane_y * cos(R_PAS);
			act = 1;
	}
	return (act);
}

static int	global_loop(t_env *env)
{
	while (env->run)
	{
		SDL_PollEvent(&env->event);
			if (env->event.type == 256)
				env->run = 0;
			if (env->event.window.type == SDL_WINDOWEVENT_CLOSE
				|| env->event.key.keysym.sym == SDLK_ESCAPE
				|| env->event.type == SDL_QUIT)
					exit (1);
			if (handle_events(env) == 1)
//				raycast(env, 0, WIN_W);
				redraw(env);
			SDL_RenderPresent(env->render);
			env->key = SDL_GetKeyboardState(NULL);
	}
	exit(1);
	return (0);
}

void	init_color(t_env *env)
{
	env->color[0][0] = 0xDEE9ED;
	env->color[0][1] = 0xC6C7CB;
	env->color[0][2] = 0x808387;
	env->color[0][3] = 0x71EBAF;
	env->color[0][4] = 0x42A857;
	env->color[0][5] = 0x332F3B;
}

int main(int argc, char **argv)
{
	t_env	env;
	int		**map;

	env.cam.pos_x = 3;
	env.cam.pos_y = 3;
	env.dir_x = -1;
	env.dir_y = 0;
	env.plane_x = 0;
	env.plane_y = 0.66f;
	env.x = 0;
	env.y = 0;
	env.run = 1;
	env.pal = 0;
	env.text = 1;
	if (argv[2] && atoi(argv[2]) > 0)
		env.thread_cnt = atoi(argv[2]);
	else
		env.thread_cnt = 1;
	env.args = (t_args *)malloc(sizeof(t_args) * (env.thread_cnt + 1));
	env.thread = malloc_thread(env.thread_cnt, env.args, &env);
	init_color(&env);
	if (SDL_Init(SDL_INIT_EVENTS) == -1)
		exit(2);
	env.win = SDL_CreateWindow("Wolf3D", 0, 0, WIN_W, WIN_H, 0);
	env.render = SDL_CreateRenderer(env.win, 0,
		SDL_RENDERER_TARGETTEXTURE |
		SDL_RENDERER_SOFTWARE |
		SDL_RENDERER_ACCELERATED |
	0	);
	SDL_SetRenderDrawColor(env.render, 0xC0, 0, 0, 255);
	SDL_RenderClear(env.render);
	SDL_RenderPresent(env.render);
//	env.texture = SDL_CreateTexture(env.render,
//		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
//		WIN_W, WIN_H);
	env.map = parsing(argv[1]);
	env.key = SDL_GetKeyboardState(NULL);
	env.bmp = SDL_LoadBMP("topars.bmp");
    return (global_loop(&env));
}
