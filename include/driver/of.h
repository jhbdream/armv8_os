#ifndef __DRIVER_OF_H__
#define __DRIVER_OF_H__

int early_init_dt_scan_memory(unsigned long node, const char *uname,
				     int depth, void *data);

int of_fdt_scan(int (*it)(unsigned long node, const char *uname,
					int depth, void *data),
					void *data);

#endif
