/* Class KisSetEmptyGlobalSelectionCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSetEmptyGlobalSelectionCommand @ 00368040 ======

/* KisSetEmptyGlobalSelectionCommand::KisSetEmptyGlobalSelectionCommand(KisWeakSharedPtr<KisImage>)
    */

void __thiscall
KisSetEmptyGlobalSelectionCommand::KisSetEmptyGlobalSelectionCommand
          (KisSetEmptyGlobalSelectionCommand *this,KisWeakSharedPtr param_1)

{
  QObject *pQVar1;
  int iVar2;
  undefined auVar3 [8];
  ulong uVar4;
  undefined auVar5 [16];
  undefined auVar6 [16];
  undefined auVar7 [16];
  undefined8 uVar8;
  QObject *pQVar9;
  KisSelection *pKVar10;
  KisImageResolutionProxy *this_00;
  KisSelectionEmptyBounds *pKVar11;
  int *piVar12;
  undefined4 in_register_00000034;
  ulong *puVar13;
  long in_FS_OFFSET;
  KisSelection *local_98;
  KisSelectionEmptyBounds *local_90;
  undefined local_88 [8];
  int *piStack_80;
  undefined local_78 [16];
  KisImageResolutionProxy *local_68;
  QObject *local_60;
  undefined local_58 [24];
  long local_40;
  
  piStack_80 = (int *)local_88;
  local_78._8_8_ = local_78._0_8_;
  local_58._8_8_ = local_58._0_8_;
  puVar13 = (ulong *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  pKVar10 = (KisSelection *)operator_new(0x20);
                    /* try { // try from 0036807a to 0036807e has its CatchHandler @ 003684aa */
  this_00 = (KisImageResolutionProxy *)operator_new(0x18);
  if (*puVar13 == 0) {
    local_58._0_8_ = 0;
    uVar4 = local_58._0_8_;
LAB_003683d9:
    local_58._0_8_ = uVar4;
    local_58._8_8_ = 0;
  }
  else if (((uint *)puVar13[1] == (uint *)0x0) || ((*(uint *)puVar13[1] & 1) == 0)) {
    local_58._0_16_ = (undefined  [16])0x0;
  }
  else {
    uVar4 = *puVar13;
    local_58._0_8_ = uVar4;
    if (uVar4 == 0) goto LAB_003683d9;
    piVar12 = *(int **)(uVar4 + 0x58);
    if (piVar12 == (int *)0x0) {
                    /* try { // try from 00368415 to 00368419 has its CatchHandler @ 003684da */
      piVar12 = (int *)operator_new(4);
      *piVar12 = 0;
      *(int **)(uVar4 + 0x58) = piVar12;
      LOCK();
      *piVar12 = *piVar12 + 1;
      UNLOCK();
      piVar12 = *(int **)(uVar4 + 0x58);
      uVar4 = local_58._0_8_;
    }
    local_58._0_8_ = uVar4;
    local_58._8_8_ = piVar12;
    LOCK();
    *piVar12 = *piVar12 + 2;
    UNLOCK();
  }
                    /* try { // try from 003680bc to 003680c0 has its CatchHandler @ 00368492 */
  KisImageResolutionProxy::KisImageResolutionProxy(this_00,(KisWeakSharedPtr)local_58);
  local_68 = this_00;
                    /* try { // try from 003680cb to 003680cf has its CatchHandler @ 00368486 */
  local_60 = (QObject *)operator_new(0x18);
  *(KisImageResolutionProxy **)(local_60 + 0x10) = this_00;
  *(code **)(local_60 + 8) = FUN_00368510;
  *(undefined4 *)(local_60 + 4) = 1;
  *(undefined4 *)local_60 = 1;
                    /* try { // try from 003680fc to 00368100 has its CatchHandler @ 003684f2 */
  QtSharedPointer::ExternalRefCountData::setQObjectShared(local_60,SUB81(this_00,0));
                    /* try { // try from 00368106 to 0036810a has its CatchHandler @ 003684ce */
  pKVar11 = (KisSelectionEmptyBounds *)operator_new(0x20);
  if (*puVar13 == 0) {
    local_78._0_8_ = 0;
    uVar4 = local_78._0_8_;
LAB_00368399:
    local_78._0_8_ = uVar4;
    local_78._8_8_ = 0;
  }
  else if (((uint *)puVar13[1] == (uint *)0x0) || ((*(uint *)puVar13[1] & 1) == 0)) {
    local_78 = (undefined  [16])0x0;
  }
  else {
    uVar4 = *puVar13;
    local_78._0_8_ = uVar4;
    if (uVar4 == 0) goto LAB_00368399;
    piVar12 = *(int **)(uVar4 + 0x58);
    if (piVar12 == (int *)0x0) {
                    /* try { // try from 00368465 to 00368469 has its CatchHandler @ 003684fe */
      piVar12 = (int *)operator_new(4);
      *piVar12 = 0;
      *(int **)(uVar4 + 0x58) = piVar12;
      LOCK();
      *piVar12 = *piVar12 + 1;
      UNLOCK();
      piVar12 = *(int **)(uVar4 + 0x58);
      uVar4 = local_78._0_8_;
    }
    local_78._0_8_ = uVar4;
    local_78._8_8_ = piVar12;
    LOCK();
    *piVar12 = *piVar12 + 2;
    UNLOCK();
  }
                    /* try { // try from 00368144 to 00368148 has its CatchHandler @ 003684c2 */
  KisSelectionEmptyBounds::KisSelectionEmptyBounds(pKVar11,(KisWeakSharedPtr)local_78);
  LOCK();
  *(int *)(pKVar11 + 8) = *(int *)(pKVar11 + 8) + 1;
  UNLOCK();
  local_90 = pKVar11;
                    /* try { // try from 00368161 to 00368165 has its CatchHandler @ 003684b6 */
  KisSelection::KisSelection(pKVar10,(KisSharedPtr)&local_90,(QSharedPointer)&local_68);
  LOCK();
  *(int *)(pKVar10 + 8) = *(int *)(pKVar10 + 8) + 1;
  UNLOCK();
  local_98 = pKVar10;
  if (*puVar13 == 0) {
    local_88 = (undefined  [8])0x0;
    auVar3 = local_88;
  }
  else {
    if (((uint *)puVar13[1] == (uint *)0x0) || ((*(uint *)puVar13[1] & 1) == 0)) {
      local_88 = (undefined  [8])0x0;
      piStack_80 = (int *)0x0;
      goto LAB_00368196;
    }
    auVar3 = (undefined  [8])*puVar13;
    local_88 = auVar3;
    if (auVar3 != (undefined  [8])0x0) {
      piVar12 = *(int **)((long)auVar3 + 0x58);
      if (piVar12 == (int *)0x0) {
                    /* try { // try from 0036843d to 00368441 has its CatchHandler @ 003684e6 */
        piVar12 = (int *)operator_new(4);
        *piVar12 = 0;
        *(int **)((long)auVar3 + 0x58) = piVar12;
        LOCK();
        *piVar12 = *piVar12 + 1;
        UNLOCK();
        piVar12 = *(int **)((long)auVar3 + 0x58);
        auVar3 = local_88;
      }
      local_88 = auVar3;
      LOCK();
      *piVar12 = *piVar12 + 2;
      UNLOCK();
      piStack_80 = piVar12;
      goto LAB_00368196;
    }
  }
  local_88 = auVar3;
  piStack_80 = (int *)0x0;
LAB_00368196:
                    /* try { // try from 003681a6 to 003681aa has its CatchHandler @ 0036849e */
  KisSetGlobalSelectionCommand::KisSetGlobalSelectionCommand
            ((KisSetGlobalSelectionCommand *)this,(KisWeakSharedPtr)local_88,(KisSharedPtr)&local_98
            );
  piVar12 = piStack_80;
  auVar5._8_8_ = 0;
  auVar5._0_8_ = piStack_80;
  _local_88 = auVar5 << 0x40;
  if (piVar12 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar12;
    *piVar12 = *piVar12 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar12 != (int *)0x0)) {
      operator_delete(piVar12,4);
    }
  }
  if (local_98 != (KisSelection *)0x0) {
    LOCK();
    pKVar10 = local_98 + 8;
    *(int *)pKVar10 = *(int *)pKVar10 + -1;
    UNLOCK();
    if (*(int *)pKVar10 == 0) {
      (**(code **)(*(long *)local_98 + 8))();
    }
  }
  if (local_90 != (KisSelectionEmptyBounds *)0x0) {
    LOCK();
    pKVar11 = local_90 + 8;
    *(int *)pKVar11 = *(int *)pKVar11 + -1;
    UNLOCK();
    if (*(int *)pKVar11 == 0) {
      (**(code **)(*(long *)local_90 + 8))();
    }
  }
  uVar8 = local_78._8_8_;
  auVar6._8_8_ = 0;
  auVar6._0_8_ = local_78._8_8_;
  local_78 = auVar6 << 0x40;
  if ((int *)uVar8 != (int *)0x0) {
    LOCK();
    iVar2 = *(int *)uVar8;
    *(int *)uVar8 = *(int *)uVar8 + -2;
    UNLOCK();
    if ((iVar2 < 3) && ((int *)uVar8 != (int *)0x0)) {
      operator_delete((void *)uVar8,4);
    }
  }
  pQVar9 = local_60;
  if (local_60 != (QObject *)0x0) {
    LOCK();
    pQVar1 = local_60 + 4;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (*(int *)pQVar1 == 0) {
      (**(code **)(local_60 + 8))(local_60);
    }
    LOCK();
    *(int *)pQVar9 = *(int *)pQVar9 + -1;
    UNLOCK();
    if (*(int *)pQVar9 == 0) {
      operator_delete(pQVar9,0x10);
    }
  }
  uVar8 = local_58._8_8_;
  auVar7._8_8_ = 0;
  auVar7._0_8_ = local_58._8_8_;
  local_58._0_16_ = auVar7 << 0x40;
  if ((int *)uVar8 != (int *)0x0) {
    LOCK();
    iVar2 = *(int *)uVar8;
    *(int *)uVar8 = *(int *)uVar8 + -2;
    UNLOCK();
    if ((iVar2 < 3) && ((int *)uVar8 != (int *)0x0)) {
      operator_delete((void *)uVar8,4);
    }
  }
  *(undefined **)this = PTR_vtable_00837eb8 + 0x10;
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



