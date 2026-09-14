/* Class KisSpatialContainer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSpatialContainer @ 00202490 ======

void __thiscall
KisSpatialContainer::KisSpatialContainer(KisSpatialContainer *this,KisSpatialContainer *param_1)

{
  (*(code *)PTR_KisSpatialContainer_00838d18)();
  return;
}



// ====== KisSpatialContainer @ 002031a0 ======

void __thiscall KisSpatialContainer::KisSpatialContainer(void)

{
  (*(code *)PTR_KisSpatialContainer_008393a0)();
  return;
}



// ====== KisSpatialContainer @ 00701290 ======

/* KisSpatialContainer::KisSpatialContainer(QRectF, int) */

void __thiscall KisSpatialContainer::KisSpatialContainer(void *this,undefined4 param_2)

{
  int iVar1;
  QArrayData *pQVar2;
  double dVar3;
  undefined *puVar4;
  undefined8 *puVar5;
  double param_11;
  double in_stack_00000010;
  double param_12;
  double in_stack_00000020;
  
  puVar4 = PTR_shared_null_008377d0;
  *(undefined4 *)((long)this + 4) = param_2;
  *(undefined4 *)this = 0;
  *(undefined4 *)((long)this + 8) = 0;
  *(undefined8 *)((long)this + 0x10) = 0;
  puVar5 = (undefined8 *)operator_new(0x50);
  *(undefined4 *)(puVar5 + 4) = 0;
  *puVar5 = puVar4;
  puVar5[1] = puVar4;
  *(undefined *)((long)puVar5 + 0x24) = 1;
  *(undefined4 *)(puVar5 + 9) = 0xffffffff;
  *(undefined (*) [16])(puVar5 + 2) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar5 + 5) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar5 + 7) = (undefined  [16])0x0;
  if (*(int *)puVar4 == 0) {
LAB_00701370:
    QArrayData::deallocate((QArrayData *)puVar4,8,8);
    pQVar2 = (QArrayData *)*puVar5;
    *puVar5 = puVar4;
    iVar1 = *(int *)pQVar2;
  }
  else {
    if (*(int *)puVar4 != -1) {
      LOCK();
      *(int *)puVar4 = *(int *)puVar4 + -1;
      UNLOCK();
      if (*(int *)puVar4 == 0) goto LAB_00701370;
    }
    pQVar2 = (QArrayData *)*puVar5;
    *puVar5 = puVar4;
    iVar1 = *(int *)pQVar2;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_0070132e;
    LOCK();
    *(int *)pQVar2 = *(int *)pQVar2 + -1;
    UNLOCK();
    if (*(int *)pQVar2 != 0) goto LAB_0070132e;
  }
  QArrayData::deallocate(pQVar2,0x18,8);
LAB_0070132e:
  dVar3 = DAT_007227c8;
  *(undefined8 **)((long)this + 0x10) = puVar5;
  iVar1 = *(int *)((long)this + 8);
  *(int *)(puVar5 + 9) = iVar1;
  *(int *)((long)this + 8) = iVar1 + 1;
  puVar5[2] = dVar3 * param_12 + param_11;
  puVar5[3] = dVar3 * in_stack_00000020 + in_stack_00000010;
  return;
}



// ====== KisSpatialContainer @ 00702110 ======

/* KisSpatialContainer::KisSpatialContainer(KisSpatialContainer const&) */

void __thiscall
KisSpatialContainer::KisSpatialContainer(KisSpatialContainer *this,KisSpatialContainer *param_1)

