/* Class KisPixelSelection - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPixelSelection @ 002086f0 ======

void __thiscall
KisPixelSelection::KisPixelSelection
          (KisPixelSelection *this,KisPixelSelection *param_1,DeviceCopyMode param_2)

{
  (*(code *)PTR_KisPixelSelection_0083be48)();
  return;
}



// ====== KisPixelSelection @ 0020a0a0 ======

void __thiscall
KisPixelSelection::KisPixelSelection
          (KisPixelSelection *this,KisSharedPtr param_1,KisWeakSharedPtr param_2)

{
  (*(code *)PTR_KisPixelSelection_0083cb20)();
  return;
}



// ====== KisPixelSelection @ 0020c7d0 ======

void __thiscall
KisPixelSelection::KisPixelSelection
          (KisPixelSelection *this,KisSharedPtr param_1,DeviceCopyMode param_2,
          KisWeakSharedPtr param_3)

{
  (*(code *)PTR_KisPixelSelection_0083deb8)();
  return;
}



// ====== KisPixelSelection @ 005e30e0 ======

/* KisPixelSelection::KisPixelSelection(KisPixelSelection const&, KritaUtils::DeviceCopyMode) */

void __thiscall
KisPixelSelection::KisPixelSelection
          (KisPixelSelection *this,KisPixelSelection *param_1,DeviceCopyMode param_2)

{
  long lVar1;
  long lVar2;
  undefined *puVar3;
  undefined (*pauVar4) [16];
  
  KisPaintDevice::KisPaintDevice
            ((KisPaintDevice *)this,(KisPaintDevice *)param_1,param_2,(KisNode *)0x0);
  puVar3 = PTR_vtable_00837778 + 0xd0;
  *(undefined **)this = PTR_vtable_00837778 + 0x10;
  *(undefined **)(this + 0x28) = puVar3;
                    /* try { // try from 005e3117 to 005e311b has its CatchHandler @ 005e31b1 */
  pauVar4 = (undefined (*) [16])operator_new(0xb0);
  *pauVar4 = (undefined  [16])0x0;
  QPainterPath::QPainterPath((QPainterPath *)(pauVar4 + 1));
  *(undefined8 *)pauVar4[2] = 0;
  QImage::QImage((QImage *)(pauVar4 + 3));
                    /* try { // try from 005e314a to 005e314e has its CatchHandler @ 005e31bd */
  QTransform::QTransform((QTransform *)(pauVar4 + 5));
  lVar1 = *(long *)(param_1 + 0x30);
  *(undefined (**) [16])(this + 0x30) = pauVar4;
  *(undefined8 *)(pauVar4[10] + 8) = 0;
                    /* try { // try from 005e316a to 005e3192 has its CatchHandler @ 005e31b1 */
  QPainterPath::operator=((QPainterPath *)(pauVar4 + 1),(QPainterPath *)(lVar1 + 0x10));
  lVar1 = *(long *)(param_1 + 0x30);
  lVar2 = *(long *)(this + 0x30);
  *(undefined *)(lVar2 + 0x18) = *(undefined *)(lVar1 + 0x18);
  *(undefined *)(lVar2 + 0x28) = *(undefined *)(lVar1 + 0x28);
  QImage::operator=((QImage *)(lVar2 + 0x30),(QImage *)(lVar1 + 0x30));
  QTransform::operator=
            ((QTransform *)(*(long *)(this + 0x30) + 0x50),
             (QTransform *)(*(long *)(param_1 + 0x30) + 0x50));
  return;
}



// ====== KisPixelSelection @ 005e43b0 ======

/* KisPixelSelection::KisPixelSelection(KisSharedPtr<KisDefaultBoundsBase>,
   KisWeakSharedPtr<KisSelection>) */

void __thiscall
KisPixelSelection::KisPixelSelection
          (KisPixelSelection *this,KisSharedPtr param_1,KisWeakSharedPtr param_2)

