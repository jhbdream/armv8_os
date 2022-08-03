#include "printk.h"
#include <libfdt.h>

extern unsigned char _binary_arch_riscv64_dts_riscv_qemu_dtb_start[];
#define _FDT_INIT_BLOB_ADDR (_binary_arch_riscv64_dts_riscv_qemu_dtb_start)
#define FDT_ROOT_BLOB (void *)_FDT_INIT_BLOB_ADDR

void *initial_boot_params = FDT_ROOT_BLOB;

/*
 * of_get_flat_dt_root - find the root node in the flat blob
 */
unsigned long of_get_flat_dt_root(void)
{
	return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brife  get prop
 *
 * @Param node
 * @Param name
 * @Param size
 *
 * @Returns
 */
/* ----------------------------------------------------------------------------*/
const void *of_get_flat_dt_prop(unsigned long node, const char *name, int *size)
{
	return fdt_getprop(FDT_ROOT_BLOB, node, name, size);
}

/**
 * of_fdt_is_compatible - Return true if given node from the given blob has
 * compat in its compatible list @blob: A device tree blob
 * @node: node to test
 * @compat: compatible string to compare with compatible list.
 *
 * Return: a non-zero value on match with smaller values returned for more
 * specific compatible values.
 */
static int of_fdt_is_compatible(const void *blob,
		      unsigned long node, const char *compat)
{
	const char *cp;
	int cplen;
	unsigned long l, score = 0;

	cp = fdt_getprop(blob, node, "compatible", &cplen);
	if (cp == NULL)
		return 0;
	while (cplen > 0) {
		score++;
		if (strncmp(cp, compat, strlen(compat)) == 0)
			return score;
		l = strlen(cp) + 1;
		cp += l;
		cplen -= l;
	}

	return 0;
}

/**
 * of_flat_dt_is_compatible - Return true if given node has compat in compatible list
 * @node: node to test
 * @compat: compatible string to compare with compatible list.
 */
int of_flat_dt_is_compatible(unsigned long node, const char *compat)
{
	return of_fdt_is_compatible(initial_boot_params, node, compat);
}

/*
 * of_get_flat_dt_phandle - Given a node in the flat blob, return the phandle
 */
uint32_t of_get_flat_dt_phandle(unsigned long node)
{
	return fdt_get_phandle(initial_boot_params, node);
}

const char * of_flat_dt_get_machine_name(void)
{
	const char *name;
	unsigned long dt_root = of_get_flat_dt_root();

	name = of_get_flat_dt_prop(dt_root, "model", NULL);
	if (!name)
		name = of_get_flat_dt_prop(dt_root, "compatible", NULL);
	return name;
}

#if 0
/*
 * early_init_dt_scan_memory - Look for and parse memory nodes
 */
int early_init_dt_scan_memory(unsigned long node, const char *uname,
				     int depth, void *data)
{
	/*
		memory@80000000 {
			device_type = "memory";
			reg = <0x0 0x80000000 0x2 0x00000000>;
		};
	*/
	const char *type = of_get_flat_dt_prop(node, "device_type", NULL);
	const __be32 *reg, *endp;
	int l;
	bool hotpluggable;

	/* We are scanning "memory" nodes only */
	if (type == NULL || strcmp(type, "memory") != 0)
		return 0;

	reg = of_get_flat_dt_prop(node, "reg", &l);
	if (reg == NULL)
		return 0;

	endp = reg + (l / sizeof(__be32));

	pr_debug("memory scan node %s, reg size %d,\n", uname, l);

	while ((endp - reg) >= (dt_root_addr_cells + dt_root_size_cells)) {
		u64 base, size;

		base = dt_mem_next_cell(dt_root_addr_cells, &reg);
		size = dt_mem_next_cell(dt_root_size_cells, &reg);

		if (size == 0)
			continue;
		printk(" - %llx, %llx\n", base, size);

		//early_init_dt_add_memory_arch(base, size);
	}

	return 0;
}
#endif

int of_fdt_scan(int (*it)(unsigned long node, const char *uname,

					int depth, void *data),
					void *data)
{
	const void *blob = _FDT_INIT_BLOB_ADDR;
	const char *pathp;
	int offset, rc = 0, depth = -1;

	if(!blob)
		return 0;

	for(offset = fdt_next_node(blob, -1, &depth);
		offset >= 0 && depth >= 0 && !rc;
		offset = fdt_next_node(blob, offset, &depth))
	{
		pathp = fdt_get_name(blob, offset, NULL);
		if(it)
			rc = it(offset, pathp, depth, data);
	}

	return rc;
}

int fdt_iterator(unsigned long node, const char *uname,
					int depth, void *data)
{
	for(int i = 0; i < depth; i++)
	{
		printk("\t");
	}

	printk("node: %-4d ", node);
	printk("name: [%-s] ", uname);
	printk("depth: %-4d", depth);

	const char *prop;
	int size;
	unsigned long dt_root;

	prop = of_get_flat_dt_prop(node, "compatible", &size);
	if (prop) {
		printk("compatible: ");
		while (size > 0) {
			printk("[\"%s\"] ", prop);
			size -= strlen(prop) + 1;
			prop += strlen(prop) + 1;
		}

	}

	printk("\n");
	return 0;
}

void fdt_test(void)
{
	of_fdt_scan(fdt_iterator, NULL);
}