/*
  Copyright (c) 2001-2005 Eliot Dresselhaus

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef included_clib_smp_h
#define included_clib_smp_h

/* Per-CPU state. */
typedef struct {
  /* Per-cpu local heap. */
  void * heap;

  u32 thread_id;
} clib_smp_per_cpu_main_t;

typedef struct {
  /* Number of CPUs used to model current computer. */
  u32 n_cpus;

  /* Number of cpus that are done and have exited. */
  u32 n_cpus_exited;

  /* Log2 stack and vm (heap) size. */
  u8 log2_n_per_cpu_stack_bytes, log2_n_per_cpu_vm_bytes;

  /* Thread local store (TLS) is stored at stack top.
     Number of 4k pages to allocate for TLS. */
  u16 n_tls_4k_pages;

  /* Per cpus stacks/heaps start at these addresses. */
  void * vm_base;

  /* Thread-safe global heap.  Objects here can be allocated/freed by any cpu. */
  void * global_heap;

  clib_smp_per_cpu_main_t * per_cpu_mains;
} clib_smp_main_t;

extern clib_smp_main_t clib_smp_main;

#define clib_smp_compare_and_swap(addr,new,old) __sync_val_compare_and_swap(addr,old,new)
#define clib_smp_swap(addr,new) __sync_lock_test_and_set(addr,new)
#define clib_smp_atomic_add(addr,increment) __sync_fetch_and_add(addr,increment)

typedef struct {
  u32 is_locked;
} clib_smp_lock_t;

always_inline void
clib_smp_lock (clib_smp_lock_t * l)
{
  while (clib_smp_compare_and_swap (&l->is_locked, /* new */ 1, /* old */ 0) != 0)
    ;
}

always_inline void
clib_smp_unlock (clib_smp_lock_t * l)
{
  clib_smp_swap (&l->is_locked, 0);
}

always_inline void *
clib_smp_vm_base_for_cpu (clib_smp_main_t * m, uword cpu)
{
  return m->vm_base + (cpu << m->log2_n_per_cpu_vm_bytes);
}

always_inline void *
clib_smp_stack_top_for_cpu (clib_smp_main_t * m, uword cpu)
{
  /* Stack is at top of per cpu VM area. */
  return clib_smp_vm_base_for_cpu (m, cpu + 1) - ((uword) 1 << m->log2_n_per_cpu_stack_bytes);
}

always_inline uword
os_get_cpu_number (void)
{
  clib_smp_main_t * m = &clib_smp_main;
  void * sp;
  uword n;

  /* Get any old stack address. */
  sp = &sp;

  n = (sp - m->vm_base) >> m->log2_n_per_cpu_vm_bytes;

  if (CLIB_DEBUG && m->n_cpus > 0 && n >= m->n_cpus)
    os_panic ();

  return n < m->n_cpus ? n : 0;
}

#define clib_atomic_exec(p,var,body)					\
do {									\
  typeof (v) * __clib_atomic_exec_p = &(p);				\
  typeof (v) __clib_atomic_exec_locked = uword_to_pointer (1, void *);	\
  typeof (v) __clib_atomic_exec_v;					\
  void * __clib_atomic_exec_saved_heap;					\
									\
  /* Switch to global (thread-safe) heap. */				\
  __clib_atomic_exec_saved_heap = clib_mem_set_heap (clib_smp_main.global_heap); \
									\
  /* Grab lock. */							\
  while ((__clib_atomic_exec_v						\
	  = clib_smp_swap (__clib_atomic_exec_p,			\
			   __clib_atomic_exec_locked))			\
	 == __clib_atomic_exec_locked)					\
    ;									\
									\
  /* Execute body. */							\
  (var) = __clib_atomic_exec_v;						\
  body;									\
  __clib_atomic_exec_v = (var);						\
									\
  /* Release lock. */							\
  (void) clib_smp_swap (__clib_atomic_exec_p, __clib_atomic_exec_v);	\
									\
  /* Switch back to previous heap. */					\
  clib_mem_set_heap (__clib_atomic_exec_saved_heap);			\
} while (0)

uword os_smp_bootstrap (uword n_cpus,
			void * bootstrap_function,
			uword bootstrap_function_arg);

void clib_smp_init (void);

#endif /* included_clib_smp_h */