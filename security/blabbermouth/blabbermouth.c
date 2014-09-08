/* Tutorial support files.  
 * see recluze.wordpress.com for details 
 *  
 */ 


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
/*
static int blabbermouth_inode_alloc_security(struct inode *inode)
{
	return 0;
}

static void blabbermouth_inode_free_security(struct inode *inode)
{
}

static int blabbermouth_inode_init_security(struct inode *inode, struct inode *dir,
				   char **name, void **value, size_t *len)
{
	return -EOPNOTSUPP;
}

static int blabbermouth_inode_create(struct inode *inode, struct dentry *dentry,
			    int mask)
{
	return 0;
}


static int blabbermouth_inode_mkdir(struct inode *inode, struct dentry *dentry,
			   int mask)
{
	return 0;
}

static int blabbermouth_inode_rmdir(struct inode *inode, struct dentry *dentry)
{
	return 0;
}

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

static void blabbermouth_inode_delete(struct inode *ino)
{
}
*/



static int blabbermouth_file_permission(struct file *file, int mask)
{
	const struct task_struct *task = current;
	unsigned int uid=task->cred->uid.val;
	
	if(uid>2000){
	struct dentry *my_dentry=file->f_path.dentry;
	
	printk(KERN_DEBUG "%d called  %s with file owner %d\n",task->cred->uid.val,my_dentry->d_iname,file->f_owner.uid.val);
	printk(KERN_DEBUG "%d is vfs uid\n",file->f_cred->fsuid.val);
	}
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
	.inode_create =			blabbermouth_inode_create,
	//.inode_link =			blabbermouth_inode_link,
	//.inode_unlink =			blabbermouth_inode_unlink,
	//.inode_symlink =		blabbermouth_inode_symlink,
	.inode_mkdir =			blabbermouth_inode_mkdir,
	.inode_rmdir =			blabbermouth_inode_rmdir,
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

