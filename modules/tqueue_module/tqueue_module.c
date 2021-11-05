#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leonid System");
MODULE_DESCRIPTION("Custom task queue");
MODULE_VERSION("1.0");

struct custom_task {
	struct work_struct work;
	int data;
};

spinlock_t wq_spin;

static void task_message(struct work_struct *arg) {
	spin_lock(&wq_spin);
	struct custom_task *task = container_of(arg, struct custom_task, work);
	printk(KERN_INFO "task_message[%d]", task->data);
	kfree(task);
	spin_unlock(&wq_spin);
}

static int __init tqueue_init_module(void) {
	spin_lock_init(&wq_spin);
	struct custom_task *tasks[3];
	
	for (int index = 0; index < 3; ++index) {
		tasks[index] = (struct custom_task*)kmalloc(sizeof(struct custom_task), GFP_ATOMIC);
		INIT_WORK(&(tasks[index]->work), task_message);
		tasks[index]->data = index;
		schedule_work(&(tasks[index]->work));
	}
	
	return 0;
}

static void __exit tqueue_exit_module(void) {}

module_init(tqueue_init_module);
module_exit(tqueue_exit_module);
