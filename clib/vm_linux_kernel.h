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

#ifndef included_vm_linux_kernel_h
#define included_vm_linux_kernel_h

#include <linux/vmalloc.h>
#include <linux/gfp.h>		/* for GFP_* */
#include <asm/pgtable.h>       /* for PAGE_KERNEL */

/* Allocate virtual address space. */
always_inline void * clib_mem_vm_alloc (uword size)
{ return vmalloc (size); }

always_inline void clib_mem_vm_free (void * addr, uword size)
{ vfree (addr); }

always_inline void * clib_mem_vm_unmap (void * addr, uword size)
{ return 0; }

always_inline void * clib_mem_vm_map (void * addr, uword size)
{ return addr; }

#endif /* included_vm_linux_kernel_h */
