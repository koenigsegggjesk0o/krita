/* Class KisReselectActiveSelectionCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisReselectActiveSelectionCommand @ 00366a60 ======

/* KisReselectActiveSelectionCommand::KisReselectActiveSelectionCommand(KisSharedPtr<KisNode>,
   KisWeakSharedPtr<KisImage>, KUndo2Command*) */

void __thiscall
KisReselectActiveSelectionCommand::KisReselectActiveSelectionCommand
          (KisReselectActiveSelectionCommand *this,KisSharedPtr param_1,KisWeakSharedPtr param_2,
          KUndo2Command *param_3)

{
  int iVar1;
  long lVar2;
  undefined auVar3 [8];
  undefined auVar4 [16];
  int *piVar5;
  undefined4 in_register_00000014;
  long *plVar6;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  undefined local_38 [8];
  int *piStack_30;
  long local_20;
  
  plVar6 = (long *)CONCAT44(in_register_00000014,param_2);
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar6 == 0) {
    local_38 = (undefined  [8])0x0;
    auVar3 = local_38;
  }
  else {
    if (((uint *)plVar6[1] == (uint *)0x0) || ((*(uint *)plVar6[1] & 1) == 0)) {
      local_38 = (undefined  [8])0x0;
      piStack_30 = (int *)0x0;
      goto LAB_00366aad;
    }
    auVar3 = (undefined  [8])*plVar6;
    piStack_30 = (int *)local_38;
    local_38 = auVar3;
    if (auVar3 != (undefined  [8])0x0) {
      piVar5 = *(int **)((long)auVar3 + 0x58);
      if (piVar5 == (int *)0x0) {
        piVar5 = (int *)operator_new(4);
        *piVar5 = 0;
        *(int **)((long)auVar3 + 0x58) = piVar5;
        LOCK();
        *piVar5 = *piVar5 + 1;
        UNLOCK();
        piVar5 = *(int **)((long)auVar3 + 0x58);
        auVar3 = local_38;
      }
      local_38 = auVar3;
      LOCK();
      *piVar5 = *piVar5 + 2;
      UNLOCK();
      piStack_30 = piVar5;
      goto LAB_00366aad;
    }
  }
  local_38 = auVar3;
  piStack_30 = (int *)0x0;
LAB_00366aad:
                    /* try { // try from 00366ab5 to 00366ab9 has its CatchHandler @ 00366baf */
  KisReselectGlobalSelectionCommand::KisReselectGlobalSelectionCommand
            ((KisReselectGlobalSelectionCommand *)this,(KisWeakSharedPtr)local_38,param_3);
  piVar5 = piStack_30;
  auVar4._8_8_ = 0;
  auVar4._0_8_ = piStack_30;
  _local_38 = auVar4 << 0x40;
  if (piVar5 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar5;
    *piVar5 = *piVar5 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar5 != (int *)0x0)) {
      operator_delete(piVar5,4);
    }
  }
  *(undefined **)this = PTR_vtable_00837ba0 + 0x10;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x58) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
  *(undefined8 *)(this + 0x60) = 0;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



