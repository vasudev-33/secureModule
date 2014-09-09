/* Tutorial support files.  
 * see recluze.wordpress.com for details 
 *  
 */ 
#define START_UID 5000
#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/module.h>    // included for all kernel modules
  // included for KERN_INFO

#include <linux/buffer_head.h>
#include <asm/segment.h>
//#include <asm/uaccess.h>

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/tracehook.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/security.h>
#include <linux/xattr.h>
#include <linux/capability.h>
#include <linux/unistd.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/spinlock.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/proc_fs.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/tty.h>
#include <net/icmp.h>
#include <net/ip.h>		/* for local_port_range[] */
#include <net/tcp.h>		/* struct or_callable used in sock_rcv_skb */
#include <net/net_namespace.h>
#include <net/netlabel.h>
#include <linux/uaccess.h>
#include <asm/ioctls.h>
#include <asm/atomic.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>	/* for network interface checks */
#include <linux/netlink.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/dccp.h>
#include <linux/quota.h>
#include <linux/un.h>		/* for Unix socket types */
#include <net/af_unix.h>	/* for Unix socket types */
#include <linux/parser.h>
#include <linux/nfs_mount.h>
#include <net/ipv6.h>
#include <linux/hugetlb.h>
#include <linux/personality.h>
#include <linux/sysctl.h>
#include <linux/audit.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/posix-timers.h>



#ifdef CONFIG_SECURITY_BLABBERMOUTH 

static int read_file(const char *filename ,  char *buf , int len , int offset)
{

	struct file *filp;
	mm_segment_t oldfs;
	int bytes = 0;
	//char *kfileName = getname(filename);
	const char *kfileName=filename;
	int valid = 0;
  printk("Inide read function:\n");

	filp  =  filp_open(kfileName ,  O_RDONLY ,  0);
	//putname(kfileName);
	if (!filp || IS_ERR(filp)) {
		printk("wrapfs_read_file err %d\n" ,  (int) PTR_ERR(filp));

		valid =  -EINVAL;  /* or do something else */
		printk("Erro writing: %d" , valid);
		goto cleanup;
	}
	if (!filp->f_op->read) {
		valid =  -EPERM;  /* file(system) doesn't allow reads */
		goto cleanup;
	}

	/* now read len bytes from offset  */
	filp->f_pos  =  offset;		/* start offset */
	oldfs  =  get_fs();
	set_fs(KERNEL_DS);
 	bytes  =  filp->f_op->read(filp ,  buf , len , &filp->f_pos);
  set_fs(oldfs);
  
	printk("Buff: %s" , buf);
	/* close the file */
cleanup:
	printk("Inside cleanup: \n");
	if (!(!filp || IS_ERR(filp))) {
		printk("Inside fileclose: \n");
		filp_close(filp ,  NULL);
	}
	if (valid < 0)
		return valid;
	if (bytes < 0)
		return -EACCES;

	return bytes;
}

static int read_policy_file(const char *filename, int req_uid, int *can_read, int *can_write, int *can_execute)
{
	//char *buf=NULL;
	char buf1[PAGE_SIZE];
	char *buf=&buf1;
	int offset=0;
	int read_bytes=0;
	char *line=NULL;
	unsigned int uid;
	char *role=NULL;
	
	//unsigned int can_read;
	//unsigned int can_write;
	//unsigned int can_execute;
	char *part=NULL;
	int count=0;
	memset(buf1, '0',PAGE_SIZE);
	//buf = kmalloc(PAGE_SIZE ,  GFP_USER);
	read_bytes=read_file(filename,buf1,PAGE_SIZE,offset);
	
	while((line=strsep(&buf, "\n")))
	{
		if(*line=='\0')
			break;
		count=0;
		printk("Line is %s\n",line);
		while((part=strsep(&line, ",")))
		{
			//if(part=="\n")
			//	break;
			printk("%s ",part);
			if(count==0)
			{
				printk("in count 0");
				kstrtouint((const char *)part, 10, &uid);

				printk("%d ",uid);
			}
			else if(count==1)

			{
				printk("in count 1");
				role=part;
			}
			else if(count==2)
			{
				printk("in count 2");
				kstrtoint((const char *)part, 10, can_read);
				printk("%d ",*can_read);
			}
			else if(count==3)
			{
				printk("in count 3");
				kstrtoint((const char *)part, 10, can_write);
				printk("%d ",*can_write);
			}
			else if(count==4)
			{
				printk("in count 4");
				kstrtoint((const char *)part, 10, can_execute);
				printk("%d ",*can_execute);
			}
			
			count++;
		}
		if(uid==req_uid)
		{
			printk("Matching id");
			return 0;	
		}		
		printk("\n");
		
	}
	return -1;	
	//if(buf!=NULL)
	//	kfree(buf);
}



