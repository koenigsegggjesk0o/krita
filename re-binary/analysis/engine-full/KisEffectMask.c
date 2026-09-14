/* Class KisEffectMask - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisEffectMask @ 00203fd0 ======

void __thiscall
KisEffectMask::KisEffectMask(KisEffectMask *this,KisWeakSharedPtr param_1,QString *param_2)

{
  (*(code *)PTR_KisEffectMask_00839ab8)();
  return;
}



// ====== KisEffectMask @ 00207f00 ======

void __thiscall KisEffectMask::KisEffectMask(KisEffectMask *this,KisEffectMask *param_1)

{
  (*(code *)PTR_KisEffectMask_0083ba50)();
  return;
}



// ====== KisEffectMask @ 004b1a90 ======

/* KisEffectMask::KisEffectMask(KisWeakSharedPtr<KisImage>, QString const&) */

void __thiscall
KisEffectMask::KisEffectMask(KisEffectMask *this,KisWeakSharedPtr param_1,QString *param_2)

{
  int iVar1;
  undefined auVar2 [8];
  undefined auVar3 [16];
  undefined *puVar4;
  int *piVar5;
  undefined4 in_register_00000034;
  long *plVar6;
  long in_FS_OFFSET;
  undefined local_38 [8];
  int *piStack_30;
  long local_20;
  
  plVar6 = (long *)CONCAT44(in_register_00000034,param_1);
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar6 == 0) {
    local_38 = (undefined  [8])0x0;
    auVar2 = local_38;
  }
  else {
    if (((uint *)plVar6[1] == (uint *)0x0) || ((*(uint *)plVar6[1] & 1) == 0)) {
      local_38 = (undefined  [8])0x0;
      piStack_30 = (int *)0x0;
      goto LAB_004b1ad1;
    }
    auVar2 = (undefined  [8])*plVar6;
    piStack_30 = (int *)local_38;
    local_38 = auVar2;
    if (auVar2 != (undefined  [8])0x0) {
      piVar5 = *(int **)((long)auVar2 + 0x58);
      if (piVar5 == (int *)0x0) {
        piVar5 = (int *)operator_new(4);
        *piVar5 = 0;
        *(int **)((long)auVar2 + 0x58) = piVar5;
        LOCK();
        *piVar5 = *piVar5 + 1;
        UNLOCK();
        piVar5 = *(int **)((long)auVar2 + 0x58);
        auVar2 = local_38;
      }
      local_38 = auVar2;
      LOCK();
      *piVar5 = *piVar5 + 2;
      UNLOCK();
      piStack_30 = piVar5;
      goto LAB_004b1ad1;
    }
  }
  local_38 = auVar2;
  piStack_30 = (int *)0x0;
LAB_004b1ad1:
                    /* try { // try from 004b1ad9 to 004b1add has its CatchHandler @ 004b1bbd */
  KisMask::KisMask((KisMask *)this,(KisWeakSharedPtr)local_38,param_2);
  piVar5 = piStack_30;
  auVar3._8_8_ = 0;
  auVar3._0_8_ = piStack_30;
  _local_38 = auVar3 << 0x40;
  if (piVar5 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar5;
    *piVar5 = *piVar5 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar5 != (int *)0x0)) {
      operator_delete(piVar5,4);
    }
  }
  puVar4 = PTR_vtable_008373f8 + 0x238;
  *(undefined **)this = PTR_vtable_008373f8 + 0x10;
  *(undefined **)(this + 0x30) = puVar4;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisEffectMask @ 004b1c70 ======

/* KisEffectMask::KisEffectMask(KisEffectMask const&) */

void __thiscall KisEffectMask::KisEffectMask(KisEffectMask *this,KisEffectMask *param_1)

{
  undefined *puVar1;
  
  KisMask::KisMask((KisMask *)this,(KisMask *)param_1);
  puVar1 = PTR_vtable_008373f8 + 0x238;
  *(undefined **)this = PTR_vtable_008373f8 + 0x10;
  *(undefined **)(this + 0x30) = puVar1;
  return;
}



