    .global task_switch_to
    .type task_switch_to, "function"

task_switch_to:
    ldr x1, [x0]
    mov x30, x1

    add x0, x0, 4
    ldr x1, [x0]

    mov sp, x1
    ret

    .global task_switch_from_to
    .type task_switch_from_to, "function"

task_switch_from_to: