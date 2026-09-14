/* Class KisMask - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMask @ 002098e0 ======

void __thiscall KisMask::KisMask(KisMask *this,KisMask *param_1)

{
  (*(code *)PTR_KisMask_0083c740)();
  return;
}



// ====== KisMask @ 0020c140 ======

void __thiscall KisMask::KisMask(KisMask *this,KisWeakSharedPtr param_1,QString *param_2)

{
  (*(code *)PTR_KisMask_0083db70)();
  return;
}



// ====== KisMask @ 00574970 ======

/* KisMask::KisMask(KisWeakSharedPtr<KisImage>, QString const&) */

void __thiscall KisMask::KisMask(KisMask *this,KisWeakSharedPtr param_1,QString *param_2)

{
  long *plVar1;
  int iVar2;
  long lVar3;
  long *plVar4;
  undefined auVar5 [8];
  undefined auVar6 [16];
  undefined auVar7 [16];
  undefined *puVar8;
  undefined (*pauVar9) [16];
  void *pvVar10;
  undefined4 *puVar11;
  KisSafeSelectionNodeProjectionStore *this_00;
  int *piVar12;
  undefined4 in_register_00000034;
  long *plVar13;
  KisSafeSelectionNodeProjectionStore *this_01;
  long in_FS_OFFSET;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  plVar13 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar13 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar5 = local_58;
LAB_00574c29:
    local_58 = auVar5;
    piStack_50 = (int *)0x0;
  }
  else if (((uint *)plVar13[1] == (uint *)0x0) || ((*(uint *)plVar13[1] & 1) == 0)) {
    local_58 = (undefined  [8])0x0;
    piStack_50 = (int *)0x0;
  }
  else {
    auVar5 = (undefined  [8])*plVar13;
    piStack_50 = (int *)local_58;
    local_58 = auVar5;
    if (auVar5 == (undefined  [8])0x0) goto LAB_00574c29;
    piVar12 = *(int **)((long)auVar5 + 0x58);
    if (piVar12 == (int *)0x0) {
      piVar12 = (int *)operator_new(4);
      *piVar12 = 0;
      *(int **)((long)auVar5 + 0x58) = piVar12;
      LOCK();
      *piVar12 = *piVar12 + 1;
      UNLOCK();
      piVar12 = *(int **)((long)auVar5 + 0x58);
      auVar5 = local_58;
    }
    local_58 = auVar5;
    LOCK();
    *piVar12 = *piVar12 + 2;
    UNLOCK();
    piStack_50 = piVar12;
  }
                    /* try { // try from 005749cc to 005749d0 has its CatchHandler @ 00574c96 */
  KisNode::KisNode((KisNode *)this,(KisWeakSharedPtr)local_58);
  piVar12 = piStack_50;
  auVar6._8_8_ = 0;
  auVar6._0_8_ = piStack_50;
  _local_58 = auVar6 << 0x40;
  if (piVar12 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar12;
    *piVar12 = *piVar12 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar12 != (int *)0x0)) {
      operator_delete(piVar12,4);
    }
  }
                    /* try { // try from 005749fd to 00574a01 has its CatchHandler @ 00574cae */
  KisIndirectPaintingSupport::KisIndirectPaintingSupport
            ((KisIndirectPaintingSupport *)(this + 0x30));
  puVar8 = PTR_vtable_00837218 + 0x238;
  *(undefined **)this = PTR_vtable_00837218 + 0x10;
  *(undefined **)(this + 0x30) = puVar8;
                    /* try { // try from 00574a1f to 00574a23 has its CatchHandler @ 00574ca2 */
  pauVar9 = (undefined (*) [16])operator_new(0x48);
  *(KisMask **)pauVar9[2] = this;
  *(undefined8 *)pauVar9[1] = 0;
  *(undefined8 *)(pauVar9[1] + 8) = 0;
  *(undefined8 *)(pauVar9[2] + 8) = 0;
  *pauVar9 = (undefined  [16])0x0;
                    /* try { // try from 00574a4f to 00574a53 has its CatchHandler @ 00574cde */
  pvVar10 = operator_new(0x10);
                    /* try { // try from 00574a5e to 00574a62 has its CatchHandler @ 00574cd2 */
  FUN_00568760(pvVar10,*(undefined8 *)pauVar9[2]);
  *(void **)pauVar9[3] = pvVar10;
                    /* try { // try from 00574a6c to 00574a70 has its CatchHandler @ 00574cde */
  puVar11 = (undefined4 *)operator_new(0x18);
  *(void **)(puVar11 + 4) = pvVar10;
  *(code **)(puVar11 + 2) = FUN_00577df0;
  puVar11[1] = 1;
  *puVar11 = 1;
  *(undefined4 **)(pauVar9[3] + 8) = puVar11;
  *(undefined8 *)pauVar9[4] = 0;
  *(undefined (**) [16])(this + 0x40) = pauVar9;
                    /* try { // try from 00574aa5 to 00574abf has its CatchHandler @ 00574ca2 */
  QObject::setObjectName((QString *)this);
  (**(code **)(*(long *)this + 0x148))(this);
  this_00 = (KisSafeSelectionNodeProjectionStore *)operator_new(0x28);
                    /* try { // try from 00574ac6 to 00574aca has its CatchHandler @ 00574cc6 */
  KisSafeSelectionNodeProjectionStore::KisSafeSelectionNodeProjectionStore(this_00);
  lVar3 = *(long *)(this + 0x40);
  this_01 = *(KisSafeSelectionNodeProjectionStore **)(lVar3 + 0x40);
  if (this_00 != this_01) {
    LOCK();
    *(int *)(this_00 + 0x10) = *(int *)(this_00 + 0x10) + 1;
    UNLOCK();
    plVar4 = *(long **)(lVar3 + 0x40);
    *(KisSafeSelectionNodeProjectionStore **)(lVar3 + 0x40) = this_00;
    if (plVar4 != (long *)0x0) {
      LOCK();
      plVar1 = plVar4 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar4 + 0x20))();
      }
    }
    this_01 = *(KisSafeSelectionNodeProjectionStore **)(*(long *)(this + 0x40) + 0x40);
  }
  if (*plVar13 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar5 = local_58;
  }
  else {
    if (((uint *)plVar13[1] == (uint *)0x0) || ((*(uint *)plVar13[1] & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
      goto LAB_00574b25;
    }
    auVar5 = (undefined  [8])*plVar13;
    local_58 = auVar5;
    if (auVar5 != (undefined  [8])0x0) {
      piVar12 = *(int **)((long)auVar5 + 0x58);
      if (piVar12 == (int *)0x0) {
                    /* try { // try from 00574c75 to 00574c79 has its CatchHandler @ 00574ca2 */
        piVar12 = (int *)operator_new(4);
        *piVar12 = 0;
        *(int **)((long)auVar5 + 0x58) = piVar12;
        LOCK();
        *piVar12 = *piVar12 + 1;
        UNLOCK();
        piVar12 = *(int **)((long)auVar5 + 0x58);
        auVar5 = local_58;
      }
      local_58 = auVar5;
      LOCK();
      *piVar12 = *piVar12 + 2;
      UNLOCK();
      piStack_50 = piVar12;
      goto LAB_00574b25;
    }
  }
  local_58 = auVar5;
  piStack_50 = (int *)0x0;
LAB_00574b25:
                    /* try { // try from 00574b2b to 00574b2f has its CatchHandler @ 00574cba */
  KisSafeNodeProjectionStoreBase::setImage
            ((KisSafeNodeProjectionStoreBase *)this_01,(KisWeakSharedPtr)local_58);
  piVar12 = piStack_50;
  auVar7._8_8_ = 0;
  auVar7._0_8_ = piStack_50;
  _local_58 = auVar7 << 0x40;
  if (piVar12 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar12;
    *piVar12 = *piVar12 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar12 != (int *)0x0)) {
      operator_delete(piVar12,4);
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisMask @ 00574cf0 ======

/* KisMask::KisMask(KisMask const&) */

void __thiscall KisMask::KisMask(KisMask *this,KisMask *param_1)

{
  QArrayData *pQVar1;
  int iVar2;
  long *plVar3;
  long *plVar4;
  undefined *puVar5;
  undefined (*pauVar6) [16];
  void *pvVar7;
  undefined4 *puVar8;
  KisSafeSelectionNodeProjectionStore *this_00;
  KisSelection *this_01;
  long lVar9;
  KisKeyframeChannel *pKVar10;
  int *piVar11;
  KisSelection *this_02;
  long in_FS_OFFSET;
  KisPaintDevice *local_48;
  int *local_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisNode::KisNode((KisNode *)this,(KisNode *)param_1);
                    /* try { // try from 00574d22 to 00574d26 has its CatchHandler @ 00575058 */
  KisIndirectPaintingSupport::KisIndirectPaintingSupport
            ((KisIndirectPaintingSupport *)(this + 0x30));
  puVar5 = PTR_vtable_00837218 + 0x238;
  *(undefined **)this = PTR_vtable_00837218 + 0x10;
  *(undefined **)(this + 0x30) = puVar5;
                    /* try { // try from 00574d44 to 00574d48 has its CatchHandler @ 00575028 */
  pauVar6 = (undefined (*) [16])operator_new(0x48);
  *(KisMask **)pauVar6[2] = this;
  *(undefined8 *)pauVar6[1] = 0;
  *(undefined8 *)(pauVar6[1] + 8) = 0;
  *(undefined8 *)(pauVar6[2] + 8) = 0;
  *pauVar6 = (undefined  [16])0x0;
                    /* try { // try from 00574d74 to 00574d78 has its CatchHandler @ 00575040 */
  pvVar7 = operator_new(0x10);
                    /* try { // try from 00574d83 to 00574d87 has its CatchHandler @ 00575010 */
  FUN_00568760(pvVar7,*(undefined8 *)pauVar6[2]);
  *(void **)pauVar6[3] = pvVar7;
                    /* try { // try from 00574d91 to 00574d95 has its CatchHandler @ 00575040 */
  puVar8 = (undefined4 *)operator_new(0x18);
  *(void **)(puVar8 + 4) = pvVar7;
  *(code **)(puVar8 + 2) = FUN_00577df0;
  puVar8[1] = 1;
  *puVar8 = 1;
  *(undefined4 **)(pauVar6[3] + 8) = puVar8;
  *(undefined8 *)pauVar6[4] = 0;
  *(undefined (**) [16])(this + 0x40) = pauVar6;
                    /* try { // try from 00574dcb to 00574dcf has its CatchHandler @ 00575028 */
  QObject::objectName();
                    /* try { // try from 00574dd6 to 00574de6 has its CatchHandler @ 0057504c */
  QObject::setObjectName((QString *)this);
  (**(code **)(*(long *)this + 0x148))(this);
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_00574e08;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_00574e08;
  }
  QArrayData::deallocate((QArrayData *)local_48,2,8);
LAB_00574e08:
                    /* try { // try from 00574e0d to 00574e11 has its CatchHandler @ 00575028 */
  this_00 = (KisSafeSelectionNodeProjectionStore *)operator_new(0x28);
                    /* try { // try from 00574e21 to 00574e25 has its CatchHandler @ 00574ff8 */
  KisSafeSelectionNodeProjectionStore::KisSafeSelectionNodeProjectionStore
            (this_00,*(KisSafeSelectionNodeProjectionStore **)(*(long *)(param_1 + 0x40) + 0x40));
  lVar9 = *(long *)(this + 0x40);
  if (this_00 != *(KisSafeSelectionNodeProjectionStore **)(lVar9 + 0x40)) {
    LOCK();
    *(int *)(this_00 + 0x10) = *(int *)(this_00 + 0x10) + 1;
    UNLOCK();
    plVar3 = *(long **)(lVar9 + 0x40);
    *(KisSafeSelectionNodeProjectionStore **)(lVar9 + 0x40) = this_00;
    if (plVar3 != (long *)0x0) {
      LOCK();
      plVar4 = plVar3 + 2;
      *(int *)plVar4 = *(int *)plVar4 + -1;
      UNLOCK();
      if (*(int *)plVar4 == 0) {
        (**(code **)(*plVar3 + 0x20))();
      }
    }
  }
  if (**(long **)(param_1 + 0x40) != 0) {
                    /* try { // try from 00574e61 to 00574e65 has its CatchHandler @ 00575028 */
    this_01 = (KisSelection *)operator_new(0x20);
                    /* try { // try from 00574e74 to 00574e78 has its CatchHandler @ 00575034 */
    KisSelection::KisSelection(this_01,(KisSelection *)**(undefined8 **)(param_1 + 0x40));
    plVar3 = *(long **)(this + 0x40);
    this_02 = (KisSelection *)*plVar3;
    if (this_01 != this_02) {
      LOCK();
      *(int *)(this_01 + 8) = *(int *)(this_01 + 8) + 1;
      UNLOCK();
      plVar4 = (long *)*plVar3;
      *plVar3 = (long)this_01;
      if (plVar4 != (long *)0x0) {
        LOCK();
        plVar3 = plVar4 + 1;
        *(int *)plVar3 = *(int *)plVar3 + -1;
        UNLOCK();
        if (*(int *)plVar3 == 0) {
          (**(code **)(*plVar4 + 8))();
        }
      }
      this_02 = (KisSelection *)**(undefined8 **)(this + 0x40);
    }
    local_40 = *(int **)(this + 0x18);
    local_48 = (KisPaintDevice *)this;
    if (local_40 == (int *)0x0) {
                    /* try { // try from 00574fc5 to 00574fc9 has its CatchHandler @ 00575028 */
      piVar11 = (int *)operator_new(4);
      *piVar11 = 0;
      *(int **)(this + 0x18) = piVar11;
      LOCK();
      *piVar11 = *piVar11 + 1;
      UNLOCK();
      local_40 = *(int **)(this + 0x18);
    }
    LOCK();
    *local_40 = *local_40 + 2;
    UNLOCK();
                    /* try { // try from 00574ec7 to 00574ecb has its CatchHandler @ 00575004 */
    KisSelection::setParentNode(this_02,(KisWeakSharedPtr)&local_48);
    local_48 = (KisPaintDevice *)0x0;
    if (local_40 != (int *)0x0) {
      LOCK();
      iVar2 = *local_40;
      *local_40 = *local_40 + -2;
      UNLOCK();
      if ((iVar2 < 3) && (local_40 != (int *)0x0)) {
        operator_delete(local_40,4);
      }
    }
                    /* try { // try from 00574efa to 00574efe has its CatchHandler @ 00575028 */
    KisSelection::pixelSelection();
                    /* try { // try from 00574f03 to 00574f28 has its CatchHandler @ 0057501c */
    lVar9 = KisPaintDevice::framesInterface(local_48);
    if (lVar9 != 0) {
      pKVar10 = (KisKeyframeChannel *)KisPaintDevice::keyframeChannel(local_48);
      KisNode::addKeyframeChannel((KisNode *)this,pKVar10);
      KisBaseNode::enableAnimation((KisBaseNode *)this);
    }
    if (local_48 != (KisPaintDevice *)0x0) {
      LOCK();
      pQVar1 = (QArrayData *)(local_48 + 0x10);
      *(int *)pQVar1 = *(int *)pQVar1 + -1;
      UNLOCK();
      if (*(int *)pQVar1 == 0) {
        (**(code **)(*(long *)local_48 + 0x20))();
      }
    }
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



