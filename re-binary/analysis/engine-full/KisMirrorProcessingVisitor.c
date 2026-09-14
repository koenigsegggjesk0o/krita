/* Class KisMirrorProcessingVisitor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMirrorProcessingVisitor @ 002069a0 ======

void __thiscall
KisMirrorProcessingVisitor::KisMirrorProcessingVisitor
          (KisMirrorProcessingVisitor *this,QRect *param_1,Orientation param_2)

{
  (*(code *)PTR_KisMirrorProcessingVisitor_0083afa0)();
  return;
}



// ====== KisMirrorProcessingVisitor @ 0037d710 ======

/* KisMirrorProcessingVisitor::KisMirrorProcessingVisitor(QRect const&, Qt::Orientation) */

void __thiscall
KisMirrorProcessingVisitor::KisMirrorProcessingVisitor
          (KisMirrorProcessingVisitor *this,QRect *param_1,Orientation param_2)

{
  long *plVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined *puVar5;
  int iVar6;
  undefined8 *puVar7;
  long in_FS_OFFSET;
  long *local_50;
  undefined local_48 [16];
  code *local_38;
  code *pcStack_30;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)(this + 8));
  puVar5 = PTR_vtable_00837008;
  *(Orientation *)(this + 0x28) = param_2;
  uVar3 = *(undefined8 *)param_1;
  uVar4 = *(undefined8 *)(param_1 + 8);
  *(undefined8 *)(this + 0x30) = 0;
  local_48 = (undefined  [16])0x0;
  *(undefined **)this = puVar5 + 0x10;
  *(undefined8 *)(this + 0x18) = uVar3;
  *(undefined8 *)(this + 0x20) = uVar4;
  local_38 = (code *)0x0;
  pcStack_30 = (code *)0x0;
                    /* try { // try from 0037d780 to 0037d784 has its CatchHandler @ 0037d88c */
  puVar7 = (undefined8 *)operator_new(0x18);
  puVar5 = PTR_mirrorDevice_00837030;
  puVar7[2] = this;
  puVar7[1] = 0;
  *puVar7 = puVar5;
  local_48._0_8_ = puVar7;
  local_50 = (long *)0x0;
  local_38 = FUN_0037dde0;
  pcStack_30 = FUN_0037def0;
                    /* try { // try from 0037d7de to 0037d7e2 has its CatchHandler @ 0037d880 */
  KisSelectionBasedProcessingHelper::KisSelectionBasedProcessingHelper
            ((KisSelectionBasedProcessingHelper *)(this + 0x38),(KisSharedPtr)&local_50,
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
  if (*(int *)(this + 0x28) == 1) {
    iVar2 = *(int *)(this + 0x18);
    iVar6 = *(int *)(this + 0x20);
  }
  else {
    iVar2 = *(int *)(this + 0x1c);
    iVar6 = *(int *)(this + 0x24);
  }
  *(double *)(this + 0x30) = (double)((iVar6 - iVar2) + 1) * DAT_007227c8 + (double)iVar2;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisMirrorProcessingVisitor @ 0037d8a0 ======

/* KisMirrorProcessingVisitor::KisMirrorProcessingVisitor(KisSharedPtr<KisSelection>,
   Qt::Orientation) */

void __thiscall
KisMirrorProcessingVisitor::KisMirrorProcessingVisitor
          (KisMirrorProcessingVisitor *this,KisSharedPtr param_1,Orientation param_2)

{
  long *plVar1;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  undefined local_48 [16];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  local_48 = KisSelection::selectedExactRect
                       (*(KisSelection **)CONCAT44(in_register_00000034,param_1));
  KisMirrorProcessingVisitor(this,(QRect *)local_48,param_2);
  local_48._0_8_ = *(long *)CONCAT44(in_register_00000034,param_1);
  if ((long *)local_48._0_8_ != (long *)0x0) {
    LOCK();
    *(int *)(local_48._0_8_ + 8) = *(int *)(local_48._0_8_ + 8) + 1;
    UNLOCK();
  }
                    /* try { // try from 0037d905 to 0037d909 has its CatchHandler @ 0037d945 */
  KisSelectionBasedProcessingHelper::setSelection
            ((KisSelectionBasedProcessingHelper *)(this + 0x38),(KisSharedPtr)local_48);
  if ((long *)local_48._0_8_ != (long *)0x0) {
    LOCK();
    plVar1 = (long *)(local_48._0_8_ + 8);
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*(long *)local_48._0_8_ + 8))();
    }
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



