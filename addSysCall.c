#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/pgtable_32_types.h>
#include <linux/uaccess.h>

#include <linux/kernel.h>
#include <linux/syscalls.h>

asmlinkage void csie_ncu_survey_TT(void) {
	unsigned long address;
	unsigned int level;
	int i = 0, j = 0;
	printk(KERN_INFO"[%s] === phys addr: %08lx\n", __func__, (unsigned long)virt_to_phys((unsigned long *)current->mm->pgd));
	printk(KERN_INFO"[%s] === phys addr: %08lx\n", __func__, (unsigned long)read_cr3());
	pgd_t *base = __va(read_cr3());
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *pte;

	for (i = 992; i <= 1023; i++){
		// (i & PTRS_PER_PGD - 1) << PGDIR_SHIFT 
		address = i << PGDIR_SHIFT;
		//TODO pgd_index(address) (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD-1))
		pgd = &base[pgd_index(address)];
		pmd = pmd_offset(pud_offset(pgd, address), address);
		
		if (!pmd_present(*pmd)){
			printk(KERN_INFO"[%s] === [%04d] NULL\n", __func__, i);
			continue;
		}

		//TODO find page level
		pte = lookup_address(address, &level);
		if (pte){
			if (level == PG_LEVEL_1G){
				printk(KERN_INFO"[%s] === [%04d] 1G 0x%08lx\n", __func__, i, address);
				continue;
			}
			else if(level == PG_LEVEL_2M){
				printk(KERN_INFO"[%s] === [%04d] 4M 0x%08lx\n", __func__, i, pmd_val(*pmd));
				continue;
			}
			printk(KERN_INFO"[%s] === [%04d] Page Table 0x%08lx\n", __func__, i, pmd_val(*pmd));
			for (j = 0; j < PTRS_PER_PTE; j++){
				pte = lookup_address(address | (j << PAGE_SHIFT), &level);
				if (!pte_present(*pte))
					continue;
				if (level == PG_LEVEL_4K)
					printk(KERN_INFO"[%s] === \t[%04d]\t0x%08lx\n", __func__, j, pte_val(*pte));
			}
		}
	}
}
