/* Class KisSelection - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSelection @ 00201710 ======

void __thiscall
KisSelection::KisSelection(KisSelection *this,KisSharedPtr param_1,QSharedPointer param_2)

{
  (*(code *)PTR_KisSelection_00838658)();
  return;
}



// ====== KisSelection @ 0020bdb0 ======

void __thiscall KisSelection::KisSelection(KisSelection *this)

{
  (*(code *)PTR_KisSelection_0083d9a8)();
  return;
}



// ====== KisSelection @ 0020d2f0 ======

void __thiscall KisSelection::KisSelection(KisSelection *this,KisSelection *param_1)

{
  (*(code *)PTR_KisSelection_0083e448)();
  return;
}



// ====== KisSelection @ 0020da40 ======

void __thiscall
KisSelection::KisSelection
          (KisSelection *this,KisSharedPtr param_1,DeviceCopyMode param_2,KisSharedPtr param_3,
          QSharedPointer param_4)

{
  (*(code *)PTR_KisSelection_0083e7f0)();
  return;
}



// ====== KisSelection @ 005f5320 ======

/* KisSelection::KisSelection(KisSelection const&) */

void __thiscall KisSelection::KisSelection(KisSelection *this,KisSelection *param_1)

{
  undefined (*pauVar1) [16];
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837fb0 + 0x10;
                    /* try { // try from 005f5354 to 005f5358 has its CatchHandler @ 005f53b4 */
  pauVar1 = (undefined (*) [16])operator_new(0x78);
  pauVar1[1][0] = 1;
  *(KisSelection **)(pauVar1[3] + 8) = this;
  *(undefined8 *)pauVar1[4] = 0;
  *(undefined8 *)(pauVar1[6] + 8) = 0;
  *pauVar1 = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar1[1] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar1[2] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar1[4] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar1[5] + 8) = (undefined  [16])0x0;
                    /* try { // try from 005f5395 to 005f5399 has its CatchHandler @ 005f53c0 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(pauVar1 + 7),0);
  *(undefined (**) [16])(this + 0x18) = pauVar1;
                    /* try { // try from 005f53a4 to 005f53a8 has its CatchHandler @ 005f53b4 */
  copyFrom(this,param_1);
  return;
}



// ====== KisSelection @ 005f53d0 ======

/* KisSelection::KisSelection(KisSharedPtr<KisDefaultBoundsBase>,
   QSharedPointer<KisImageResolutionProxy>) */

void __thiscall
KisSelection::KisSelection(KisSelection *this,KisSharedPtr param_1,QSharedPointer param_2)

