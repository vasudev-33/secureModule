#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/slab.h>



MODULE_LICENSE("GPL");



/*read part of file*/
int read_file(const char *filename ,  char *buf , int len , int offset)
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
  
	printk("Buff: %s" , (char *)buf);
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

void read_policy_file(const char *filename)
{
	char *buf=NULL;
	int offset=0;
	int read_bytes=0;
	char *line=NULL;
	unsigned int uid;
	char *role=NULL;
	unsigned int access;
	char *part=NULL;
	
	
	buf = kmalloc(PAGE_SIZE ,  GFP_USER);
	read_bytes=read_file(filename,buf,PAGE_SIZE,offset);
	
	while((line=strsep(&buf, "\n")))
	{
		while((part=strsep(&line, ",")))
		{
			printk("%s ",part);
			kstrtouint((const char *)part, 10, &uid);
			printk("%d ",uid);
		}		
		printk("\n");
	}	
	kfree(buf);

}

static int __init read_policy_init(void)
{
    printk(KERN_INFO "Hello world!\n");
    read_policy_file("/usr/src/policy_file");
    return 0;    // Non-zero return means that the module couldn't be loaded.
}


static void __exit read_policy_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
}

module_init(read_policy_init);
module_exit(read_policy_cleanup);
