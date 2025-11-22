#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE2(get_pids, pid_t __user *, pids, size_t, capacity)
{
    struct task_struct *task;
    pid_t *kern_pids;
    size_t count = 0;
    int i = 0;
    
    if (capacity == 0)
        return 0;
    
    kern_pids = kmalloc_array(capacity, sizeof(pid_t), GFP_KERNEL);
    if (!kern_pids)
        return -ENOMEM;
    
    rcu_read_lock();
    for_each_process(task) {
        if (i < capacity) {
            kern_pids[i++] = task->pid;
            count++;
        } else {
            break;
        }
    }
    rcu_read_unlock();
    
    if (copy_to_user(pids, kern_pids, count * sizeof(pid_t))) {
        kfree(kern_pids);
        return -EFAULT;
    }
    
    kfree(kern_pids);
    return count;
}
