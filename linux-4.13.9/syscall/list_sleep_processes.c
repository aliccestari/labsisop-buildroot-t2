#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/syscalls.h>

asmlinkage long sys_list_sleep_processes(char *buf, int size) {
    struct task_struct *task;
    char kbuf[256];
    int bufsz = 0;
    int ret;

    // Iterar sobre todos os processos
    for_each_process(task) {
        if (task->state == TASK_INTERRUPTIBLE || task->state == TASK_UNINTERRUPTIBLE) {
            // Se o processo estiver em sleep, coletar informações sobre ele
            snprintf(kbuf, sizeof(kbuf), "PID: %d, Name: %s, State: %ld\n",
                     task->pid, task->comm, task->state);
            bufsz = strlen(kbuf) + 1;

            // Verificar se o buffer do usuário é grande o suficiente
            if (bufsz > size) {
                return -EINVAL; // Buffer too small
            }

            // Copiar as informações para o buffer do usuário
            ret = copy_to_user(buf, kbuf, bufsz);
            if (ret != 0) {
                return -EFAULT; // Error copying data to user space
            }
        }
    }

    return 0;
}
