/* Class KisProcessingInformation - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisProcessingInformation @ 00202050 ======

void __thiscall
KisProcessingInformation::KisProcessingInformation
          (KisProcessingInformation *this,KisSharedPtr param_1,QPoint *param_2,KisSharedPtr param_3)

{
  (*(code *)PTR_KisProcessingInformation_00838af8)();
  return;
}



// ====== KisProcessingInformation @ 00205c90 ======

void __thiscall
KisProcessingInformation::KisProcessingInformation
          (KisProcessingInformation *this,KisProcessingInformation *param_1)

{
  (*(code *)PTR_KisProcessingInformation_0083a918)();
  return;
}



// ====== KisProcessingInformation @ 005e8050 ======

/* KisProcessingInformation::KisProcessingInformation(KisSharedPtr<KisPaintDevice>, QPoint const&,
   KisSharedPtr<KisSelection>) */

void __thiscall
KisProcessingInformation::KisProcessingInformation
          (KisProcessingInformation *this,KisSharedPtr param_1,QPoint *param_2,KisSharedPtr param_3)

{
  long lVar1;
  long *plVar2;
  long *plVar3;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_30;
  long *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_28 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (local_28 != (long *)0x0) {
    LOCK();
    *(int *)(local_28 + 1) = *(int *)(local_28 + 1) + 1;
    UNLOCK();
  }
  local_30 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_30 != (long *)0x0) {
    LOCK();
    *(int *)(local_30 + 2) = *(int *)(local_30 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 005e80a0 to 005e80a4 has its CatchHandler @ 005e8160 */
  KisConstProcessingInformation::KisConstProcessingInformation
            ((KisConstProcessingInformation *)this,(KisSharedPtr)&local_30,param_2,
             (KisSharedPtr)&local_28);
  if (local_30 != (long *)0x0) {
    LOCK();
    plVar3 = local_30 + 2;
    *(int *)plVar3 = *(int *)plVar3 + -1;
    UNLOCK();
    if (*(int *)plVar3 == 0) {
      (**(code **)(*local_30 + 0x20))();
    }
  }
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar3 = local_28 + 1;
    *(int *)plVar3 = *(int *)plVar3 + -1;
    UNLOCK();
    if (*(int *)plVar3 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
                    /* try { // try from 005e80cc to 005e80d0 has its CatchHandler @ 005e8154 */
  plVar3 = (long *)operator_new(8);
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *plVar3 = 0;
  *(long **)(this + 8) = plVar3;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
    plVar2 = (long *)*plVar3;
    *plVar3 = lVar1;
    if (plVar2 != (long *)0x0) {
      LOCK();
      plVar3 = plVar2 + 2;
      *(int *)plVar3 = *(int *)plVar3 + -1;
      UNLOCK();
      if (*(int *)plVar3 == 0) {
        if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x005e814d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          (**(code **)(*plVar2 + 0x20))();
          return;
        }
        goto LAB_005e814f;
      }
    }
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_005e814f:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisProcessingInformation @ 005e8170 ======

/* KisProcessingInformation::KisProcessingInformation(KisProcessingInformation const&) */

void __thiscall
KisProcessingInformation::KisProcessingInformation
          (KisProcessingInformation *this,KisProcessingInformation *param_1)

{
  long lVar1;
  long *plVar2;
  
  KisConstProcessingInformation::KisConstProcessingInformation
            ((KisConstProcessingInformation *)this,(KisConstProcessingInformation *)param_1);
                    /* try { // try from 005e818a to 005e818e has its CatchHandler @ 005e81ae */
  plVar2 = (long *)operator_new(8);
  lVar1 = **(long **)(param_1 + 8);
  *plVar2 = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  *(long **)(this + 8) = plVar2;
  return;
}



