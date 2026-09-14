/* Class KisNUBSpline2D - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNUBSpline2D @ 0060b160 ======

/* KisBSplines::KisNUBSpline2D::KisNUBSpline2D(QVector<double> const&, KisBSplines::BorderCondition,
   QVector<double> const&, KisBSplines::BorderCondition) */

void __thiscall
KisBSplines::KisNUBSpline2D::KisNUBSpline2D
          (KisNUBSpline2D *this,QVector *param_1,BorderCondition param_2,QVector *param_3,
          BorderCondition param_4)

{
  double dVar1;
  double dVar2;
  double dVar3;
  double dVar4;
  BorderCondition *pBVar5;
  long lVar6;
  long lVar7;
  long lVar8;
  void *pvVar9;
  int *piVar10;
  undefined8 uVar11;
  long lVar12;
  double *pdVar13;
  double *pdVar14;
  
  pvVar9 = operator_new(0x30);
  *(void **)this = pvVar9;
  piVar10 = *(int **)param_1;
  if (*piVar10 == 0) {
    if (*(char *)((long)piVar10 + 0xb) < '\0') {
      lVar12 = QArrayData::allocate(8,8,(ulong)(piVar10[2] & 0x7fffffff),0);
      *(long *)(this + 8) = lVar12;
      if (lVar12 == 0) {
        qBadAlloc();
        lVar12 = *(long *)(this + 8);
      }
      *(byte *)(lVar12 + 0xb) = *(byte *)(lVar12 + 0xb) | 0x80;
    }
    else {
      lVar12 = QArrayData::allocate(8,8,(long)piVar10[1],0);
      *(long *)(this + 8) = lVar12;
      if (lVar12 == 0) {
        qBadAlloc();
        lVar12 = *(long *)(this + 8);
      }
    }
    if ((*(uint *)(lVar12 + 8) & 0x7fffffff) != 0) {
      lVar6 = *(long *)param_1;
      memcpy((void *)(lVar12 + *(long *)(lVar12 + 0x10)),(void *)(lVar6 + *(long *)(lVar6 + 0x10)),
             (long)*(int *)(lVar6 + 4) << 3);
      *(undefined4 *)(*(long *)(this + 8) + 4) = *(undefined4 *)(*(long *)param_1 + 4);
    }
  }
  else {
    if (*piVar10 != -1) {
      LOCK();
      *piVar10 = *piVar10 + 1;
      UNLOCK();
      piVar10 = *(int **)param_1;
    }
    *(int **)(this + 8) = piVar10;
  }
  piVar10 = *(int **)param_3;
  if (*piVar10 == 0) {
    if (*(char *)((long)piVar10 + 0xb) < '\0') {
      lVar12 = QArrayData::allocate(8,8,(ulong)(piVar10[2] & 0x7fffffff),0);
      *(long *)(this + 0x10) = lVar12;
      if (lVar12 == 0) {
        qBadAlloc();
        lVar12 = *(long *)(this + 0x10);
      }
      *(byte *)(lVar12 + 0xb) = *(byte *)(lVar12 + 0xb) | 0x80;
    }
    else {
      lVar12 = QArrayData::allocate(8,8,(long)piVar10[1],0);
      *(long *)(this + 0x10) = lVar12;
      if (lVar12 == 0) {
        qBadAlloc();
        lVar12 = *(long *)(this + 0x10);
      }
    }
    if ((*(uint *)(lVar12 + 8) & 0x7fffffff) != 0) {
      lVar6 = *(long *)param_3;
      memcpy((void *)(lVar12 + *(long *)(lVar12 + 0x10)),(void *)(lVar6 + *(long *)(lVar6 + 0x10)),
             (long)*(int *)(lVar6 + 4) << 3);
      *(undefined4 *)(*(long *)(this + 0x10) + 4) = *(undefined4 *)(*(long *)param_3 + 4);
    }
  }
  else {
    if (*piVar10 != -1) {
      LOCK();
      *piVar10 = *piVar10 + 1;
      UNLOCK();
      piVar10 = *(int **)param_3;
    }
    *(int **)(this + 0x10) = piVar10;
  }
  lVar12 = *(long *)(this + 8);
                    /* try { // try from 0060b1c9 to 0060b1e4 has its CatchHandler @ 0060b3b0 */
  uVar11 = FUN_0071a0d0(lVar12 + *(long *)(lVar12 + 0x10),*(undefined4 *)(lVar12 + 4));
  lVar12 = *(long *)(this + 0x10);
  *(undefined8 *)(*(long *)this + 0x10) = uVar11;
  uVar11 = FUN_0071a0d0(lVar12 + *(long *)(lVar12 + 0x10),*(undefined4 *)(lVar12 + 4));
  pBVar5 = *(BorderCondition **)this;
  lVar12 = *(long *)param_1;
  *(undefined8 *)(pBVar5 + 6) = uVar11;
  lVar6 = *(long *)param_3;
  lVar7 = *(long *)(lVar12 + 0x10);
  *pBVar5 = param_2;
  lVar8 = *(long *)(lVar6 + 0x10);
  pBVar5[1] = param_4;
  pdVar14 = (double *)(lVar7 + lVar12);
  pdVar13 = (double *)(lVar8 + lVar6);
  dVar1 = *pdVar14;
  dVar2 = *pdVar13;
  dVar3 = pdVar14[(long)*(int *)(lVar12 + 4) + -1];
  dVar4 = pdVar13[(long)*(int *)(lVar6 + 4) + -1];
  pBVar5[2] = 0;
  pBVar5[3] = 0;
  pBVar5[8] = (BorderCondition)(float)dVar1;
  pBVar5[9] = (BorderCondition)(float)dVar3;
  pBVar5[10] = (BorderCondition)(float)dVar2;
  pBVar5[0xb] = (BorderCondition)(float)dVar4;
  return;
}



