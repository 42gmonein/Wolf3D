# include "rt.h"
#define FOV 120.0f
#define DIV_FOV 60.0f

typedef struct		s_pos
{
	float			x;
	float			y;
}					t_pos;

typedef struct		s_vector_inc
{
	float			dx;
	float			dy;
	float			dif;
}					t_vector_inc;

typedef struct		s_ray
{
	float			floor_x_wall;
	float			floor_y_wall;
	float			cam_x;
	float			pos_x;
	float			pos_y;
	float			map_x;
	float			map_y;
	float			dir_x;
	float			dir_y;
	float			plane_x;
	float			plane_y;
	float			side_x;
	float			side_y;
	float			wall_dist;
	float			step_x;
	float			step_y;
	float			delta_dist_x;
	float			delta_dist_y;
	float			wallx;
	float			current_floor_x;
	float			current_floor_y;
	float			dist_player;
	float			weight;
	float			current_dist;
	int				floor_text_x;
	int				floor_text_y;
	int				hit;
	int				side;
	int				nside;
	int				line_height;
	int				draw_start;
	int				draw_end;
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
	ray->cam_x = 2 * x / (float)WIN_W - 1;
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

void	get_wall_inf(t_env *env, t_ray *ray)
{
	if (ray->side == 0)
		ray->wall_dist =
			(ray->map_x - ray->pos_x + (1 - ray->step_x) / 2) / ray->dir_x;
	else
		ray->wall_dist =
			(ray->map_y - ray->pos_y + (1 - ray->step_y) / 2) / ray->dir_y;
	if (ray->side == 0 && ray->pos_x - ray->map_x < 0
		|| ray->side == 1 && ray->pos_y - ray->map_y < 0)
		ray->nside = 2;
	else
		ray->nside = 0;
	ray->line_height = (int)(WIN_H / ray->wall_dist);
	ray->draw_start = -ray->line_height / 2 + (int)WIN_H / 2;
	ray->draw_end = ray->line_height / 2 + (int)WIN_H / 2;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	if (ray->draw_end >= (int)WIN_H)
		ray->draw_end = (int)WIN_H - 1;
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
	while (i + 10 < ray->draw_start)
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
	while (++i < ray->draw_start)
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

int		blend(unsigned char fg[4], unsigned char bg[4])
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

	if (ray->side == 0 || ray->side == 2)
		ray->wallx = ray->pos_y + ray->wall_dist * ray->dir_y;
	else
		ray->wallx = ray->pos_x + ray->wall_dist * ray->dir_x;
	ray->wallx -= floor(ray->wallx);
	textx = (int)(ray->wallx * (float)(env->bmp->w));
	if ((ray->side == 0) && ray->dir_x > 0)
		textx = env->bmp->w - textx - 1;
	if ((ray->side == 1) && ray->dir_y < 0)
		textx = env->bmp->w - textx - 1;
	y = ray->draw_start - 1;
	if (env->blur == 1)
	{
		fg = ray->wall_dist * 26;
		if (!(fg < 0xFF))
			fg = 0xFF;
		fg <<= 24;
	}
	while (++y < ray->draw_end)
	{
		d = (y << 8) - ((int)WIN_H << 7)
			+ ((int)ray->line_height << 7);
		texty = ((d * env->bmp->h) / ray->line_height) / 256;
		color = get_pixel(env->bmp, textx, texty);
		color = blend((char *)&fg, (char *)&color);
		px2img(env->pixels, color, x, y);
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

void	draw_floor_text(t_env *env, t_ray *ray, int x)
{
	int			y;
	int			color;
	int			fg;

	y = ray->draw_end;
	ray->dist_player = 0.0f;
	set_side(ray);
	while (++y < WIN_H)
	{
		ray->current_dist = WIN_H / (2.0f * y - WIN_H);
		ray->weight = (ray->current_dist - ray->dist_player)
					/ (ray->wall_dist - ray->dist_player);
		ray->current_floor_x =
			ray->weight * ray->floor_x_wall + (1.0 - ray->weight) * ray->pos_x;
		ray->current_floor_y =
			ray->weight * ray->floor_y_wall + (1.0 - ray->weight) * ray->pos_y;
		ray->floor_text_x =
						(int)(ray->current_floor_x * env->bmp->w) % env->bmp->w;
		ray->floor_text_y =
						(int)(ray->current_floor_y * env->bmp->h) % env->bmp->h;
		if (env->blur == 1)
		{
			fg = ray->current_dist * 26;
			if (!(fg < 0xFF))
				fg = 0xFF;
			fg = (fg << 24) & 0xFF000000;
		}
		color = get_pixel(env->bmp, ray->floor_text_x, ray->floor_text_y);
		color = blend((char *)&fg, (char *)&color);
		px2img(env->pixels, color, x, y - 1);
		color = get_pixel(env->bmp, ray->floor_text_x, ray->floor_text_y);
		color = blend((char *)&fg, (char *)&color);
		px2img(env->pixels, color, x, WIN_H - y);
	}
}

void	draw_sol(t_env *env, t_ray *ray, int x)
{
	int		y;

	y = ray->draw_end - 1;
	while (++y < WIN_H)
		px2img(env->pixels, 0x00A0A0A0, x, y);
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
		get_wall_inf(env, &ray);
		if (env->text == 1)
		{
			print_wall_text(env, &ray, x);
			draw_floor_text(env, &ray, x);
		}
		else
		{
			print_wall_uni(env, &ray, x);
			print_floor_uni(env, &ray, x);
			print_roof_uni(env, &ray, x);
		}
	}
}
