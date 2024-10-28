/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   concurrent_sort.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: baschnit <baschnit@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 00:27:59 by baschnit          #+#    #+#             */
/*   Updated: 2024/10/27 02:52:05 by baschnit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONCURRENT_SORT_H
# define CONCURRENT_SORT_H

void	sort_concurrently(t_ring **stack_a, t_ring **stack_b, t_ring **ops);
void	sort_adverse_concurrently(t_ring **stack_a, t_ring **stack_b, t_ring **ops);
void	join_ops(t_ring **ops_a, t_ring **ops_b, t_ring **ops, int reversed);

#endif