/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_sort.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: baschnit <baschnit@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 23:46:56 by baschnit          #+#    #+#             */
/*   Updated: 2024/10/25 17:14:26 by baschnit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

#include "e_op.h"
#include "s_ring.h"
#include "ring_helper.h"
#include "concurrent_sort.h"

// void optimize_ops(t_ring *ops)
// {
// 	t_ring *start;

// 	ops = ring_start(ops);
// 	start = ops;
// 	while (ops->next)
// 	{
// 		if ((ops->val == RA && ops->next->val == RB) || (ops->val == RB && ops->next->val == RA))
// 		{
// 			ops->val = RR;
// 			remove_ring(ops->next);
// 		}
// 		else if((ops->val == RRA && ops->next->val == RRB) || (ops->val == RRB && ops->next->val == RRA))
// 		{
// 			ops->val = RRR;
// 			remove_ring(ops->next);
// 		}
// 		else if((ops->val == SA && ops->next->val == SB) || (ops->val == SB && ops->next->val == SA))
// 		{
// 			ops->val = SS;
// 			remove_ring(ops->next);
// 		}
// 		ops = ops->next;
// 	}
// 	return (start);
// }

// void	push_back_to_a(**stack_a, **stack_b, t_ring *ops)
// {
// 	while (*stack_b)
// 	{
// 		ring_push_to_front(stack_b, stack_a);
// 		ops = new_ring(PA, ops);
// 	}
// }

// t_ring	*double_sort(t_ring *stack_a)
// {
// 	t_ring *stack_a2;
// 	t_ring	*stack_b;
// 	t_ring	*stack_b2;
// 	t_ring	*ops1;
// 	t_ring	*ops2;

// 	stack_b = NULL;
// 	stack_b2 = NULL;
// 	stack_a2 = ring_copy(stack_a);
// 	ops1 = push_half_to_b(&stack_a, &stack_b, 0);
// 	ops2 = push_half_to_b(&stack_a2, &stack_b2, 1);
// 	sort_concurrently(&stack_a, &stack_b, &ops1);
// 	sort_concurrently(&stack_a2, &stack_b2, &ops2);
// 	push_back_to_a(&stack_a, &stack_b, &ops1);
// 	push_back_to_a(&stack_a2, &stack_b2, &ops2);
// 	free_ring(stack_a);
// 	free_ring(stack_a2);
// 	if(ring_len(ops2) < ring_len(ops1))
// 	{
// 		stack_a = ops1;
// 		ops1 = ops2;
// 		ops2 = stack_a
// 	}
// 	free(ops2)
// 	return (ops1);
// }

int	find_next_ring(t_ring *stack_a, t_ring *sorted)
{
	int		up;
	int		down;
	t_ring	*start;
	t_ring	*start_sorted;

	start = ring_start(stack_a);
	start_sorted = ring_start(sorted);
	up = 0;
	down = 1;
	while (stack_a)
	{
		while (sorted)
		{
			if (sorted->val == stack_a->val)
				break ;
			sorted = sorted->next;
		}
		if (sorted != NULL)
			break ;
		sorted = start_sorted;
		up++;
		stack_a = stack_a->next;
	}
	stack_a = ring_end(start);
	while (stack_a)
	{
		while (sorted)
		{
			if (sorted->val == stack_a->val)
				break ;
			sorted = sorted->next;
		}
		if (sorted != NULL)
			break ;
		sorted = start_sorted;
		down++;
		stack_a = stack_a->previous;
	}
	if (down < up)
		up = -down;
	return (up);
}

/**
 * @brief Push half of items on stack_a to stack_b Half of the items (either the
 *  smallest or the largest items on stack_a) are pushed to stack_b while
 * checking before each push if a rotation brings more order to one of the
 * stacks.
 * @param stack_a
 * @param stack_b
 * @return t_list*
*/

/**
 * @brief push the lower half of the numbers to b, keep the greater half on a
 * 
 * The idea is to divide a stack like 346152 into the stacks 546 / 321 (a and b
 * respectively. So the greater half on the numbers on the left and the inferior
 * half on the right.
 * 
 * TODO: while shifting and pushing from left to right: if swapping is
 * opportune, do it (a or b or both)
 * 
 * @param stack_a
 * @param stack_b
 * @param high_or_low
 * @return t_list*
*/
t_ring	*push_half_to_b(t_ring **stack_a, t_ring **stack_b)
{
	t_ring	*sorted;
	t_ops	*ops;
	int		i;
	int		move;

	ops = NULL;
	sorted = ring_copy(*stack_a);
	ring_sort(sorted, 0);
	i = 0;
	move = ring_len(sorted);
	while (i < move)
	{
		if (i >= move / 2)
			sorted = ring_remove(sorted);
		else
			sorted = sorted->next;
		i++;
	}
	sorted = ring_start(sorted);
	while (sorted)
	{
		move = find_next_ring(*stack_a, sorted);
		ops = apply_n_moves(move, stack_a, ops, 0);
		//ring_append(&ops, apply_n_moves(move, stack_a, ops));
		sorted = ring_start(ring_remove(ring_find((*stack_a)->val, sorted)));
		ring_push_to_front(stack_a, stack_b);
		ops = new_ring(PB, ops);
	}
	return (ops);
}

void	find_moves_up_down(int *up, int *down, t_ring *needle)
{
	if (needle == ring_start(needle))
	{
		*up = 0;
		*down = 0;
	}
	else
	{
		*up = ring_index_front(needle);
		*down = -1 * (ring_index_back(needle) + 1);
	}
}

