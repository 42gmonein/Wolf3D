# include "rt.h"

typedef struct		s_pos
{
	double			x;
	double			y;
}					t_pos;

typedef struct		s_vector_inc
{
	double			dx;
	double			dy;
	double			dif;
}					t_vector_inc;

typedef struct		s_ray
{
	double			floor_x_wall;
	double			floor_y_wall;
	double			cam_x;
	double			pos_x;
	double			pos_y;
	double			map_x;
	double			map_y;
	double			dir_x;
	double			dir_y;
	double			plane_x;
	double			plane_y;
	double			side_x;
	double			side_y;
	double			wall_dist;
	double			step_x;
	double			step_y;
	double			delta_dist_x;
	double			delta_dist_y;
	double			wallx;
	double			current_floor_x;
	double			current_floor_y;
	double			dist_player;
	double			weight;
	double			current_dist;
	int				floor_text_x;
	int				floor_text_y;
	int				hit;
	int				side;
	int				nside;
	int				line_height;
	int				draw_start;
	int				draw_end;
	int				real_draw_end;
	int				real_draw_start;
}					t_ray;

t_vector_inc		set_vector_by_x(t_vector_inc vec)
{
	return ((t_vector_inc){
		vec.dx * fabs(1 / vec.dx),
		vec.dy * fabs(1 / vec.dx), 0});
}

t_vector_inc		set_vector_by_y(t_vector_inc vec)
{
	return ((t_vector_inc){
		vec.dx * fabs(1 / vec.dy),
		vec.dy * fabs(1 / vec.dy), 0});
}


void	set_ray(t_env *env, t_ray *ray, int x)
{
	ray->cam_x = 2 * x / (double)WIN_W - 1;
	ray->pos_x = env->cam.pos_x;
	ray->pos_y = env->cam.pos_y;
	ray->dir_x = env->dir_x + ray->plane_x * ray->cam_x;
	ray->dir_y = env->dir_y + ray->plane_y * ray->cam_x;
	if (x == (int)WIN_W >> 1)
	{
		env->mid_dir_x = ray->dir_x;
		env->mid_dir_y = ray->dir_y;
	}
	ray->map_x = (int)ray->pos_x;
	ray->map_y = (int)ray->pos_y;
	ray->delta_dist_x =
		sqrt(1 + (ray->dir_y * ray->dir_y) / (ray->dir_x * ray->dir_x));
	ray->delta_dist_y =
		sqrt(1 + (ray->dir_x * ray->dir_x) / (ray->dir_y * ray->dir_y));
}

void	set_raystep(t_env *env, t_ray *ray)
{
	if (ray->dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_x = (ray->pos_x - ray->map_x) * ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_x = (ray->map_x + 1.0f - ray->pos_x) * ray->delta_dist_x;
	}
	if (ray->dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_y = (ray->pos_y - ray->map_y) * ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_y = (ray->map_y + 1.0f - ray->pos_y) * ray->delta_dist_y;
	}
}

int		launch_ray(t_env *env, t_ray *ray)
{
	while (1)
	{
		if (ray->side_x < ray->side_y)
		{
			ray->side_x += ray->delta_dist_x;
			ray->map_x += ray->step_x;
			ray->side = 0;
		}
		else
		{
			ray->side_y += ray->delta_dist_y;
			ray->map_y += ray->step_y;
			ray->side = 1;
		}
		if (ray->map_y < 0 || ray->map_x < 0)
			return (-1);
		if (env->map[(int)ray->map_y][(int)ray->map_x] != 0)
			return (env->map[(int)ray->map_y][(int)ray->map_x]);
	}
}

void	get_wall_inf(t_env *env, t_ray *ray, int x)
{
	if (ray->side == 0)
		ray->wall_dist =
			(ray->map_x - ray->pos_x + (1 - ray->step_x) / 2) / ray->dir_x;
	else
		ray->wall_dist =
			(ray->map_y - ray->pos_y + (1 - ray->step_y) / 2) / ray->dir_y;
	if ((ray->side == 0 && (ray->pos_x - ray->map_x < 0))
		|| (ray->side == 1 && (ray->pos_y - ray->map_y < 0)))
		ray->nside = 2;
	else
		ray->nside = 0;
	ray->line_height = (int)(256 / ray->wall_dist);
	ray->draw_start = -ray->line_height / 2 + (WIN_H / 4);
	ray->draw_end = ray->line_height / 2 + (WIN_H / 4);
	ray->real_draw_end = ray->draw_end;
	ray->real_draw_start = ray->draw_start;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	if (ray->draw_end >= (int)WIN_H)
		ray->draw_end = (int)WIN_H - 1;
	if (ray->side == 0 || ray->side == 2)
		ray->wallx = ray->pos_y + ray->wall_dist * ray->dir_y;
	else
		ray->wallx = ray->pos_x + ray->wall_dist * ray->dir_x;
	ray->wallx -= floor(ray->wallx);
}

