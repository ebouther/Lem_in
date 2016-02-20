/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lem_in.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebouther <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/02/19 18:05:08 by ebouther          #+#    #+#             */
/*   Updated: 2016/02/20 01:57:13 by ebouther         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lem_in.h"

static t_list	*ft_get_tunnel(char **tunnels, t_list **lst)
{
	t_list	*tmp;
	int		i;
	int		len;

	i = 0;
	len = ft_split_len(tunnels);
	ft_printf("LEN : '%d'\n", len);
	while (i < len)
	{
		tmp = *lst;
		while (tmp != NULL)
		{
			if (ft_strcmp(((t_room *)(tmp->content))->name, tunnels[i]) == 0)
			{
				if (((t_room *)(tmp->content))->checked == 0)
				{
					((t_room *)(tmp->content))->checked = 1;
					return (tmp);
				}
				else
					break ;
			}
			tmp = tmp->next;
		}
		i++;
	}
	return (NULL);
}

static void	ft_get_all_paths(t_list *node, t_list **lst, t_env *e)
{
	char **path;

	(void)e;
	if ((path = (char **)malloc(sizeof(char *))) == NULL)
		ft_error_exit("Cannot allocate path in ft_get_all_paths().\n");
	*path = NULL;
	path = ft_split_join_free(path, ((t_room *)(node->content))->name);
	
	ft_printf("NODE NAME : '%s'\n", ((t_room *)(node->content))->name);
	while (((t_room *)(node->content))->start_end != -1)
	{
		if ((node = ft_get_tunnel(((t_room *)(node->content))->tunnels, lst)) == NULL)
			ft_error_exit("I'll think about that later.\n");
		ft_printf("NODE NAME : '%s'\n", ((t_room *)(node->content))->name);
		path = ft_split_join_free(path, ((t_room *)(node->content))->name);
	}
}

static void	ft_print_all_paths(t_list **lst, t_env *e)
{
	t_list	*tmp;
   
	tmp	= *lst;
	while (tmp != NULL)
	{
		if (((t_room *)(tmp->content))->start_end == 1)
		{
			((t_room *)(tmp->content))->checked = 1;
			ft_get_all_paths(tmp, lst, e);
		}
		tmp = tmp->next;
	}
}

static void	ft_print_list_content(t_list *lst)
{
	int	i;

	while (lst != NULL)
	{
		i = 0;
		ft_printf("\n\nROOM : '%s'\n", ((t_room *)(lst->content))->name);
		(((t_room *)(lst->content))->start_end == -1) ? ft_printf("END\n") : 0;
		(((t_room *)(lst->content))->start_end == 1) ? ft_printf("START\n") : 0;
		while (((t_room *)(lst->content))->tunnels[i] != NULL)
		{
			ft_printf("LINK[%d] : '%s'\n", i, ((t_room *)(lst->content))->tunnels[i]);
			i++;
		}
		lst = lst->next;
	}
}

static void	ft_link_rooms(char *line, t_list **lst)
{
	t_list	*begin;
	t_list	*tmp;
	char	**split;
	int		count;

	count = 0;
	begin = *lst;
	tmp = *lst;
	split = ft_strsplit(line, '-'); // Free dat shit later
	if (ft_strcmp(split[0], split[1]) == 0)
		ft_error_exit("Room is linked to itself.\n");
	if (ft_split_len(split) > 2)
		ft_error_exit("Bad format for room links.\n");
	while (tmp != NULL)
	{
		if (ft_strcmp(((t_room *)(tmp->content))->name, split[0]) == 0)
		{
			((t_room *)(tmp->content))->tunnels = ft_split_join_free(((t_room *)
				(tmp->content))->tunnels, ft_strdup(split[1]));
			count++;
		}
		else if (ft_strcmp(((t_room *)(tmp->content))->name, split[1]) == 0)
		{
			((t_room *)(tmp->content))->tunnels = ft_split_join_free(((t_room *)
				(tmp->content))->tunnels, ft_strdup(split[0]));
			count++;
		}
		tmp = tmp->next;
	}
	if (count != 2)
		ft_error_exit("Bad room name in room links.\n");
}

static void	ft_get_rooms(int *start_end, char *line, t_list **lst, t_env *e)
{
	t_room	*tmp;
	char	**split;
	
	(void)e;
	tmp = NULL;
	split = ft_strsplit(line, ' '); // Free dat shit later
	if ((tmp = (t_room *)malloc(sizeof(t_room))) == NULL)
		ft_error_exit("Cannot allocate list content\n");
	tmp->name = ft_strdup(split[0]);
	tmp->pos.x = ft_atoi_error_exit(split[1], "INT OVERFLOW. Exit.\n");
	tmp->pos.x = ft_atoi_error_exit(split[2], "INT OVERFLOW. Exit.\n");
	tmp->ants_there = 0;
	tmp->checked = 0;
	if ((tmp->tunnels = (char **)malloc(sizeof(char *))) == NULL)
		ft_error_exit("Cannot allocate tunnels string.\n");
		tmp->tunnels[0] = NULL;
	if (*start_end == 1 || *start_end == -1)
	{
		(*start_end == 1) ? (e->start = ft_strdup(tmp->name)) : 
			(e->end = ft_strdup(tmp->name));
		tmp->start_end = *start_end;
		*start_end = 0;
	}
	ft_lstadd(lst, ft_lstnew((void *)tmp, sizeof(t_room)));
}

static void	ft_parse(t_list **lst, t_env *e)
{
	char	*line;
	int		i;
	int		start_end;

	i = 0;
	start_end = 0;
	while (get_next_line(0, &line))
	{
		if (ft_strncmp(line, "##", 2) == 0)
		{
			if (ft_strcmp(line, "##start") == 0)
				start_end = 1;
			else if (ft_strcmp(line, "##end") == 0)
				start_end = -1;
			else
				ft_error_exit("Bad line starting with \"##\"");
		}
		if (*line != '#')
		{
			if (i == 0)
				e->ants_nb = ft_atoi(line);
			else
			{
				if (ft_strchr(line, ' ') != NULL)
					ft_get_rooms(&start_end, line, lst, e);
				else
					ft_link_rooms(line, lst);
			}
			i++;
		}
	}
}

int main(int argc, char **argv)
{
	t_list	*lst;
	t_env	env;

	(void)argc;
	(void)argv;
	if ((lst = (t_list *)malloc(sizeof(t_list))) == NULL)
		return (0); //Should print error there.
	lst = NULL;
	ft_parse(&lst, &env);
	ft_print_list_content(lst);
	ft_print_all_paths(&lst, &env);
	return (0);
}
