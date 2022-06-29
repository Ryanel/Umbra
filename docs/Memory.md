# Memory Subsystem

The memory subsystem is a layered subsystem.

Layers:
* PMM - What physical ram is actually free?
* VMTables
    * VAS - Handles the vitual address space (page tables)
    * Pager - Handles hardware interrupts related to the VAS
* Kernel Heap
* VMObjs
    * Virtual Memory Objects - Represent a 
    * Virtual Memory Containers - Contain VMOs. A Virtual Memory Container should be renderable into a VAS.

## Goal for how things work

* PMM
    * Several different queues (Wired, Free, Active, etc)
    * LRU Cache of pages
* VMTables
    * VAS - Constructs these on the fly, can be cached but also discarded.
