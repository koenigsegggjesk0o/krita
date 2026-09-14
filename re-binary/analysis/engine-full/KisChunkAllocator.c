/* Class KisChunkAllocator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisChunkAllocator @ 00202eb0 ======

void __thiscall
KisChunkAllocator::KisChunkAllocator(KisChunkAllocator *this,ulonglong param_1,ulonglong param_2)

{
  (*(code *)PTR_KisChunkAllocator_00839228)();
  return;
}



// ====== KisChunkAllocator @ 00306e90 ======

/* KisChunkAllocator::KisChunkAllocator(unsigned long long, unsigned long long) */

void __thiscall
KisChunkAllocator::KisChunkAllocator(KisChunkAllocator *this,ulonglong param_1,ulonglong param_2)

{
  undefined *puVar1;
  long in_FS_OFFSET;
  undefined *local_30;
  undefined local_28 [8];
  long local_20;
  
  puVar1 = PTR_shared_null_00837348;
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  *(undefined8 *)(this + 0x18) = 0;
  *(ulonglong *)this = param_2;
  *(ulonglong *)(this + 8) = param_1;
  *(undefined **)(this + 0x10) = puVar1;
  if (1 < *(uint *)(puVar1 + 0x10)) {
    local_30 = puVar1;
                    /* try { // try from 00306f13 to 00306f17 has its CatchHandler @ 00306f1f */
    FUN_00308170(local_28,this + 0x10,&local_30);
  }
  *(undefined8 *)(this + 0x18) = **(undefined8 **)(this + 0x10);
  *(undefined8 *)(this + 0x20) = *(undefined8 *)(this + 8);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



