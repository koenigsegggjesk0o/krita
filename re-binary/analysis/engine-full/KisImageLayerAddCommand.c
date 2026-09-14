/* Class KisImageLayerAddCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageLayerAddCommand @ 00203940 ======

void __thiscall
KisImageLayerAddCommand::KisImageLayerAddCommand
          (KisImageLayerAddCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KisSharedPtr param_3,KisSharedPtr param_4,bool param_5,bool param_6)

{
  (*(code *)PTR_KisImageLayerAddCommand_00839770)();
  return;
}



// ====== KisImageLayerAddCommand @ 00209990 ======

void __thiscall
KisImageLayerAddCommand::KisImageLayerAddCommand
          (KisImageLayerAddCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KisSharedPtr param_3,uint param_4,bool param_5,bool param_6)

{
  (*(code *)PTR_KisImageLayerAddCommand_0083c798)();
  return;
}



// ====== KisImageLayerAddCommand @ 0035cb90 ======

/* KisImageLayerAddCommand::KisImageLayerAddCommand(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisNode>, KisSharedPtr<KisNode>, KisSharedPtr<KisNode>, bool, bool) */

void __thiscall
KisImageLayerAddCommand::KisImageLayerAddCommand
          (KisImageLayerAddCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KisSharedPtr param_3,KisSharedPtr param_4,bool param_5,bool param_6)