{
  long *plVar1;
  int iVar2;
  undefined8 uVar3;
  undefined (*pauVar4) [16];
  long lVar5;
  undefined auVar6 [16];
  KoColorSpace *pKVar7;
  undefined *puVar8;
  undefined (*pauVar9) [16];
  uint *puVar10;
  int *piVar11;
  undefined4 in_register_00000014;
  long *plVar12;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_a8;
  QArrayData *local_a0;
  undefined local_98 [16];
  undefined8 local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  long local_40;
  
  plVar12 = (long *)CONCAT44(in_register_00000014,param_2);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_a0 = (QArrayData *)PTR_shared_null_008377d0;
  local_a8 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_a8 != (long *)0x0) {
    LOCK();
    *(int *)(local_a8 + 1) = *(int *)(local_a8 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 005e43f7 to 005e4403 has its CatchHandler @ 005e46c0 */
  KoColorSpaceRegistry::instance();
  pKVar7 = (KoColorSpace *)KoColorSpaceRegistry::alpha8();
  local_98 = (undefined  [16])0x0;
                    /* try { // try from 005e4428 to 005e442c has its CatchHandler @ 005e46e9 */
  KisPaintDevice::KisPaintDevice
            ((KisPaintDevice *)this,(KisWeakSharedPtr)(QImage *)local_98,pKVar7,
             (KisSharedPtr)&local_a8,(QString *)&local_a0);
  uVar3 = local_98._8_8_;
  auVar6._8_8_ = 0;
  auVar6._0_8_ = local_98._8_8_;
  local_98 = auVar6 << 0x40;
  if ((int *)uVar3 != (int *)0x0) {
    LOCK();
    iVar2 = *(int *)uVar3;
    *(int *)uVar3 = *(int *)uVar3 + -2;
    UNLOCK();
    if ((iVar2 < 3) && ((int *)uVar3 != (int *)0x0)) {
      operator_delete((void *)uVar3,4);
    }
  }
  if (local_a8 != (long *)0x0) {
    LOCK();
    plVar1 = local_a8 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_a8 + 8))();
    }
  }
  if (*(int *)local_a0 == 0) {
LAB_005e4670:
    QArrayData::deallocate(local_a0,2,8);
  }
  else if (*(int *)local_a0 != -1) {
    LOCK();
    *(int *)local_a0 = *(int *)local_a0 + -1;
    UNLOCK();
    if (*(int *)local_a0 == 0) goto LAB_005e4670;
  }
  puVar8 = PTR_vtable_00837778 + 0xd0;
  *(undefined **)this = PTR_vtable_00837778 + 0x10;
  *(undefined **)(this + 0x28) = puVar8;
                    /* try { // try from 005e44a8 to 005e44ac has its CatchHandler @ 005e46d1 */
  pauVar9 = (undefined (*) [16])operator_new(0xb0);
  *pauVar9 = (undefined  [16])0x0;
  QPainterPath::QPainterPath((QPainterPath *)(pauVar9 + 1));
  *(undefined8 *)pauVar9[2] = 0;
  QImage::QImage((QImage *)(pauVar9 + 3));
                    /* try { // try from 005e44e4 to 005e44e8 has its CatchHandler @ 005e46dd */
  QTransform::QTransform((QTransform *)(pauVar9 + 5));
  *(undefined8 *)(pauVar9[10] + 8) = 0;
  *(undefined (**) [16])(this + 0x30) = pauVar9;
  pauVar9[1][8] = 1;
  pauVar9[2][8] = 0;
  QImage::QImage((QImage *)local_98);
  uVar3 = *(undefined8 *)(pauVar9[4] + 8);
  *(undefined8 *)(pauVar9[4] + 8) = uStack_80;
  uStack_80 = uVar3;
  QImage::~QImage((QImage *)local_98);
                    /* try { // try from 005e4525 to 005e46a1 has its CatchHandler @ 005e46d1 */
  QTransform::QTransform((QTransform *)local_98);
  *(undefined8 *)pauVar9[5] = local_98._0_8_;
  *(undefined8 *)(pauVar9[5] + 8) = local_98._8_8_;
  pauVar4 = *(undefined (**) [16])(this + 0x30);
  *(undefined8 *)pauVar9[10] = local_48;
  *(undefined8 *)pauVar9[6] = local_88;
  *(undefined8 *)(pauVar9[6] + 8) = uStack_80;
  *(undefined8 *)pauVar9[7] = local_78;
  *(undefined8 *)(pauVar9[7] + 8) = uStack_70;
  *(undefined8 *)pauVar9[8] = local_68;
  *(undefined8 *)(pauVar9[8] + 8) = uStack_60;
  *(undefined8 *)pauVar9[9] = local_58;
  *(undefined8 *)(pauVar9[9] + 8) = uStack_50;
  piVar11 = *(int **)(*pauVar4 + 8);
  *(undefined8 *)*pauVar4 = 0;
  if (piVar11 == (int *)0x0) {
LAB_005e4594:
    puVar10 = (uint *)plVar12[1];
    if (*plVar12 != 0) goto LAB_005e45a3;
LAB_005e465a:
    *(undefined8 *)*pauVar4 = 0;
  }
  else {
    LOCK();
    iVar2 = *piVar11;
    *piVar11 = *piVar11 + -2;
    UNLOCK();
    if (2 < iVar2) goto LAB_005e4594;
    if (*(void **)(*pauVar4 + 8) != (void *)0x0) {
      operator_delete(*(void **)(*pauVar4 + 8),4);
    }
    lVar5 = *plVar12;
    *(undefined8 *)(*pauVar4 + 8) = 0;
    puVar10 = (uint *)plVar12[1];
    if (lVar5 == 0) goto LAB_005e465a;
LAB_005e45a3:
    if ((puVar10 == (uint *)0x0) || ((*puVar10 & 1) == 0)) {
      *pauVar4 = (undefined  [16])0x0;
      goto LAB_005e45e7;
    }
    lVar5 = *plVar12;
    *(long *)*pauVar4 = lVar5;
    if (lVar5 != 0) {
      piVar11 = *(int **)(lVar5 + 0x10);
      if (piVar11 == (int *)0x0) {
        piVar11 = (int *)operator_new(4);
        *piVar11 = 0;
        *(int **)(lVar5 + 0x10) = piVar11;
        LOCK();
        *piVar11 = *piVar11 + 1;
        UNLOCK();
        piVar11 = *(int **)(lVar5 + 0x10);
      }
      *(int **)(*pauVar4 + 8) = piVar11;
      LOCK();
      *piVar11 = *piVar11 + 2;
      UNLOCK();
      goto LAB_005e45e7;
    }
  }
  *(undefined8 *)(*pauVar4 + 8) = 0;
