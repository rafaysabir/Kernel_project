#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/task_info.h>

SYSCALL_DEFINE2(get_task_info, pid_t, pid, struct task_info __user *, info)
{
    struct task_struct *task;
    struct task_info kinfo = {0};
    int ret = 0;
    
    rcu_read_lock();
    if (pid == 0) {
        task = current;
    } else {
        task = find_task_by_vpid(pid);
        if (!task) {
            rcu_read_unlock();
            return -ESRCH;
        }
    }
    
    get_task_struct(task);
    rcu_read_unlock();
    
    // Fill task_info structure
    kinfo.pid = task->pid;
    kinfo.ppid = task->real_parent->pid;
    kinfo.uid = from_kuid_munged(current_user_ns(), task_uid(task));
    strncpy(kinfo.comm, task->comm, sizeof(kinfo.comm) - 1);
    kinfo.comm[sizeof(kinfo.comm) - 1] = '\0';
    kinfo.state = task->__state;
    kinfo.start_time = task->start_time;
    kinfo.utime = task->utime;
    kinfo.stime = task->stime;
    
    put_task_struct(task);
    
    if (copy_to_user(info, &kinfo, sizeof(kinfo))) {
        ret = -EFAULT;
    }
    
    return ret;
}
