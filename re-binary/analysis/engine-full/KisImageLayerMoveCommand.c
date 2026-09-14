/* Class KisImageLayerMoveCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageLayerMoveCommand @ 00204fe0 ======

void __thiscall
KisImageLayerMoveCommand::KisImageLayerMoveCommand
          (KisImageLayerMoveCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KisSharedPtr param_3,KisSharedPtr param_4,bool param_5)

{
  (*(code *)PTR_KisImageLayerMoveCommand_0083a2c0)();
  return;
}



// ====== KisImageLayerMoveCommand @ 0035df00 ======

/* KisImageLayerMoveCommand::KisImageLayerMoveCommand(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisNode>, KisSharedPtr<KisNode>, KisSharedPtr<KisNode>, bool) */

void __thiscall
KisImageLayerMoveCommand::KisImageLayerMoveCommand
          (KisImageLayerMoveCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KisSharedPtr param_3,KisSharedPtr param_4,bool param_5)

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
LAB_0035e249:
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
    if (uVar3 == 0) goto LAB_0035e249;
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
                    /* try { // try from 0035df75 to 0035df79 has its CatchHandler @ 0035e2f6 */
  ki18ndc((char *)&local_60,"krita","(qtundo-format)");
                    /* try { // try from 0035df85 to 0035df89 has its CatchHandler @ 0035e32b */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_60);
                    /* try { // try from 0035df98 to 0035df9c has its CatchHandler @ 0035e31f */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_60,(QString *)&local_68);
  if (*(int *)local_68 == 0) {
LAB_0035e208:
    QArrayData::deallocate(local_68,2,8);
  }
  else if (*(int *)local_68 != -1) {
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 == 0) goto LAB_0035e208;
  }
                    /* try { // try from 0035dfd0 to 0035dfd4 has its CatchHandler @ 0035e313 */
  KisImageCommand::KisImageCommand
            ((KisImageCommand *)this,(KUndo2MagicString *)&local_60,(KisWeakSharedPtr)local_58,
             (KUndo2Command *)0x0);
  if (*(int *)local_60 == 0) {
LAB_0035e1f0:
    QArrayData::deallocate(local_60,2,8);
  }
  else if (*(int *)local_60 != -1) {
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 == 0) goto LAB_0035e1f0;
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
  puVar5 = PTR_vtable_00837560;
  *(undefined8 *)(this + 0x58) = 0;
  *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
  *(undefined **)this = puVar5 + 0x10;
  lVar2 = *(long *)CONCAT44(in_register_00000014,param_2);
  if (lVar2 == 0) {
    plVar7 = *(long **)CONCAT44(in_register_0000000c,param_3);
    if (plVar7 == (long *)0x0) goto LAB_0035e09c;
LAB_0035e07f:
    LOCK();
    *(int *)(plVar7 + 2) = *(int *)(plVar7 + 2) + 1;
    UNLOCK();
    plVar10 = *(long **)(this + 0x50);
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
    plVar10 = *(long **)(this + 0x50);
    plVar7 = *(long **)CONCAT44(in_register_0000000c,param_3);
    if (plVar7 == plVar10) goto LAB_0035e09c;
    if (plVar7 != (long *)0x0) goto LAB_0035e07f;
  }
  *(long **)(this + 0x50) = plVar7;
  if (plVar10 != (long *)0x0) {
    LOCK();
    plVar7 = plVar10 + 2;
    *(int *)plVar7 = *(int *)plVar7 + -1;
    UNLOCK();
    if (*(int *)plVar7 == 0) {
      (**(code **)(*plVar10 + 0x20))();
    }
  }
