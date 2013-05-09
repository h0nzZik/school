/**
 * Pro kazdy exportovany symbol existuje zaznam v sekci .ksymtab (linux/export.h)
 * kallsyms.c
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/syscalls.h>
#include <asm/asm-offsets.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/fcntl.h>
#include <asm/unistd.h>

struct sc_hook {
	unsigned long **sctab;
	unsigned long sc_entries;
	char string[80];

	int read_idx;
};

static struct sc_hook hooker;

/* saved functions */


static asmlinkage int (*orig_sys_open)(const char * __user, int, int);


static asmlinkage int my_sys_open(const char *__user buf, int flags, int mode)
{
	static int cnt=0;
	cnt++;
	pr_info("my_sys_open: %d\n", cnt);
	return orig_sys_open(buf, flags, mode);
}

/*
 * Don't check anything ;)
 */

void enable_write_at(void *addr)
{
	unsigned int level;
	pte_t *pte;

	/* get page table entry */
	pte = lookup_address((unsigned long)addr, &level);
	pte->pte |= _PAGE_RW;
}

void disable_write_at(void *addr)
{
	unsigned int level;
	pte_t *pte;

	/* get page table entry */
	pte = lookup_address((unsigned long)addr, &level);
	pte->pte &= ~_PAGE_RW;
}

static int get_sct_idx(struct sc_hook *self, unsigned long sym)
{
	int i;
	for (i=0; i<500; i++) {
		if ((unsigned long)self->sctab[i] == sym)
			return i;
	}
	return -1;

}

static int inicialize_hooker(struct sc_hook *self)
{
	int idx;

	idx = get_sct_idx(self, (unsigned long)sys_read);
	pr_info("sys_read: %d\n", idx);

	orig_sys_open = (void *)self->sctab[idx];
	enable_write_at(self->sctab + idx);
	self->sctab[idx] = (void *)my_sys_open;
	disable_write_at(self->sctab + idx);

	pr_info("hook completed\n");
	return 0;
}

static int clean_hooker(struct sc_hook *self)
{
	int idx;

	idx = self->read_idx;

	enable_write_at(self->sctab + idx);
	self->sctab[idx] = (void *)orig_sys_open;
	disable_write_at(self->sctab + idx);

	pr_info("hook removed\n");
	return 0;
}

static void print_addr(const char *s, void *a)
{
	pr_info("[sc_hook] --- printing '%s' ---\n", s);
	pr_info("[sc_hook] address: %p\n", a);
	pr_info("[sc_hook] symbol:  %pS\n", a);
}

void sc_lookup_kallsyms(struct sc_hook *self)
{
	/* We will try to find 'kallsyms_lookup_size_offset' */
	int (*ks_lkp_sz_off)(unsigned long addr, unsigned long *sz, unsigned long *off) = NULL;

	unsigned long sz = 0;
	unsigned long off = 0;

	self->sctab = (unsigned long **)kallsyms_lookup_name("sys_call_table");
	ks_lkp_sz_off = (void *)kallsyms_lookup_name("kallsyms_lookup_size_offset");
	pr_info("some lovely function: %pS\n", ks_lkp_sz_off);


	ks_lkp_sz_off((long)self->sctab, &sz, &off);
	pr_info("size: %lu, off: %lu\n", sz, off);

	self->sc_entries = sz / sizeof(self->sctab[0]);
	pr_info("we have %lu entries\n", self->sc_entries);
}


/**
 * Returns sys_call_table
 */
int sc_lookup(int howto, struct sc_hook *self)
{
	self->sctab = NULL;
	switch (howto) {
		case 0:
			/* some built-in magic numbers */
			pr_info("[sc_hook] lookup: using built-int pointer\n");
			self->sctab = (unsigned long **)0xffffffff81805a60;
			self->sc_entries = 314;
			break;
		case 1:
			pr_info("[sc_hook] lookup: using kallsyms\n");
			sc_lookup_kallsyms(self);
			break;
	};

	if (self->sctab == NULL || self->sc_entries == 0)
		return -1;
	return 0;
}

static int sc_check(struct sc_hook *self)
{
/*
	print_addr("this may be the table", self->sctab);
	print_addr("trying sys_close", sys_close);

	pr_info("__NR_close == %d\n", __NR_close);
	print_addr("at close", self->sctab[__NR_close]);

	pr_info("__NR_openat == %d\n", __NR_openat);
	print_addr("openat", self->sctab[__NR_openat]);
*/
	if (self->sctab[__NR_close] != sys_close)
		return -1;

	return 0;

}

static int dump_sc_table(struct sc_hook *self)
{
	unsigned long x;
	int i;


	pr_info("--- trying to dump sc_table ---\n");

	for (i=0; i<self->sc_entries; i++) {
		sprint_symbol_no_offset(self->string, (unsigned long)self->sctab[i]);
//		pr_info("current: %s\n", self->string);
		print_addr("iterating over sctab", self->sctab[i]);
	}
	return 0;
}




int hook_init(void)
{
	int rv;
	printk(KERN_INFO "*********** New try ***********\n");
	memset(&hooker, 0, sizeof(hooker));

	rv = sc_lookup(1, &hooker);
	if (rv) {
		pr_info("[sc_hook] can't find the table\n");
		return -1;
	}
	pr_info("[sc_hook] sys_call_table @ %p\n", hooker.sctab);
	rv = dump_sc_table(&hooker);
	if (rv) {
		pr_info("[sc_hook] something went wrong\n");
		return -1;
	}
	rv = sc_check(&hooker);
	if (rv) {
		pr_info("[sc_hook] check failed\n");
		return -1;
	}


	/* module won't load */
	return -ENOMEM;

	
	inicialize_hooker(&hooker);

	
	return 0;
}

void hook_exit(void)
{
	clean_hooker(&hooker);
	printk(KERN_INFO "Unloading module.\n");
}

module_init(hook_init);
module_exit(hook_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Empty module");
