#include "linux/kernel.h"
#include "linux/module.h"
#include "linux/fs.h"
#include "linux/init.h"//驱动初始化和退出相关函数
#include "linux/types.h"
#include "linux/errno.h"
#include "linux/uaccess.h"
#include "linux/kdev_t.h"

#define MAX_SIZE 1024

static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char __user *user, size_t t, loff_t *f);
//size_t为要读取/写入信息的长度  loff_t 代表了当前文件的位置
static ssize_t my_write(struct file *file, const char __user *user, size_t t, loff_t *f);

char message[MAX_SIZE] = "--------welcome---------";
int device_num;//设备号
char* devName = "myDrive";//设备名

struct file_operations pStruct = 
{
   open : my_open,
   release : my_release,
   read : my_read,
   write : my_write,
};

/***注册模块***/
int init_module()
{
    int ret;

    /**
     *参数一：为0表示系统动态注册一个没有被占用的设备号返回
     *              不为0静态注册
     *参数二：设备名
     *参数三：指向file_operations的指针
     */
    ret = register_chrdev(0,devName,&pStruct);
    if(ret < 0)
    {
        printk("regist failure!\n");
        return -1;
    }
    else
    {
        printk("the mydev has been registered!\n");
        device_num = ret;
        printk("<<---the mydev's number(id): %d\n",ret);
        printk("<<---create a dev file ,please enter:\n");
        printk("<<----'mknod /dev/mydev c %d 0'----\n",device_num);
        printk("<<---delete device,please enter < rm /dev/%s >",devName);
        printk("<<---delete module , please enter < rmmode mydev >");
        return 0;
    }

}

/***注销模块***/
void cleanup_module()
{
    unregister_chrdev(device_num,devName);
    printk("unregister is success !\n");
}

/***打开***/
int my_open(struct inode *inode, struct file *file)
{
    printk("open myDrive OK ! \n");
    try_module_get(THIS_MODULE);//判断module模块是否处于活动状态，将该模块的引用计数加1
    return 0;
}

/***使用完release***/
int my_release(struct inode *inode, struct file *file)
{
    printk("Device released !\n");
    module_put(THIS_MODULE);//使指定的模块使用量减一
    //拥有该结构的模块的指针
    return 0;
}

//读设备里的信息
ssize_t my_read(struct file *file, char __user *user, size_t t, loff_t *f)
{
    //copy_to_user(void __user *to, const void *from, unsigned long n)
    //从from指针所指向的数据传n个字节到to
    if (copy_to_user(user ,message,sizeof(message)))
    {
        //返回值：函数的返回值是指定要读取的n个字节中还剩下多少字节还没有被拷贝。
        //如果返回值不为0时，调用copy_to_user的函数会返回错误号-EFAULT表示操作出错
        return -EFAULT;
    }
    return sizeof(message);
}

//向设备里写信息
ssize_t my_write(struct file *file, const char __user *user, size_t t, loff_t *f)
{
    //copy_from_user(void *to, const void __user *from, unsigned long n)
    //从from指针所指向的数据传n个字节到to
    if(copy_from_user(message,user,sizeof(message)))
    {//to:内核空间指针 ; from用户空间指针 ; 拷贝字节数 
        //返回值：函数的返回值是指定要读取的n个字节中还剩下多少字节还没有被拷贝。
        //如果返回值不为0时，调用copy_to_user的函数会返回错误号-EFAULT表示操作出错
        return -EFAULT;
    }
    return sizeof(message);
}

