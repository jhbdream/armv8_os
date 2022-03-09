/*
 * Copyright (C) 2002 ARM Limited, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GIC_H__
#define __GIC_H__
#include <ee_stdint.h>

int gic_init(void);
void irq_mask(uint32_t hwirq);
void irq_unmask(uint32_t hwirq);
uint32_t irq_read_iar(void);
void irq_eoi(uint32_t hwirq);

#endif /* __GIC_H__ */
