/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ring_helper.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: baschnit <baschnit@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 15:53:08 by baschnit          #+#    #+#             */
/*   Updated: 2024/10/27 02:52:06 by baschnit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RING_HELPER_H
# define RING_HELPER_H

# include <stddef.h>

# include "s_ring.h"

unsigned int	ft_abs(int i);
int				ft_max(int a, int b);

t_ring			*new_ring(int val, t_ring *prev);
int				ring_len(t_ring *start);
t_ring			*ring_start(t_ring *somewhere);
t_ring			*ring_copy(t_ring *somewhere);
t_ring			*free_ring(t_ring *start);

int				ring_index_front(t_ring *somewhere);
int				ring_index_back(t_ring *somewhere);
t_ring			*ring_from_index(int index, t_ring *somewhere);
t_ring			*ring_min(t_ring *somewhere);
t_ring			*ring_max(t_ring *somewhere);
int				ring_is_sorted(t_ring *somewhere, char reversed);
int				ring_shift_sorted(t_ring *stack_a, char reversed);

t_ring			*ring_end(t_ring *somewhere);
void			ring_push_to_front(t_ring **src, t_ring **target);
t_ring			*ring_swap_first_two_elems(t_ring *somewhere);
t_ring			*ring_shift_down(t_ring *somewhere);
t_ring			*ring_shift_up(t_ring *somewhere);

t_ring			*ring_append(t_ring **target, t_ring *appendix);
t_ring			*ring_find(int needle, t_ring *heap);
t_ring			*ring_remove(t_ring *item);
int				shortest_path_to_front(t_ring *needle);

t_ring			*apply_n_moves(int move, t_ring **stack_a, t_ring *ops, char stack_b_or_a);
t_ring			*move_min_or_max_to_front(t_ring **stack_a, t_ring *ops, \
char max_to_front, char stack_b_or_a);
t_ring			*ring_sort(t_ring *start, char reversed);

void			find_moves_up_down(int *up, int *down, t_ring *needle);
t_ring			*rotate_min_max_to_front(t_ring **stack, t_ring *ops, \
char stack_b_or_a, char max_not_min_to_front);
// t_ring	*bring_min_up(t_ring *min, t_ring *stack_a, t_ring *ops);

#endif