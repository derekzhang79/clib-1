/*
  Copyright (c) 2001, 2002, 2003 Eliot Dresselhaus

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

#include <clib/longjmp.h>
#include <clib/mheap.h>
#include <clib/os.h>

void clib_smp_free (clib_smp_main_t * m)
{
  clib_mem_vm_free (m->vm_base, (uword) ((1 + m->n_cpus) << m->log2_n_per_cpu_vm_bytes));
}

static uword allocate_per_cpu_mheap (uword cpu)
{
  clib_smp_main_t * m = &clib_smp_main;
  void * heap;
  uword vm_size, stack_size;

  ASSERT (os_get_cpu_number () == cpu);

  vm_size = (uword) 1 << m->log2_n_per_cpu_vm_bytes;
  stack_size = (uword) 1 << m->log2_n_per_cpu_stack_bytes;

  /* Heap extends up to start of stack. */
  heap = mheap_alloc_with_flags (clib_smp_vm_base_for_cpu (m, cpu),
				 vm_size - stack_size,
				 /* flags */ 0);
  clib_mem_set_heap (heap);

  if (cpu == 0)
    {
      /* Now that we have a heap, allocate main structure on cpu 0. */
      vec_resize (m->per_cpu_mains, m->n_cpus);

      /* Allocate shared global heap (thread safe). */
      m->global_heap =
	mheap_alloc_with_flags (clib_smp_vm_base_for_cpu (m, cpu + m->n_cpus),
				vm_size,
				/* flags */ MHEAP_FLAG_THREAD_SAFE);
    }

  m->per_cpu_mains[cpu].heap = heap;
  return 0;
}

void clib_smp_init (void)
{
  clib_smp_main_t * m = &clib_smp_main;
  uword cpu;

  m->vm_base = clib_mem_vm_alloc ((uword) (m->n_cpus + 1) << m->log2_n_per_cpu_vm_bytes);
  if (! m->vm_base)
    clib_error ("error allocating virtual memory");

  for (cpu = 0; cpu < m->n_cpus; cpu++)
    clib_calljmp (allocate_per_cpu_mheap, cpu,
		  clib_smp_stack_top_for_cpu (m, cpu));
}