LAB_005e45e7:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPixelSelection @ 005e4f30 ======

/* KisPixelSelection::KisPixelSelection(KisSharedPtr<KisPaintDevice>, KritaUtils::DeviceCopyMode,
   KisWeakSharedPtr<KisSelection>) */

void __thiscall
KisPixelSelection::KisPixelSelection
          (KisPixelSelection *this,KisSharedPtr param_1,DeviceCopyMode param_2,
          KisWeakSharedPtr param_3)

{
  long *plVar1;
  KisPaintDevice *pKVar2;
  int iVar3;
  long lVar4;
  undefined8 uVar5;
  undefined auVar6 [16];
  KoColorSpace *pKVar7;
  undefined *puVar8;
  undefined (*pauVar9) [16];
  KisPaintDevice *this_00;
  uint *puVar10;
  int *piVar11;
  undefined4 in_register_0000000c;
  long *plVar12;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_a8;
  QArrayData *local_a0;
  undefined local_98 [16];
  undefined8 local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  long local_40;
  
  plVar12 = (long *)CONCAT44(in_register_0000000c,param_3);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_a0 = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 005e4f76 to 005e4f7a has its CatchHandler @ 005e52e2 */
  KisPaintDevice::defaultBounds();
                    /* try { // try from 005e4f7b to 005e4f87 has its CatchHandler @ 005e5303 */
  KoColorSpaceRegistry::instance();
  pKVar7 = (KoColorSpace *)KoColorSpaceRegistry::alpha8();
  local_98 = (undefined  [16])0x0;
                    /* try { // try from 005e4fae to 005e4fb2 has its CatchHandler @ 005e52f7 */
  KisPaintDevice::KisPaintDevice
            ((KisPaintDevice *)this,(KisWeakSharedPtr)(QImage *)local_98,pKVar7,
             (KisSharedPtr)&local_a8,(QString *)&local_a0);
  uVar5 = local_98._8_8_;
  auVar6._8_8_ = 0;
  auVar6._0_8_ = local_98._8_8_;
  local_98 = auVar6 << 0x40;
  if ((int *)uVar5 != (int *)0x0) {
    LOCK();
    iVar3 = *(int *)uVar5;
    *(int *)uVar5 = *(int *)uVar5 + -2;
    UNLOCK();
    if ((iVar3 < 3) && ((int *)uVar5 != (int *)0x0)) {
      operator_delete((void *)uVar5,4);
    }
  }
  if (local_a8 != (long *)0x0) {
    LOCK();
    plVar1 = local_a8 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_a8 + 8))();
    }
  }
  if (*(int *)local_a0 == 0) {
LAB_005e5258:
    QArrayData::deallocate(local_a0,2,8);
  }
  else if (*(int *)local_a0 != -1) {
    LOCK();
    *(int *)local_a0 = *(int *)local_a0 + -1;
    UNLOCK();
    if (*(int *)local_a0 == 0) goto LAB_005e5258;
  }
  puVar8 = PTR_vtable_00837778 + 0xd0;
  *(undefined **)this = PTR_vtable_00837778 + 0x10;
  *(undefined **)(this + 0x28) = puVar8;
                    /* try { // try from 005e502d to 005e5031 has its CatchHandler @ 005e52d6 */
  pauVar9 = (undefined (*) [16])operator_new(0xb0);
  *pauVar9 = (undefined  [16])0x0;
  QPainterPath::QPainterPath((QPainterPath *)(pauVar9 + 1));
  *(undefined8 *)pauVar9[2] = 0;
  QImage::QImage((QImage *)(pauVar9 + 3));
                    /* try { // try from 005e506a to 005e506e has its CatchHandler @ 005e5330 */
  QTransform::QTransform((QTransform *)(pauVar9 + 5));
  *(undefined8 *)(pauVar9[10] + 8) = 0;
  *(undefined (**) [16])(this + 0x30) = pauVar9;
                    /* try { // try from 005e5083 to 005e5087 has its CatchHandler @ 005e52d6 */
  this_00 = (KisPaintDevice *)operator_new(0x28);
                    /* try { // try from 005e5096 to 005e509a has its CatchHandler @ 005e5324 */
  KisPaintDevice::KisPaintDevice
            (this_00,*(KisPaintDevice **)CONCAT44(in_register_00000034,param_1),param_2,
             (KisNode *)0x0);
  pKVar2 = this_00 + 0x10;
  LOCK();
  *(int *)(this_00 + 0x10) = *(int *)(this_00 + 0x10) + 1;
  UNLOCK();
                    /* try { // try from 005e50a8 to 005e52b9 has its CatchHandler @ 005e5318 */
  pKVar7 = (KoColorSpace *)KisPaintDevice::colorSpace((KisPaintDevice *)this);
  KisPaintDevice::convertTo(this_00,pKVar7,0,0x2000,(KUndo2Command *)0x0,(KoUpdater *)0x0);
  KisPaintDevice::makeFullCopyFrom((KisPaintDevice *)this,this_00,param_2,(KisNode *)0x0);
  pauVar9 = *(undefined (**) [16])(this + 0x30);
  piVar11 = *(int **)(*pauVar9 + 8);
  *(undefined8 *)*pauVar9 = 0;
  if (piVar11 == (int *)0x0) {
LAB_005e50fc:
    puVar10 = (uint *)plVar12[1];
    if (*plVar12 != 0) goto LAB_005e510b;
LAB_005e521a:
    *(undefined8 *)*pauVar9 = 0;
  }
  else {
    LOCK();
    iVar3 = *piVar11;
    *piVar11 = *piVar11 + -2;
    UNLOCK();
    if (2 < iVar3) goto LAB_005e50fc;
    if (*(void **)(*pauVar9 + 8) != (void *)0x0) {
      operator_delete(*(void **)(*pauVar9 + 8),4);
    }
    lVar4 = *plVar12;
    *(undefined8 *)(*pauVar9 + 8) = 0;
    puVar10 = (uint *)plVar12[1];
    if (lVar4 == 0) goto LAB_005e521a;
LAB_005e510b:
    if ((puVar10 == (uint *)0x0) || ((*puVar10 & 1) == 0)) {
      *pauVar9 = (undefined  [16])0x0;
      goto LAB_005e5123;
    }
    lVar4 = *plVar12;
    *(long *)*pauVar9 = lVar4;
    if (lVar4 != 0) {
      piVar11 = *(int **)(lVar4 + 0x10);
      if (piVar11 == (int *)0x0) {
        piVar11 = (int *)operator_new(4);
        *piVar11 = 0;
        *(int **)(lVar4 + 0x10) = piVar11;
        LOCK();
        *piVar11 = *piVar11 + 1;
        UNLOCK();
        piVar11 = *(int **)(lVar4 + 0x10);
      }
      *(int **)(*pauVar9 + 8) = piVar11;
      LOCK();
      *piVar11 = *piVar11 + 2;
      UNLOCK();
      goto LAB_005e5123;
    }
  }
  *(undefined8 *)(*pauVar9 + 8) = 0;
