#include <kernel/task.h>
#include <shell.h>
#include <stdio.h>

static void task_info_cmd(void)
{
    task_t t;
    printf("task info>>>\n");

    printf("%-16s%-6s%-6s%-6s\n", "name", "pid", "pri", "state");
    for(int i = 0; i < G_TASK_NUMBER; i++)
    {
        t = &g_task[i];

        if(t->task_state != TASK_STATE_NONE)
        {
            printf("%-16s%-6d%-6d%-6d\n", t->task_name, t->pid, t->priority, t->task_state);
        }
    }
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
ps, task_info_cmd, show task info);