static int blabbermouth_inode_create(struct inode *inode, struct dentry *dentry,
			    int mask)
{
	const struct task_struct *task = current;
	unsigned int uid=task->cred->uid.val;
	int can_read;
	int can_write;
	int can_execute;
	int status;
	
	if(uid>START_UID){
	//struct dentry *my_dentry=file->f_path.dentry;
	
	//printk(KERN_DEBUG "%d called  %s with file owner %d\n",task->cred->uid.val,my_dentry->d_iname,file->f_owner.uid.val);
	//printk(KERN_DEBUG "%d is vfs uid\n",file->f_cred->fsuid.val);
	
	status=read_policy_file("/usr/src/linux-3.14.17/read_policy/policy_file", uid, &can_read, &can_write, &can_execute);
	if(status==0)
	{
		if(can_write==0)
		{
			return -EPERM;
		}
	}
	
	}
	return 0;
}


static int blabbermouth_inode_mkdir(struct inode *inode, struct dentry *dentry,
			   int mask)
{
	const struct task_struct *task = current;
	unsigned int uid=task->cred->uid.val;
	int can_read;
	int can_write;
	int can_execute;
	int status;
	
	if(uid>START_UID){
	//struct dentry *my_dentry=file->f_path.dentry;
	
	//printk(KERN_DEBUG "%d called  %s with file owner %d\n",task->cred->uid.val,my_dentry->d_iname,file->f_owner.uid.val);
	//printk(KERN_DEBUG "%d is vfs uid\n",file->f_cred->fsuid.val);
	
	status=read_policy_file("/usr/src/linux-3.14.17/read_policy/policy_file", uid, &can_read, &can_write, &can_execute);
	if(status==0)
	{
		if(can_write==0)
		{
			return -EPERM;
		}
	}
	
	}
	return 0;
}

static int blabbermouth_inode_rmdir(struct inode *inode, struct dentry *dentry)
{
	const struct task_struct *task = current;
	unsigned int uid=task->cred->uid.val;
	int can_read;
	int can_write;
	int can_execute;
	int status;
	
	if(uid>START_UID){
	//struct dentry *my_dentry=file->f_path.dentry;
	
	//printk(KERN_DEBUG "%d called  %s with file owner %d\n",task->cred->uid.val,my_dentry->d_iname,file->f_owner.uid.val);
	//printk(KERN_DEBUG "%d is vfs uid\n",file->f_cred->fsuid.val);
	
	status=read_policy_file("/usr/src/linux-3.14.17/read_policy/policy_file", uid, &can_read, &can_write, &can_execute);
	if(status==0)
	{
		if(can_write==0)
		{
			return -EPERM;
		}
	}
	
	}
	return 0;
}

static int blabbermouth_inode_unlink(struct inode *inode, struct dentry *dentry){
	const struct task_struct *task = current;
	unsigned int uid=task->cred->uid.val;
	int can_read;
	int can_write;
	int can_execute;
	int status;
	
	if(uid>START_UID){
	//struct dentry *my_dentry=file->f_path.dentry;
	
	//printk(KERN_DEBUG "%d called  %s with file owner %d\n",task->cred->uid.val,my_dentry->d_iname,file->f_owner.uid.val);
	//printk(KERN_DEBUG "%d is vfs uid\n",file->f_cred->fsuid.val);
	
	status=read_policy_file("/usr/src/linux-3.14.17/read_policy/policy_file", uid, &can_read, &can_write, &can_execute);
	if(status==0)
	{
		if(can_write==0)
		{
			return -EPERM;
		}
	}
	
	}
	return 0;


}