void	print_wall_uni(t_env *env, t_ray *ray, int x)
{
	int		color;;
	int		i;

	i = ray->draw_start;
	color = env->color[env->pal][ray->side + ray->nside];
	while (i + 10 < ray->draw_end)
	{
		px2img(env->pixels, color, x, i);
		px2img(env->pixels, color, x, i + 1);
		px2img(env->pixels, color, x, i + 2);
		px2img(env->pixels, color, x, i + 3);
		px2img(env->pixels, color, x, i + 4);
		px2img(env->pixels, color, x, i + 5);
		px2img(env->pixels, color, x, i + 6);
		px2img(env->pixels, color, x, i + 7);
		px2img(env->pixels, color, x, i + 8);
		px2img(env->pixels, color, x, i + 9);
		i += 10;
	}
	i--;
	while (++i < ray->draw_end)
		px2img(env->pixels, color, x, i);
}

void	print_roof_uni(t_env *env, t_ray *ray, int x)
{
	int		color;;
	int		i;

	i = 0;
	color = env->color[env->pal][4];
	while (i + 10 < ray->draw_end)
	{
		px2img(env->pixels, color, x, i);
		px2img(env->pixels, color, x, i + 1);
		px2img(env->pixels, color, x, i + 2);
		px2img(env->pixels, color, x, i + 3);
		px2img(env->pixels, color, x, i + 4);
		px2img(env->pixels, color, x, i + 5);
		px2img(env->pixels, color, x, i + 6);
		px2img(env->pixels, color, x, i + 7);
		px2img(env->pixels, color, x, i + 8);
		px2img(env->pixels, color, x, i + 9);
		i += 10;
	}
	i--;
	while (++i < ray->draw_end)
		px2img(env->pixels, color, x, i);
}

void	print_floor_uni(t_env *env, t_ray *ray, int x)
{
	int		color;;
	int		i;

	i = ray->draw_end;
	color = env->color[env->pal][5];
	while (i + 10 < WIN_H - 1)
	{
		px2img(env->pixels, color, x, i);
		px2img(env->pixels, color, x, i + 1);
		px2img(env->pixels, color, x, i + 2);
		px2img(env->pixels, color, x, i + 3);
		px2img(env->pixels, color, x, i + 4);
		px2img(env->pixels, color, x, i + 5);
		px2img(env->pixels, color, x, i + 6);
		px2img(env->pixels, color, x, i + 7);
		px2img(env->pixels, color, x, i + 8);
		px2img(env->pixels, color, x, i + 9);
		i += 10;
	}
	i--;
	while (++i < WIN_H - 1)
		px2img(env->pixels, color, x, i);
}

int		blend(unsigned char *fg, unsigned char *bg)
{
	unsigned int		alpha;
	unsigned int		inv_alpha;
	unsigned char		res[4];

	alpha = fg[3] + 1;
	inv_alpha = 256 - fg[3];
	res[2] = (unsigned char)((alpha * fg[2] + inv_alpha * bg[2]) >> 8);
	res[0] = (unsigned char)((alpha * fg[0] + inv_alpha * bg[0]) >> 8);
	res[1] = (unsigned char)((alpha * fg[1] + inv_alpha * bg[1]) >> 8);
	res[3] = 0xFF;
	return (*(unsigned int *)res);
}

