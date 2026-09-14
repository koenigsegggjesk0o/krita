/* Class KisCageTransformWorker - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCageTransformWorker @ 00615500 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisCageTransformWorker::KisCageTransformWorker(QRect const&, QVector<QPointF> const&, KoUpdater*,
   int) */

void __thiscall
KisCageTransformWorker::KisCageTransformWorker
          (KisCageTransformWorker *this,QRect *param_1,QVector *param_2,KoUpdater *param_3,
          int param_4)

{
  undefined8 *puVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined *puVar5;
  undefined *puVar6;
  undefined8 *puVar7;
  int *piVar8;
  long lVar9;
  undefined8 *puVar10;
  undefined8 *puVar11;
  long lVar12;
  
  puVar5 = PTR_shared_null_008377d0;
  puVar7 = (undefined8 *)operator_new(0x88);
  uVar3 = DAT_00721778;
  *puVar7 = _DAT_00721770;
  puVar7[1] = uVar3;
  QImage::QImage((QImage *)(puVar7 + 2));
  piVar8 = *(int **)param_2;
  *(undefined (*) [16])(puVar7 + 6) = (undefined  [16])0x0;
  if (*piVar8 == 0) {
    if (*(char *)((long)piVar8 + 0xb) < '\0') {
      lVar9 = QArrayData::allocate(0x10,8,(ulong)(piVar8[2] & 0x7fffffff),0);
      puVar7[8] = lVar9;
      if (lVar9 == 0) {
        qBadAlloc();
        lVar9 = puVar7[8];
      }
      *(byte *)(lVar9 + 0xb) = *(byte *)(lVar9 + 0xb) | 0x80;
    }
    else {
      lVar9 = QArrayData::allocate(0x10,8,(long)piVar8[1],0);
      puVar7[8] = lVar9;
      if (lVar9 == 0) {
        qBadAlloc();
        lVar9 = puVar7[8];
      }
    }
    if ((*(uint *)(lVar9 + 8) & 0x7fffffff) != 0) {
      lVar12 = *(long *)param_2;
      iVar2 = *(int *)(lVar12 + 4);
      puVar10 = (undefined8 *)(*(long *)(lVar12 + 0x10) + lVar12);
      puVar11 = puVar10 + (long)iVar2 * 2;
      lVar12 = (*(long *)(lVar9 + 0x10) + lVar9) - (long)puVar10;
      for (; puVar10 != puVar11; puVar10 = puVar10 + 2) {
        puVar1 = (undefined8 *)((long)puVar10 + lVar12);
        uVar3 = puVar10[1];
        *puVar1 = *puVar10;
        puVar1[1] = uVar3;
      }
      *(int *)(lVar9 + 4) = iVar2;
    }
  }
  else {
    if (*piVar8 != -1) {
      LOCK();
      *piVar8 = *piVar8 + 1;
      UNLOCK();
      piVar8 = *(int **)param_2;
    }
    puVar7[8] = piVar8;
  }
  puVar6 = PTR_shared_null_008377d0;
  puVar7[10] = param_3;
  *(int *)(puVar7 + 0xb) = param_4;
  puVar7[9] = puVar6;
  puVar7[0xe] = puVar6;
  puVar7[0xc] = puVar5;
  puVar7[0xd] = puVar5;
                    /* try { // try from 0061559e to 006155a2 has its CatchHandler @ 006156a3 */
  KisGreenCoordinatesMath::KisGreenCoordinatesMath((KisGreenCoordinatesMath *)(puVar7 + 0xf));
  uVar3 = *(undefined8 *)param_1;
  uVar4 = *(undefined8 *)(param_1 + 8);
  *(undefined8 **)this = puVar7;
  puVar7[0x10] = 0xffffffffffffffff;
  *puVar7 = uVar3;
  puVar7[1] = uVar4;
  return;
}



// ====== KisCageTransformWorker @ 006156b0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisCageTransformWorker::KisCageTransformWorker(QImage const&, QPointF const&, QVector<QPointF>
   const&, KoUpdater*, int) */

void __thiscall
KisCageTransformWorker::KisCageTransformWorker
          (KisCageTransformWorker *this,QImage *param_1,QPointF *param_2,QVector *param_3,
          KoUpdater *param_4,int param_5)