LAB_0035e09c:
  plVar7 = *(long **)CONCAT44(in_register_00000084,param_4);
  plVar10 = *(long **)(this + 0x58);
  if (plVar7 != plVar10) {
    if (plVar7 != (long *)0x0) {
      LOCK();
      *(int *)(plVar7 + 2) = *(int *)(plVar7 + 2) + 1;
      UNLOCK();
      plVar10 = *(long **)(this + 0x58);
    }
    *(long **)(this + 0x58) = plVar7;
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
                    /* try { // try from 0035e0d1 to 0035e126 has its CatchHandler @ 0035e307 */
  KisNode::parent();
  plVar7 = *(long **)(this + 0x40);
  if ((long *)local_58._0_8_ != plVar7) {
    if ((long *)local_58._0_8_ != (long *)0x0) {
      LOCK();
      *(int *)(local_58._0_8_ + 0x10) = *(int *)(local_58._0_8_ + 0x10) + 1;
      UNLOCK();
      plVar7 = *(long **)(this + 0x40);
    }
    *(undefined8 *)(this + 0x40) = local_58._0_8_;
    if (plVar7 != (long *)0x0) {
      LOCK();
      plVar10 = plVar7 + 2;
      *(int *)plVar10 = *(int *)plVar10 + -1;
      UNLOCK();
      if (*(int *)plVar10 == 0) {
        (**(code **)(*plVar7 + 0x20))();
      }
    }
    plVar7 = (long *)local_58._0_8_;
  }
  if (plVar7 != (long *)0x0) {
    LOCK();
    plVar10 = plVar7 + 2;
    *(int *)plVar10 = *(int *)plVar10 + -1;
    UNLOCK();
    if (*(int *)plVar10 == 0) {
      (**(code **)(*plVar7 + 0x20))();
    }
  }
  KisNode::prevSibling();
  plVar7 = *(long **)(this + 0x48);
  if ((long *)local_58._0_8_ != plVar7) {
    if ((long *)local_58._0_8_ != (long *)0x0) {
      LOCK();
      *(int *)(local_58._0_8_ + 0x10) = *(int *)(local_58._0_8_ + 0x10) + 1;
      UNLOCK();
      plVar7 = *(long **)(this + 0x48);
    }
    *(undefined8 *)(this + 0x48) = local_58._0_8_;
    if (plVar7 != (long *)0x0) {
      LOCK();
      plVar10 = plVar7 + 2;
      *(int *)plVar10 = *(int *)plVar10 + -1;
      UNLOCK();
      if (*(int *)plVar10 == 0) {
        (**(code **)(*plVar7 + 0x20))();
      }
    }
    plVar7 = (long *)local_58._0_8_;
  }
  if (plVar7 != (long *)0x0) {
    LOCK();
    plVar10 = plVar7 + 2;
    *(int *)plVar10 = *(int *)plVar10 + -1;
    UNLOCK();
    if (*(int *)plVar10 == 0) {
      (**(code **)(*plVar7 + 0x20))();
    }
  }
  *(undefined4 *)(this + 0x60) = 0xffffffff;
  this[100] = (KisImageLayerMoveCommand)0x0;
  this[0x65] = (KisImageLayerMoveCommand)param_5;
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisImageLayerMoveCommand @ 0035e340 ======

/* KisImageLayerMoveCommand::KisImageLayerMoveCommand(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisNode>, KisSharedPtr<KisNode>, unsigned int) */

void __thiscall
KisImageLayerMoveCommand::KisImageLayerMoveCommand
          (KisImageLayerMoveCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KisSharedPtr param_3,uint param_4)

{
  long *plVar1;
  int iVar2;
  long lVar3;
  ulong uVar4;
  undefined auVar5 [16];
  undefined *puVar6;
  undefined8 uVar7;
  int *piVar8;
  undefined4 in_register_0000000c;
  long *plVar9;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  ulong *puVar10;
  long *plVar11;
  long in_FS_OFFSET;
  QArrayData *local_68;
  QArrayData *local_60;
  undefined local_58 [24];
  long local_40;
  
  puVar10 = (ulong *)CONCAT44(in_register_00000034,param_1);
  plVar9 = (long *)CONCAT44(in_register_0000000c,param_3);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*puVar10 == 0) {
    local_58._0_8_ = 0;
    uVar4 = local_58._0_8_;
LAB_0035e669:
    local_58._0_8_ = uVar4;
    local_58._8_8_ = 0;
  }
  else if (((uint *)puVar10[1] == (uint *)0x0) || ((*(uint *)puVar10[1] & 1) == 0)) {
    local_58._0_16_ = (undefined  [16])0x0;
  }
  else {
    uVar4 = *puVar10;
    local_58._8_8_ = local_58._0_8_;
    local_58._0_8_ = uVar4;
    if (uVar4 == 0) goto LAB_0035e669;
    piVar8 = *(int **)(uVar4 + 0x58);
    if (piVar8 == (int *)0x0) {
      piVar8 = (int *)operator_new(4);
      *piVar8 = 0;
      *(int **)(uVar4 + 0x58) = piVar8;
      LOCK();
      *piVar8 = *piVar8 + 1;
      UNLOCK();
      piVar8 = *(int **)(uVar4 + 0x58);
      uVar4 = local_58._0_8_;
    }
    local_58._0_8_ = uVar4;
    local_58._8_8_ = piVar8;
    LOCK();
    *piVar8 = *piVar8 + 2;
    UNLOCK();
  }
                    /* try { // try from 0035e3af to 0035e3b3 has its CatchHandler @ 0035e732 */
  ki18ndc((char *)&local_60,"krita","(qtundo-format)");
                    /* try { // try from 0035e3bd to 0035e3c1 has its CatchHandler @ 0035e726 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_60);
                    /* try { // try from 0035e3d0 to 0035e3d4 has its CatchHandler @ 0035e75b */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_60,(QString *)&local_68);
  if (*(int *)local_68 == 0) {
LAB_0035e610:
    QArrayData::deallocate(local_68,2,8);
  }
  else if (*(int *)local_68 != -1) {
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 == 0) goto LAB_0035e610;
  }
                    /* try { // try from 0035e406 to 0035e40a has its CatchHandler @ 0035e743 */
  KisImageCommand::KisImageCommand
            ((KisImageCommand *)this,(KUndo2MagicString *)&local_60,(KisWeakSharedPtr)local_58,
             (KUndo2Command *)0x0);
  if (*(int *)local_60 == 0) {
LAB_0035e628:
    QArrayData::deallocate(local_60,2,8);
  }
  else if (*(int *)local_60 != -1) {
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 == 0) goto LAB_0035e628;
  }
  uVar7 = local_58._8_8_;
  auVar5._8_8_ = 0;
  auVar5._0_8_ = local_58._8_8_;
  local_58._0_16_ = auVar5 << 0x40;
  if ((int *)uVar7 != (int *)0x0) {
    LOCK();
    iVar2 = *(int *)uVar7;
    *(int *)uVar7 = *(int *)uVar7 + -2;
    UNLOCK();
    if ((iVar2 < 3) && ((int *)uVar7 != (int *)0x0)) {
      operator_delete((void *)uVar7,4);
    }
  }
  puVar6 = PTR_vtable_00837560;
  *(undefined8 *)(this + 0x58) = 0;
  *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
  *(undefined **)this = puVar6 + 0x10;
  lVar3 = *(long *)CONCAT44(in_register_00000014,param_2);
  if (lVar3 == 0) {
    plVar9 = (long *)*plVar9;
    if (plVar9 == (long *)0x0) goto LAB_0035e4e8;
LAB_0035e64c:
    LOCK();
    *(int *)(plVar9 + 2) = *(int *)(plVar9 + 2) + 1;
    UNLOCK();
    plVar11 = *(long **)(this + 0x50);
LAB_0035e4b4:
    *(long **)(this + 0x50) = plVar9;
    if (plVar11 != (long *)0x0) {
      LOCK();
      plVar9 = plVar11 + 2;
      *(int *)plVar9 = *(int *)plVar9 + -1;
      UNLOCK();
      if (*(int *)plVar9 == 0) {
        (**(code **)(*plVar11 + 0x20))();
      }
    }
  }
  else {
    LOCK();
    *(int *)(lVar3 + 0x10) = *(int *)(lVar3 + 0x10) + 1;
    UNLOCK();
    plVar11 = *(long **)(this + 0x38);
    *(long *)(this + 0x38) = lVar3;
    if (plVar11 == (long *)0x0) {
LAB_0035e49f:
      plVar9 = (long *)*plVar9;
      plVar11 = *(long **)(this + 0x50);
      if (plVar9 != plVar11) {
LAB_0035e4ab:
        if (plVar9 != (long *)0x0) goto LAB_0035e64c;
        goto LAB_0035e4b4;
      }
    }
    else {
      LOCK();
      plVar1 = plVar11 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 != 0) goto LAB_0035e49f;
      (**(code **)(*plVar11 + 0x20))();
      plVar9 = (long *)*plVar9;
      plVar11 = *(long **)(this + 0x50);
      if (plVar9 != plVar11) goto LAB_0035e4ab;
    }
  }
  plVar9 = *(long **)(this + 0x58);
  if (plVar9 != (long *)0x0) {
    *(undefined8 *)(this + 0x58) = 0;
    LOCK();
    plVar11 = plVar9 + 2;
    *(int *)plVar11 = *(int *)plVar11 + -1;
    UNLOCK();
    if (*(int *)plVar11 == 0) {
      (**(code **)(*plVar9 + 0x20))();
    }
  }