/*
static int blabbermouth_inode_rename(struct inode *old_inode, struct dentry *old_dentry,
			    struct inode *new_inode, struct dentry *new_dentry)
{
	return 0;
}
*/
static int blabbermouth_inode_permission(struct inode *inode, int mask)
{
	return 0;
}
/*
static int blabbermouth_inode_setattr(struct dentry *dentry, struct iattr *iattr)
{
	return 0;
}

static int blabbermouth_inode_getattr(struct vfsmount *mnt, struct dentry *dentry)
{
	return 0;
}

static int blabbermouth_inode_delete(struct inode *ino)
{
	return 0;
}

*/


static int blabbermouth_file_permission(struct file *file, int mask)
{
	/*
	const struct task_struct *task = current;
	unsigned int uid=task->cred->uid.val;
	
	if(uid>2000){
	struct dentry *my_dentry=file->f_path.dentry;
	
	printk(KERN_DEBUG "%d called  %s with file owner %d\n",task->cred->uid.val,my_dentry->d_iname,file->f_owner.uid.val);
	printk(KERN_DEBUG "%d is vfs uid\n",file->f_cred->fsuid.val);
	}
	*/
	return 0;
}


/*
static int blabbermouth_file_alloc_security(struct file *file)
{
	return 0;
}

static void blabbermouth_file_free_security(struct file *file)
{
}

*/
/*
static int blabbermouth_dentry_open(struct file *file, const struct cred *cred)
{
	return 0;
}
*/

static struct security_operations blabbermouth_ops = {
	.name =				"blabber",
/*
	.inode_alloc_security =		blabbermouth_inode_alloc_security,
	.inode_free_security =		blabbermouth_inode_free_security,
	.inode_init_security =		blabbermouth_inode_init_security,
*/

	.inode_create =			blabbermouth_inode_create,
	//.inode_link =			blabbermouth_inode_link,
	.inode_unlink =			blabbermouth_inode_unlink,
	//.inode_symlink =		blabbermouth_inode_symlink,
	.inode_mkdir =			blabbermouth_inode_mkdir,
	.inode_rmdir =			blabbermouth_inode_rmdir,
/*
	//.inode_mknod =			blabbermouth_inode_mknod,
	//.inode_rename =			blabbermouth_inode_rename,
	//.inode_readlink =		blabbermouth_inode_readlink,
	//.inode_follow_link =		blabbermouth_inode_follow_link,
*/
	.inode_permission =		blabbermouth_inode_permission,
/*
	//.inode_setattr =		blabbermouth_inode_setattr,
	//.inode_getattr =		blabbermouth_inode_getattr,
	//.inode_setxattr =		blabbermouth_inode_setxattr,
	//.inode_post_setxattr =		blabbermouth_inode_post_setxattr,
	//.inode_getxattr =		blabbermouth_inode_getxattr,
	//.inode_listxattr =		blabbermouth_inode_listxattr,
	//.inode_removexattr =		blabbermouth_inode_removexattr,
	//.inode_getsecurity =		blabbermouth_inode_getsecurity,
	//.inode_setsecurity =		blabbermouth_inode_setsecurity,
	//.inode_listsecurity =		blabbermouth_inode_listsecurity,
	//.inode_getsecid =		blabbermouth_inode_getsecid,

*/
	.file_permission =		blabbermouth_file_permission,

/*
	.file_alloc_security =		blabbermouth_file_alloc_security,
	.file_free_security =		blabbermouth_file_free_security,
	//.file_ioctl =			blabbermouth_file_ioctl,
	//.dentry_open =			blabbermouth_dentry_open,
*/
	/*
	.task_setpgid =			blabbermouth_task_setpgid,
	.task_getpgid =			blabbermouth_task_getpgid,
	.task_getsid =			blabbermouth_task_getsid,
	.task_getsecid =		blabbermouth_task_getsecid,
	*/
};


static int __init blabbermouth_init(void){
	/* register the hooks */	
	
	if (register_security(&blabbermouth_ops))
		panic("blabbermouth: Unable to register blabbermouth with kernel.\n");
	else 
		printk("blabbermouth: registered with the kernel\n");

	return 0;
}

static void __exit blabbermouth_exit (void)
{	
	return;
}



module_init (blabbermouth_init);
module_exit (blabbermouth_exit);

//MODULE_DESCRIPTION("blabbermouth");
//MODULE_LICENSE("GPL");
#endif /* CONFIG_SECURITY_blabbermouth */

