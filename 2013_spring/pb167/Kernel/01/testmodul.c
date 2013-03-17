/**
 * Ok, nechova se to jako kalkulacka, ale nekdy to udelam
 *
 *
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>	
#include <linux/slab.h>
#include <linux/ctype.h>


int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80		/* Max length of the message from the device */

static int Major;		/* Major number assigned to our device driver */
static int Device_Open = 0;	/* Is device open?  
				 * Used to prevent multiple access to device */
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

int init_module(void)
{
        Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", Major);
	  return Major;
	}

	printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");

	return SUCCESS;
}

void cleanup_module(void)
{
	/* 
	 * Unregister the device 
	 */
	unregister_chrdev(Major, DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *file)
{
	static int counter = 0;

	if (Device_Open)
		return -EBUSY;

	
	++counter;

	Device_Open++;
	sprintf(msg, "[%d]\tHello world!\n", counter);
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
	/*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;

	/*
	 * If we're at the end of the message, 
	 * return 0 signifying end of file 
	 */
	if (*msg_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (length && *msg_Ptr) {

		/* 
		 * The buffer is in the user data segment, not the kernel 
		 * segment so "*" assignment won't work.  We have to use 
		 * put_user which copies data from the kernel data segment to
		 * the user data segment. 
		 */
		put_user(*(msg_Ptr++), buffer++);

		length--;
		bytes_read++;
	}

	/* 
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}

//#if 0
struct stack_entry {
	int n;
	char f;	/* '\0', '+', '-', '*', '/' */
};


struct my_stack
{
	struct stack_entry entries[40];
	int top;
};

int my_stack_push(struct my_stack *s, int n, char f)
{
	if(s->top == 40)
		return -1;
	s->entries[s->top].f = f;
	s->entries[s->top].n = n;
	s->top++;
	return 0;
}

int my_stack_pop(struct my_stack *s, int *n, int *f)
{
	if (s->top == 0)
		return -1;
	s->top--;
	*f = s->entries[s->top].f;
	*n = s->entries[s->top].n;
	return 0;
}

void my_stack_dump(struct my_stack *s)
{
	int n;
	int f;

	while (!my_stack_pop(s, &n, &f)) {
		pr_info("poped: %d, '%c'\n", n, f);
	}
}


int my_stack_calc(struct my_stack *s)
{
	int state;
	int var[2];
	int x;
	int f;

	state = 0;
	while (!my_stack_pop(s, &x, &f)) {
		pr_info("poped: %d, '%c'\n", x, f);
		if (f == '\0') {
			if (state != 2) {
				var[state] = x;
				state++;
			} else {
				pr_info("unexp. token: %d\n", x);
				return -1;
			}
		} else {
			if (state == 2) {
				if (f == (int)'-')
					var[0] = var[1] - var[0];
				else if (f == (int)'+')
					var[0] = var[1] + var[0];
				else {
					pr_info("unknown token: '%c'\n", f);
					return -1;
				}
				state = 1;
			} else {
				pr_info("unexp. token: '%c'\n", f);
				return -1;
			}
		}
	}
	pr_info("result: %d\n", var[0]);
	return 0;
}

struct my_stack data;

struct calc_struct {
	int params[2];
	int sum;
	int state;
};

struct calc_struct data_state;

int my_getnum(const char *str)
{
	int n;
	n = 0;
	while(*str && isdigit(*str)) {
		n*=10;
		n+=*str-'0';
		str++;
	}
	return n;
}
//#endif
static void do_some_stuff(const char *str)
{
//#if 0
	memset(&data, 0, sizeof(data));
	memset(&data_state, 0, sizeof(data_state));

	while (*str)
	{
		/* check if operation */
		if (*str == '+' || *str == '-') {
			my_stack_push(&data, 0, *str);
			str++;
		}
		/* check if space */
		else if (isspace(*str))
			str++;
		/* check if error */
		else if (isalpha(*str)) {
			pr_info("bad format\n");
			return;
		}
		/* check number */
		else if (isdigit(*str)) {
			my_stack_push(&data, my_getnum(str), 0);
			while (isdigit(*str))
				str++;
		} else {
			pr_info("bad token: '%c'\n", *str);
			return;
		}
	}
//#endif

	if (my_stack_calc(&data))
		pr_info("so bad :-(\n");
//	my_stack_dump(&data);
}


/*  
 * Called when a process writes to dev file: echo "hi" > /dev/hello 
 */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	char *my_buff;
	int rv;

//	len++;
	my_buff = kmalloc(len+1, GFP_KERNEL);
	if (my_buff == NULL) {
		pr_info("can't kmalloc\n");
		return -ENOMEM;
	}
//	printk(KERN_ALERT "Sorry, this operation isn't supported.\n");


	rv = copy_from_user(my_buff, buff, len);
	if (rv > 0) {
		pr_info("unread: %d\n", rv);
		len -= rv;
	}
	if (rv < 0) {
		pr_info("error: %d\n", rv);
		kfree(my_buff);
		return -EPERM;
	}

	my_buff[len] = 0;

	pr_info("got: %s\n", my_buff);

	do_some_stuff(my_buff);

	kfree(my_buff);
	return len;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Test module");
