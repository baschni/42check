
#include <stddef.h>

#include "e_op.h"
#include "s_ring.h"
#include "ring_helper.h"
#include "limits.h"

int sum_combined_moves(int move_a, int move_b)
{
	if ((move_a < 0) != (move_b < 0))
		return (ft_abs(move_a) + ft_abs (move_b));
	else
		return (ft_abs(ft_max(move_a, move_b)));
}

void find_best_moves_a_b(t_ring *needle, int *move_a_up, int *move_a_down)
{
	int move_b_up;
	int move_b_down;

	int move_both_up;
	int move_both_down;
	int move_up_down;

	find_moves_up_down(&move_b_up, &move_b_down, needle);
	move_both_up = sum_combined_moves(*move_a_up, move_b_up);
	move_both_down = sum_combined_moves(*move_a_down, move_b_down);
	move_up_down = sum_combined_moves(*move_a_up, move_b_down);
	if (sum_combined_moves(*move_a_down, move_b_up) < move_both_up && \
	sum_combined_moves(*move_a_down, move_b_up) < move_both_down && \
	sum_combined_moves(*move_a_down, move_b_up) < move_up_down)
	{
		*move_a_up = *move_a_down;
		*move_a_down = move_b_up;
	}
	else if (move_both_up < move_both_down && move_both_up < move_up_down)
	{
		*move_a_down = move_b_up;
	}
	else if (move_up_down < move_both_down)
	{
		*move_a_down = move_b_down;
	}
	else
	{
		*move_a_up = *move_a_down;
		*move_a_down = move_b_down;
	}
}

void find_best_moves_turk(t_ring *stack_a, t_ring *stack_b, int *move_a, int *move_b)
{
	t_ring *i_a;
	t_ring *i_b;
	t_ring *copy_start;
	int move_a_up;
	int move_a_down;

	i_a = ring_start(stack_a);
	i_b = ring_copy(stack_b);
	i_b = ring_sort(i_b, 1);
	copy_start = i_b;
	while (i_a)
	{	
		while (i_b)
		{
			if (i_a->val > i_b->val)
				break;
			i_b = i_b->next;
		}
		if (!i_b)
			i_b = ring_max(copy_start);
		find_moves_up_down(&move_a_up, &move_a_down, i_a);
		find_best_moves_a_b(ring_find(i_b->val, stack_b), &move_a_up, &move_a_down);
		if (sum_combined_moves(move_a_up, move_a_down) < sum_combined_moves(*move_a, *move_b))
		{
			*move_a = move_a_up;
			*move_b = move_a_down;
		}		
		i_a = i_a->next;
	}
	free_ring(copy_start);
}

t_ops *execute_moves_concurrently(t_ring **stack_a, t_ring **stack_b, int move_a, int move_b)
{
	t_ring *ops;
	int instruction_a;
	int instruction_b;
	int sign_a;
	int sign_b;

	ops = NULL;
	sign_a = -1;
	sign_b = 1;
	instruction_a = RA;
	instruction_b = RB;
	if (move_a < 0)
	{
		instruction_a = RRA;
		sign_a = -1;
	}
	if (move_b < 0)
	{
		instruction_b = RRB;
		sign_b = -1;
	}

	move_a = ft_abs(move_a);
	move_b = ft_abs(move_b);

	while (move_a || move_b)
	{
		if (instruction_a + 1 == instruction_b && move_a && move_b)
		{
			ops = new_ring(instruction_a + 2, ops);
			if(instruction_a == RA)
			{
				ring_shift_up(*stack_a);
				ring_shift_up(*stack_b);
			}
			else
			{
				ring_shift_down(*stack_a);
				ring_shift_down(*stack_b);
			}
			move_a--;
			move_b--;
		}
		else
		{
			ops = apply_n_moves(move_a * sign_a, stack_a, ops, 0);
			move_a = 0;
			ops = apply_n_moves(move_b * sign_b, stack_b, ops, 1);
			move_b = 0;
		}
	}
	return (ops);
}

t_ring	*push_to_b_sorted(t_ring **stack_a, t_ring **stack_b, t_ops *ops)
{
	t_ops	*temp_ops;
	int		move_a;
	int		move_b;

	while (*stack_a)
	{
		move_a = INT_MAX;
		move_b = 0;
		find_best_moves_turk(*stack_a, *stack_b, &move_a, &move_b);
		temp_ops = execute_moves_concurrently(stack_a, stack_b, move_a, move_b);
		ops = ring_append(&ops, temp_ops);
		ops = new_ring(PB, ops);
		ring_push_to_front(stack_a, stack_b);
	}
	return (ops);
}
