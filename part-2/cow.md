# xv6 - CoW Reference

All memory management happens with 4KB pages allocated by `kalloc()` and freed by `kfree()` in `kernel/kalloc.c`. Lock is associated with `kmem` which stores linked list of free pages.

`satp`- superwiser register for address translation pointer - control and status register - points to page table in use

`Translation Lookaside Buffer - TLB` - caches recent PTE
`PTE` - 10 bits Unused - 44 bits Physical page number -
    `V` - valid
    `R` - Read
    `W` - Write
    `X` - Executable


File `kernel/vm.c`
Page Table hierarchy - Root Page -> Index Pages -> Data Pages

`walk(pagetable, va, alloc)` - walks the tree and returns pte that points to data page. Create pages if needed

`mappages` - add PTE to a pagetable - calls `walk` which allocs pages and final pte is mapped to this physical address.

`kvm` - all kernel page tables creation and initialization

`walkaddr` - use page table to map virtial address to physical address

`uvmcreate()` - create an empty virutal address space - returns pagetable

`uvminit()` - initialize for init process

`uvmallc()` - add pages to a virtual address space
`uvmunmap()` - remove npages of mappings starting from va
`uvmfree()` - calls `freewalk()` - which frees all pages in page table except data pages - `uvmunmap()` has to be called first
`uvmcopy()`

`PTE_FLAGS` in `kernel/riscv.h` is defined by the lower 10 bits - `_ _ D A G U X W R V`
The 9th and 10th bits can be used by superviser software.
Bit 8 - Set to check if CoW Page
Bit 9 - set to store if original PTE_W

`r_scause` - 12 (Instruction Page Fault), 13(Load page fault), 15 (Store Page Fault)

Virtual address space of `[KERNBASE, KERNBASE+PHYSTOP]` gets mapped to `[0, PHYSTOP]` in physical address space.
Total number of physical pages is given by `TOTAL_PHY_PAGES = PGROUNDUP(PHYSTOP)/PGSIZE`

Defined struct `pteReferences` to store the reference counts of all pages along with a lock to avoid race conditions. Initialized in `kinit()` where reference count is set to 0 for all pages.