void	print_wall_text(t_env *env, t_ray *ray, int x)
{
	int			textx;
	int			y;
	int			d;
	int			texty;
	int			color;
	int			fg;
	char		alpha;

	textx = (int)(ray->wallx * (double)((env->bmp[0]->w * 8))) % env->bmp[0]->w;
	if ((ray->side == 0) && ray->dir_x > 0)
		textx = env->bmp[0]->w - textx - 1;
	if ((ray->side == 1) && ray->dir_y < 0)
		textx = env->bmp[0]->w - textx - 1;
	y = (ray->real_draw_end - ray->real_draw_start) * 3 / 4 + ray->real_draw_start;
	if (env->blur == 1)
	{
		fg = ray->wall_dist * 26;
		if (!(fg < 0xFF))
			fg = 0xFF;
		fg <<= 24;
		fg |= 0x00FFFFFF;
	}
	while (++y < ray->draw_end)
	{
		d = (y << 8) - ((int)WIN_H << 6)
			+ ((int)ray->line_height << 7);
		texty = ((d * env->bmp[0]->h) / ray->line_height) / 64;
		texty %= env->bmp[0]->h;
		color = get_pixel(env->bmp[0], textx, texty % env->bmp[0]->h);
	//	color = blend((void *)&fg, (void *)&color);
		color = blend((void *)&color, (void *)&env->pixels[x + y * (int)(WIN_W)]);
		if (env->zbuffer[y][x] > ray->wall_dist)
		{
			env->zbuffer[y][x] = ray->wall_dist;
			px2img(env->pixels, color, x, y);
		}
	}
}

void	set_side(t_ray *ray)
{
	if (ray->side == 0 && ray->dir_x > 0)
	{
		ray->floor_x_wall = ray->map_x;
		ray->floor_y_wall = ray->map_y + ray->wallx;
	}
	else if (ray->side == 0 && ray->dir_x < 0)
	{
		ray->floor_x_wall = ray->map_x + 1;
		ray->floor_y_wall = ray->map_y + ray->wallx;
	}
	else if (ray->side == 1 && ray->dir_y > 0)
	{
		ray->floor_x_wall = ray->map_x + ray->wallx;
		ray->floor_y_wall = ray->map_y;
	}
	else
	{
		ray->floor_x_wall = ray->map_x + ray->wallx;
		ray->floor_y_wall = ray->map_y + 1;
	}
}

void	print_skybox(t_env *env, t_ray *ray, int x)
{
	double	radius;
	int		textx;
	int		y;

	radius = atan2(ray->dir_x, ray->dir_y) + M_PI;
	textx = 1.32 * env->skybox->w * radius / M_PI;
	textx = textx >> 1;
	y = 0;
	while (y < WIN_H / 4)
	{
		px2img(env->pixels, get_pixel(env->skybox, textx, (y * 1 / 7) % env->skybox->h), x, y);
		y++;
	}
}

void	draw_floor_text(t_env *env, t_ray *ray, int x)
{
	int			y;
	int			color;
	int			fg;
	SDL_Surface	*text;
;
	int			tmp;
	y = WIN_H / 4;
	ray->dist_player = 0.0f;
	set_side(ray);
	while (++y < WIN_H)
	{
		tmp = 0;
		if (y < ray->draw_end)
			text = env->bmp[3];
		else
			text = env->bmp[2];
		ray->current_dist = 128 / (y * 2 - WIN_H / 2);
		ray->weight = (ray->current_dist - ray->dist_player)
					/ (ray->wall_dist - ray->dist_player);
		ray->current_floor_x =
			ray->weight * ray->floor_x_wall + (1.0 - ray->weight) * ray->pos_x;
		ray->current_floor_y =
			ray->weight * ray->floor_y_wall + (1.0 - ray->weight) * ray->pos_y;
		if (((int)(ray->current_floor_x) > 0 && (int)ray->current_floor_y > 0)
		&& (ray->current_floor_x < env->map_w && ray->current_floor_y < env->map_h)
		&& (env->map[(int)ray->current_floor_y][(int)ray->current_floor_x] != 0))
		{
			text = env->bmp[0];
			tmp = 1;
		}
		else
		{
			ray->current_dist = 256 / (y * 2 - WIN_H / 2);
			ray->weight = (ray->current_dist - ray->dist_player)
						/ (ray->wall_dist - ray->dist_player);
			ray->current_floor_x =
				ray->weight * ray->floor_x_wall + (1.0 - ray->weight) * ray->pos_x;
			ray->current_floor_y =
				ray->weight * ray->floor_y_wall + (1.0 - ray->weight) * ray->pos_y;

		}
		ray->floor_text_x =
						abs((int)(ray->current_floor_x * 64 - 64)) % (text->w);
		ray->floor_text_y =
						abs((int)(ray->current_floor_y * 64 - 64)) % (text->h);
		color = get_pixel(text, ray->floor_text_x, ray->floor_text_y);
		if (env->blur == 1)
		{
			fg = ray->current_dist * 26;
			if (!(fg < 0xFF))
				fg = 0xFF;
			fg = (fg << 24) & 0xFF000000;
			fg |= 0x00FFFFFF;
			color = blend((void *)&fg, (void *)&color);
		}
		if (env->zbuffer[y - 1][x] == 42 || tmp == 1)
			px2img(env->pixels, color, x, y - 1);
		env->zbuffer[y - 1][x] = 42;
	}
}

