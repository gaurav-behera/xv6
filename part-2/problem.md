---
layout: page
title: Mini Project 3
permalink: /mini-projects/mp3
parent: Mini Projects
nav_order: 3
---

# Mini Project 3: PBS, CoW (Memory management) and Concurrency.

<!-- ## Operating Systems and Networks, Monsoon 2023 -->

## Deadline: 11:59pm 10 November 2023

## GitHub Classroom

Welcome to the final mini-project of the course! Follow [this link](https://classroom.github.com/a/JH3nieSp) to accept the mini-project. You will then be assigned a private repository on GitHub. This is where you will be working on the mini project. All relevant instructions regarding this project can be found below.

# xv-6 Revisited:

## 1. Modified Priority Based Scheduler in xv-6. [30 Marks]

Implement a preemptive priority-based scheduler that selects the process with the highest priority for execution. In case two or more processes have the same priority, we use the number of times the process has been scheduled to break the tie. If the tie remains, use the start-time of the process to break the tie(processes with lower start times should be scheduled further).

There are two types of priorities.

- The **Static Priority of a process** (SP) can be in the range $$[0,100]$$, a smaller value will represent higher priority. Set the default priority of a process as $$50$$.
- The **Dynamic Priority** (DP) of a process depends on its Static Priority and RBI (recent behaviour index).

The **RBI** (Recent Behaviour Index) of a process measures its recent behavior and is used to adjust its dynamic priority. It is a weighted sum of three factors: *Running Time (RTime), Sleeping Time (STime), and Waiting Time (WTime)*. The default value of **RBI** is 25.

- Definition of the variables:
  
    - $RTime$ : The total time the process has been running since it was last scheduled.
    
    - $STime$: The total time the process has spent sleeping (i.e., blocked and not using CPU time) since it was last scheduled.
    
    - $WTime$: The total time the process has spent in the ready queue waiting to be scheduled.
    
    - $$RBI = max\bigg(Int\bigg(\dfrac{3*RTime - STime - WTime}{RTime + WTime + STime + 1} * 50\bigg), 0\bigg)$$
    
    - $$DP = \min\left(SP + RBI, 100\right)$$

- Use *Dynamic Priority (DP)* to schedule processes.

- To change the Static Priority add a new system call `set_priority()`.
    
        int set_priority(int pid, int new_priority)
    
    The system call returns the old Static Priority of the process. In case the priority of the process increases(the value is lower than before), then rescheduling should be done.
    *Note that calling this system call will also reset the Recent Behaviour Index (RBI) of the process to 25 as well.*
    
    **Also make sure to implement a user program `setpriority`, which uses the above system call to change the priority. And takes the syscall arguments as command-line arguments.**
    
    ```bash
    setpriority pid priority
    ```

- *Along with implementing the scheduler, analyze and observe the effectiveness of SP and RBI (thus RTime, WTime, STime variables) in updating the DP values and assigning priorities to multiple processes. Submit your brief analysis along with the report. Feel free to add plots/proofs to support your analysis and observations.*
    
## 2. Copy on Write Fork in xv-6 [20 Marks]

In xv6 the `fork` system call creates a duplicate process of the parent process and also copies the memory content of the parent process into the child process. This results in inefficient usage of memory since the child may only read from memory.

The idea behind a copy-on-write is that when a parent process creates a child process then both of these processes initially will share the same pages in memory and these shared pages will be marked as copy-on-write which means that if any of these processes will try to modify the shared pages then only a copy of these pages will be created and the modifications will be done on the copy of pages by that process and thus not affecting the other process.

The basic plan in COW fork is for the parent and child to initially share all physical pages, but to map them read-only. Thus, when the child or parent executes a store instruction, the RISC-V CPU raises a page-fault exception. In response to this exception, the kernel makes a copy of the page that contains the faulted address. It maps one copy read/write in the child’s address space and the other copy read/write in the parent’s address space. After updating the page tables, the kernel resumes the faulting process at the instruction that caused the fault. Because the kernel has updated the relevant PTE to allow writes, the faulting instruction will now execute without a fault.

**Implement copy-on-write (COW) fork in xv6**.

# Guidelines:
1. Do not change the basic file structure given on Github Classroom.
2. No deadline extensions will be granted.

**Do NOT take codes from seniors or your batch mates, by any chance. We will extensively evaluate cheating scenarios along with the previous few year’s submissions.**

**Viva will be conducted during the evaluations, related to your code and also the logic/concept involved. If you’re unable to answer them, you’ll get no marks for that feature/topic that you’ve implemented.**