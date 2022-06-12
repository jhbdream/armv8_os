
    .global idmap_cpu_replace_ttbr1
    .type idmap_cpu_replace_ttbr1, "function"
idmap_cpu_replace_ttbr1:
    msr ttbr1_el1, x0
    isb
    dsb nsh
    ret