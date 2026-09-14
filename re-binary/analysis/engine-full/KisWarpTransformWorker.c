/* Class KisWarpTransformWorker - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisWarpTransformWorker @ 0060b5f0 ======

/* KisWarpTransformWorker::KisWarpTransformWorker(KisWarpTransformWorker::WarpType_,
   QVector<QPointF>, QVector<QPointF>, double, KoUpdater*) */

void __thiscall
KisWarpTransformWorker::KisWarpTransformWorker
          (KisWarpTransformWorker *this,WarpType_ param_1,QVector param_2,QVector param_3,
          double param_4,KoUpdater *param_5)

{
  undefined8 *puVar1;
  int iVar2;
  QArrayData *pQVar3;
  int *piVar4;
  code *pcVar5;
  undefined8 uVar6;
  undefined *puVar7;
  int *piVar8;
  undefined *puVar9;
  long lVar10;
  undefined8 *puVar11;
  undefined4 in_register_0000000c;
  long *plVar12;
  undefined4 in_register_00000014;
  long *plVar13;
  undefined8 *puVar14;
  long lVar15;
  
  puVar9 = PTR_shared_null_008377d0;
  plVar13 = (long *)CONCAT44(in_register_00000014,param_2);
  plVar12 = (long *)CONCAT44(in_register_0000000c,param_3);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  puVar7 = PTR_vtable_00837930;
  *(undefined4 *)(this + 0x18) = 0;
  *(KoUpdater **)(this + 0x38) = param_5;
  *(undefined **)(this + 0x20) = puVar9;
  *(undefined **)(this + 0x28) = puVar9;
  *(undefined **)this = puVar7 + 0x10;
  *(undefined8 *)(this + 0x30) = DAT_007227c0;
  piVar8 = (int *)*plVar13;
  if (piVar8 != (int *)puVar9) {
    if (*piVar8 == 0) {
      if (*(char *)((long)piVar8 + 0xb) < '\0') {
        lVar10 = QArrayData::allocate(0x10,8,(ulong)(piVar8[2] & 0x7fffffff),0);
        if (lVar10 == 0) {
                    /* try { // try from 00244a8b to 00244a99 has its CatchHandler @ 00244a62 */
          qBadAlloc();
        }
        *(byte *)(lVar10 + 0xb) = *(byte *)(lVar10 + 0xb) | 0x80;
      }
      else {
        lVar10 = QArrayData::allocate(0x10,8,(long)piVar8[1],0);
        if (lVar10 == 0) {
          qBadAlloc();
                    /* WARNING: Does not return */
          pcVar5 = (code *)invalidInstructionException();
          (*pcVar5)();
        }
      }
      if ((*(uint *)(lVar10 + 8) & 0x7fffffff) == 0) goto LAB_0060b67e;
      lVar15 = *plVar13;
      iVar2 = *(int *)(lVar15 + 4);
      puVar11 = (undefined8 *)(*(long *)(lVar15 + 0x10) + lVar15);
      puVar14 = puVar11 + (long)iVar2 * 2;
      lVar15 = (*(long *)(lVar10 + 0x10) + lVar10) - (long)puVar11;
      for (; puVar11 != puVar14; puVar11 = puVar11 + 2) {
        puVar1 = (undefined8 *)((long)puVar11 + lVar15);
        uVar6 = puVar11[1];
        *puVar1 = *puVar11;
        puVar1[1] = uVar6;
      }
      pQVar3 = *(QArrayData **)(this + 0x20);
      *(int *)(lVar10 + 4) = iVar2;
      *(long *)(this + 0x20) = lVar10;
      iVar2 = *(int *)pQVar3;
    }
    else {
      if (*piVar8 != -1) {
        LOCK();
        *piVar8 = *piVar8 + 1;
        UNLOCK();
      }
      lVar10 = *plVar13;
LAB_0060b67e:
      pQVar3 = *(QArrayData **)(this + 0x20);
      *(long *)(this + 0x20) = lVar10;
      iVar2 = *(int *)pQVar3;
    }
    if (iVar2 == 0) {
LAB_0060b86d:
      QArrayData::deallocate(pQVar3,0x10,8);
      piVar8 = *(int **)(this + 0x28);
    }
    else {
      if (iVar2 != -1) {
        LOCK();
        *(int *)pQVar3 = *(int *)pQVar3 + -1;
        UNLOCK();
        if (*(int *)pQVar3 == 0) goto LAB_0060b86d;
      }
      piVar8 = *(int **)(this + 0x28);
    }
  }
  piVar4 = (int *)*plVar12;
  if (piVar4 == piVar8) goto joined_r0x0060b7c7;
  if (*piVar4 == 0) {
    if (*(char *)((long)piVar4 + 0xb) < '\0') {
      lVar10 = QArrayData::allocate(0x10,8,(ulong)(piVar4[2] & 0x7fffffff),0);
      if (lVar10 == 0) {
        qBadAlloc();
      }
      *(byte *)(lVar10 + 0xb) = *(byte *)(lVar10 + 0xb) | 0x80;
    }
    else {
      lVar10 = QArrayData::allocate(0x10,8,(long)piVar4[1],0);
      if (lVar10 == 0) {
        qBadAlloc();
                    /* WARNING: Does not return */
        pcVar5 = (code *)invalidInstructionException();
        (*pcVar5)();
      }
    }
    if ((*(uint *)(lVar10 + 8) & 0x7fffffff) == 0) goto LAB_0060b6bf;
    lVar15 = *plVar12;
    iVar2 = *(int *)(lVar15 + 4);
    puVar11 = (undefined8 *)(*(long *)(lVar15 + 0x10) + lVar15);
    puVar14 = puVar11 + (long)iVar2 * 2;
    lVar15 = (*(long *)(lVar10 + 0x10) + lVar10) - (long)puVar11;
    for (; puVar11 != puVar14; puVar11 = puVar11 + 2) {
      puVar1 = (undefined8 *)((long)puVar11 + lVar15);
      uVar6 = puVar11[1];
      *puVar1 = *puVar11;
      puVar1[1] = uVar6;
    }
    pQVar3 = *(QArrayData **)(this + 0x28);
    *(int *)(lVar10 + 4) = iVar2;
    *(long *)(this + 0x28) = lVar10;
    iVar2 = *(int *)pQVar3;
  }
  else {
    if (*piVar4 != -1) {
      LOCK();
      *piVar4 = *piVar4 + 1;
      UNLOCK();
    }
    lVar10 = *plVar12;
LAB_0060b6bf:
    pQVar3 = *(QArrayData **)(this + 0x28);
    *(long *)(this + 0x28) = lVar10;
    iVar2 = *(int *)pQVar3;
  }
  if (iVar2 != 0) {
    if (iVar2 == -1) goto joined_r0x0060b7c7;
    LOCK();
    *(int *)pQVar3 = *(int *)pQVar3 + -1;
    UNLOCK();
    if (*(int *)pQVar3 != 0) goto joined_r0x0060b7c7;
  }
  QArrayData::deallocate(pQVar3,0x10,8);
joined_r0x0060b7c7:
  puVar9 = PTR_similitudeTransformMath_00837b18;
  if (((param_1 != 1) && (puVar9 = PTR_rigidTransformMath_008374b8, param_1 != 2)) &&
     (puVar9 = (undefined *)0x0, param_1 == 0)) {
    puVar9 = PTR_affineTransformMath_008371a8;
  }
  *(undefined **)(this + 0x10) = puVar9;
  *(double *)(this + 0x30) = param_4;
  return;
}