{
  long *plVar1;
  int iVar2;
  long lVar3;
  int *piVar4;
  KisPaintDevice *this_00;
  undefined auVar5 [8];
  long *plVar6;
  undefined auVar7 [16];
  undefined auVar8 [16];
  undefined auVar9 [16];
  undefined auVar10 [16];
  undefined (*pauVar11) [16];
  long lVar12;
  KisPixelSelection *this_01;
  KisImageResolutionProxy *this_02;
  QObject *pQVar13;
  KisSelectionEmptyBounds *this_03;
  int *piVar14;
  undefined4 in_register_00000014;
  long *plVar15;
  undefined4 in_register_00000034;
  long *plVar16;
  long in_FS_OFFSET;
  long *local_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  plVar16 = (long *)CONCAT44(in_register_00000034,param_1);
  plVar15 = (long *)CONCAT44(in_register_00000014,param_2);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837fb0 + 0x10;
                    /* try { // try from 005f541a to 005f541e has its CatchHandler @ 005f58c6 */
  pauVar11 = (undefined (*) [16])operator_new(0x78);
  pauVar11[1][0] = 1;
  *(KisSelection **)(pauVar11[3] + 8) = this;
  *(undefined8 *)pauVar11[4] = 0;
  *(undefined8 *)(pauVar11[6] + 8) = 0;
  *pauVar11 = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar11[1] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar11[2] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar11[4] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar11[5] + 8) = (undefined  [16])0x0;
                    /* try { // try from 005f545b to 005f545f has its CatchHandler @ 005f5896 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(pauVar11 + 7),0);
  lVar12 = *plVar16;
  *(undefined (**) [16])(this + 0x18) = pauVar11;
  if (lVar12 == 0) {
                    /* try { // try from 005f57ad to 005f57b1 has its CatchHandler @ 005f58c6 */
    this_03 = (KisSelectionEmptyBounds *)operator_new(0x20);
    _local_58 = (undefined  [16])0x0;
                    /* try { // try from 005f57c9 to 005f57cd has its CatchHandler @ 005f58ba */
    KisSelectionEmptyBounds::KisSelectionEmptyBounds(this_03,(KisWeakSharedPtr)local_58);
    if (this_03 != (KisSelectionEmptyBounds *)*plVar16) {
      LOCK();
      *(int *)(this_03 + 8) = *(int *)(this_03 + 8) + 1;
      UNLOCK();
      plVar6 = (long *)*plVar16;
      *plVar16 = (long)this_03;
      if (plVar6 != (long *)0x0) {
        LOCK();
        plVar1 = plVar6 + 1;
        *(int *)plVar1 = *(int *)plVar1 + -1;
        UNLOCK();
        if (*(int *)plVar1 == 0) {
          (**(code **)(*plVar6 + 8))();
        }
      }
    }
    piVar14 = piStack_50;
    auVar10._8_8_ = 0;
    auVar10._0_8_ = piStack_50;
    _local_58 = auVar10 << 0x40;
    if (piVar14 != (int *)0x0) {
      LOCK();
      iVar2 = *piVar14;
      *piVar14 = *piVar14 + -2;
      UNLOCK();
      if ((iVar2 < 3) && (piVar14 != (int *)0x0)) {
        operator_delete(piVar14,4);
      }
    }
  }
  lVar12 = *plVar15;
  if (lVar12 == 0) {
                    /* try { // try from 005f567d to 005f5681 has its CatchHandler @ 005f58c6 */
    this_02 = (KisImageResolutionProxy *)operator_new(0x18);
    _local_58 = (undefined  [16])0x0;
                    /* try { // try from 005f569b to 005f569f has its CatchHandler @ 005f58a2 */
    KisImageResolutionProxy::KisImageResolutionProxy(this_02,(KisWeakSharedPtr)local_58);
                    /* try { // try from 005f56a5 to 005f56a9 has its CatchHandler @ 005f58d2 */
    pQVar13 = (QObject *)operator_new(0x18);
    *(KisImageResolutionProxy **)(pQVar13 + 0x10) = this_02;
    *(code **)(pQVar13 + 8) = FUN_00368510;
    *(undefined4 *)(pQVar13 + 4) = 1;
    *(undefined4 *)pQVar13 = 1;
                    /* try { // try from 005f56d8 to 005f56dc has its CatchHandler @ 005f58ae */
    QtSharedPointer::ExternalRefCountData::setQObjectShared(pQVar13,SUB81(this_02,0));
    piVar14 = (int *)plVar15[1];
    plVar15[1] = (long)pQVar13;
    *plVar15 = (long)this_02;
    if (piVar14 != (int *)0x0) {
      LOCK();
      piVar4 = piVar14 + 1;
      *piVar4 = *piVar4 + -1;
      UNLOCK();
      if (*piVar4 == 0) {
        (**(code **)(piVar14 + 2))(piVar14);
      }
      LOCK();
      *piVar14 = *piVar14 + -1;
      UNLOCK();
      if (*piVar14 == 0) {
        operator_delete(piVar14,0x10);
      }
    }
    piVar14 = piStack_50;
    auVar9._8_8_ = 0;
    auVar9._0_8_ = piStack_50;
    _local_58 = auVar9 << 0x40;
    if (piVar14 != (int *)0x0) {
      LOCK();
      iVar2 = *piVar14;
      *piVar14 = *piVar14 + -2;
      UNLOCK();
      if ((iVar2 < 3) && (piVar14 != (int *)0x0)) {
        operator_delete(piVar14,4);
      }
    }
    lVar12 = *plVar15;
  }
  piVar14 = (int *)plVar15[1];
  lVar3 = *(long *)(this + 0x18);
  if (piVar14 != (int *)0x0) {
    LOCK();
    *piVar14 = *piVar14 + 1;
    UNLOCK();
    LOCK();
    piVar14[1] = piVar14[1] + 1;
    UNLOCK();
  }
  piVar4 = *(int **)(lVar3 + 0x20);
  *(long *)(lVar3 + 0x18) = lVar12;
  *(int **)(lVar3 + 0x20) = piVar14;
  if (piVar4 != (int *)0x0) {
    LOCK();
    piVar14 = piVar4 + 1;
    *piVar14 = *piVar14 + -1;
    UNLOCK();
    if (*piVar14 == 0) {
      (**(code **)(piVar4 + 2))(piVar4);
    }
    LOCK();
    *piVar4 = *piVar4 + -1;
    UNLOCK();
    if (*piVar4 == 0) {
      operator_delete(piVar4,0x10);
    }
  }
                    /* try { // try from 005f54ca to 005f54ce has its CatchHandler @ 005f58c6 */
  this_01 = (KisPixelSelection *)operator_new(0x38);
  piVar14 = *(int **)(this + 0x10);
  local_58 = (undefined  [8])this;
  auVar5 = (undefined  [8])this;
  if (piVar14 == (int *)0x0) {
                    /* try { // try from 005f583d to 005f5841 has its CatchHandler @ 005f58ea */
    piVar14 = (int *)operator_new(4);
    *piVar14 = 0;
    *(int **)(this + 0x10) = piVar14;
    LOCK();
    *piVar14 = *piVar14 + 1;
    UNLOCK();
    piVar14 = *(int **)(this + 0x10);
    auVar5 = local_58;
  }
  local_58 = auVar5;
  piStack_50 = piVar14;
  LOCK();
  *piVar14 = *piVar14 + 2;
  UNLOCK();
  local_60 = (long *)*plVar16;
  if (local_60 != (long *)0x0) {
    LOCK();
    *(int *)(local_60 + 1) = *(int *)(local_60 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 005f5510 to 005f5514 has its CatchHandler @ 005f58de */
  KisPixelSelection::KisPixelSelection(this_01,(KisSharedPtr)&local_60,(KisWeakSharedPtr)local_58);
  lVar12 = *(long *)(this + 0x18);
  if (this_01 != *(KisPixelSelection **)(lVar12 + 0x28)) {
    LOCK();
    *(int *)(this_01 + 0x10) = *(int *)(this_01 + 0x10) + 1;
    UNLOCK();
    plVar15 = *(long **)(lVar12 + 0x28);
    *(KisPixelSelection **)(lVar12 + 0x28) = this_01;
    if (plVar15 != (long *)0x0) {
      LOCK();
      plVar16 = plVar15 + 2;
      *(int *)plVar16 = *(int *)plVar16 + -1;
      UNLOCK();
      if (*(int *)plVar16 == 0) {
        (**(code **)(*plVar15 + 0x20))();
      }
    }
  }
  if (local_60 != (long *)0x0) {
    LOCK();
    plVar15 = local_60 + 1;
    *(int *)plVar15 = *(int *)plVar15 + -1;
    UNLOCK();
    if (*(int *)plVar15 == 0) {
      (**(code **)(*local_60 + 8))();
    }
  }
  piVar14 = piStack_50;
  auVar7._8_8_ = 0;
  auVar7._0_8_ = piStack_50;
  _local_58 = auVar7 << 0x40;
  if (piVar14 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar14;
    *piVar14 = *piVar14 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar14 != (int *)0x0)) {
      operator_delete(piVar14,4);
      piVar14 = piStack_50;
    }
  }
  piStack_50 = piVar14;
  plVar15 = *(long **)(this + 0x18);
  this_00 = (KisPaintDevice *)plVar15[5];
  if (*plVar15 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar5 = local_58;
  }
  else {
    if (((uint *)plVar15[1] == (uint *)0x0) || ((*(uint *)plVar15[1] & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
      goto LAB_005f55a5;
    }
    auVar5 = (undefined  [8])*plVar15;
    local_58 = auVar5;
    if (auVar5 != (undefined  [8])0x0) {
      piVar14 = *(int **)((long)auVar5 + 0x18);
      if (piVar14 == (int *)0x0) {
                    /* try { // try from 005f5875 to 005f5879 has its CatchHandler @ 005f58c6 */
        piVar14 = (int *)operator_new(4);
        *piVar14 = 0;
        *(int **)((long)auVar5 + 0x18) = piVar14;
        LOCK();
        *piVar14 = *piVar14 + 1;
        UNLOCK();
        piVar14 = *(int **)((long)auVar5 + 0x18);
        auVar5 = local_58;
      }
      local_58 = auVar5;
      LOCK();
      *piVar14 = *piVar14 + 2;
      UNLOCK();
      piStack_50 = piVar14;
      goto LAB_005f55a5;
    }
  }
  local_58 = auVar5;
  piStack_50 = (int *)0x0;
LAB_005f55a5:
                    /* try { // try from 005f55ab to 005f55af has its CatchHandler @ 005f58f6 */
  KisPaintDevice::setParentNode(this_00,(KisWeakSharedPtr)local_58);
  piVar14 = piStack_50;
  auVar8._8_8_ = 0;
  auVar8._0_8_ = piStack_50;
  _local_58 = auVar8 << 0x40;
  if (piVar14 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar14;
    *piVar14 = *piVar14 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar14 != (int *)0x0)) {
      operator_delete(piVar14,4);
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSelection @ 005f5910 ======

/* KisSelection::KisSelection() */

void __thiscall KisSelection::KisSelection(KisSelection *this)

{
  long *plVar1;
  int *piVar2;
  undefined8 uVar3;
  long in_FS_OFFSET;
  long *local_30;
  undefined local_28 [24];
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_30 = (long *)0x0;
  local_28._0_16_ = (undefined  [16])0x0;
                    /* try { // try from 005f5945 to 005f5949 has its CatchHandler @ 005f59ad */
  KisSelection(this,(KisSharedPtr)&local_30,(QSharedPointer)local_28);
  if (local_30 != (long *)0x0) {
    LOCK();
    plVar1 = local_30 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_30 + 8))();
    }
  }
  uVar3 = local_28._8_8_;
  if ((int *)local_28._8_8_ != (int *)0x0) {
    LOCK();
    piVar2 = (int *)(local_28._8_8_ + 4);
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (*piVar2 == 0) {
      (**(code **)(local_28._8_8_ + 8))(local_28._8_8_);
    }
    LOCK();
    *(int *)uVar3 = *(int *)uVar3 + -1;
    UNLOCK();
    if (*(int *)uVar3 == 0) {
      operator_delete((void *)uVar3,0x10);
    }
  }
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSelection @ 005f59c0 ======

