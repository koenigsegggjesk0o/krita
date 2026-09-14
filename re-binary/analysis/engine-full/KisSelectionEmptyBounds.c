/* Class KisSelectionEmptyBounds - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSelectionEmptyBounds @ 002026b0 ======

void __thiscall
KisSelectionEmptyBounds::KisSelectionEmptyBounds
          (KisSelectionEmptyBounds *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisSelectionEmptyBounds_00838e28)();
  return;
}



// ====== KisSelectionEmptyBounds @ 00205430 ======

void __thiscall KisSelectionEmptyBounds::KisSelectionEmptyBounds(KisSelectionEmptyBounds *this)

{
  (*(code *)PTR_KisSelectionEmptyBounds_0083a4e8)();
  return;
}



// ====== KisSelectionEmptyBounds @ 004af250 ======

/* KisSelectionEmptyBounds::KisSelectionEmptyBounds(KisWeakSharedPtr<KisImage>) */

void __thiscall
KisSelectionEmptyBounds::KisSelectionEmptyBounds
          (KisSelectionEmptyBounds *this,KisWeakSharedPtr param_1)

{
  int iVar1;
  undefined auVar2 [8];
  undefined auVar3 [16];
  int *piVar4;
  undefined4 in_register_00000034;
  long *plVar5;
  long in_FS_OFFSET;
  undefined local_38 [8];
  int *piStack_30;
  long local_20;
  
  plVar5 = (long *)CONCAT44(in_register_00000034,param_1);
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar5 == 0) {
    local_38 = (undefined  [8])0x0;
    auVar2 = local_38;
  }
  else {
    if (((uint *)plVar5[1] == (uint *)0x0) || ((*(uint *)plVar5[1] & 1) == 0)) {
      local_38 = (undefined  [8])0x0;
      piStack_30 = (int *)0x0;
      goto LAB_004af28c;
    }
    auVar2 = (undefined  [8])*plVar5;
    piStack_30 = (int *)local_38;
    local_38 = auVar2;
    if (auVar2 != (undefined  [8])0x0) {
      piVar4 = *(int **)((long)auVar2 + 0x58);
      if (piVar4 == (int *)0x0) {
        piVar4 = (int *)operator_new(4);
        *piVar4 = 0;
        *(int **)((long)auVar2 + 0x58) = piVar4;
        LOCK();
        *piVar4 = *piVar4 + 1;
        UNLOCK();
        piVar4 = *(int **)((long)auVar2 + 0x58);
        auVar2 = local_38;
      }
      local_38 = auVar2;
      LOCK();
      *piVar4 = *piVar4 + 2;
      UNLOCK();
      piStack_30 = piVar4;
      goto LAB_004af28c;
    }
  }
  local_38 = auVar2;
  piStack_30 = (int *)0x0;
LAB_004af28c:
                    /* try { // try from 004af295 to 004af299 has its CatchHandler @ 004af363 */
  KisDefaultBounds::KisDefaultBounds((KisDefaultBounds *)this,(KisWeakSharedPtr)local_38);
  piVar4 = piStack_30;
  auVar3._8_8_ = 0;
  auVar3._0_8_ = piStack_30;
  _local_38 = auVar3 << 0x40;
  if (piVar4 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar4;
    *piVar4 = *piVar4 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar4 != (int *)0x0)) {
      operator_delete(piVar4,4);
    }
  }
  *(undefined **)this = PTR_vtable_00837548 + 0x10;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSelectionEmptyBounds @ 004af370 ======

/* KisSelectionEmptyBounds::KisSelectionEmptyBounds() */

void __thiscall KisSelectionEmptyBounds::KisSelectionEmptyBounds(KisSelectionEmptyBounds *this)

{
  int iVar1;
  undefined auVar2 [16];
  undefined8 uVar3;
  long in_FS_OFFSET;
  undefined local_38 [24];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_38._0_16_ = (undefined  [16])0x0;
                    /* try { // try from 004af398 to 004af39c has its CatchHandler @ 004af3f3 */
  KisSelectionEmptyBounds(this,(KisWeakSharedPtr)local_38);
  uVar3 = local_38._8_8_;
  auVar2._8_8_ = 0;
  auVar2._0_8_ = local_38._8_8_;
  local_38._0_16_ = auVar2 << 0x40;
  if ((int *)uVar3 != (int *)0x0) {
    LOCK();
    iVar1 = *(int *)uVar3;
    *(int *)uVar3 = *(int *)uVar3 + -2;
    UNLOCK();
    if ((iVar1 < 3) && ((int *)uVar3 != (int *)0x0)) {
      operator_delete((void *)uVar3,4);
    }
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