{
  undefined8 *puVar1;
  int iVar2;
  long lVar3;
  undefined8 uVar4;
  undefined *puVar5;
  undefined *puVar6;
  undefined8 *puVar7;
  int *piVar8;
  long lVar9;
  undefined8 *puVar10;
  undefined8 *puVar11;
  long lVar12;
  long in_FS_OFFSET;
  undefined auVar13 [16];
  
  puVar5 = PTR_shared_null_008377d0;
  lVar3 = *(long *)(in_FS_OFFSET + 0x28);
  puVar7 = (undefined8 *)operator_new(0x88);
  uVar4 = DAT_00721778;
  *puVar7 = _DAT_00721770;
  puVar7[1] = uVar4;
  QImage::QImage((QImage *)(puVar7 + 2));
  piVar8 = *(int **)param_3;
  *(undefined (*) [16])(puVar7 + 6) = (undefined  [16])0x0;
  if (*piVar8 == 0) {
    if (*(char *)((long)piVar8 + 0xb) < '\0') {
      lVar9 = QArrayData::allocate(0x10,8,(ulong)(piVar8[2] & 0x7fffffff),0);
      puVar7[8] = lVar9;
      if (lVar9 == 0) {
        qBadAlloc();
        lVar9 = puVar7[8];
      }
      *(byte *)(lVar9 + 0xb) = *(byte *)(lVar9 + 0xb) | 0x80;
    }
    else {
      lVar9 = QArrayData::allocate(0x10,8,(long)piVar8[1],0);
      puVar7[8] = lVar9;
      if (lVar9 == 0) {
                    /* try { // try from 00244fb6 to 00244fc8 has its CatchHandler @ 00244fd2 */
        qBadAlloc();
        lVar9 = puVar7[8];
      }
    }
    if ((*(uint *)(lVar9 + 8) & 0x7fffffff) != 0) {
      lVar12 = *(long *)param_3;
      iVar2 = *(int *)(lVar12 + 4);
      puVar10 = (undefined8 *)(*(long *)(lVar12 + 0x10) + lVar12);
      puVar11 = puVar10 + (long)iVar2 * 2;
      lVar12 = (*(long *)(lVar9 + 0x10) + lVar9) - (long)puVar10;
      for (; puVar10 != puVar11; puVar10 = puVar10 + 2) {
        puVar1 = (undefined8 *)((long)puVar10 + lVar12);
        uVar4 = puVar10[1];
        *puVar1 = *puVar10;
        puVar1[1] = uVar4;
      }
      *(int *)(lVar9 + 4) = iVar2;
    }
  }
  else {
    if (*piVar8 != -1) {
      LOCK();
      *piVar8 = *piVar8 + 1;
      UNLOCK();
      piVar8 = *(int **)param_3;
    }
    puVar7[8] = piVar8;
  }
  puVar6 = PTR_shared_null_008377d0;
  puVar7[10] = param_4;
  *(int *)(puVar7 + 0xb) = param_5;
  puVar7[9] = puVar6;
  puVar7[0xe] = puVar6;
  puVar7[0xc] = puVar5;
  puVar7[0xd] = puVar5;
                    /* try { // try from 00615763 to 00615767 has its CatchHandler @ 006158e4 */
  KisGreenCoordinatesMath::KisGreenCoordinatesMath((KisGreenCoordinatesMath *)(puVar7 + 0xf));
  *(undefined8 **)this = puVar7;
  puVar7[0x10] = 0xffffffffffffffff;
                    /* try { // try from 0061577f to 0061579d has its CatchHandler @ 006158d8 */
  QImage::operator=((QImage *)(puVar7 + 2),param_1);
  lVar9 = *(long *)this;
  uVar4 = *(undefined8 *)(param_2 + 8);
  *(undefined8 *)(lVar9 + 0x30) = *(undefined8 *)param_2;
  *(undefined8 *)(lVar9 + 0x38) = uVar4;
  QImage::size();
  auVar13 = QRectF::toAlignedRect();
  **(undefined (**) [16])this = auVar13;
  if (lVar3 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



