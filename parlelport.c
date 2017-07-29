#include <linux/init.h>			// Initialisation of a module
#include <linux/module.h>		// Loading a module in kernel
#include <linux/kernel.h>		// printk()
#include <linux/slab.h>			// kmalloc()
#include <linux/fs.h>			// File table, structues, etc
#include <linux/errno.h>		// Error code
#include <linux/types.h>		// size_t
#include <linux/proc_fs.h>		// Virtual file system, as everything including ports are treated as files
#include <linux/fcntl.h>		// O_ACCMODE
#include <linux/ioport.h>		// Input and output operations on ports
#include <asm/system.h>			// cli(), *_flags
#include <asm/uaccess.h>		// copy_from/to_user
#include <asm/io.h>				// inb, outb

#define DRIVER_AUTHOR "Krishna Y, B Ramesh, A Sachin"
#define DRIVER_DESC   "Flashing LED Lights"
#define DRIVER_LICENSE "Dual BSD/GPL"

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("parlelport");

// Function declaration of parlelport.c
int parlelport_open(struct inode *inode, struct file *filp);
int parlelport_release(struct inode *inode, struct file *filp);

ssize_t parlelport_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t parlelport_write(struct file *filp, char *buf, size_t count, loff_t *f_pos);

void parlelport_exit(void);
int parlelport_init(void);

// Structure that declares the common file access functions
struct file_operations parlelport_fops = {
											read: parlelport_read,
											write: parlelport_write,
											open: parlelport_open,
											release: pa	rlelport_release
										};
										

int parlelport_major = 61;				// Driver global variables Major number


int port;								// Control variable for memory reservation of the parallel port
module_init(parlelport_init);
module_exit(parlelport_exit);

int parlelport_init(void)
{
	   int result;
	   
	   result = register_chrdev(parlelport_major, "parlelport", &parlelport_fops);	// Registering device
	   
	   if (result < 0)
	   {
			 printk("<1>parlelport: cannot obtain major number %d\n", parlelport_major);
			 return result;
	   }
	 
	  port = check_region(0x378, 1);				// Registering port
	  
	  if (port)
	  {
			printk("<1>parlelport: cannot reserve 0x378\n");
			result = port;
			goto fail;
	  }
	  
	  request_region(0x378, 1, "parlelport");

	  printk("<1>Inserting parlelport module\n");
	  return 0;
	   
	  fail:
		 parlelport_exit();
		 return result;
		 
}


void parlelport_exit(void)
{
	   unregister_chrdev(parlelport_major, "parlelport");		// Free major number

	  if (!port)												// Free port
	  {
			release_region(0x378,1);
	  }

	   printk("<1>Removing parlelport module\n");
   
}
int parlelport_open(struct inode *inode, struct file *filp)
{
   // Open port: Success as there is nothing to initialize
   return 0;

}

int parlelport_release(struct inode *inode, struct file *filp)
{

   // Release port: Success as there is nothing to free
   return 0;

}

ssize_t parlelport_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{

	   char parlelport_buffer;						// Buffer to read the device

	   parlelport_buffer = inb(0x378);

	   copy_to_user(buf, &parlelport_buffer, 1);	// We transfer data to user space
	   
	   /* We change the reading position as best suits */
	  
	  if (*f_pos == 0)
	   {
		  *f_pos += 1;
		  
		  return 1;
		  
	   } 
	   
	   else
	   {
		  return 0;
	   }
	   
}

ssize_t parlelport_write(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
		char *tmp;
	   
		char parlelport_buffer;					// Buffer writing to the device

		tmp = buf + count-1;
		
		copy_from_user(&parlelport_buffer, tmp, 1);
	  
		outb(parlelport_buffer,0x378);			// Writing to the port

		return 1;
		
}

