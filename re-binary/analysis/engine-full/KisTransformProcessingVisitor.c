/* Class KisTransformProcessingVisitor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTransformProcessingVisitor @ 00201350 ======

void __thiscall
KisTransformProcessingVisitor::KisTransformProcessingVisitor
          (KisTransformProcessingVisitor *this,double param_1,double param_2,double param_3,
          double param_4,double param_5,double param_6,double param_7,KisFilterStrategy *param_8,
          QTransform *param_9)

{
  (*(code *)PTR_KisTransformProcessingVisitor_00838478)();
  return;
}



// ====== KisTransformProcessingVisitor @ 0037b8f0 ======

/* KisTransformProcessingVisitor::KisTransformProcessingVisitor(double, double, double, double,
   double, double, double, KisFilterStrategy*, QTransform const&) */

void __thiscall
KisTransformProcessingVisitor::KisTransformProcessingVisitor
          (KisTransformProcessingVisitor *this,double param_1,double param_2,double param_3,
          double param_4,double param_5,double param_6,double param_7,KisFilterStrategy *param_8,
          QTransform *param_9)

{
  long *plVar1;
  undefined8 uVar2;
  undefined *puVar3;
  long in_FS_OFFSET;
  long *local_50;
  undefined local_48 [16];
  code *local_38;
  undefined8 local_30;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)(this + 8));
  puVar3 = PTR_vtable_00837468;
  *(KisFilterStrategy **)(this + 0x48) = param_8;
  *(double *)(this + 0x18) = param_1;
  *(double *)(this + 0x20) = param_2;
  *(undefined **)this = puVar3 + 0x10;
  *(double *)(this + 0x28) = param_6;
  *(double *)(this + 0x30) = param_7;
  *(double *)(this + 0x38) = param_3;
  *(double *)(this + 0x40) = param_4;
  *(double *)(this + 0x50) = param_5;
  uVar2 = *(undefined8 *)(param_9 + 2);
  local_38 = (code *)0x0;
  *(undefined8 *)(this + 0x58) = *(undefined8 *)param_9;
  *(undefined8 *)(this + 0x60) = uVar2;
  uVar2 = *(undefined8 *)(param_9 + 6);
  local_30 = 0;
  *(undefined8 *)(this + 0x68) = *(undefined8 *)(param_9 + 4);
  *(undefined8 *)(this + 0x70) = uVar2;
  uVar2 = *(undefined8 *)(param_9 + 10);
  local_50 = (long *)0x0;
  *(undefined8 *)(this + 0x78) = *(undefined8 *)(param_9 + 8);
  *(undefined8 *)(this + 0x80) = uVar2;
  uVar2 = *(undefined8 *)(param_9 + 0xe);
  local_48 = (undefined  [16])0x0;
  *(undefined8 *)(this + 0x88) = *(undefined8 *)(param_9 + 0xc);
  *(undefined8 *)(this + 0x90) = uVar2;
  uVar2 = *(undefined8 *)(param_9 + 0x12);
  *(undefined8 *)(this + 0x98) = *(undefined8 *)(param_9 + 0x10);
  *(undefined8 *)(this + 0xa0) = uVar2;
  *(undefined8 *)(this + 0xa8) = *(undefined8 *)(param_9 + 0x14);
                    /* try { // try from 0037b9ee to 0037b9f2 has its CatchHandler @ 0037ba45 */
  KisSelectionBasedProcessingHelper::KisSelectionBasedProcessingHelper
            ((KisSelectionBasedProcessingHelper *)(this + 0xb0),(KisSharedPtr)&local_50,
             (function)local_48);
  if (local_50 != (long *)0x0) {
    LOCK();
    plVar1 = local_50 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_50 + 8))();
    }
  }
  if (local_38 != (code *)0x0) {
    (*local_38)(local_48,local_48,3);
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



