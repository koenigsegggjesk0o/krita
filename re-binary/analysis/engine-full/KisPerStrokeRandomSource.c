/* Class KisPerStrokeRandomSource - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPerStrokeRandomSource @ 00202ed0 ======

void __thiscall
KisPerStrokeRandomSource::KisPerStrokeRandomSource
          (KisPerStrokeRandomSource *this,KisPerStrokeRandomSource *param_1)

{
  (*(code *)PTR_KisPerStrokeRandomSource_00839238)();
  return;
}



// ====== KisPerStrokeRandomSource @ 00203070 ======

void __thiscall KisPerStrokeRandomSource::KisPerStrokeRandomSource(KisPerStrokeRandomSource *this)

{
  (*(code *)PTR_KisPerStrokeRandomSource_00839308)();
  return;
}



// ====== KisPerStrokeRandomSource @ 00336060 ======

/* KisPerStrokeRandomSource::KisPerStrokeRandomSource() */

void __thiscall KisPerStrokeRandomSource::KisPerStrokeRandomSource(KisPerStrokeRandomSource *this)

{
  undefined4 *puVar1;
  void *pvVar2;
  long in_FS_OFFSET;
  undefined4 local_24;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)this);
                    /* try { // try from 00336089 to 0033608d has its CatchHandler @ 003360f8 */
  puVar1 = (undefined4 *)operator_new(0x20);
                    /* try { // try from 00336091 to 003360a7 has its CatchHandler @ 003360ec */
  pvVar2 = (void *)QRandomGenerator64::global();
  QRandomGenerator::_fillRange(pvVar2,&local_24);
  *(undefined8 *)(puVar1 + 6) = 0;
  *(undefined4 **)(this + 0x10) = puVar1;
  *puVar1 = local_24;
  *(undefined **)(puVar1 + 4) = PTR_shared_null_00836c40;
  *(undefined8 *)(puVar1 + 2) = 0xffffffff;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPerStrokeRandomSource @ 00336110 ======

/* KisPerStrokeRandomSource::KisPerStrokeRandomSource(KisPerStrokeRandomSource const&) */

void __thiscall
KisPerStrokeRandomSource::KisPerStrokeRandomSource
          (KisPerStrokeRandomSource *this,KisPerStrokeRandomSource *param_1)

{
  code *pcVar1;
  undefined4 *puVar2;
  long lVar3;
  _func_void_Node_ptr_void_ptr *p_Var4;
  undefined4 *puVar5;
  undefined8 uVar6;
  _func_void_Node_ptr *p_Var7;
  
  KisShared::KisShared((KisShared *)this);
                    /* try { // try from 0033612e to 00336132 has its CatchHandler @ 003361fa */
  puVar5 = (undefined4 *)operator_new(0x20);
  puVar2 = *(undefined4 **)(param_1 + 0x10);
  *puVar5 = *puVar2;
  *(undefined8 *)(puVar5 + 2) = *(undefined8 *)(puVar2 + 2);
  lVar3 = *(long *)(puVar2 + 4);
  *(long *)(puVar5 + 4) = lVar3;
  if (1 < *(int *)(lVar3 + 0x10) + 1U) {
    LOCK();
    *(int *)(lVar3 + 0x10) = *(int *)(lVar3 + 0x10) + 1;
    UNLOCK();
  }
  p_Var4 = *(_func_void_Node_ptr_void_ptr **)(puVar5 + 4);
  if ((((byte)p_Var4[0x28] & 1) != 0) || (*(uint *)(p_Var4 + 0x10) < 2)) goto LAB_00336164;
                    /* try { // try from 003361a4 to 003361eb has its CatchHandler @ 003361ee */
  uVar6 = QHashData::detach_helper(p_Var4,FUN_00336720,0x336760,0x20);
  p_Var7 = *(_func_void_Node_ptr **)(puVar5 + 4);
  pcVar1 = p_Var7 + 0x10;
  if (*(int *)(p_Var7 + 0x10) == 0) {
LAB_003361e4:
    QHashData::free_helper(p_Var7);
  }
  else if (*(int *)(p_Var7 + 0x10) != -1) {
    LOCK();
    *(int *)pcVar1 = *(int *)pcVar1 + -1;
    UNLOCK();
    if (*(int *)pcVar1 == 0) {
      p_Var7 = *(_func_void_Node_ptr **)(puVar5 + 4);
      goto LAB_003361e4;
    }
  }
  *(undefined8 *)(puVar5 + 4) = uVar6;
LAB_00336164:
  *(undefined8 *)(puVar5 + 6) = 0;
  *(undefined4 **)(this + 0x10) = puVar5;
  return;
}