{
  int iVar1;
  long lVar2;
  ulong uVar3;
  undefined auVar4 [16];
  undefined *puVar5;
  undefined8 uVar6;
  long *plVar7;
  int *piVar8;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  ulong *puVar9;
  long *plVar10;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  QArrayData *local_68;
  QArrayData *local_60;
  undefined local_58 [24];
  long local_40;
  
  puVar9 = (ulong *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*puVar9 == 0) {
    local_58._0_8_ = 0;
    uVar3 = local_58._0_8_;
LAB_0035ce49:
    local_58._0_8_ = uVar3;
    local_58._8_8_ = 0;
  }
  else if (((uint *)puVar9[1] == (uint *)0x0) || ((*(uint *)puVar9[1] & 1) == 0)) {
    local_58._0_16_ = (undefined  [16])0x0;
  }
  else {
    uVar3 = *puVar9;
    local_58._8_8_ = local_58._0_8_;
    local_58._0_8_ = uVar3;
    if (uVar3 == 0) goto LAB_0035ce49;
    piVar8 = *(int **)(uVar3 + 0x58);
    if (piVar8 == (int *)0x0) {
      piVar8 = (int *)operator_new(4);
      *piVar8 = 0;
      *(int **)(uVar3 + 0x58) = piVar8;
      LOCK();
      *piVar8 = *piVar8 + 1;
      UNLOCK();
      piVar8 = *(int **)(uVar3 + 0x58);
      uVar3 = local_58._0_8_;
    }
    local_58._0_8_ = uVar3;
    local_58._8_8_ = piVar8;
    LOCK();
    *piVar8 = *piVar8 + 2;
    UNLOCK();
  }
                    /* try { // try from 0035cc0e to 0035cc12 has its CatchHandler @ 0035cefa */
  ki18ndc((char *)&local_60,"krita","(qtundo-format)");
                    /* try { // try from 0035cc1e to 0035cc22 has its CatchHandler @ 0035ceee */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_60);
                    /* try { // try from 0035cc31 to 0035cc35 has its CatchHandler @ 0035ced6 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_60,(QString *)&local_68);
  if (*(int *)local_68 == 0) {
LAB_0035cdf0:
    QArrayData::deallocate(local_68,2,8);
  }
  else if (*(int *)local_68 != -1) {
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 == 0) goto LAB_0035cdf0;
  }
                    /* try { // try from 0035cc69 to 0035cc6d has its CatchHandler @ 0035cee2 */
  KisImageCommand::KisImageCommand
            ((KisImageCommand *)this,(KUndo2MagicString *)&local_60,(KisWeakSharedPtr)local_58,
             (KUndo2Command *)0x0);
  if (*(int *)local_60 == 0) {
LAB_0035ce08:
    QArrayData::deallocate(local_60,2,8);
  }
  else if (*(int *)local_60 != -1) {
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 == 0) goto LAB_0035ce08;
  }
  uVar6 = local_58._8_8_;
  auVar4._8_8_ = 0;
  auVar4._0_8_ = local_58._8_8_;
  local_58._0_16_ = auVar4 << 0x40;
  if ((int *)uVar6 != (int *)0x0) {
    LOCK();
    iVar1 = *(int *)uVar6;
    *(int *)uVar6 = *(int *)uVar6 + -2;
    UNLOCK();
    if ((iVar1 < 3) && ((int *)uVar6 != (int *)0x0)) {
      operator_delete((void *)uVar6,4);
    }
  }
  puVar5 = PTR_vtable_00837240;
  this[0x54] = (KisImageLayerAddCommand)param_5;
  *(undefined8 *)(this + 0x48) = 0;
  *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
  *(undefined **)this = puVar5 + 0x10;
  *(undefined4 *)(this + 0x50) = 0xffffffff;
  lVar2 = *(long *)CONCAT44(in_register_00000014,param_2);
  this[0x55] = (KisImageLayerAddCommand)param_6;
  *(undefined4 *)(this + 0x58) = 0;
  if (lVar2 == 0) {
    plVar7 = *(long **)CONCAT44(in_register_0000000c,param_3);
    if (plVar7 != (long *)0x0) {
LAB_0035cd2d:
      LOCK();
      *(int *)(plVar7 + 2) = *(int *)(plVar7 + 2) + 1;
      UNLOCK();
      plVar10 = *(long **)(this + 0x40);
      goto LAB_0035cd36;
    }
  }
  else {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
    plVar7 = *(long **)(this + 0x38);
    *(long *)(this + 0x38) = lVar2;
    if (plVar7 != (long *)0x0) {
      LOCK();
      plVar10 = plVar7 + 2;
      *(int *)plVar10 = *(int *)plVar10 + -1;
      UNLOCK();
      if (*(int *)plVar10 == 0) {
        (**(code **)(*plVar7 + 0x20))();
      }
    }
    plVar10 = *(long **)(this + 0x40);
    plVar7 = *(long **)CONCAT44(in_register_0000000c,param_3);
    if (plVar7 != plVar10) {
      if (plVar7 != (long *)0x0) goto LAB_0035cd2d;
LAB_0035cd36:
      *(long **)(this + 0x40) = plVar7;
      if (plVar10 != (long *)0x0) {
        LOCK();
        plVar7 = plVar10 + 2;
        *(int *)plVar7 = *(int *)plVar7 + -1;
        UNLOCK();
        if (*(int *)plVar7 == 0) {
          (**(code **)(*plVar10 + 0x20))();
        }
      }
    }
  }
  plVar7 = *(long **)CONCAT44(in_register_00000084,param_4);
  plVar10 = *(long **)(this + 0x48);
  if (plVar7 != plVar10) {
    if (plVar7 != (long *)0x0) {
      LOCK();
      *(int *)(plVar7 + 2) = *(int *)(plVar7 + 2) + 1;
      UNLOCK();
      plVar10 = *(long **)(this + 0x48);
    }
    *(long **)(this + 0x48) = plVar7;
    if (plVar10 != (long *)0x0) {
      LOCK();
      plVar7 = plVar10 + 2;
      *(int *)plVar7 = *(int *)plVar7 + -1;
      UNLOCK();
      if (*(int *)plVar7 == 0) {
        if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0035ce85. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          (**(code **)(*plVar10 + 0x20))();
          return;
        }
        goto LAB_0035ced1;
      }
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_0035ced1:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisImageLayerAddCommand @ 0035cf10 ======

/* KisImageLayerAddCommand::KisImageLayerAddCommand(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisNode>, KisSharedPtr<KisNode>, unsigned int, bool, bool) */

void __thiscall
KisImageLayerAddCommand::KisImageLayerAddCommand
          (KisImageLayerAddCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KisSharedPtr param_3,uint param_4,bool param_5,bool param_6)

{
  int iVar1;
  long lVar2;
  ulong uVar3;
  undefined auVar4 [16];
  undefined *puVar5;
  undefined8 uVar6;
  long *plVar7;
  int *piVar8;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  ulong *puVar9;
  long *plVar10;
  long in_FS_OFFSET;
  QArrayData *local_68;
  QArrayData *local_60;
  undefined local_58 [24];
  long local_40;
  
  puVar9 = (ulong *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*puVar9 == 0) {
    local_58._0_8_ = 0;
    uVar3 = local_58._0_8_;
LAB_0035d1a9:
    local_58._0_8_ = uVar3;
    local_58._8_8_ = 0;
  }
  else if (((uint *)puVar9[1] == (uint *)0x0) || ((*(uint *)puVar9[1] & 1) == 0)) {
    local_58._0_16_ = (undefined  [16])0x0;
  }
  else {
    uVar3 = *puVar9;
    local_58._8_8_ = local_58._0_8_;
    local_58._0_8_ = uVar3;
    if (uVar3 == 0) goto LAB_0035d1a9;
    piVar8 = *(int **)(uVar3 + 0x58);
    if (piVar8 == (int *)0x0) {
      piVar8 = (int *)operator_new(4);
      *piVar8 = 0;
      *(int **)(uVar3 + 0x58) = piVar8;
      LOCK();
      *piVar8 = *piVar8 + 1;
      UNLOCK();
      piVar8 = *(int **)(uVar3 + 0x58);
      uVar3 = local_58._0_8_;
    }
    local_58._0_8_ = uVar3;
    local_58._8_8_ = piVar8;
    LOCK();
    *piVar8 = *piVar8 + 2;
    UNLOCK();
  }
                    /* try { // try from 0035cf8e to 0035cf92 has its CatchHandler @ 0035d242 */
  ki18ndc((char *)&local_60,"krita","(qtundo-format)");
                    /* try { // try from 0035cf9e to 0035cfa2 has its CatchHandler @ 0035d25f */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_60);
                    /* try { // try from 0035cfb1 to 0035cfb5 has its CatchHandler @ 0035d253 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_60,(QString *)&local_68);
  if (*(int *)local_68 == 0) {
LAB_0035d168:
    QArrayData::deallocate(local_68,2,8);
  }
  else if (*(int *)local_68 != -1) {
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 == 0) goto LAB_0035d168;
  }
                    /* try { // try from 0035cfe9 to 0035cfed has its CatchHandler @ 0035d236 */
  KisImageCommand::KisImageCommand
            ((KisImageCommand *)this,(KUndo2MagicString *)&local_60,(KisWeakSharedPtr)local_58,
             (KUndo2Command *)0x0);
  if (*(int *)local_60 == 0) {
LAB_0035d150:
    QArrayData::deallocate(local_60,2,8);
  }
  else if (*(int *)local_60 != -1) {
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 == 0) goto LAB_0035d150;
  }
  uVar6 = local_58._8_8_;
  auVar4._8_8_ = 0;
  auVar4._0_8_ = local_58._8_8_;
  local_58._0_16_ = auVar4 << 0x40;
  if ((int *)uVar6 != (int *)0x0) {
    LOCK();
    iVar1 = *(int *)uVar6;
    *(int *)uVar6 = *(int *)uVar6 + -2;
    UNLOCK();
    if ((iVar1 < 3) && ((int *)uVar6 != (int *)0x0)) {
      operator_delete((void *)uVar6,4);
    }
  }
  puVar5 = PTR_vtable_00837240;
  *(undefined8 *)(this + 0x48) = 0;
  *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
  *(uint *)(this + 0x50) = param_4;
  *(undefined **)this = puVar5 + 0x10;
  this[0x54] = (KisImageLayerAddCommand)param_5;
  lVar2 = *(long *)CONCAT44(in_register_00000014,param_2);
  this[0x55] = (KisImageLayerAddCommand)param_6;
  *(undefined4 *)(this + 0x58) = 0;
  if (lVar2 == 0) {
    plVar7 = *(long **)CONCAT44(in_register_0000000c,param_3);
    if (plVar7 != (long *)0x0) {
LAB_0035d191:
      LOCK();
      *(int *)(plVar7 + 2) = *(int *)(plVar7 + 2) + 1;
      UNLOCK();
      plVar10 = *(long **)(this + 0x40);
      goto LAB_0035d0ae;
    }
  }
  else {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
    plVar7 = *(long **)(this + 0x38);
    *(long *)(this + 0x38) = lVar2;
    if (plVar7 != (long *)0x0) {
      LOCK();
      plVar10 = plVar7 + 2;
      *(int *)plVar10 = *(int *)plVar10 + -1;
      UNLOCK();
      if (*(int *)plVar10 == 0) {
        (**(code **)(*plVar7 + 0x20))();
      }
    }
    plVar10 = *(long **)(this + 0x40);
    plVar7 = *(long **)CONCAT44(in_register_0000000c,param_3);
    if (plVar7 != plVar10) {
      if (plVar7 != (long *)0x0) goto LAB_0035d191;
LAB_0035d0ae:
      *(long **)(this + 0x40) = plVar7;
      if (plVar10 != (long *)0x0) {
        LOCK();
        plVar7 = plVar10 + 2;
        *(int *)plVar7 = *(int *)plVar7 + -1;
        UNLOCK();
        if (*(int *)plVar7 == 0) {
          (**(code **)(*plVar10 + 0x20))();
        }
      }
    }
    plVar7 = *(long **)(this + 0x48);
    if (plVar7 != (long *)0x0) {
      *(undefined8 *)(this + 0x48) = 0;
      LOCK();
      plVar10 = plVar7 + 2;
      *(int *)plVar10 = *(int *)plVar10 + -1;
      UNLOCK();
      if (*(int *)plVar10 == 0) {
        if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0035d1e5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          (**(code **)(*plVar7 + 0x20))();
          return;
        }
        goto LAB_0035d231;
      }
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_0035d231:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisImageLayerAddCommand @ 0035d270 ======

/* KisImageLayerAddCommand::KisImageLayerAddCommand(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisNode>, KisSharedPtr<KisNode>, KisSharedPtr<KisNode>,
   QFlags<KisImageLayerAddCommand::Flag>) */

void __thiscall
KisImageLayerAddCommand::KisImageLayerAddCommand
          (KisImageLayerAddCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KisSharedPtr param_3,KisSharedPtr param_4,QFlags param_5)

{
  int iVar1;
  undefined auVar2 [8];
  undefined auVar3 [16];
  uint uVar4;
  int *piVar5;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long *plVar6;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  long *local_70;
  long *local_68;
  long *local_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  plVar6 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_60 = *(long **)CONCAT44(in_register_00000084,param_4);
  if (local_60 != (long *)0x0) {
    LOCK();
    *(int *)(local_60 + 2) = *(int *)(local_60 + 2) + 1;
    UNLOCK();
  }
  local_68 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (local_68 != (long *)0x0) {
    LOCK();
    *(int *)(local_68 + 2) = *(int *)(local_68 + 2) + 1;
    UNLOCK();
  }
  local_70 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_70 != (long *)0x0) {
    LOCK();
    *(int *)(local_70 + 2) = *(int *)(local_70 + 2) + 1;
    UNLOCK();
  }
  if (*plVar6 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar2 = local_58;
  }
  else {
    if (((uint *)plVar6[1] == (uint *)0x0) || ((*(uint *)plVar6[1] & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
      goto LAB_0035d30c;
    }
    auVar2 = (undefined  [8])*plVar6;
    piStack_50 = (int *)local_58;
    local_58 = auVar2;
    if (auVar2 != (undefined  [8])0x0) {
      piVar5 = *(int **)((long)auVar2 + 0x58);
      if (piVar5 == (int *)0x0) {
                    /* try { // try from 0035d465 to 0035d469 has its CatchHandler @ 0035d48f */
        piVar5 = (int *)operator_new(4);
        *piVar5 = 0;
        *(int **)((long)auVar2 + 0x58) = piVar5;
        LOCK();
        *piVar5 = *piVar5 + 1;
        UNLOCK();
        piVar5 = *(int **)((long)auVar2 + 0x58);
        auVar2 = local_58;
      }
      local_58 = auVar2;
      LOCK();
      *piVar5 = *piVar5 + 2;
      UNLOCK();
      piStack_50 = piVar5;
      goto LAB_0035d30c;
    }
  }
  local_58 = auVar2;
  piStack_50 = (int *)0x0;
LAB_0035d30c:
                    /* try { // try from 0035d32b to 0035d32f has its CatchHandler @ 0035d483 */
  KisImageLayerAddCommand
            (this,(KisWeakSharedPtr)local_58,(KisSharedPtr)&local_70,(KisSharedPtr)&local_68,
             (KisSharedPtr)&local_60,(bool)((byte)param_5 & 1),(bool)((byte)(param_5 >> 1) & 1));
  piVar5 = piStack_50;
  auVar3._8_8_ = 0;
  auVar3._0_8_ = piStack_50;
  _local_58 = auVar3 << 0x40;
  if (piVar5 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar5;
    *piVar5 = *piVar5 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar5 != (int *)0x0)) {
      operator_delete(piVar5,4);
    }
  }
  if (local_70 != (long *)0x0) {
    LOCK();
    plVar6 = local_70 + 2;
    *(int *)plVar6 = *(int *)plVar6 + -1;
    UNLOCK();
    if (*(int *)plVar6 == 0) {
      (**(code **)(*local_70 + 0x20))();
    }
  }
  if (local_68 != (long *)0x0) {
    LOCK();
    plVar6 = local_68 + 2;
    *(int *)plVar6 = *(int *)plVar6 + -1;
    UNLOCK();
    if (*(int *)plVar6 == 0) {
      (**(code **)(*local_68 + 0x20))();
    }
  }
  if (local_60 != (long *)0x0) {
    LOCK();
    plVar6 = local_60 + 2;
    *(int *)plVar6 = *(int *)plVar6 + -1;
    UNLOCK();
    if (*(int *)plVar6 == 0) {
      (**(code **)(*local_60 + 0x20))();
    }
  }
  uVar4 = *(uint *)(this + 0x58) | 1;
  if ((param_5 & 4) == 0) {
    uVar4 = *(uint *)(this + 0x58) & 0xfffffffe;
  }
  *(uint *)(this + 0x58) = uVar4;
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisImageLayerAddCommand @ 0035d4a0 ======

/* KisImageLayerAddCommand::KisImageLayerAddCommand(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisNode>, KisSharedPtr<KisNode>, unsigned int,
   QFlags<KisImageLayerAddCommand::Flag>) */

void __thiscall
KisImageLayerAddCommand::KisImageLayerAddCommand
          (KisImageLayerAddCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KisSharedPtr param_3,uint param_4,QFlags param_5)

{
  int iVar1;
  undefined auVar2 [8];
  undefined auVar3 [16];
  uint uVar4;
  int *piVar5;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long *plVar6;
  long in_FS_OFFSET;
  long *local_68;
  long *local_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  plVar6 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_60 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (local_60 != (long *)0x0) {
    LOCK();
    *(int *)(local_60 + 2) = *(int *)(local_60 + 2) + 1;
    UNLOCK();
  }
  local_68 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_68 != (long *)0x0) {
    LOCK();
    *(int *)(local_68 + 2) = *(int *)(local_68 + 2) + 1;
    UNLOCK();
  }
  if (*plVar6 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar2 = local_58;
  }
  else {
    if (((uint *)plVar6[1] == (uint *)0x0) || ((*(uint *)plVar6[1] & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
      goto LAB_0035d52f;
    }
    auVar2 = (undefined  [8])*plVar6;
    piStack_50 = (int *)local_58;
    local_58 = auVar2;
    if (auVar2 != (undefined  [8])0x0) {
      piVar5 = *(int **)((long)auVar2 + 0x58);
      if (piVar5 == (int *)0x0) {
                    /* try { // try from 0035d655 to 0035d659 has its CatchHandler @ 0035d67f */
        piVar5 = (int *)operator_new(4);
        *piVar5 = 0;
        *(int **)((long)auVar2 + 0x58) = piVar5;
        LOCK();
        *piVar5 = *piVar5 + 1;
        UNLOCK();
        piVar5 = *(int **)((long)auVar2 + 0x58);
        auVar2 = local_58;
      }
      local_58 = auVar2;
      LOCK();
      *piVar5 = *piVar5 + 2;
      UNLOCK();
      piStack_50 = piVar5;
      goto LAB_0035d52f;
    }
  }
  local_58 = auVar2;
  piStack_50 = (int *)0x0;
LAB_0035d52f:
                    /* try { // try from 0035d54d to 0035d551 has its CatchHandler @ 0035d673 */
  KisImageLayerAddCommand
            (this,(KisWeakSharedPtr)local_58,(KisSharedPtr)&local_68,(KisSharedPtr)&local_60,param_4
             ,(bool)((byte)param_5 & 1),(bool)((byte)(param_5 >> 1) & 1));
  piVar5 = piStack_50;
  auVar3._8_8_ = 0;
  auVar3._0_8_ = piStack_50;
  _local_58 = auVar3 << 0x40;
  if (piVar5 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar5;
    *piVar5 = *piVar5 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar5 != (int *)0x0)) {
      operator_delete(piVar5,4);
    }
  }
  if (local_68 != (long *)0x0) {
    LOCK();
    plVar6 = local_68 + 2;
    *(int *)plVar6 = *(int *)plVar6 + -1;
    UNLOCK();
    if (*(int *)plVar6 == 0) {
      (**(code **)(*local_68 + 0x20))();
    }
  }
  if (local_60 != (long *)0x0) {
    LOCK();
    plVar6 = local_60 + 2;
    *(int *)plVar6 = *(int *)plVar6 + -1;
    UNLOCK();
    if (*(int *)plVar6 == 0) {
      (**(code **)(*local_60 + 0x20))();
    }
  }
  uVar4 = *(uint *)(this + 0x58) | 1;
  if ((param_5 & 4) == 0) {
    uVar4 = *(uint *)(this + 0x58) & 0xfffffffe;
  }
  *(uint *)(this + 0x58) = uVar4;
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