void	bring_min_max_up_concurrently(t_ring **stack_a, t_ring **stack_b, t_ops **ops)
{
	int		stack_a_up;
	int		stack_a_down;
	int		stack_b_up;
	int		stack_b_down;
	t_ring	*ops_a;
	t_ring	*ops_b;

	ops_a = NULL;
	ops_b = NULL;
	find_moves_up_down(&stack_a_up, &stack_a_down, ring_min(*stack_a));
	find_moves_up_down(&stack_b_up, &stack_b_down, ring_max(*stack_b));
	if (ft_abs(stack_a_up - stack_b_up) + stack_a_up < ft_abs(stack_a_down - stack_b_down) + stack_a_down)
	{
		ops_a = apply_n_moves(stack_a_up, stack_a, ops_a, 0);
		ops_b = apply_n_moves(stack_b_up, stack_b, ops_b, 1);
		join_ops(&ops_a, &ops_b, ops, 0);
		join_ops(&ops_a, &ops_b, ops, 0);
	}
	else
	{
		ops_a = apply_n_moves(stack_a_down, stack_a, ops_a, 0);
		ops_b = apply_n_moves(stack_b_down, stack_b, ops_b, 1);
		join_ops(&ops_a, &ops_b, ops, 1);
		join_ops(&ops_a, &ops_b, ops, 1);
	}
}

static t_ring	*push_back_to_a(t_ring **stack_a, t_ring **stack_b, t_ring *ops)
{
	while (*stack_b)
	{
		ring_push_to_front(stack_b, stack_a);
		ops = new_ring(PA, ops);
	}
	return (ops);
}

/**
 * @brief sort the items on stack_a using stack_b
 * 
 * The idea of double sort is to separate all items into two groups in stack_a
 * and stack_b (push_half_to_b) which are then put in order concurrently
 * (stack_a sorted increasingly, stack_b sorted decreasingly) and finally
 * joined, in order, on stack_a (push_back_to_a). There are to options to
 * proceed: either push the smallest elements of stack_a to stack_b or the
 * largest elements. Both variants are tested (op1 and op2 respectively) and are
 * compared. The variant with the smallest number of oper- ations is returend.
 * 
 * @param stack_a stack configuration to bring in order
 * @return t_ring* list of operations to sort stack_a using stack_b
*/
#include <stdio.h>
t_ring	*double_sort(t_ring *stack_a)
{
	t_ring	*stack_b;
	t_ops	*ops;
	int		nu_ops;
	int		debug;

	debug = 0;

	stack_b = NULL;

	if (debug)
	printf("len a %i ", ring_len(stack_a));
	ops = push_half_to_b(&stack_a, &stack_b);
	nu_ops = ring_len(ops);
	if (debug)
		printf("push b %i ", nu_ops);
	sort_adverse_concurrently(&stack_a, &stack_b, &ops);
	if (debug)
		printf("sort %i ", ring_len(ops) - nu_ops);
	nu_ops = ring_len(ops);
	bring_min_max_up_concurrently(&stack_a, &stack_b, &ops);
	if (debug)
		printf("bring min up %i ", ring_len(ops) - nu_ops);
	nu_ops = ring_len(ops);
	ops = push_back_to_a(&stack_a, &stack_b, ops);
	if (debug)
	{
		printf("push back %i ", ring_len(ops) - nu_ops);
		printf("sum %i\n", ring_len(ops));
	}
	// free_ring(stack_a);
	// free_ring(stack_b);
	//return (optimize_ops(ops));
	return (ring_start(ops));
}

// todo swap elements on both stacks if opportune or do a synchronised rotate
// t_ring *apply_n_moves(int move, t_ring *sorted, t_ring **stack_a, t_ring **stack_b)
// {
// 	t_ring	*ops;
// 	int		instruction;

// 	ops = (NULL);
// 	if (move < 0)
// 		instruction = RRA;
// 	else
// 		instruction = RA;
// 	move = ft_abs(move);
// 	while (move > 0)
// 	{
// 		swap_a_or_b(ops, sorted, stack_a, stack_b);
// 		if (instruction == RA)
// 			*stack_a = ring_shift_up(*stack_a);
// 		else
// 			*stack_a = ring_shift_down(*stack_a);
// 		check_instruction(instruction, ops, stack_b);

// 		ops = new_ring(instruction, ops);
// 		move--;
// 	}
// 	return (ops);
// }

// void check_instruction(int instruction, t_ring *ops, t_ring *stack_b)
// {

// }

// void swap_a_or_b(t_ring **ops, t_ring *sorted, t_ring **stack_a, t_ring **stack_b)
// {
// 	if ((*stack_a)->next)
// 	{
// 		if(!ring_find((*stack_a)->val, sorted) && !ring_find((*stack_a)->next->val, sorted))
// 			if ((*stack_a)->val > (*stack_a)->next->val)
// 			{
// 				*stack_a = ring_swap_first_to_elem(*stack_a);
// 				*ops = new_ring(SA, ops);
// 			}
// 	}
// 	if ((*stack_b)->next)
// 	{
// 		if ((*stack_b)->val < (*stack_b)->next->val)
// 		{
// 				*stack_b = ring_swap_first_to_elem(*stack_b);
// 				*ops = new_ring(SB, ops);
// 		}
// 	}
// }