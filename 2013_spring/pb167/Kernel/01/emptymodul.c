#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/syscalls.h>
#include <asm/asm-offsets.h>
#include <linux/sched.h>


int init_module(void);
void cleanup_module(void);


void print_task(struct task_struct *task)
{
	pr_info("** printing task %d\n", task->pid);
	pr_info("name:\t%s\n", task->comm);

	pr_info("state:\t%ld\n", task->state);

}

void print_tasks(void)
{
	struct task_struct *t;

	t = &init_task;
	do {
		print_task(t);
		t = next_task(t);
	} while (t && t->pid != 0);
}

int init_module(void)
{
	printk(KERN_INFO "Hello world in kernel space.\n");


	print_tasks();
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Unloading module.\n");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Empty module");
