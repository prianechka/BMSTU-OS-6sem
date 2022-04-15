#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init_task.h>

// <<<<<<< STATES >>>>>>>
//#define TASK_RUNNING            0
//#define TASK_INTERRUPTIBLE      1
//#define TASK_UNINTERRUPTIBLE    2
//#define __TASK_STOPPED          4
//#define __TASK_TRACED           8
//#define EXIT_DEAD               16
//#define EXIT_ZOMBIE             32
//#define EXIT_TRACE              (EXIT_ZOMBIE | EXIT_DEAD)
//#define TASK_DEAD               64
//#define TASK_WAKEKILL           128
//#define TASK_WAKING             256
//#define TASK_PARKED             512
//#define TASK_NOLOAD             1024
//#define TASK_STATE_MAX          2048


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexander Prianishnikov");


static int __init init_md(void)
{
    struct task_struct *task = &init_task;

    char* prompt = KERN_INFO"+\tpid=%d\t%15s\t\tstate=%4lu\t\tprio=%d\tppid=%5d\tpname=%s\n";

    do {
        printk(prompt,
                task->pid, task->comm,
                task->state, task->prio,
                task->parent->pid, task->parent->comm);

    } while ((task = next_task(task)) != &init_task);

    printk(KERN_INFO"+ <<< CURRENT >>>");
    printk(KERN_INFO"+\tpid=%d\t%15s\t\tstate=%4lu\t\tprio=%d\tstatic_prio=%d\tnormal_prio=%d\tpriority=%u\tppid=%5d\tpname=%s\n",
            current->pid, current->comm,
            current->state, current->prio, current->static_prio, current->normal_prio, current->rt_priority,
            current->parent->pid, current->parent->comm);

    return 0;
}

static void __exit exit_md(void) {
    printk(KERN_INFO
    "+ module unloaded!\n");

    printk("Goodbye!\n");


    printk(KERN_INFO"+ <<< CURRENT >>>");
    printk(KERN_INFO"+\tpid=%d\t%15s\t\tstate=%4lu\t\tprio=%d\tppid=%5d\tpname=%s\n",
           current->pid, current->comm,
           current->state, current->prio,
           current->parent->pid, current->parent->comm);
}

module_init(init_md);
module_exit(exit_md);