/* KisSelection::KisSelection(KisSharedPtr<KisPaintDevice>, KritaUtils::DeviceCopyMode,
   KisSharedPtr<KisDefaultBoundsBase>, QSharedPointer<KisImageResolutionProxy>) */

void __thiscall
KisSelection::KisSelection
          (KisSelection *this,KisSharedPtr param_1,DeviceCopyMode param_2,KisSharedPtr param_3,
          QSharedPointer param_4)

{
  long *plVar1;
  int iVar2;
  int *piVar3;
  long *plVar4;
  KisPaintDevice *pKVar5;
  undefined auVar6 [8];
  long lVar7;
  undefined8 uVar8;
  undefined auVar9 [16];
  undefined auVar10 [16];
  undefined (*pauVar11) [16];
  KisPixelSelection *pKVar12;
  KisSelectionEmptyBounds *this_00;
  int *piVar13;
  KisWeakSharedPtr KVar14;
  undefined4 in_register_0000000c;
  long *plVar15;
  undefined4 in_register_00000034;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  long *local_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  plVar15 = (long *)CONCAT44(in_register_0000000c,param_3);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837fb0 + 0x10;
                    /* try { // try from 005f5a12 to 005f5a16 has its CatchHandler @ 005f5e66 */
  pauVar11 = (undefined (*) [16])operator_new(0x78);
  pauVar11[1][0] = 1;
  *(KisSelection **)(pauVar11[3] + 8) = this;
  *(undefined8 *)pauVar11[4] = 0;
  *(undefined8 *)(pauVar11[6] + 8) = 0;
  *pauVar11 = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar11[1] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar11[2] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar11[4] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar11[5] + 8) = (undefined  [16])0x0;
                    /* try { // try from 005f5a53 to 005f5a57 has its CatchHandler @ 005f5e7e */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(pauVar11 + 7),0);
  lVar7 = *plVar15;
  *(undefined (**) [16])(this + 0x18) = pauVar11;
  if (lVar7 == 0) {
                    /* try { // try from 005f5d0d to 005f5d11 has its CatchHandler @ 005f5e66 */
    this_00 = (KisSelectionEmptyBounds *)operator_new(0x20);
    _local_58 = (undefined  [16])0x0;
                    /* try { // try from 005f5d2b to 005f5d2f has its CatchHandler @ 005f5e4e */
    KisSelectionEmptyBounds::KisSelectionEmptyBounds(this_00,(KisWeakSharedPtr)local_58);
    if (this_00 != (KisSelectionEmptyBounds *)*plVar15) {
      LOCK();
      *(int *)(this_00 + 8) = *(int *)(this_00 + 8) + 1;
      UNLOCK();
      plVar4 = (long *)*plVar15;
      *plVar15 = (long)this_00;
      if (plVar4 != (long *)0x0) {
        LOCK();
        plVar1 = plVar4 + 1;
        *(int *)plVar1 = *(int *)plVar1 + -1;
        UNLOCK();
        if (*(int *)plVar1 == 0) {
          (**(code **)(*plVar4 + 8))();
        }
      }
    }
    piVar13 = piStack_50;
    auVar10._8_8_ = 0;
    auVar10._0_8_ = piStack_50;
    _local_58 = auVar10 << 0x40;
    if (piVar13 != (int *)0x0) {
      LOCK();
      iVar2 = *piVar13;
      *piVar13 = *piVar13 + -2;
      UNLOCK();
      if ((iVar2 < 3) && (piVar13 != (int *)0x0)) {
        operator_delete(piVar13,4);
      }
    }
    pauVar11 = *(undefined (**) [16])(this + 0x18);
  }
  uVar8 = *(undefined8 *)CONCAT44(in_register_00000084,param_4);
  piVar13 = (int *)((undefined8 *)CONCAT44(in_register_00000084,param_4))[1];
  if (piVar13 != (int *)0x0) {
    LOCK();
    *piVar13 = *piVar13 + 1;
    UNLOCK();
    LOCK();
    piVar13[1] = piVar13[1] + 1;
    UNLOCK();
  }
  piVar3 = *(int **)pauVar11[2];
  *(undefined8 *)(pauVar11[1] + 8) = uVar8;
  *(int **)pauVar11[2] = piVar13;
  if (piVar3 != (int *)0x0) {
    LOCK();
    piVar13 = piVar3 + 1;
    *piVar13 = *piVar13 + -1;
    UNLOCK();
    if (*piVar13 == 0) {
      (**(code **)(piVar3 + 2))(piVar3);
    }
    LOCK();
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      operator_delete(piVar3,0x10);
    }
  }
                    /* try { // try from 005f5aae to 005f5ab2 has its CatchHandler @ 005f5e66 */
  pKVar12 = (KisPixelSelection *)operator_new(0x38);
  local_60 = *(long **)CONCAT44(in_register_00000034,param_1);
  _local_58 = (undefined  [16])0x0;
  if (local_60 != (long *)0x0) {
    LOCK();
    *(int *)(local_60 + 2) = *(int *)(local_60 + 2) + 1;
    UNLOCK();
  }
  KVar14 = (KisWeakSharedPtr)local_58;
                    /* try { // try from 005f5ae6 to 005f5aea has its CatchHandler @ 005f5e96 */
  KisPixelSelection::KisPixelSelection(pKVar12,(KisSharedPtr)&local_60,param_2,KVar14);
  lVar7 = *(long *)(this + 0x18);
  if (pKVar12 != *(KisPixelSelection **)(lVar7 + 0x28)) {
    LOCK();
    *(int *)(pKVar12 + 0x10) = *(int *)(pKVar12 + 0x10) + 1;
    UNLOCK();
    plVar4 = *(long **)(lVar7 + 0x28);
    *(KisPixelSelection **)(lVar7 + 0x28) = pKVar12;
    if (plVar4 != (long *)0x0) {
      LOCK();
      plVar1 = plVar4 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar4 + 0x20))();
      }
    }
  }
  if (local_60 != (long *)0x0) {
    LOCK();
    plVar4 = local_60 + 2;
    *(int *)plVar4 = *(int *)plVar4 + -1;
    UNLOCK();
    if (*(int *)plVar4 == 0) {
      (**(code **)(*local_60 + 0x20))();
    }
  }
  local_58 = (undefined  [8])0x0;
  if (piStack_50 != (int *)0x0) {
    LOCK();
    iVar2 = *piStack_50;
    *piStack_50 = *piStack_50 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piStack_50 != (int *)0x0)) {
      operator_delete(piStack_50,4);
    }
  }
  pKVar12 = *(KisPixelSelection **)(*(long *)(this + 0x18) + 0x28);
  piVar13 = *(int **)(this + 0x10);
  local_58 = (undefined  [8])this;
  auVar6 = (undefined  [8])this;
  if (piVar13 == (int *)0x0) {
                    /* try { // try from 005f5df5 to 005f5e31 has its CatchHandler @ 005f5e66 */
    piVar13 = (int *)operator_new(4);
    *piVar13 = 0;
    *(int **)(this + 0x10) = piVar13;
    LOCK();
    *piVar13 = *piVar13 + 1;
    UNLOCK();
    piVar13 = *(int **)(this + 0x10);
    auVar6 = local_58;
  }
  local_58 = auVar6;
  LOCK();
  *piVar13 = *piVar13 + 2;
  UNLOCK();
  piStack_50 = piVar13;
                    /* try { // try from 005f5b76 to 005f5b7a has its CatchHandler @ 005f5e5a */
  KisPixelSelection::setParentSelection(pKVar12,KVar14);
  piVar13 = piStack_50;
  auVar9._8_8_ = 0;
  auVar9._0_8_ = piStack_50;
  _local_58 = auVar9 << 0x40;
  if (piVar13 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar13;
    *piVar13 = *piVar13 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar13 != (int *)0x0)) {
      operator_delete(piVar13,4);
      piVar13 = piStack_50;
    }
  }
  piStack_50 = piVar13;
  plVar4 = *(long **)(this + 0x18);
  pKVar5 = (KisPaintDevice *)plVar4[5];
  if (*plVar4 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar6 = local_58;
  }
  else {
    if (((uint *)plVar4[1] == (uint *)0x0) || ((*(uint *)plVar4[1] & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
      goto LAB_005f5bcf;
    }
    auVar6 = (undefined  [8])*plVar4;
    local_58 = auVar6;
    if (auVar6 != (undefined  [8])0x0) {
      piVar13 = *(int **)((long)auVar6 + 0x18);
      if (piVar13 == (int *)0x0) {
        piVar13 = (int *)operator_new(4);
        *piVar13 = 0;
        *(int **)((long)auVar6 + 0x18) = piVar13;
        LOCK();
        *piVar13 = *piVar13 + 1;
        UNLOCK();
        piVar13 = *(int **)((long)auVar6 + 0x18);
        auVar6 = local_58;
      }
      local_58 = auVar6;
      LOCK();
      *piVar13 = *piVar13 + 2;
      UNLOCK();
      piStack_50 = piVar13;
      goto LAB_005f5bcf;
    }
  }
  local_58 = auVar6;
  piStack_50 = (int *)0x0;
LAB_005f5bcf:
                    /* try { // try from 005f5bd5 to 005f5bd9 has its CatchHandler @ 005f5e8a */
  KisPaintDevice::setParentNode(pKVar5,KVar14);
  local_58 = (undefined  [8])0x0;
  if (piStack_50 != (int *)0x0) {
    LOCK();
    iVar2 = *piStack_50;
    *piStack_50 = *piStack_50 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piStack_50 != (int *)0x0)) {
      operator_delete(piStack_50,4);
    }
  }
  pKVar5 = *(KisPaintDevice **)(*(long *)(this + 0x18) + 0x28);
  local_58 = (undefined  [8])*plVar15;
  if (local_58 != (undefined  [8])0x0) {
    LOCK();
    *(int *)((long)local_58 + 8) = *(int *)((long)local_58 + 8) + 1;
    UNLOCK();
  }
                    /* try { // try from 005f5c19 to 005f5c1d has its CatchHandler @ 005f5e72 */
  KisPaintDevice::setDefaultBounds(pKVar5,KVar14);
  if (local_58 != (undefined  [8])0x0) {
    LOCK();
    plVar15 = (long *)((long)local_58 + 8);
    *(int *)plVar15 = *(int *)plVar15 + -1;
    UNLOCK();
    if (*(int *)plVar15 == 0) {
      (**(code **)(*(long *)local_58 + 8))();
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



