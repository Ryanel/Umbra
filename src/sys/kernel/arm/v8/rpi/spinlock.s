.section .text

.global _k_spinlock_acquire

// Attempts to acquire a spinlock, and will write 1 to the lock to indicate it is held.
// Paramaters:
// * x0 = spinlock address to acquire
_k_spinlock_acquire:
	mov	w2, #1
	sevl
l1:	wfe
l2:	ldaxr	w1, [x0]
	cbnz	w1, l1
	stxr	w1, w2, [x0]
	cbnz	w1, l1
	ret