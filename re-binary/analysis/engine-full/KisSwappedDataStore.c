/* Class KisSwappedDataStore - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSwappedDataStore @ 00209b60 ======

void __thiscall KisSwappedDataStore::KisSwappedDataStore(KisSwappedDataStore *this)

{
  (*(code *)PTR_KisSwappedDataStore_0083c880)();
  return;
}



// ====== KisSwappedDataStore @ 00308970 ======

/* KisSwappedDataStore::KisSwappedDataStore() */

void __thiscall KisSwappedDataStore::KisSwappedDataStore(KisSwappedDataStore *this)

{
  undefined *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  KisChunkAllocator *this_00;
  KisMemoryWindow *this_01;
  KisTileCompressor2 *this_02;
  long in_FS_OFFSET;
  QArrayData *local_60;
  KisImageConfig local_58 [24];
  long local_40;
  
  puVar1 = PTR_shared_null_008377d0;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  *(undefined8 *)(this + 0x20) = 0;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined **)this = puVar1;
                    /* try { // try from 003089ba to 003089be has its CatchHandler @ 00308af2 */
  KisImageConfig::KisImageConfig(local_58,true);
                    /* try { // try from 003089c4 to 003089f7 has its CatchHandler @ 00308ae6 */
  iVar2 = KisImageConfig::maxSwapSize(local_58,false);
  iVar3 = KisImageConfig::swapSlabSize(local_58);
  iVar4 = KisImageConfig::swapWindowSize(local_58);
  this_00 = (KisChunkAllocator *)operator_new(0x28);
                    /* try { // try from 00308a04 to 00308a08 has its CatchHandler @ 00308ac2 */
  KisChunkAllocator::KisChunkAllocator(this_00,(long)iVar3 << 0x14,(long)iVar2 << 0x14);
  *(KisChunkAllocator **)(this + 0x10) = this_00;
                    /* try { // try from 00308a12 to 00308a16 has its CatchHandler @ 00308ae6 */
  this_01 = (KisMemoryWindow *)operator_new(0x58);
                    /* try { // try from 00308a27 to 00308a2b has its CatchHandler @ 00308ab6 */
  KisImageConfig::swapDir((KisImageConfig *)&local_60,SUB81(local_58,0));
                    /* try { // try from 00308a35 to 00308a39 has its CatchHandler @ 00308ace */
  KisMemoryWindow::KisMemoryWindow(this_01,(QString *)&local_60,(long)iVar4 << 0x14);
  *(KisMemoryWindow **)(this + 0x18) = this_01;
  if (*(int *)local_60 != 0) {
    if (*(int *)local_60 == -1) goto LAB_00308a59;
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 != 0) goto LAB_00308a59;
  }
  QArrayData::deallocate(local_60,2,8);
LAB_00308a59:
                    /* try { // try from 00308a5e to 00308a62 has its CatchHandler @ 00308ae6 */
  this_02 = (KisTileCompressor2 *)operator_new(0x38);
                    /* try { // try from 00308a69 to 00308a6d has its CatchHandler @ 00308ada */
  KisTileCompressor2::KisTileCompressor2(this_02);
  *(KisTileCompressor2 **)(this + 8) = this_02;
  KisImageConfig::~KisImageConfig(local_58);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



