#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rculist.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
// #include <linux/delay.h>

#define BUFLENGTH 255

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leonid System");
MODULE_DESCRIPTION("Test RCU");
MODULE_VERSION("1.0");

struct msg_list {
	int data;
	struct list_head list;
	struct rcu_head rcu;
	struct msg_list *next;
};

spinlock_t rcu_spin;
static struct msg_list mlist;
static struct task_struct *reader[3];
static struct task_struct *writer;


static int task_read(void *arg) {
	rcu_read_lock();
	
	struct msg_list *pos;
	list_for_each_entry_rcu(pos, &mlist.list, list) {
		spin_lock(&rcu_spin);
		printk(KERN_INFO "reader: %d", pos->data);
		spin_unlock(&rcu_spin);
	}
	
	rcu_read_unlock();
	
	while (!kthread_should_stop())
		schedule();
	
	return 0;
}

static int task_write(void *arg) {
	struct msg_list *pos, *new_pos;
	
	spin_lock(&rcu_spin);
	printk(KERN_INFO "writer");
	spin_unlock(&rcu_spin);
	
	list_for_each_entry(pos, &mlist.list, list) {
		new_pos = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_ATOMIC);
		*new_pos = *pos;
		new_pos->data = 1;
		list_replace_rcu(&pos->list, &new_pos->list);
		synchronize_rcu();
		kfree(pos);
	}
	
	while (!kthread_should_stop())
		schedule();
	
	return 0;
}

static int __init rcu_init_module(void) {
	struct msg_list *tmp;
	INIT_LIST_HEAD_RCU(&mlist.list);
	
	spin_lock_init(&rcu_spin);
	
	for (int index = 0; index < 10; ++index) {
		tmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_ATOMIC);
		INIT_LIST_HEAD_RCU(&tmp->list);
		
		list_add_rcu(&tmp->list, &mlist.list);
	}
	
	reader[0] = kthread_run(task_read, NULL, "task_read");
	writer = kthread_run(task_write, NULL, "task_write");
	reader[1] = kthread_run(task_read, NULL, "task_read");
	reader[2] = kthread_run(task_read, NULL, "task_read");
	
	return 0;
}

static void __exit rcu_exit_module(void) {
	if (!kthread_stop(writer)) {
		spin_lock(&rcu_spin);
		printk(KERN_INFO "Writer stopped");
		spin_unlock(&rcu_spin);
		
	}

	for (int index = 0; index < 3; ++index)
		if (!kthread_stop(reader[index])) {
			spin_lock(&rcu_spin);
			printk(KERN_INFO "Reader[%d] stopped", index);
			spin_unlock(&rcu_spin);			
		}
	
	struct list_head *pos, *pos2;
	struct msg_list *tmp;
	
	list_for_each_safe(pos, pos2, &mlist.list) {
		tmp = list_entry(pos, struct msg_list, list);
		list_del(pos);
		kfree(tmp);
	}
}

module_init(rcu_init_module);
module_exit(rcu_exit_module);