LAB_005e5123:
  lVar4 = *(long *)(this + 0x30);
  *(undefined *)(lVar4 + 0x18) = 0;
  *(undefined *)(lVar4 + 0x28) = 0;
  QImage::QImage((QImage *)local_98);
  uVar5 = *(undefined8 *)(lVar4 + 0x48);
  *(undefined8 *)(lVar4 + 0x48) = uStack_80;
  uStack_80 = uVar5;
  QImage::~QImage((QImage *)local_98);
  QTransform::QTransform((QTransform *)local_98);
  *(undefined8 *)(lVar4 + 0x50) = local_98._0_8_;
  *(undefined8 *)(lVar4 + 0x58) = local_98._8_8_;
  *(undefined8 *)(lVar4 + 0x60) = local_88;
  *(undefined8 *)(lVar4 + 0x68) = uStack_80;
  *(undefined8 *)(lVar4 + 0x70) = local_78;
  *(undefined8 *)(lVar4 + 0x78) = uStack_70;
  *(undefined8 *)(lVar4 + 0x80) = local_68;
  *(undefined8 *)(lVar4 + 0x88) = uStack_60;
  *(undefined8 *)(lVar4 + 0x90) = local_58;
  *(undefined8 *)(lVar4 + 0x98) = uStack_50;
  *(undefined8 *)(lVar4 + 0xa0) = local_48;
  LOCK();
  *(int *)pKVar2 = *(int *)pKVar2 + -1;
  UNLOCK();
  if (*(int *)pKVar2 == 0) {
    if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x005e52ab. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(*(long *)this_00 + 0x20))(this_00);
      return;
    }
  }
  else if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