LAB_0035e4e8:
                    /* try { // try from 0035e4eb to 0035e540 has its CatchHandler @ 0035e74f */
  KisNode::parent();
  plVar9 = *(long **)(this + 0x40);
  if ((long *)local_58._0_8_ != plVar9) {
    if ((long *)local_58._0_8_ != (long *)0x0) {
      LOCK();
      *(int *)(local_58._0_8_ + 0x10) = *(int *)(local_58._0_8_ + 0x10) + 1;
      UNLOCK();
      plVar9 = *(long **)(this + 0x40);
    }
    *(undefined8 *)(this + 0x40) = local_58._0_8_;
    if (plVar9 != (long *)0x0) {
      LOCK();
      plVar11 = plVar9 + 2;
      *(int *)plVar11 = *(int *)plVar11 + -1;
      UNLOCK();
      if (*(int *)plVar11 == 0) {
        (**(code **)(*plVar9 + 0x20))();
      }
    }
    plVar9 = (long *)local_58._0_8_;
  }
  if (plVar9 != (long *)0x0) {
    LOCK();
    plVar11 = plVar9 + 2;
    *(int *)plVar11 = *(int *)plVar11 + -1;
    UNLOCK();
    if (*(int *)plVar11 == 0) {
      (**(code **)(*plVar9 + 0x20))();
    }
  }
  KisNode::prevSibling();
  plVar9 = *(long **)(this + 0x48);
  if ((long *)local_58._0_8_ != plVar9) {
    if ((long *)local_58._0_8_ != (long *)0x0) {
      LOCK();
      *(int *)(local_58._0_8_ + 0x10) = *(int *)(local_58._0_8_ + 0x10) + 1;
      UNLOCK();
      plVar9 = *(long **)(this + 0x48);
    }
    *(undefined8 *)(this + 0x48) = local_58._0_8_;
    if (plVar9 != (long *)0x0) {
      LOCK();
      plVar11 = plVar9 + 2;
      *(int *)plVar11 = *(int *)plVar11 + -1;
      UNLOCK();
      if (*(int *)plVar11 == 0) {
        (**(code **)(*plVar9 + 0x20))();
      }
    }
    plVar9 = (long *)local_58._0_8_;
  }
  if (plVar9 != (long *)0x0) {
    LOCK();
    plVar11 = plVar9 + 2;
    *(int *)plVar11 = *(int *)plVar11 + -1;
    UNLOCK();
    if (*(int *)plVar11 == 0) {
      (**(code **)(*plVar9 + 0x20))();
    }
  }
  *(uint *)(this + 0x60) = param_4;
  *(undefined2 *)(this + 100) = 0x101;
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