void	draw_sprite(t_env *env)
{
	double		sprite_dist;
	double		sprite_x;
	double		sprite_y;

	double		invdet;
	double		transform_x;
	double		transform_y;

	int			sprite_screen_x;

	int			sprite_height;
	int			sprite_width;
	int			draw_start_x;
	int			draw_end_x;
	int			draw_start_y;
	int			draw_end_y;
	int			text_x;
	int			text_y;

	sprite_dist = ((env->cam.pos_x - env->sprite_pos_x) * (env->cam.pos_x - env->sprite_pos_x)
				+ (env->cam.pos_y - env->sprite_pos_y) * (env->cam.pos_y - env->sprite_pos_y));
	sprite_x = env->sprite_pos_x - env->cam.pos_x;
	sprite_y = env->sprite_pos_y - env->cam.pos_y;

	invdet = 1.0 / (env->dir_y * env->plane_x - env->dir_x * env->plane_y);

	transform_x = invdet * (env->dir_y * sprite_x - env->dir_x * sprite_y);
	transform_y = invdet * (-env->plane_y * sprite_x + env->plane_x * sprite_y);

	sprite_screen_x = (int)((WIN_W / 2) * (1 + transform_x / transform_y));

	sprite_height = abs((int)(WIN_H / transform_y));
	draw_start_y = -sprite_height / 2 + WIN_H / 2;
	if (draw_start_y < 0)
		draw_start_y = 0;
	draw_end_y = sprite_height / 2 + WIN_H / 2;
	if (draw_end_y >= WIN_H)
		draw_end_y = WIN_H - 1;

	sprite_width = abs((int)(WIN_H / transform_y));
	draw_start_x = -sprite_width / 2 + sprite_screen_x;
	if (draw_start_x < 0)
		draw_start_x = 0;
	draw_end_x = sprite_width / 2 + sprite_screen_x;
	if (draw_end_x >= WIN_W)
		draw_end_x = WIN_W - 1;

	int			i;
	int			y;
	int			d;
	int			color;
	int			pix;;
	i = draw_start_x - 1;
	while (++i < draw_end_x)
	{
		text_x = (int)(256 * (i - (-sprite_width / 2 + sprite_screen_x))
								* env->sprite->w / sprite_width) / 256;
		y = draw_start_y - 1;
		if (transform_y > 0 && i > 0 && i < WIN_W && transform_y < env->zbuffer[0][i])
			while (++y < draw_end_y)
			{
				d = y * 256 - WIN_H * 128 + sprite_height * 128;
				text_y = ((d * env->sprite->h) / sprite_height) / 256;
				color = get_pixel(env->sprite, text_x, text_y /*% env->sprite->h*/);
				color = blend((void *)&color, (void *)&env->pixels[i + y * (int)(WIN_W)]);
				px2img(env->pixels, color, i, y);
			}
	}
}

int		raycast(t_env *env, int start, int end)
{
	t_ray	ray;
	int		x;
	int		i;

	ray.plane_x = env->plane_x;
	ray.plane_y = env->plane_y;
	x = start - 1;
	while (++x < end)
	{
		set_ray(env, &ray, x);
		set_raystep(env, &ray);
		ray.hit = launch_ray(env, &ray);
		get_wall_inf(env, &ray, x);
		print_wall_text(env, &ray, x);
		while (ray.hit != -1)
		{
			ray.hit = launch_ray(env, &ray);
			get_wall_inf(env, &ray, x);
			print_wall_text(env, &ray, x);
		}
		if (env->text == 1)
		{
	//		print_wall_text(env, &ray, x);
	//		print_roof_uni(env, &ray, x);
			print_skybox(env, &ray, x);
			draw_floor_text(env, &ray, x);
		}
		else
		{
			print_wall_uni(env, &ray, x);
			print_floor_uni(env, &ray, x);
			print_roof_uni(env, &ray, x);
		}
	}
	return (1);
}
