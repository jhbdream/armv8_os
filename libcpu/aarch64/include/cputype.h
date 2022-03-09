/*
 * Copyright (C) 2012 ARM Ltd.
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
#ifndef __ASM_CPUTYPE_H
#define __ASM_CPUTYPE_H

#include <const.h>

#define INVALID_HWID		ULONG_MAX

#define MPIDR_UP_BITMASK	(0x1 << 30)
#define MPIDR_MT_BITMASK	(0x1 << 24)
#define MPIDR_HWID_BITMASK	UL(0xff00ffffff)

#define MPIDR_LEVEL_BITS_SHIFT	3
#define MPIDR_LEVEL_BITS	(1 << MPIDR_LEVEL_BITS_SHIFT)
#define MPIDR_LEVEL_MASK	((1 << MPIDR_LEVEL_BITS) - 1)

#define MPIDR_LEVEL_SHIFT(level) \
	(((1 << level) >> 1) << MPIDR_LEVEL_BITS_SHIFT)

#define MPIDR_AFFINITY_LEVEL(mpidr, level) \
	((mpidr >> MPIDR_LEVEL_SHIFT(level)) & MPIDR_LEVEL_MASK)

#define MIDR_REVISION_MASK	0xf
#define MIDR_REVISION(midr)	((midr) & MIDR_REVISION_MASK)
#define MIDR_PARTNUM_SHIFT	4
#define MIDR_PARTNUM_MASK	(0xfff << MIDR_PARTNUM_SHIFT)
#define MIDR_PARTNUM(midr)	\
	(((midr) & MIDR_PARTNUM_MASK) >> MIDR_PARTNUM_SHIFT)
#define MIDR_ARCHITECTURE_SHIFT	16
#define MIDR_ARCHITECTURE_MASK	(0xf << MIDR_ARCHITECTURE_SHIFT)
#define MIDR_ARCHITECTURE(midr)	\
	(((midr) & MIDR_ARCHITECTURE_MASK) >> MIDR_ARCHITECTURE_SHIFT)
#define MIDR_VARIANT_SHIFT	20
#define MIDR_VARIANT_MASK	(0xf << MIDR_VARIANT_SHIFT)
#define MIDR_VARIANT(midr)	\
	(((midr) & MIDR_VARIANT_MASK) >> MIDR_VARIANT_SHIFT)
#define MIDR_IMPLEMENTOR_SHIFT	24
#define MIDR_IMPLEMENTOR_MASK	(0xff << MIDR_IMPLEMENTOR_SHIFT)
#define MIDR_IMPLEMENTOR(midr)	\
	(((midr) & MIDR_IMPLEMENTOR_MASK) >> MIDR_IMPLEMENTOR_SHIFT)

#define MIDR_CPU_MODEL(imp, partnum) \
	(((imp)			<< MIDR_IMPLEMENTOR_SHIFT) | \
	(0xf			<< MIDR_ARCHITECTURE_SHIFT) | \
	((partnum)		<< MIDR_PARTNUM_SHIFT))

#define MIDR_CPU_VAR_REV(var, rev) \
	(((var)	<< MIDR_VARIANT_SHIFT) | (rev))

#define MIDR_CPU_MODEL_MASK (MIDR_IMPLEMENTOR_MASK | MIDR_PARTNUM_MASK | \
			     MIDR_ARCHITECTURE_MASK)

#ifndef __ASSEMBLY__

#include <sysreg.h>
#include <type.h>
#include <compiler_types.h>
#include <compiler_attribute.h>

#define read_cpuid(reg)			read_sysreg_s(SYS_ ## reg)

/*
 * Represent a range of MIDR values for a given CPU model and a
 * range of variant/revision values.
 *
 * @model	- CPU model as defined by MIDR_CPU_MODEL
 * @rv_min	- Minimum value for the revision/variant as defined by
 *		  MIDR_CPU_VAR_REV
 * @rv_max	- Maximum value for the variant/revision for the range.
 */
struct midr_range {
	u32 model;
	u32 rv_min;
	u32 rv_max;
};

#define MIDR_RANGE(m, v_min, r_min, v_max, r_max)		\
	{							\
		.model = m,					\
		.rv_min = MIDR_CPU_VAR_REV(v_min, r_min),	\
		.rv_max = MIDR_CPU_VAR_REV(v_max, r_max),	\
	}

#define MIDR_ALL_VERSIONS(m) MIDR_RANGE(m, 0, 0, 0xf, 0xf)

static inline bool midr_is_cpu_model_range(u32 midr, u32 model, u32 rv_min,
					   u32 rv_max)
{
	u32 _model = midr & MIDR_CPU_MODEL_MASK;
	u32 rv = midr & (MIDR_REVISION_MASK | MIDR_VARIANT_MASK);

	return _model == model && rv >= rv_min && rv <= rv_max;
}

static inline bool is_midr_in_range(u32 midr, struct midr_range const *range)
{
	return midr_is_cpu_model_range(midr, range->model,
				       range->rv_min, range->rv_max);
}

static inline bool
is_midr_in_range_list(u32 midr, struct midr_range const *ranges)
{
	while (ranges->model)
		if (is_midr_in_range(midr, ranges++))
			return true;
	return false;
}

/*
 * The CPU ID never changes at run time, so we might as well tell the
 * compiler that it's constant.  Use this function to read the CPU ID
 * rather than directly reading processor_id or read_cpuid() directly.
 */
static inline u32 __attribute_const__ read_cpuid_id(void)
{
	return read_cpuid(MIDR_EL1);
}

static inline u64 __attribute_const__ read_cpuid_mpidr(void)
{
	return read_cpuid(MPIDR_EL1);
}

static inline unsigned int __attribute_const__ read_cpuid_implementor(void)
{
	return MIDR_IMPLEMENTOR(read_cpuid_id());
}

static inline unsigned int __attribute_const__ read_cpuid_part_number(void)
{
	return MIDR_PARTNUM(read_cpuid_id());
}

static inline u32 __attribute_const__ read_cpuid_cachetype(void)
{
	return read_cpuid(CTR_EL0);
}
#endif /* __ASSEMBLY__ */

#endif