{
  int iVar1;
  QArrayData *pQVar2;
  undefined8 uVar3;
  SpatialNode *pSVar4;
  undefined *puVar5;
  SpatialNode *pSVar6;
  
  puVar5 = PTR_shared_null_008377d0;
  uVar3 = DAT_00749ab8;
  *(undefined4 *)(this + 8) = 0;
  *(undefined8 *)this = uVar3;
  *(undefined8 *)(this + 0x10) = *(undefined8 *)(param_1 + 0x10);
  pSVar6 = (SpatialNode *)operator_new(0x50);
  *(undefined4 *)(pSVar6 + 0x20) = 0;
  *(undefined **)pSVar6 = puVar5;
  *(undefined **)(pSVar6 + 8) = puVar5;
  pSVar6[0x24] = (SpatialNode)0x1;
  *(undefined4 *)(pSVar6 + 0x48) = 0xffffffff;
  *(undefined (*) [16])(pSVar6 + 0x10) = (undefined  [16])0x0;
  *(undefined (*) [16])(pSVar6 + 0x28) = (undefined  [16])0x0;
  *(undefined (*) [16])(pSVar6 + 0x38) = (undefined  [16])0x0;
  if (*(int *)puVar5 == 0) {
LAB_007021e8:
    QArrayData::deallocate((QArrayData *)puVar5,8,8);
    pQVar2 = *(QArrayData **)pSVar6;
    *(undefined **)pSVar6 = puVar5;
    iVar1 = *(int *)pQVar2;
  }
  else {
    if (*(int *)puVar5 != -1) {
      LOCK();
      *(int *)puVar5 = *(int *)puVar5 + -1;
      UNLOCK();
      if (*(int *)puVar5 == 0) goto LAB_007021e8;
    }
    pQVar2 = *(QArrayData **)pSVar6;
    *(undefined **)pSVar6 = puVar5;
    iVar1 = *(int *)pQVar2;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_007021b4;
    LOCK();
    *(int *)pQVar2 = *(int *)pQVar2 + -1;
    UNLOCK();
    if (*(int *)pQVar2 != 0) goto LAB_007021b4;
  }
  QArrayData::deallocate(pQVar2,0x18,8);
LAB_007021b4:
  uVar3 = *(undefined8 *)param_1;
  *(SpatialNode **)(this + 0x10) = pSVar6;
  pSVar4 = *(SpatialNode **)(param_1 + 0x10);
  *(undefined8 *)this = uVar3;
  *(undefined4 *)(this + 8) = *(undefined4 *)(param_1 + 8);
  deepCopyData(this,pSVar6,pSVar4);
  return;
}



// ====== KisSpatialContainer @ 00706020 ======

/* KisSpatialContainer::KisSpatialContainer(QRectF, QVector<QPointF>&) */

void __thiscall KisSpatialContainer::KisSpatialContainer(void *this,QVector *param_2)

{
  int iVar1;
  QArrayData *pQVar2;
  double dVar3;
  undefined8 uVar4;
  undefined *puVar5;
  undefined8 *puVar6;
  double param_11;
  double in_stack_00000010;
  double param_12;
  double in_stack_00000020;
  
  puVar5 = PTR_shared_null_008377d0;
  uVar4 = DAT_00749ab8;
  *(undefined4 *)((long)this + 8) = 0;
  *(undefined8 *)((long)this + 0x10) = 0;
  *(undefined8 *)this = uVar4;
  puVar6 = (undefined8 *)operator_new(0x50);
  *(undefined4 *)(puVar6 + 4) = 0;
  *puVar6 = puVar5;
  puVar6[1] = puVar5;
  *(undefined *)((long)puVar6 + 0x24) = 1;
  *(undefined4 *)(puVar6 + 9) = 0xffffffff;
  *(undefined (*) [16])(puVar6 + 2) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar6 + 5) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar6 + 7) = (undefined  [16])0x0;
  if (*(int *)puVar5 == 0) {
LAB_00706110:
    QArrayData::deallocate((QArrayData *)puVar5,8,8);
    pQVar2 = (QArrayData *)*puVar6;
    *puVar6 = puVar5;
    iVar1 = *(int *)pQVar2;
  }
  else {
    if (*(int *)puVar5 != -1) {
      LOCK();
      *(int *)puVar5 = *(int *)puVar5 + -1;
      UNLOCK();
      if (*(int *)puVar5 == 0) goto LAB_00706110;
    }
    pQVar2 = (QArrayData *)*puVar6;
    *puVar6 = puVar5;
    iVar1 = *(int *)pQVar2;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_007060c4;
    LOCK();
    *(int *)pQVar2 = *(int *)pQVar2 + -1;
    UNLOCK();
    if (*(int *)pQVar2 != 0) goto LAB_007060c4;
  }
  QArrayData::deallocate(pQVar2,0x18,8);
LAB_007060c4:
  iVar1 = *(int *)((long)this + 8);
  *(undefined8 **)((long)this + 0x10) = puVar6;
  dVar3 = DAT_007227c8;
  *(int *)(puVar6 + 9) = iVar1;
  *(int *)((long)this + 8) = iVar1 + 1;
  puVar6[2] = dVar3 * param_12 + param_11;
  puVar6[3] = dVar3 * in_stack_00000020 + in_stack_00000010;
  initializeWith((KisSpatialContainer *)this,param_2);
  return;
}



