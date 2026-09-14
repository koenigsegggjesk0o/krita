/* Class KisImageLayerRemoveCommandImpl - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageLayerRemoveCommandImpl @ 0020c010 ======

void __thiscall
KisImageLayerRemoveCommandImpl::KisImageLayerRemoveCommandImpl
          (KisImageLayerRemoveCommandImpl *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KUndo2Command *param_3)

{
  (*(code *)PTR_KisImageLayerRemoveCommandImpl_0083dad8)();
  return;
}



// ====== KisImageLayerRemoveCommandImpl @ 0035f6c0 ======

/* KisImageLayerRemoveCommandImpl::KisImageLayerRemoveCommandImpl(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisNode>, KUndo2Command*) */

void __thiscall
KisImageLayerRemoveCommandImpl::KisImageLayerRemoveCommandImpl
          (KisImageLayerRemoveCommandImpl *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          KUndo2Command *param_3)

{
  long *plVar1;
  int iVar2;
  long lVar3;
  ulong uVar4;
  undefined auVar5 [16];
  undefined *puVar6;
  undefined8 uVar7;
  undefined8 *puVar8;
  int *piVar9;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  ulong *puVar10;
  long *plVar11;
  long in_FS_OFFSET;
  QArrayData *local_58;
  QArrayData *local_50;
  undefined local_48 [24];
  long local_30;
  
  puVar10 = (ulong *)CONCAT44(in_register_00000034,param_1);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  if (*puVar10 == 0) {
    local_48._0_8_ = 0;
    uVar4 = local_48._0_8_;
LAB_0035f999:
    local_48._0_8_ = uVar4;
    local_48._8_8_ = 0;
  }
  else if (((uint *)puVar10[1] == (uint *)0x0) || ((*(uint *)puVar10[1] & 1) == 0)) {
    local_48._0_16_ = (undefined  [16])0x0;
  }
  else {
    uVar4 = *puVar10;
    local_48._8_8_ = local_48._0_8_;
    local_48._0_8_ = uVar4;
    if (uVar4 == 0) goto LAB_0035f999;
    piVar9 = *(int **)(uVar4 + 0x58);
    if (piVar9 == (int *)0x0) {
      piVar9 = (int *)operator_new(4);
      *piVar9 = 0;
      *(int **)(uVar4 + 0x58) = piVar9;
      LOCK();
      *piVar9 = *piVar9 + 1;
      UNLOCK();
      piVar9 = *(int **)(uVar4 + 0x58);
      uVar4 = local_48._0_8_;
    }
    local_48._0_8_ = uVar4;
    local_48._8_8_ = piVar9;
    LOCK();
    *piVar9 = *piVar9 + 2;
    UNLOCK();
  }
                    /* try { // try from 0035f72a to 0035f72e has its CatchHandler @ 0035fa28 */
  ki18ndc((char *)&local_50,"krita","(qtundo-format)");
                    /* try { // try from 0035f738 to 0035f73c has its CatchHandler @ 0035fa58 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_50);
                    /* try { // try from 0035f74b to 0035f74f has its CatchHandler @ 0035fa40 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_50,(QString *)&local_58);
  if (*(int *)local_58 == 0) {
LAB_0035f978:
    QArrayData::deallocate(local_58,2,8);
  }
  else if (*(int *)local_58 != -1) {
    LOCK();
    *(int *)local_58 = *(int *)local_58 + -1;
    UNLOCK();
    if (*(int *)local_58 == 0) goto LAB_0035f978;
  }
                    /* try { // try from 0035f782 to 0035f786 has its CatchHandler @ 0035fa4c */
  KisImageCommand::KisImageCommand
            ((KisImageCommand *)this,(KUndo2MagicString *)&local_50,(KisWeakSharedPtr)local_48,
             param_3);
  if (*(int *)local_50 != 0) {
    if (*(int *)local_50 == -1) goto LAB_0035f7aa;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_0035f7aa;
  }
  QArrayData::deallocate(local_50,2,8);
LAB_0035f7aa:
  uVar7 = local_48._8_8_;
  auVar5._8_8_ = 0;
  auVar5._0_8_ = local_48._8_8_;
  local_48._0_16_ = auVar5 << 0x40;
  if ((int *)uVar7 != (int *)0x0) {
    LOCK();
    iVar2 = *(int *)uVar7;
    *(int *)uVar7 = *(int *)uVar7 + -2;
    UNLOCK();
    if ((iVar2 < 3) && ((int *)uVar7 != (int *)0x0)) {
      operator_delete((void *)uVar7,4);
    }
  }
  *(undefined **)this = PTR_vtable_00837d80 + 0x10;
                    /* try { // try from 0035f7e2 to 0035f89b has its CatchHandler @ 0035fa34 */
  puVar8 = (undefined8 *)operator_new(0x30);
  puVar6 = PTR_shared_null_00837830;
  lVar3 = *(long *)CONCAT44(in_register_00000014,param_2);
  *puVar8 = this;
  puVar8[1] = 0;
  puVar8[2] = 0;
  puVar8[3] = 0;
  puVar8[4] = puVar6;
  puVar8[5] = puVar6;
  *(undefined8 **)(this + 0x38) = puVar8;
  if (lVar3 != 0) {
    LOCK();
    *(int *)(lVar3 + 0x10) = *(int *)(lVar3 + 0x10) + 1;
    UNLOCK();
    plVar11 = (long *)puVar8[1];
    puVar8[1] = lVar3;
    if (plVar11 != (long *)0x0) {
      LOCK();
      plVar1 = plVar11 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar11 + 0x20))();
      }
    }
  }
  KisNode::parent();
  lVar3 = *(long *)(this + 0x38);
  plVar11 = *(long **)(lVar3 + 0x10);
  if ((long *)local_48._0_8_ != plVar11) {
    if ((long *)local_48._0_8_ != (long *)0x0) {
      LOCK();
      *(int *)(local_48._0_8_ + 0x10) = *(int *)(local_48._0_8_ + 0x10) + 1;
      UNLOCK();
      plVar11 = *(long **)(lVar3 + 0x10);
    }
    *(undefined8 *)(lVar3 + 0x10) = local_48._0_8_;
    if (plVar11 != (long *)0x0) {
      LOCK();
      plVar1 = plVar11 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar11 + 0x20))();
      }
    }
    plVar11 = (long *)local_48._0_8_;
  }
  if (plVar11 != (long *)0x0) {
    LOCK();
    plVar1 = plVar11 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*plVar11 + 0x20))();
    }
  }
  KisNode::prevSibling();
  lVar3 = *(long *)(this + 0x38);
  plVar11 = *(long **)(lVar3 + 0x18);
  if ((long *)local_48._0_8_ != plVar11) {
    if ((long *)local_48._0_8_ != (long *)0x0) {
      LOCK();
      *(int *)(local_48._0_8_ + 0x10) = *(int *)(local_48._0_8_ + 0x10) + 1;
      UNLOCK();
      plVar11 = *(long **)(lVar3 + 0x18);
    }
    *(undefined8 *)(lVar3 + 0x18) = local_48._0_8_;
    if (plVar11 != (long *)0x0) {
      LOCK();
      plVar1 = plVar11 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar11 + 0x20))();
      }
    }
    plVar11 = (long *)local_48._0_8_;
  }
  if (plVar11 != (long *)0x0) {
    LOCK();
    plVar1 = plVar11 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*plVar11 + 0x20))();
    }
  }
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



