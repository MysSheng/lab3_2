#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>
#include <linux/uaccess.h> // for copy_from_user and copy_to_user
#include <linux/jiffies.h> // for jiffies and time
#include <linux/sched.h>   // for current (process and thread info)
#include <linux/kthread.h> // for kthread creation
#include <linux/delay.h>   // for msleep

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE]; //kernel buffer

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    if (buffer_len > BUFSIZE)
        buffer_len = BUFSIZE;

    if (copy_from_user(buf, ubuf, buffer_len))
        return -EFAULT;

    buf[buffer_len] = '\0';
    pr_info("Received from user: %s\n", buf);

    return buffer_len;
    /****************/
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    int len;
    unsigned long time_ms = jiffies_to_msecs(jiffies);

    len = snprintf(buf, BUFSIZE, "String: %s\nProcess ID: %d\nThread ID: %d\nTime(ms): %lu\n",
                   buf, current->pid, current->pid, time_ms);

    if (*offset > 0 || buffer_len < len)
        return 0;

    if (copy_to_user(ubuf, buf, len))
        return -EFAULT;

    *offset = len;
    return len;
    /****************/
}

static struct proc_ops Myops = {
    .proc_read = Myread,
    .proc_write = Mywrite,
};

static int My_Kernel_Init(void){
    proc_create(procfs_name, 0644, NULL, &Myops);   
    pr_info("My kernel says Hi");
    return 0;
}

static void My_Kernel_Exit(void){
    pr_info("My kernel says GOODBYE");
}

module_init(My_Kernel_Init);
module_exit(My_Kernel_Exit);

MODULE_LICENSE("GPL");
