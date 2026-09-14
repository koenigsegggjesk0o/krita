/* Painting functions extracted from kritacolorsmudgepaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintAt @ 0012be04 ======

/* KisColorSmudgeOp::paintAt(KisPaintInformation const&) [clone .cold] */

void KisColorSmudgeOp::paintAt
               (KisPaintInformation *param_1,undefined param_2,undefined param_3,undefined param_4,
               undefined param_5,undefined param_6,undefined param_7,undefined param_8,
               undefined param_9,QList<KisHSVOption*> *param_10,undefined param_11,long param_12)

{
  ExternalRefCountData *unaff_R15;
  long in_FS_OFFSET;
  
  QList<KisHSVOption*>::~QList(param_10);
  QMap<QString,QVariant>::~QMap((QMap<QString,QVariant> *)&param_11);
  if (unaff_R15 != (ExternalRefCountData *)0x0) {
    QSharedPointer<KoAbstractGradient>::deref(unaff_R15);
  }
  if (param_12 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintDab @ 00131008 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisColorSmudgeStrategyLightness::paintDab(QRect const&, QRect const&, KoColor const&, double,
   double, double, double, double, double) [clone .cold] */

undefined8
KisColorSmudgeStrategyLightness::paintDab
          (QRect *param_1,QRect *param_2,KoColor *param_3,double param_4,double param_5,
          double param_6,double param_7,double param_8,double param_9)

{
  int *piVar1;
  long *plVar2;
  byte *pbVar3;
  undefined8 *puVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined uVar7;
  uint uVar8;
  int iVar9;
  long *plVar10;
  uchar *puVar11;
  code *pcVar12;
  long lVar13;
  undefined8 *puVar14;
  long lVar15;
  void *pvVar16;
  KoColorSpace *pKVar17;
  KisFixedPaintDevice *pKVar18;
  int *piVar19;
  QRect *pQVar20;
  undefined8 uVar21;
  QRect *pQVar22;
  undefined8 *puVar23;
  undefined8 *puVar24;
  QFlags QVar25;
  undefined8 *unaff_RBX;
  long lVar26;
  long unaff_RBP;
  uint uVar27;
  uint *puVar28;
  QArrayData *pQVar29;
  long unaff_R12;
  undefined8 unaff_R14;
  long unaff_R15;
  long in_FS_OFFSET;
  double dVar30;
  undefined auVar31 [16];
  
  *(long *)(unaff_RBP + -0xd0) = unaff_RBP + -0x80;
                    /* try { // try from 00131013 to 0013102c has its CatchHandler @ 00131036 */
  qBadAlloc();
  *(byte *)(unaff_R12 + 0xb) = *(byte *)(unaff_R12 + 0xb) | 0x80;
  if ((*(uint *)(unaff_R12 + 8) & 0x7fffffff) != 0) {
    lVar15 = *(long *)(unaff_R12 + 0x10);
    uVar21 = *(undefined8 *)(unaff_RBP + -0x90);
    lVar26 = **(long **)(unaff_RBP + -0xc0);
    iVar9 = *(int *)(lVar26 + 4);
    puVar24 = (undefined8 *)(*(long *)(lVar26 + 0x10) + lVar26);
    puVar14 = puVar24;
    if (puVar24 != puVar24 + (long)iVar9 * 2) {
      do {
        puVar4 = (undefined8 *)((long)puVar14 + ((lVar15 + unaff_R12) - (long)puVar24));
        puVar23 = puVar14 + 2;
        uVar5 = puVar14[1];
        *puVar4 = *puVar14;
        puVar4[1] = uVar5;
        puVar14 = puVar23;
      } while (puVar24 + (long)iVar9 * 2 != puVar23);
      *(undefined8 *)(unaff_RBP + -0x90) = uVar21;
    }
    *(int *)(unaff_R12 + 4) = iVar9;
  }
  pQVar29 = *(QArrayData **)(unaff_RBP + -0x80);
  *(long *)(unaff_RBP + -0x80) = unaff_R12;
  if (*(int *)pQVar29 == 0) {
LAB_001a90a8:
    QArrayData::deallocate(pQVar29,0x10,8);
  }
  else if (*(int *)pQVar29 != -1) {
    LOCK();
    *(int *)pQVar29 = *(int *)pQVar29 + -1;
    UNLOCK();
    if (*(int *)pQVar29 == 0) goto LAB_001a90a8;
  }
  puVar28 = *(uint **)(unaff_RBP + -0x80);
  *(long *)(unaff_RBP + -0xd0) = unaff_RBP + -0x80;
  uVar27 = puVar28[1];
  uVar8 = puVar28[2];
  if (*puVar28 < 2) {
    if ((uVar8 & 0x7fffffff) < uVar27 + 1) {
      uVar21 = (*(undefined8 **)(unaff_RBP + -0x90))[1];
      *(undefined8 *)(unaff_RBP + -0xa0) = **(undefined8 **)(unaff_RBP + -0x90);
      *(undefined8 *)(unaff_RBP + -0x98) = uVar21;
      goto LAB_001a8df2;
    }
    uVar27 = puVar28[1];
    uVar21 = **(undefined8 **)(unaff_RBP + -0x90);
    uVar5 = (*(undefined8 **)(unaff_RBP + -0x90))[1];
    puVar14 = (undefined8 *)((long)puVar28 + *(long *)(puVar28 + 4) + (long)(int)uVar27 * 0x10);
    *(undefined8 *)(unaff_RBP + -0xa0) = uVar21;
    *(undefined8 *)(unaff_RBP + -0x98) = uVar5;
    *puVar14 = uVar21;
    puVar14[1] = uVar5;
  }
  else {
    uVar21 = (*(undefined8 **)(unaff_RBP + -0x90))[1];
    *(undefined8 *)(unaff_RBP + -0xa0) = **(undefined8 **)(unaff_RBP + -0x90);
    *(undefined8 *)(unaff_RBP + -0x98) = uVar21;
    if ((uVar8 & 0x7fffffff) < uVar27 + 1) {
LAB_001a8df2:
      QVar25 = 8;
      uVar27 = puVar28[1] + 1;
    }
    else {
      QVar25 = 0;
      uVar27 = puVar28[2] & 0x7fffffff;
    }
    QVector<QRect>::realloc(*(QVector<QRect> **)(unaff_RBP + -0xd0),uVar27,QVar25);
    puVar28 = *(uint **)(unaff_RBP + -0x80);
    uVar21 = *(undefined8 *)(unaff_RBP + -0x98);
    uVar27 = puVar28[1];
    puVar14 = (undefined8 *)((long)puVar28 + *(long *)(puVar28 + 4) + (long)(int)uVar27 * 0x10);
    *puVar14 = *(undefined8 *)(unaff_RBP + -0xa0);
    puVar14[1] = uVar21;
  }
  uVar21 = *(undefined8 *)(unaff_RBP + -0xd0);
  puVar28[1] = uVar27 + 1;
  (**(code **)(**(long **)(unaff_R15 + 0xa8) + 0x10))(*(long **)(unaff_R15 + 0xa8),uVar21);
  lVar15 = *(long *)(unaff_R15 + 0x78);
  *(long *)(unaff_RBP + -0x70) = lVar15;
  if (lVar15 != 0) {
    LOCK();
    *(int *)(lVar15 + 8) = *(int *)(lVar15 + 8) + 1;
    UNLOCK();
  }
  uVar21 = *(undefined8 *)(unaff_R15 + 0xb0);
  piVar19 = *(int **)(unaff_R15 + 0xb0);
  *(undefined8 *)(unaff_RBP + -0x60) = *(undefined8 *)(unaff_R15 + 0xa8);
  *(undefined8 *)(unaff_RBP + -0x58) = uVar21;
  if (piVar19 != (int *)0x0) {
    LOCK();
    *piVar19 = *piVar19 + 1;
    UNLOCK();
    LOCK();
    piVar19 = (int *)(*(long *)(unaff_RBP + -0x58) + 4);
    *piVar19 = *piVar19 + 1;
    UNLOCK();
  }
  lVar15 = QArrayData::allocate(8,8,1,0);
  *(long *)(unaff_RBP + -0x78) = lVar15;
  if (lVar15 == 0) {
                    /* try { // try from 001310ee to 001310f2 has its CatchHandler @ 001310fc */
    qBadAlloc();
    lVar15 = *(long *)(unaff_RBP + -0x78);
  }
  lVar26 = *(long *)(lVar15 + 0x10);
  *(long *)(unaff_RBP + -0xa0) = unaff_RBP + -0x70;
  uVar21 = *(undefined8 *)(unaff_RBP + -0xe8);
  *(undefined8 *)(lVar15 + lVar26) = unaff_R14;
  uVar5 = *(undefined8 *)(unaff_RBP + -0xd8);
  uVar6 = *(undefined8 *)(unaff_RBP + -0xe0);
  *(undefined4 *)(*(long *)(unaff_RBP + -0x78) + 4) = 1;
  *(long *)(unaff_RBP + -0xb8) = unaff_RBP + -0x60;
  KisColorSmudgeStrategyBase::blendBrush
            (*(undefined8 *)(unaff_RBP + -0x88),*(undefined8 *)(unaff_RBP + -0xb0),uVar6,uVar5,
             uVar21);
  pQVar29 = *(QArrayData **)(unaff_RBP + -0x78);
  if (*(int *)pQVar29 == 0) {
LAB_001a8d90:
    QArrayData::deallocate(pQVar29,8,8);
  }
  else if (*(int *)pQVar29 != -1) {
    LOCK();
    *(int *)pQVar29 = *(int *)pQVar29 + -1;
    iVar9 = *(int *)pQVar29;
    UNLOCK();
    pQVar29 = *(QArrayData **)(unaff_RBP + -0x78);
    if (iVar9 == 0) goto LAB_001a8d90;
  }
  piVar19 = *(int **)(unaff_RBP + -0x58);
  if (piVar19 != (int *)0x0) {
    LOCK();
    piVar1 = piVar19 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piVar19 + 2))(piVar19);
    }
    LOCK();
    *piVar19 = *piVar19 + -1;
    UNLOCK();
    if (*piVar19 == 0) {
      operator_delete(piVar19,0x10);
    }
  }
  plVar10 = *(long **)(unaff_RBP + -0x70);
  if (plVar10 != (long *)0x0) {
    LOCK();
    plVar2 = plVar10 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*plVar10 + 8))();
    }
  }
  if (*(int *)(unaff_R15 + 0xf8) != 1) {
    dVar30 = *(double *)(unaff_RBP + -0xb0) - _DAT_001ca370;
    *(double *)(unaff_RBP + -0x88) =
         ((DAT_001c8848 - dVar30) * *(double *)(unaff_RBP + -0xf0) + dVar30) *
         *(double *)(unaff_RBP + -0x88);
  }
  KisPainter::setOpacityF(*(double *)(unaff_RBP + -0x88));
  auVar31 = KisFixedPaintDevice::bounds();
  lVar15 = *(long *)(unaff_R15 + 0x80);
  *(undefined (*) [16])(unaff_RBP + -0x60) = auVar31;
  *(long *)(unaff_RBP + -0x70) = lVar15;
  if (lVar15 != 0) {
    LOCK();
    *(int *)(lVar15 + 8) = *(int *)(lVar15 + 8) + 1;
    UNLOCK();
  }
  *(undefined8 *)(unaff_RBP + -0x78) = *unaff_RBX;
  KisPainter::bltFixed
            (unaff_R15 + 200,unaff_RBP + -0x78,*(undefined8 *)(unaff_RBP + -0xa0),
             *(undefined8 *)(unaff_RBP + -0xb8));
  plVar10 = *(long **)(unaff_RBP + -0x70);
  if (plVar10 != (long *)0x0) {
    LOCK();
    plVar2 = plVar10 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*plVar10 + 8))();
    }
  }
  lVar15 = *(long *)(unaff_R15 + 0x80);
  uVar7 = *(undefined *)(unaff_R15 + 0xd8);
  *(long *)(unaff_RBP + -0x60) = lVar15;
  if (lVar15 != 0) {
    LOCK();
    *(int *)(lVar15 + 8) = *(int *)(lVar15 + 8) + 1;
    UNLOCK();
  }
  KisPainter::renderMirrorMaskSafe
            (unaff_R15 + 200,*unaff_RBX,unaff_RBX[1],*(undefined8 *)(unaff_RBP + -0xb8),uVar7);
  plVar10 = *(long **)(unaff_RBP + -0x60);
  if (plVar10 != (long *)0x0) {
    LOCK();
    plVar2 = plVar10 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*plVar10 + 8))();
    }
  }
  pvVar16 = operator_new(0x38);
  *(void **)(unaff_RBP + -0x88) = pvVar16;
  uVar21 = *(undefined8 *)(unaff_R15 + 0x10);
  piVar19 = *(int **)(unaff_R15 + 0x10);
  *(undefined8 *)(unaff_RBP + -0x60) = *(undefined8 *)(unaff_R15 + 8);
  *(undefined8 *)(unaff_RBP + -0x58) = uVar21;
  if (piVar19 != (int *)0x0) {
    LOCK();
    *piVar19 = *piVar19 + 1;
    UNLOCK();
    LOCK();
    piVar19 = (int *)(*(long *)(unaff_RBP + -0x58) + 4);
    *piVar19 = *piVar19 + 1;
    UNLOCK();
  }
  pKVar17 = (KoColorSpace *)KisPaintDevice::colorSpace();
  pKVar18 = *(KisFixedPaintDevice **)(unaff_RBP + -0x88);
  KisFixedPaintDevice::KisFixedPaintDevice
            (pKVar18,pKVar17,(QSharedPointer)*(undefined8 *)(unaff_RBP + -0xb8));
  *(KisFixedPaintDevice **)(unaff_RBP + -0xd8) = pKVar18 + 8;
  LOCK();
  *(int *)(pKVar18 + 8) = *(int *)(pKVar18 + 8) + 1;
  UNLOCK();
  piVar19 = *(int **)(unaff_RBP + -0x58);
  if (piVar19 != (int *)0x0) {
    LOCK();
    piVar1 = piVar19 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piVar19 + 2))(piVar19);
    }
    LOCK();
    *piVar19 = *piVar19 + -1;
    UNLOCK();
    if (*piVar19 == 0) {
      operator_delete(piVar19,0x10);
    }
  }
  pKVar18 = (KisFixedPaintDevice *)operator_new(0x38);
  uVar21 = *(undefined8 *)(unaff_R15 + 0x10);
  piVar19 = *(int **)(unaff_R15 + 0x10);
  *(undefined8 *)(unaff_RBP + -0x60) = *(undefined8 *)(unaff_R15 + 8);
  *(undefined8 *)(unaff_RBP + -0x58) = uVar21;
  if (piVar19 != (int *)0x0) {
    LOCK();
    *piVar19 = *piVar19 + 1;
    UNLOCK();
    LOCK();
    piVar19 = (int *)(*(long *)(unaff_RBP + -0x58) + 4);
    *piVar19 = *piVar19 + 1;
    UNLOCK();
  }
  pKVar17 = (KoColorSpace *)KisPaintDevice::colorSpace();
  KisFixedPaintDevice::KisFixedPaintDevice
            (pKVar18,pKVar17,(QSharedPointer)*(undefined8 *)(unaff_RBP + -0xb8));
  *(KisFixedPaintDevice **)(unaff_RBP + -0xe0) = pKVar18 + 8;
  LOCK();
  *(int *)(pKVar18 + 8) = *(int *)(pKVar18 + 8) + 1;
  UNLOCK();
  piVar19 = *(int **)(unaff_RBP + -0x58);
  if (piVar19 != (int *)0x0) {
    LOCK();
    piVar1 = piVar19 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piVar19 + 2))(piVar19);
    }
    LOCK();
    *piVar19 = *piVar19 + -1;
    UNLOCK();
    if (*piVar19 == 0) {
      operator_delete(piVar19,0x10);
    }
  }
  puVar14 = *(undefined8 **)(unaff_RBP + -0xc0);
  piVar19 = (int *)*puVar14;
  if (*piVar19 == 0) {
    if (*(char *)((long)piVar19 + 0xb) < '\0') {
      lVar15 = QArrayData::allocate(0x10,8,(ulong)(piVar19[2] & 0x7fffffff),0);
      *(long *)(unaff_RBP + -200) = lVar15;
      *(long *)(unaff_RBP + -0x60) = lVar15;
      if (lVar15 == 0) {
                    /* try { // try from 001310d1 to 001310df has its CatchHandler @ 001310e5 */
        qBadAlloc();
      }
      pbVar3 = (byte *)(*(long *)(unaff_RBP + -200) + 0xb);
      *pbVar3 = *pbVar3 | 0x80;
    }
    else {
      lVar15 = QArrayData::allocate(0x10,8,(long)piVar19[1],0);
      *(long *)(unaff_RBP + -200) = lVar15;
      *(long *)(unaff_RBP + -0x60) = lVar15;
      if (lVar15 == 0) {
        qBadAlloc();
      }
    }
    lVar15 = *(long *)(unaff_RBP + -200);
    if ((*(uint *)(lVar15 + 8) & 0x7fffffff) == 0) {
      lVar26 = *(long *)(lVar15 + 0x10) + lVar15;
      lVar15 = (long)*(int *)(lVar15 + 4) << 4;
    }
    else {
      uVar21 = *(undefined8 *)(unaff_RBP + -0x88);
      lVar13 = **(long **)(unaff_RBP + -0xc0);
      lVar26 = *(long *)(lVar15 + 0x10) + *(long *)(unaff_RBP + -200);
      puVar24 = (undefined8 *)(*(long *)(lVar13 + 0x10) + lVar13);
      iVar9 = *(int *)(lVar13 + 4);
      lVar15 = (long)iVar9 * 0x10;
      puVar14 = puVar24;
      if (puVar24 != puVar24 + (long)iVar9 * 2) {
        do {
          puVar4 = (undefined8 *)((long)puVar14 + (lVar26 - (long)puVar24));
          puVar23 = puVar14 + 2;
          uVar5 = puVar14[1];
          *puVar4 = *puVar14;
          puVar4[1] = uVar5;
          puVar14 = puVar23;
        } while (puVar24 + (long)iVar9 * 2 != puVar23);
        *(undefined8 *)(unaff_RBP + -0x88) = uVar21;
      }
      *(int *)(*(long *)(unaff_RBP + -200) + 4) = iVar9;
    }
  }
  else {
    if (*piVar19 != -1) {
      LOCK();
      *piVar19 = *piVar19 + 1;
      UNLOCK();
      piVar19 = (int *)*puVar14;
    }
    iVar9 = piVar19[1];
    lVar26 = *(long *)(piVar19 + 4);
    *(int **)(unaff_RBP + -200) = piVar19;
    *(int **)(unaff_RBP + -0x60) = piVar19;
    lVar15 = (long)iVar9 << 4;
    lVar26 = lVar26 + (long)piVar19;
  }
  lVar15 = lVar26 + lVar15;
  *(long *)(unaff_RBP + -0x58) = lVar26;
  *(long *)(unaff_RBP + -0xb0) = lVar15;
  *(long *)(unaff_RBP + -0x50) = lVar15;
  *(undefined4 *)(unaff_RBP + -0x48) = 1;
  if (lVar26 != lVar15) {
    do {
      pQVar22 = *(QRect **)(unaff_RBP + -0x88);
      KisFixedPaintDevice::setRect(pQVar22);
      KisFixedPaintDevice::lazyGrowBufferWithoutInitialization();
      KisFixedPaintDevice::setRect((QRect *)pKVar18);
      KisFixedPaintDevice::lazyGrowBufferWithoutInitialization();
      puVar11 = *(uchar **)(unaff_R15 + 0x90);
      pQVar20 = (QRect *)KisFixedPaintDevice::data();
      KisPaintDevice::readBytes(puVar11,pQVar20);
      puVar11 = *(uchar **)(unaff_R15 + 0x88);
      pQVar20 = (QRect *)KisFixedPaintDevice::data();
      KisPaintDevice::readBytes(puVar11,pQVar20);
      *(QRect **)(unaff_RBP + -0x88) = pQVar22;
      plVar10 = *(long **)(pQVar22 + 0x18);
      pcVar12 = *(code **)(*plVar10 + 0x1a8);
      uVar21 = KisFixedPaintDevice::data();
      *(undefined8 *)(unaff_RBP + -0x90) = uVar21;
      uVar21 = KisFixedPaintDevice::data();
      (*pcVar12)(DAT_001c8848,plVar10,uVar21,*(undefined8 *)(unaff_RBP + -0x90),
                 *(undefined4 *)(unaff_RBP + -0xa4));
      puVar11 = *(uchar **)(unaff_R15 + 0x98);
      auVar31 = KisFixedPaintDevice::bounds();
      *(undefined (*) [16])(unaff_RBP + -0x70) = auVar31;
      pQVar22 = (QRect *)KisFixedPaintDevice::data();
      KisPaintDevice::writeBytes(puVar11,pQVar22);
      lVar26 = lVar26 + 0x10;
      *(long *)(unaff_RBP + -0x58) = lVar26;
    } while (*(long *)(unaff_RBP + -0xb0) != lVar26);
  }
  if (**(int **)(unaff_RBP + -200) == 0) {
LAB_001a8d70:
    QArrayData::deallocate(*(QArrayData **)(unaff_RBP + -0x60),0x10,8);
  }
  else if (**(int **)(unaff_RBP + -200) != -1) {
    piVar19 = *(int **)(unaff_RBP + -200);
    LOCK();
    *piVar19 = *piVar19 + -1;
    UNLOCK();
    if (*piVar19 == 0) goto LAB_001a8d70;
  }
  KisOverlayPaintDeviceWrapper::writeRects
            (*(QVector **)(unaff_R15 + 0xa0),(int)*(undefined8 *)(unaff_RBP + -0xc0));
  piVar19 = *(int **)(unaff_RBP + -0xe0);
  LOCK();
  *piVar19 = *piVar19 + -1;
  UNLOCK();
  if (*piVar19 == 0) {
    (**(code **)(*(long *)pKVar18 + 8))(pKVar18);
  }
  piVar19 = *(int **)(unaff_RBP + -0xd8);
  LOCK();
  *piVar19 = *piVar19 + -1;
  UNLOCK();
  if (*piVar19 == 0) {
    (**(code **)(**(long **)(unaff_RBP + -0x88) + 8))();
  }
  pQVar29 = *(QArrayData **)(unaff_RBP + -0x80);
  if (*(int *)pQVar29 != 0) {
    if (*(int *)pQVar29 == -1) goto LAB_001a8cc8;
    LOCK();
    *(int *)pQVar29 = *(int *)pQVar29 + -1;
    iVar9 = *(int *)pQVar29;
    UNLOCK();
    pQVar29 = *(QArrayData **)(unaff_RBP + -0x80);
    if (iVar9 != 0) goto LAB_001a8cc8;
  }
  QArrayData::deallocate(pQVar29,0x10,8);
LAB_001a8cc8:
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
    return *(undefined8 *)(unaff_RBP + -0xc0);
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintDab @ 001312dc ======

/* KisColorSmudgeStrategyWithOverlay::paintDab(QRect const&, QRect const&, KoColor const&, double,
   double, double, double, double, double) [clone .cold] */

void KisColorSmudgeStrategyWithOverlay::paintDab
               (QRect *param_1,QRect *param_2,KoColor *param_3,double param_4,double param_5,
               double param_6,double param_7,double param_8,double param_9)

{
  int *piVar1;
  long *plVar2;
  undefined8 *puVar3;
  int iVar4;
  undefined8 uVar5;
  int *piVar6;
  long *plVar7;
  undefined8 *puVar8;
  uint uVar9;
  QFlags QVar10;
  long unaff_RBX;
  long unaff_RBP;
  uint uVar11;
  undefined8 *puVar12;
  uint *puVar13;
  QArrayData *pQVar14;
  long lVar15;
  undefined8 unaff_R12;
  long unaff_R13;
  long *unaff_R14;
  undefined8 *unaff_R15;
  long in_FS_OFFSET;
  
  *(long *)(unaff_RBP + -0x88) = unaff_RBP + -0x70;
                    /* try { // try from 001312e7 to 00131300 has its CatchHandler @ 00131355 */
  qBadAlloc();
  *(byte *)(unaff_R13 + 0xb) = *(byte *)(unaff_R13 + 0xb) | 0x80;
  if ((*(uint *)(unaff_R13 + 8) & 0x7fffffff) != 0) {
    lVar15 = *unaff_R14;
    iVar4 = *(int *)(lVar15 + 4);
    puVar8 = (undefined8 *)(*(long *)(lVar15 + 0x10) + lVar15);
    puVar12 = puVar8 + (long)iVar4 * 2;
    lVar15 = (*(long *)(unaff_R13 + 0x10) + unaff_R13) - (long)puVar8;
    for (; puVar8 != puVar12; puVar8 = puVar8 + 2) {
      puVar3 = (undefined8 *)((long)puVar8 + lVar15);
      uVar5 = puVar8[1];
      *puVar3 = *puVar8;
      puVar3[1] = uVar5;
    }
    *(int *)(unaff_R13 + 4) = iVar4;
  }
  pQVar14 = *(QArrayData **)(unaff_RBP + -0x70);
  *(long *)(unaff_RBP + -0x70) = unaff_R13;
  if (*(int *)pQVar14 == 0) {
LAB_001ae008:
    QArrayData::deallocate(pQVar14,0x10,8);
  }
  else if (*(int *)pQVar14 != -1) {
    LOCK();
    *(int *)pQVar14 = *(int *)pQVar14 + -1;
    UNLOCK();
    if (*(int *)pQVar14 == 0) goto LAB_001ae008;
  }
  puVar13 = *(uint **)(unaff_RBP + -0x70);
  *(uint *)(unaff_RBP + -0x80) = puVar13[1];
  *(long *)(unaff_RBP + -0x88) = unaff_RBP + -0x70;
  uVar11 = puVar13[2];
  uVar9 = *(int *)(unaff_RBP + -0x80) + 1;
  if (*puVar13 < 2) {
    if ((uVar11 & 0x7fffffff) < uVar9) {
      uVar5 = unaff_R15[1];
      *(undefined8 *)(unaff_RBP + -0x80) = *unaff_R15;
      *(undefined8 *)(unaff_RBP + -0x78) = uVar5;
      goto LAB_001ade61;
    }
    uVar11 = puVar13[1];
    uVar5 = unaff_R15[1];
    puVar8 = (undefined8 *)((long)puVar13 + *(long *)(puVar13 + 4) + (long)(int)uVar11 * 0x10);
    *puVar8 = *unaff_R15;
    puVar8[1] = uVar5;
  }
  else {
    uVar5 = unaff_R15[1];
    *(undefined8 *)(unaff_RBP + -0x80) = *unaff_R15;
    *(undefined8 *)(unaff_RBP + -0x78) = uVar5;
    if ((uVar11 & 0x7fffffff) < uVar9) {
LAB_001ade61:
      QVar10 = 8;
      uVar11 = puVar13[1] + 1;
    }
    else {
      QVar10 = 0;
      uVar11 = puVar13[2] & 0x7fffffff;
    }
    QVector<QRect>::realloc(*(QVector<QRect> **)(unaff_RBP + -0x88),uVar11,QVar10);
    puVar13 = *(uint **)(unaff_RBP + -0x70);
    uVar5 = *(undefined8 *)(unaff_RBP + -0x78);
    uVar11 = puVar13[1];
    puVar8 = (undefined8 *)((long)puVar13 + *(long *)(puVar13 + 4) + (long)(int)uVar11 * 0x10);
    *puVar8 = *(undefined8 *)(unaff_RBP + -0x80);
    puVar8[1] = uVar5;
  }
  uVar5 = *(undefined8 *)(unaff_RBP + -0x88);
  puVar13[1] = uVar11 + 1;
  (**(code **)(**(long **)(unaff_RBX + 0x98) + 0x10))(*(long **)(unaff_RBX + 0x98),uVar5);
  if (*(long *)(unaff_RBX + 0x90) != 0) {
    KisOverlayPaintDeviceWrapper::readRects(*(QVector **)(unaff_RBX + 0x88));
  }
  lVar15 = *(long *)(unaff_RBX + 0x78);
  *(long *)(unaff_RBP + -0x60) = lVar15;
  if (lVar15 != 0) {
    LOCK();
    *(int *)(lVar15 + 8) = *(int *)(lVar15 + 8) + 1;
    UNLOCK();
  }
  uVar5 = *(undefined8 *)(unaff_RBX + 0xa0);
  piVar6 = *(int **)(unaff_RBX + 0xa0);
  *(undefined8 *)(unaff_RBP + -0x50) = *(undefined8 *)(unaff_RBX + 0x98);
  *(undefined8 *)(unaff_RBP + -0x48) = uVar5;
  if (piVar6 != (int *)0x0) {
    LOCK();
    *piVar6 = *piVar6 + 1;
    UNLOCK();
    LOCK();
    piVar6 = (int *)(*(long *)(unaff_RBP + -0x48) + 4);
    *piVar6 = *piVar6 + 1;
    UNLOCK();
  }
  *(undefined8 *)(unaff_RBP + -0x68) = unaff_R12;
  *(undefined8 *)(unaff_RBP + -0x58) = *(undefined8 *)(unaff_RBP + -0x90);
  *(QVector<KisPainter*> **)(unaff_RBP + -0x80) = (QVector<KisPainter*> *)(unaff_RBP + -0x68);
  QVector<KisPainter*>::append
            ((QVector<KisPainter*> *)(unaff_RBP + -0x68),(KisPainter **)(unaff_RBP + -0x58));
  if (*(long *)(unaff_RBX + 0xb8) != 0) {
    *(long *)(unaff_RBP + -0x58) = *(long *)(unaff_RBX + 0xb8);
    QVector<KisPainter*>::append
              (*(QVector<KisPainter*> **)(unaff_RBP + -0x80),(KisPainter **)(unaff_RBP + -0x58));
  }
  KisColorSmudgeStrategyBase::blendBrush
            (*(undefined8 *)(unaff_RBP + -0xa8),*(undefined8 *)(unaff_RBP + -0xb8),
             *(undefined8 *)(unaff_RBP + -0xc0),*(undefined8 *)(unaff_RBP + -0xb0),
             *(undefined8 *)(unaff_RBP + -200));
  pQVar14 = *(QArrayData **)(unaff_RBP + -0x68);
  if (*(int *)pQVar14 == 0) {
LAB_001ade18:
    QArrayData::deallocate(pQVar14,8,8);
  }
  else if (*(int *)pQVar14 != -1) {
    LOCK();
    *(int *)pQVar14 = *(int *)pQVar14 + -1;
    iVar4 = *(int *)pQVar14;
    UNLOCK();
    pQVar14 = *(QArrayData **)(unaff_RBP + -0x68);
    if (iVar4 == 0) goto LAB_001ade18;
  }
  piVar6 = *(int **)(unaff_RBP + -0x48);
  if (piVar6 != (int *)0x0) {
    LOCK();
    piVar1 = piVar6 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piVar6 + 2))(piVar6);
    }
    LOCK();
    *piVar6 = *piVar6 + -1;
    UNLOCK();
    if (*piVar6 == 0) {
      operator_delete(piVar6,0x10);
    }
  }
  plVar7 = *(long **)(unaff_RBP + -0x60);
  if (plVar7 != (long *)0x0) {
    LOCK();
    plVar2 = plVar7 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*plVar7 + 8))();
    }
  }
  KisOverlayPaintDeviceWrapper::writeRects(*(QVector **)(unaff_RBX + 0x88),(int)unaff_R14);
  pQVar14 = *(QArrayData **)(unaff_RBP + -0x70);
  if (*(int *)pQVar14 != 0) {
    if (*(int *)pQVar14 == -1) goto LAB_001addda;
    LOCK();
    *(int *)pQVar14 = *(int *)pQVar14 + -1;
    iVar4 = *(int *)pQVar14;
    UNLOCK();
    pQVar14 = *(QArrayData **)(unaff_RBP + -0x70);
    if (iVar4 != 0) goto LAB_001addda;
  }
  QArrayData::deallocate(pQVar14,0x10,8);
LAB_001addda:
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 00149dc0 ======

/* KisColorSmudgeOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisColorSmudgeOp::paintAt(KisPaintInformation *param_1)

{
  long *plVar1;
  QArrayData *pQVar2;
  Data *pDVar3;
  Data *pDVar4;
  int *piVar5;
  long *plVar6;
  int *piVar7;
  char cVar8;
  int iVar9;
  int iVar10;
  QVector *pQVar11;
  QMapData *pQVar12;
  undefined8 uVar13;
  size_t __n;
  QSharedPointer QVar14;
  KisPaintInformation *in_RDX;
  bool bVar15;
  KisSpacingOption *in_RSI;
  long lVar16;
  long lVar17;
  long in_FS_OFFSET;
  double dVar18;
  double dVar19;
  undefined8 uVar20;
  undefined8 uVar21;
  double dVar22;
  double dVar23;
  double local_160;
  double local_158;
  undefined8 local_138;
  undefined8 local_130;
  double local_128;
  int local_108;
  int iStack_104;
  int iStack_100;
  int iStack_fc;
  double local_f8;
  double dStack_f0;
  double local_e8;
  QArrayData *local_d8;
  Data *pDStack_d0;
  Data *local_c8;
  undefined4 local_c0;
  long *local_b8;
  undefined8 uStack_b0;
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined8 local_98;
  undefined8 local_88;
  undefined local_80 [40];
  KisSpacingOption local_58;
  QMapData *local_50 [2];
  long local_40;
  
  piVar5 = *(int **)(in_RSI + 0x30);
  plVar6 = *(long **)(in_RSI + 0x28);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (piVar5 != (int *)0x0) {
    LOCK();
    *piVar5 = *piVar5 + 1;
    UNLOCK();
    LOCK();
    piVar5[1] = piVar5[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 00149e0c to 00149e23 has its CatchHandler @ 0014a871 */
  KisPaintOp::painter();
  KisPainter::device();
  if (local_b8 != (long *)0x0) {
    if (plVar6 != (long *)0x0) {
                    /* try { // try from 00149e48 to 00149e4d has its CatchHandler @ 0014a87d */
      cVar8 = (**(code **)(*plVar6 + 0xe0))(plVar6);
      if (cVar8 != '\0') {
        if (local_b8 != (long *)0x0) {
          LOCK();
          plVar1 = local_b8 + 2;
          *(int *)plVar1 = *(int *)plVar1 + -1;
          UNLOCK();
          if (*(int *)plVar1 == 0) {
            (**(code **)(*local_b8 + 0x20))();
          }
        }
                    /* try { // try from 00149e7d to 00149ed3 has its CatchHandler @ 0014a871 */
        iVar9 = KisSmudgeLengthOption::mode((KisSmudgeLengthOption *)(in_RSI + 0x418));
        if (iVar9 == 0) {
                    /* try { // try from 0014a074 to 0014a4a6 has its CatchHandler @ 0014a871 */
          KisDabCacheBase::disableSubpixelPrecision();
        }
        cVar8 = KisCurveOption::isChecked();
        local_160 = DAT_001b2a98;
        bVar15 = SUB81(in_RDX,0);
        if (cVar8 != '\0') {
          local_160 = (double)KisCurveOption::computeSizeLikeValue
                                        ((KisPaintInformation *)(in_RSI + 0x1f0),bVar15);
        }
        KisPaintOp::painter();
        KisPainter::device();
                    /* try { // try from 00149edf to 00149ee3 has its CatchHandler @ 0014a895 */
        KisPaintDevice::defaultBounds();
                    /* try { // try from 00149eef to 00149ef1 has its CatchHandler @ 0014a8ad */
        iVar9 = (**(code **)(*local_b8 + 0x30))();
        local_158 = DAT_001b2a98;
        dVar18 = DAT_001b2a98;
        if (0 < iVar9) {
          dVar18 = DAT_001b2a98 / (double)(1 << ((byte)iVar9 & 0x1f));
        }
        if (local_b8 != (long *)0x0) {
          LOCK();
          plVar1 = local_b8 + 1;
          *(int *)plVar1 = *(int *)plVar1 + -1;
          UNLOCK();
          if (*(int *)plVar1 == 0) {
            (**(code **)(*local_b8 + 8))();
          }
        }
        dVar18 = dVar18 * local_160;
        if (local_d8 != (QArrayData *)0x0) {
          LOCK();
          pQVar2 = local_d8 + 0x10;
          *(int *)pQVar2 = *(int *)pQVar2 + -1;
          UNLOCK();
          if (*(int *)pQVar2 == 0) {
            (**(code **)(*(long *)local_d8 + 0x20))();
          }
        }
                    /* try { // try from 00149f8a to 00149fa2 has its CatchHandler @ 0014a871 */
        dVar19 = (double)KisRotationOption::apply((KisPaintInformation *)(in_RSI + 0x318));
        cVar8 = KisBrushBasedPaintOp::checkSizeTooSmall(dVar18);
        if (cVar8 != '\0') {
          *(undefined8 *)param_1 = 1;
          *(undefined8 *)(param_1 + 0x18) = 0;
          param_1[0x20] = (KisPaintInformation)0x0;
          *(undefined (*) [16])(param_1 + 8) = (undefined  [16])0x0;
          goto joined_r0x0014a039;
        }
        cVar8 = KisCurveOption::isChecked();
        dStack_f0 = local_158;
        if (cVar8 != '\0') {
          dStack_f0 = (double)KisCurveOption::computeSizeLikeValue
                                        ((KisPaintInformation *)(in_RSI + 0x228),bVar15);
        }
        local_f8 = dVar18;
        local_e8 = dVar19;
        iVar9 = (**(code **)(*plVar6 + 0xc0))(0,plVar6,&local_f8);
        iVar10 = (**(code **)(*plVar6 + 0xb8))(0,plVar6,&local_f8);
        KisScatterOption::apply
                  ((KisPaintInformation *)(in_RSI + 0x360),(double)iVar10,(double)iVar9);
        cVar8 = KisCurveOption::isChecked();
        if (cVar8 == '\0') {
          local_130 = 0;
        }
        else {
          local_130 = KisCurveOption::computeSizeLikeValue
                                ((KisPaintInformation *)(in_RSI + 0x490),bVar15);
        }
        KisBrushBasedPaintOp::effectiveSpacing
                  (dVar18,dVar19,(KisAirbrushOptionData *)&local_b8,in_RSI,
                   (KisPaintInformation *)(in_RSI + 0x4d0));
        if (*(long *)(in_RSI + 0x518) == 0) {
          kis_safe_assert_recoverable
                    ("m_strategy",
                     "/builds/graphics/krita/plugins/paintops/colorsmudge/kis_colorsmudgeop.cpp",
                     0xb2);
          *(undefined8 *)(param_1 + 0x20) = local_98;
          *(long **)param_1 = local_b8;
          *(undefined8 *)(param_1 + 8) = uStack_b0;
          *(undefined8 *)(param_1 + 0x10) = local_a8;
          *(undefined8 *)(param_1 + 0x18) = uStack_a0;
          goto joined_r0x0014a039;
        }
        uVar20 = KisPaintThicknessOption::apply((KisPaintThicknessOption *)(in_RSI + 0x3a0),in_RDX);
        (**(code **)(**(long **)(in_RSI + 0x518) + 0x18))
                  (*(long **)(in_RSI + 0x518),*(undefined8 *)(in_RSI + 0x20));
        iVar9 = (int)*(undefined8 *)(in_RSI + 0x4ec);
        iVar10 = (int)((ulong)*(undefined8 *)(in_RSI + 0x4ec) >> 0x20);
        dVar18 = (double)(((int)*(undefined8 *)(in_RSI + 0x4f4) - iVar9) + (int)DAT_001b2a78) *
                 DAT_001b2a80 + (double)iVar9;
        dVar19 = (double)(((int)((ulong)*(undefined8 *)(in_RSI + 0x4f4) >> 0x20) - iVar10) +
                         DAT_001b2a78._4_4_) * DAT_001b2a80 + (double)iVar10;
        dVar22 = *(double *)(in_RSI + 0x500) - dVar18;
        dVar23 = *(double *)(in_RSI + 0x508) - dVar19;
        if (dVar23 < 0.0) {
          iStack_fc = (int)((dVar23 - (double)(int)(dVar23 - local_158)) + DAT_001b2a80) +
                      (int)(dVar23 - local_158);
        }
        else {
          iStack_fc = (int)(dVar23 + DAT_001b2a80);
        }
        if (dVar22 < 0.0) {
          iStack_100 = (int)((dVar22 - (double)(int)(dVar22 - local_158)) + DAT_001b2a80) +
                       (int)(dVar22 - local_158);
        }
        else {
          iStack_100 = (int)(dVar22 + DAT_001b2a80);
        }
        *(double *)(in_RSI + 0x500) = dVar18;
        *(double *)(in_RSI + 0x508) = dVar19;
        local_108 = iStack_100 + *(int *)(in_RSI + 0x4ec);
        iStack_104 = iStack_fc + *(int *)(in_RSI + 0x4f0);
        iStack_100 = iStack_100 + *(int *)(in_RSI + 0x4f4);
        iStack_fc = iStack_fc + *(int *)(in_RSI + 0x4f8);
        if (in_RSI[0x19c] != (KisSpacingOption)0x0) {
          in_RSI[0x19c] = (KisSpacingOption)0x0;
          *(undefined8 *)(param_1 + 0x20) = local_98;
          *(long **)param_1 = local_b8;
          *(undefined8 *)(param_1 + 8) = uStack_b0;
          *(undefined8 *)(param_1 + 0x10) = local_a8;
          *(undefined8 *)(param_1 + 0x18) = uStack_a0;
          goto joined_r0x0014a039;
        }
        cVar8 = KisCurveOption::isChecked();
        if (cVar8 == '\0') {
          local_138 = 0;
        }
        else {
          local_138 = KisCurveOption::computeSizeLikeValue
                                ((KisPaintInformation *)(in_RSI + 0x458),bVar15);
        }
        cVar8 = KisCurveOption::isChecked();
        if (cVar8 == '\0') {
          local_128 = local_158;
        }
        else {
          local_128 = (double)KisCurveOption::computeSizeLikeValue
                                        ((KisPaintInformation *)(in_RSI + 0x418),bVar15);
        }
        uVar21 = KisCurveOption::strengthValue();
        cVar8 = KisCurveOption::isChecked();
        if (cVar8 != '\0') {
          local_158 = (double)KisCurveOption::computeSizeLikeValue
                                        ((KisPaintInformation *)(in_RSI + 0x260),bVar15);
        }
        local_88 = *(undefined8 *)(in_RSI + 0x1a0);
        local_58 = in_RSI[0x1d0];
        local_50[0] = *(QMapData **)(in_RSI + 0x1d8);
        if (*(int *)local_50[0] == 0) {
                    /* try { // try from 0014a771 to 0014a7c1 has its CatchHandler @ 0014a871 */
          pQVar12 = (QMapData *)QMapDataBase::createData();
          local_50[0] = pQVar12;
          if (*(QMapNode<QString,QVariant> **)(*(long *)(in_RSI + 0x1d8) + 0x10) !=
              (QMapNode<QString,QVariant> *)0x0) {
            uVar13 = QMapNode<QString,QVariant>::copy
                               (*(QMapNode<QString,QVariant> **)(*(long *)(in_RSI + 0x1d8) + 0x10),
                                pQVar12);
            *(undefined8 *)(pQVar12 + 0x10) = uVar13;
            **(ulong **)(local_50[0] + 0x10) =
                 (ulong)((uint)**(ulong **)(local_50[0] + 0x10) & 3) | (ulong)(local_50[0] + 8);
            QMapDataBase::recalcMostLeftNode();
          }
        }
        else if (*(int *)local_50[0] != -1) {
          LOCK();
          *(int *)local_50[0] = *(int *)local_50[0] + 1;
          UNLOCK();
          local_50[0] = *(QMapData **)(in_RSI + 0x1d8);
        }
        __memcpy_chk(local_80,in_RSI + 0x1a8,local_58,0x38);
        local_d8 = *(QArrayData **)(in_RSI + 0x1e0);
        pDStack_d0 = *(Data **)(in_RSI + 0x1e8);
        piVar7 = *(int **)(in_RSI + 0x1e8);
        if (piVar7 != (int *)0x0) {
          LOCK();
          *piVar7 = *piVar7 + 1;
          UNLOCK();
          LOCK();
          *(int *)(pDStack_d0 + 4) = *(int *)(pDStack_d0 + 4) + 1;
          UNLOCK();
        }
        QVar14 = (QSharedPointer)&local_d8;
                    /* try { // try from 0014a563 to 0014a567 has its CatchHandler @ 0014a889 */
        KisGradientOption::apply
                  ((KisGradientOption *)(in_RSI + 0x3e0),(KoColor *)&local_88,QVar14,in_RDX);
        pDVar4 = pDStack_d0;
        if (pDStack_d0 != (Data *)0x0) {
          LOCK();
          pDVar3 = pDStack_d0 + 4;
          *(int *)pDVar3 = *(int *)pDVar3 + -1;
          UNLOCK();
          if (*(int *)pDVar3 == 0) {
            (**(code **)(pDStack_d0 + 8))();
          }
          LOCK();
          *(int *)pDVar4 = *(int *)pDVar4 + -1;
          UNLOCK();
          if (*(int *)pDVar4 == 0) {
            operator_delete(pDVar4,0x10);
          }
        }
        if (*(long *)(in_RSI + 0x510) != 0) {
          local_d8 = *(QArrayData **)(in_RSI + 0x4c8);
          if (*(int *)local_d8 == 0) {
                    /* try { // try from 0014a7f0 to 0014a7f4 has its CatchHandler @ 0014a859 */
            QListData::detach(QVar14);
            pDVar4 = (Data *)(*(long *)(in_RSI + 0x4c8) + 0x10 +
                             (long)*(int *)(*(long *)(in_RSI + 0x4c8) + 8) * 8);
            local_c8 = (Data *)(local_d8 + 0x10);
            lVar16 = (long)*(int *)(local_d8 + 0xc);
            lVar17 = (long)*(int *)(local_d8 + 8) * 8;
            if ((pDVar4 != local_c8 + lVar17) &&
               (__n = (lVar16 - *(int *)(local_d8 + 8)) * 8, 0 < (long)__n)) {
              memcpy(local_c8 + lVar17,pDVar4,__n);
              goto LAB_0014a5cb;
            }
          }
          else {
            if (*(int *)local_d8 != -1) {
              LOCK();
              *(int *)local_d8 = *(int *)local_d8 + 1;
              UNLOCK();
            }
LAB_0014a5cb:
            lVar16 = (long)*(int *)(local_d8 + 0xc);
            local_c8 = (Data *)(local_d8 + 0x10);
            lVar17 = (long)*(int *)(local_d8 + 8) << 3;
          }
          pDStack_d0 = local_c8 + lVar17;
          local_c0 = 1;
          local_c8 = local_c8 + lVar16 * 8;
          if (lVar16 * 8 != lVar17) {
            do {
                    /* try { // try from 0014a61d to 0014a621 has its CatchHandler @ 0014a8a1 */
              KisHSVOption::apply(*(KoColorTransformation **)pDStack_d0,
                                  *(KisPaintInformation **)(in_RSI + 0x510));
              pDStack_d0 = pDStack_d0 + 8;
            } while (pDStack_d0 != local_c8);
          }
          if (*(int *)local_d8 == 0) {
LAB_0014a7d6:
            QListData::dispose((Data *)local_d8);
          }
          else if (*(int *)local_d8 != -1) {
            LOCK();
            *(int *)local_d8 = *(int *)local_d8 + -1;
            UNLOCK();
            if (*(int *)local_d8 == 0) goto LAB_0014a7d6;
          }
                    /* try { // try from 0014a676 to 0014a6be has its CatchHandler @ 0014a859 */
          (**(code **)(**(long **)(in_RSI + 0x510) + 0x10))
                    (*(long **)(in_RSI + 0x510),local_80,local_80,1);
        }
        (**(code **)(**(long **)(in_RSI + 0x518) + 0x20))
                  (local_158,local_138,local_128,uVar21,uVar20,local_130,&local_d8,
                   *(long **)(in_RSI + 0x518),&local_108,in_RSI + 0x4ec,(KoColor *)&local_88);
                    /* try { // try from 0014a6c2 to 0014a6d1 has its CatchHandler @ 0014a865 */
        pQVar11 = (QVector *)KisPaintOp::painter();
        KisPainter::addDirtyRects(pQVar11);
        *(undefined8 *)(param_1 + 0x20) = local_98;
        *(long **)param_1 = local_b8;
        *(undefined8 *)(param_1 + 8) = uStack_b0;
        *(undefined8 *)(param_1 + 0x10) = local_a8;
        *(undefined8 *)(param_1 + 0x18) = uStack_a0;
        if (*(int *)local_d8 == 0) {
LAB_0014a760:
          QArrayData::deallocate(local_d8,0x10,8);
        }
        else if (*(int *)local_d8 != -1) {
          LOCK();
          *(int *)local_d8 = *(int *)local_d8 + -1;
          UNLOCK();
          if (*(int *)local_d8 == 0) goto LAB_0014a760;
        }
        QMap<QString,QVariant>::~QMap((QMap<QString,QVariant> *)local_50);
        goto joined_r0x0014a039;
      }
      if (local_b8 == (long *)0x0) goto LAB_0014a010;
    }
    LOCK();
    plVar6 = local_b8 + 2;
    *(int *)plVar6 = *(int *)plVar6 + -1;
    UNLOCK();
    if (*(int *)plVar6 == 0) {
      (**(code **)(*local_b8 + 0x20))();
    }
  }
LAB_0014a010:
  dVar18 = DAT_001b2a98;
  *(undefined8 *)param_1 = 1;
  *(undefined8 *)(param_1 + 0x18) = 0;
  param_1[0x20] = (KisPaintInformation)0x0;
  *(double *)(param_1 + 8) = dVar18;
  *(double *)(param_1 + 0x10) = dVar18;
joined_r0x0014a039:
  if (piVar5 != (int *)0x0) {
    LOCK();
    piVar7 = piVar5 + 1;
    *piVar7 = *piVar7 + -1;
    UNLOCK();
    if (*piVar7 == 0) {
      (**(code **)(piVar5 + 2))(piVar5);
    }
    LOCK();
    *piVar5 = *piVar5 + -1;
    UNLOCK();
    if (*piVar5 == 0) {
      operator_delete(piVar5,0x10);
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return param_1;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintDab @ 001a85d0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisColorSmudgeStrategyLightness::paintDab(QRect const&, QRect const&, KoColor const&, double,
   double, double, double, double, double) */

KisColorSmudgeStrategyLightness * __thiscall
KisColorSmudgeStrategyLightness::paintDab
          (KisColorSmudgeStrategyLightness *this,QRect *param_1,QRect *param_2,KoColor *param_3,
          double param_4,double param_5,double param_6,double param_7,double param_8,double param_9)

{
  KisFixedPaintDevice *pKVar1;
  QArrayData *pQVar2;
  undefined8 *puVar3;
  KisFixedPaintDevice *pKVar4;
  QRect QVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int *piVar11;
  uchar *puVar12;
  long *plVar13;
  code *pcVar14;
  KisFixedPaintDevice *this_00;
  KoColorSpace *pKVar15;
  KisFixedPaintDevice *this_01;
  QRect *pQVar16;
  undefined8 uVar17;
  undefined8 uVar18;
  undefined8 *puVar19;
  undefined8 *puVar20;
  QFlags QVar21;
  QSharedPointer QVar22;
  long lVar23;
  QArrayData *pQVar24;
  uint uVar25;
  uint uVar26;
  long lVar27;
  long in_FS_OFFSET;
  undefined auVar28 [16];
  QVector<QRect> *local_d8;
  QArrayData *local_d0;
  undefined8 local_a8;
  undefined8 uStack_a0;
  double local_90;
  QArrayData *local_88;
  QArrayData *local_80;
  undefined local_78 [16];
  undefined local_68 [16];
  QArrayData *local_58;
  undefined4 local_50;
  long local_40;
  
  iVar6 = *(int *)(param_3 + 8);
  iVar7 = *(int *)param_3;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  iVar8 = *(int *)(param_3 + 0xc);
  iVar9 = *(int *)(param_3 + 4);
  KisPainter::calculateAllMirroredRects((QRect *)this);
  local_88 = (QArrayData *)PTR_shared_null_001eefa0;
  if (*(int *)(PTR_shared_null_001eefa0 + 4) == 0) {
    pQVar24 = *(QArrayData **)this;
    if (pQVar24 == (QArrayData *)PTR_shared_null_001eefa0) {
      uVar25 = 0;
    }
    else {
      if (*(uint *)pQVar24 == 0) {
        if ((char)pQVar24[0xb] < '\0') {
          pQVar24 = (QArrayData *)
                    QArrayData::allocate(0x10,8,(ulong)(*(uint *)(pQVar24 + 8) & 0x7fffffff),0);
          if (pQVar24 == (QArrayData *)0x0) {
            qBadAlloc();
          }
          pQVar24[0xb] = (QArrayData)((byte)pQVar24[0xb] | 0x80);
        }
        else {
          pQVar24 = (QArrayData *)QArrayData::allocate(0x10,8,(long)(int)*(uint *)(pQVar24 + 4),0);
          if (pQVar24 == (QArrayData *)0x0) {
            qBadAlloc();
                    /* WARNING: Does not return */
            pcVar14 = (code *)invalidInstructionException();
            (*pcVar14)();
          }
        }
        if ((*(uint *)(pQVar24 + 8) & 0x7fffffff) != 0) {
          lVar23 = *(long *)this;
          uVar25 = *(uint *)(lVar23 + 4);
          puVar20 = (undefined8 *)(*(long *)(lVar23 + 0x10) + lVar23);
          puVar19 = puVar20 + (long)(int)uVar25 * 2;
          lVar23 = *(long *)(pQVar24 + 0x10) - (long)puVar20;
          for (; puVar20 != puVar19; puVar20 = puVar20 + 2) {
            uVar17 = puVar20[1];
            *(undefined8 *)((long)puVar20 + (long)(pQVar24 + lVar23)) = *puVar20;
            *(undefined8 *)((QArrayData *)((long)puVar20 + (long)(pQVar24 + lVar23)) + 8) = uVar17;
          }
          *(uint *)(pQVar24 + 4) = uVar25;
        }
      }
      else {
        if (*(uint *)pQVar24 != 0xffffffff) {
          LOCK();
          *(uint *)pQVar24 = *(uint *)pQVar24 + 1;
          UNLOCK();
        }
        pQVar24 = *(QArrayData **)this;
      }
      pQVar2 = local_88;
      if (*(int *)local_88 == 0) {
LAB_001a90a8:
        local_88 = pQVar24;
        QArrayData::deallocate(pQVar2,0x10,8);
        pQVar24 = local_88;
      }
      else if (*(int *)local_88 != -1) {
        LOCK();
        *(int *)local_88 = *(int *)local_88 + -1;
        UNLOCK();
        if (*(int *)local_88 == 0) goto LAB_001a90a8;
      }
      local_88 = pQVar24;
      uVar25 = *(uint *)(local_88 + 4);
      pQVar24 = local_88;
    }
  }
  else {
    uVar26 = *(uint *)(PTR_shared_null_001eefa0 + 8) & 0x7fffffff;
    uVar25 = *(int *)(PTR_shared_null_001eefa0 + 4) + *(int *)(*(long *)this + 4);
    if (*(uint *)PTR_shared_null_001eefa0 < 2) {
      if (uVar26 < uVar25) goto LAB_001a90c0;
    }
    else {
      QVar21 = 0;
      if (uVar26 < uVar25) {
LAB_001a90c0:
        QVar21 = 8;
        uVar26 = uVar25;
      }
                    /* try { // try from 001a8e46 to 001a8e4a has its CatchHandler @ 001a913c */
      QVector<QRect>::realloc((QVector<QRect> *)&local_88,uVar26,QVar21);
    }
    pQVar24 = local_88;
    if ((*(uint *)(local_88 + 8) & 0x7fffffff) == 0) {
      uVar25 = *(uint *)(local_88 + 4);
    }
    else {
      lVar23 = *(long *)this;
      lVar27 = *(long *)(lVar23 + 0x10) + lVar23;
      pQVar2 = local_88 + (long)(int)uVar25 * 0x10 + *(long *)(local_88 + 0x10);
      for (lVar23 = (long)*(int *)(lVar23 + 4) * 0x10 + lVar27; lVar27 != lVar23;
          lVar23 = lVar23 + -0x10) {
        uVar17 = *(undefined8 *)(lVar23 + -8);
        *(undefined8 *)(pQVar2 + -0x10) = *(undefined8 *)(lVar23 + -0x10);
        *(undefined8 *)(pQVar2 + -8) = uVar17;
        pQVar2 = pQVar2 + -0x10;
      }
      *(uint *)(local_88 + 4) = uVar25;
    }
  }
  local_d8 = (QVector<QRect> *)&local_88;
  if (*(uint *)pQVar24 < 2) {
    if ((*(uint *)(pQVar24 + 8) & 0x7fffffff) < uVar25 + 1) {
      local_a8 = *(undefined8 *)param_2;
      uStack_a0 = *(undefined8 *)(param_2 + 8);
      goto LAB_001a8df2;
    }
    uVar25 = *(uint *)(pQVar24 + 4);
    uVar17 = *(undefined8 *)(param_2 + 8);
    lVar23 = *(long *)(pQVar24 + 0x10);
    *(undefined8 *)(pQVar24 + lVar23 + (long)(int)uVar25 * 0x10) = *(undefined8 *)param_2;
    *(undefined8 *)(pQVar24 + lVar23 + (long)(int)uVar25 * 0x10 + 8) = uVar17;
  }
  else {
    local_a8 = *(undefined8 *)param_2;
    uStack_a0 = *(undefined8 *)(param_2 + 8);
    if ((*(uint *)(pQVar24 + 8) & 0x7fffffff) < uVar25 + 1) {
LAB_001a8df2:
      QVar21 = 8;
      uVar25 = *(uint *)(pQVar24 + 4) + 1;
    }
    else {
      QVar21 = 0;
      uVar25 = *(uint *)(pQVar24 + 8) & 0x7fffffff;
    }
                    /* try { // try from 001a8742 to 001a8781 has its CatchHandler @ 001a913c */
    QVector<QRect>::realloc(local_d8,uVar25,QVar21);
    uVar25 = *(uint *)(local_88 + 4);
    lVar23 = *(long *)(local_88 + 0x10);
    *(undefined8 *)(local_88 + lVar23 + (long)(int)uVar25 * 0x10) = local_a8;
    *(undefined8 *)(local_88 + lVar23 + (long)(int)uVar25 * 0x10 + 8) = uStack_a0;
    pQVar24 = local_88;
  }
  *(uint *)(pQVar24 + 4) = uVar25 + 1;
  (**(code **)(**(long **)(param_1 + 0xa8) + 0x10))(*(long **)(param_1 + 0xa8),local_d8);
  local_78._0_8_ = *(undefined8 *)(param_1 + 0x78);
  QVar5 = param_1[0xd8];
  if ((long *)local_78._0_8_ != (long *)0x0) {
    LOCK();
    *(int *)(local_78._0_8_ + 8) = *(int *)(local_78._0_8_ + 8) + 1;
    UNLOCK();
  }
  local_68._0_8_ = *(undefined8 *)(param_1 + 0xa8);
  local_68._8_8_ = *(undefined8 *)(param_1 + 0xb0);
  piVar11 = *(int **)(param_1 + 0xb0);
  if (piVar11 != (int *)0x0) {
    LOCK();
    *piVar11 = *piVar11 + 1;
    UNLOCK();
    LOCK();
    *(int *)(local_68._8_8_ + 4) = *(int *)(local_68._8_8_ + 4) + 1;
    UNLOCK();
  }
  local_80 = (QArrayData *)QArrayData::allocate(8,8,1,0);
  if (local_80 == (QArrayData *)0x0) {
    qBadAlloc();
  }
  *(QRect **)(local_80 + *(long *)(local_80 + 0x10)) = param_1 + 0xb8;
  *(undefined4 *)(local_80 + 4) = 1;
                    /* try { // try from 001a8854 to 001a8858 has its CatchHandler @ 001a9145 */
  KisColorSmudgeStrategyBase::blendBrush
            (param_4,param_6,param_7,param_5,param_9,param_1,&local_80,local_68,local_78,QVar5,
             param_2,param_3);
  if (*(int *)local_80 == 0) {
LAB_001a8d90:
    QArrayData::deallocate(local_80,8,8);
  }
  else if (*(int *)local_80 != -1) {
    LOCK();
    *(int *)local_80 = *(int *)local_80 + -1;
    UNLOCK();
    if (*(int *)local_80 == 0) goto LAB_001a8d90;
  }
  uVar17 = local_68._8_8_;
  if ((int *)local_68._8_8_ != (int *)0x0) {
    LOCK();
    piVar11 = (int *)(local_68._8_8_ + 4);
    *piVar11 = *piVar11 + -1;
    UNLOCK();
    if (*piVar11 == 0) {
      (**(code **)(local_68._8_8_ + 8))(local_68._8_8_);
    }
    LOCK();
    *(int *)uVar17 = *(int *)uVar17 + -1;
    UNLOCK();
    if (*(int *)uVar17 == 0) {
      operator_delete((void *)uVar17,0x10);
    }
  }
  if ((long *)local_78._0_8_ != (long *)0x0) {
    LOCK();
    plVar13 = (long *)(local_78._0_8_ + 8);
    *(int *)plVar13 = *(int *)plVar13 + -1;
    UNLOCK();
    if (*(int *)plVar13 == 0) {
      (**(code **)(*(long *)local_78._0_8_ + 8))();
    }
  }
  local_90 = param_4;
  if (*(int *)(param_1 + 0xf8) != 1) {
    local_90 = ((DAT_001c8848 - (param_6 - _DAT_001ca370)) * param_8 + (param_6 - _DAT_001ca370)) *
               param_4;
  }
                    /* try { // try from 001a8917 to 001a8927 has its CatchHandler @ 001a913c */
  KisPainter::setOpacityF(local_90);
  local_68 = KisFixedPaintDevice::bounds();
  local_78._0_8_ = *(undefined8 *)(param_1 + 0x80);
  if ((long *)local_78._0_8_ != (long *)0x0) {
    LOCK();
    *(int *)(local_78._0_8_ + 8) = *(int *)(local_78._0_8_ + 8) + 1;
    UNLOCK();
  }
  local_80 = *(QArrayData **)param_3;
                    /* try { // try from 001a8960 to 001a8964 has its CatchHandler @ 001a9151 */
  KisPainter::bltFixed(param_1 + 200,&local_80,local_78,local_68);
  if ((long *)local_78._0_8_ != (long *)0x0) {
    LOCK();
    plVar13 = (long *)(local_78._0_8_ + 8);
    *(int *)plVar13 = *(int *)plVar13 + -1;
    UNLOCK();
    if (*(int *)plVar13 == 0) {
      (**(code **)(*(long *)local_78._0_8_ + 8))();
    }
  }
  local_68._0_8_ = *(undefined8 *)(param_1 + 0x80);
  QVar5 = param_1[0xd8];
  if ((long *)local_68._0_8_ != (long *)0x0) {
    LOCK();
    *(int *)(local_68._0_8_ + 8) = *(int *)(local_68._0_8_ + 8) + 1;
    UNLOCK();
  }
                    /* try { // try from 001a89ae to 001a89b2 has its CatchHandler @ 001a915d */
  KisPainter::renderMirrorMaskSafe
            (param_1 + 200,*(undefined8 *)param_3,*(undefined8 *)(param_3 + 8),local_68,QVar5);
  if ((long *)local_68._0_8_ != (long *)0x0) {
    LOCK();
    plVar13 = (long *)(local_68._0_8_ + 8);
    *(int *)plVar13 = *(int *)plVar13 + -1;
    UNLOCK();
    if (*(int *)plVar13 == 0) {
      (**(code **)(*(long *)local_68._0_8_ + 8))();
    }
  }
                    /* try { // try from 001a89d5 to 001a89d9 has its CatchHandler @ 001a913c */
  this_00 = (KisFixedPaintDevice *)operator_new(0x38);
  local_68._0_8_ = *(undefined8 *)(param_1 + 8);
  local_68._8_8_ = *(undefined8 *)(param_1 + 0x10);
  piVar11 = *(int **)(param_1 + 0x10);
  if (piVar11 != (int *)0x0) {
    LOCK();
    *piVar11 = *piVar11 + 1;
    UNLOCK();
    LOCK();
    *(int *)(local_68._8_8_ + 4) = *(int *)(local_68._8_8_ + 4) + 1;
    UNLOCK();
  }
                    /* try { // try from 001a8a08 to 001a8a25 has its CatchHandler @ 001a9103 */
  pKVar15 = (KoColorSpace *)KisPaintDevice::colorSpace();
  QVar22 = (QSharedPointer)local_68;
  KisFixedPaintDevice::KisFixedPaintDevice(this_00,pKVar15,QVar22);
  uVar17 = local_68._8_8_;
  pKVar1 = this_00 + 8;
  LOCK();
  *(int *)(this_00 + 8) = *(int *)(this_00 + 8) + 1;
  UNLOCK();
  if ((int *)local_68._8_8_ != (int *)0x0) {
    LOCK();
    piVar11 = (int *)(local_68._8_8_ + 4);
    *piVar11 = *piVar11 + -1;
    UNLOCK();
    if (*piVar11 == 0) {
      (**(code **)(local_68._8_8_ + 8))(local_68._8_8_);
    }
    LOCK();
    *(int *)uVar17 = *(int *)uVar17 + -1;
    UNLOCK();
    if (*(int *)uVar17 == 0) {
      operator_delete((void *)uVar17,0x10);
    }
  }
                    /* try { // try from 001a8a5b to 001a8a5f has its CatchHandler @ 001a911b */
  this_01 = (KisFixedPaintDevice *)operator_new(0x38);
  local_68._0_8_ = *(undefined8 *)(param_1 + 8);
  local_68._8_8_ = *(undefined8 *)(param_1 + 0x10);
  piVar11 = *(int **)(param_1 + 0x10);
  if (piVar11 != (int *)0x0) {
    LOCK();
    *piVar11 = *piVar11 + 1;
    UNLOCK();
    LOCK();
    *(int *)(local_68._8_8_ + 4) = *(int *)(local_68._8_8_ + 4) + 1;
    UNLOCK();
  }
                    /* try { // try from 001a8a8a to 001a8aa0 has its CatchHandler @ 001a910f */
  pKVar15 = (KoColorSpace *)KisPaintDevice::colorSpace();
  KisFixedPaintDevice::KisFixedPaintDevice(this_01,pKVar15,QVar22);
  uVar17 = local_68._8_8_;
  pKVar4 = this_01 + 8;
  LOCK();
  *(int *)(this_01 + 8) = *(int *)(this_01 + 8) + 1;
  UNLOCK();
  if ((int *)local_68._8_8_ != (int *)0x0) {
    LOCK();
    piVar11 = (int *)(local_68._8_8_ + 4);
    *piVar11 = *piVar11 + -1;
    UNLOCK();
    if (*piVar11 == 0) {
      (**(code **)(local_68._8_8_ + 8))(local_68._8_8_);
    }
    LOCK();
    *(int *)uVar17 = *(int *)uVar17 + -1;
    UNLOCK();
    if (*(int *)uVar17 == 0) {
      operator_delete((void *)uVar17,0x10);
    }
  }
  local_d0 = *(QArrayData **)this;
  if (*(int *)local_d0 == 0) {
    if ((char)local_d0[0xb] < '\0') {
      local_d0 = (QArrayData *)
                 QArrayData::allocate(0x10,8,(ulong)(*(uint *)(local_d0 + 8) & 0x7fffffff),0);
      local_68._0_8_ = local_d0;
      if (local_d0 == (QArrayData *)0x0) {
        qBadAlloc();
      }
      local_d0[0xb] = (QArrayData)((byte)local_d0[0xb] | 0x80);
    }
    else {
      local_d0 = (QArrayData *)QArrayData::allocate(0x10,8,(long)*(int *)(local_d0 + 4),0);
      local_68._0_8_ = local_d0;
      if (local_d0 == (QArrayData *)0x0) {
        qBadAlloc();
      }
    }
    if ((*(uint *)(local_d0 + 8) & 0x7fffffff) == 0) {
      pQVar24 = local_d0 + *(long *)(local_d0 + 0x10);
      lVar23 = (long)*(int *)(local_d0 + 4) << 4;
    }
    else {
      lVar23 = *(long *)this;
      pQVar24 = local_d0 + *(long *)(local_d0 + 0x10);
      puVar19 = (undefined8 *)(*(long *)(lVar23 + 0x10) + lVar23);
      iVar10 = *(int *)(lVar23 + 4);
      lVar27 = (long)pQVar24 - (long)puVar19;
      lVar23 = (long)iVar10 * 0x10;
      puVar20 = puVar19 + (long)iVar10 * 2;
      for (; puVar19 != puVar20; puVar19 = puVar19 + 2) {
        puVar3 = (undefined8 *)((long)puVar19 + lVar27);
        uVar17 = puVar19[1];
        *puVar3 = *puVar19;
        puVar3[1] = uVar17;
      }
      *(int *)(local_d0 + 4) = iVar10;
    }
  }
  else {
    if (*(int *)local_d0 != -1) {
      LOCK();
      *(int *)local_d0 = *(int *)local_d0 + 1;
      UNLOCK();
      local_d0 = *(QArrayData **)this;
    }
    lVar23 = (long)*(int *)(local_d0 + 4) << 4;
    pQVar24 = local_d0 + *(long *)(local_d0 + 0x10);
    local_68._0_8_ = local_d0;
  }
  pQVar2 = pQVar24 + lVar23;
  local_50 = 1;
  local_58 = pQVar2;
  for (; local_68._8_8_ = pQVar24, pQVar24 != pQVar2; pQVar24 = pQVar24 + 0x10) {
                    /* try { // try from 001a8b45 to 001a8c24 has its CatchHandler @ 001a9127 */
    KisFixedPaintDevice::setRect((QRect *)this_00);
    KisFixedPaintDevice::lazyGrowBufferWithoutInitialization();
    KisFixedPaintDevice::setRect((QRect *)this_01);
    KisFixedPaintDevice::lazyGrowBufferWithoutInitialization();
    puVar12 = *(uchar **)(param_1 + 0x90);
    pQVar16 = (QRect *)KisFixedPaintDevice::data();
    KisPaintDevice::readBytes(puVar12,pQVar16);
    puVar12 = *(uchar **)(param_1 + 0x88);
    pQVar16 = (QRect *)KisFixedPaintDevice::data();
    KisPaintDevice::readBytes(puVar12,pQVar16);
    plVar13 = *(long **)(this_00 + 0x18);
    pcVar14 = *(code **)(*plVar13 + 0x1a8);
    uVar17 = KisFixedPaintDevice::data();
    uVar18 = KisFixedPaintDevice::data();
    (*pcVar14)(DAT_001c8848,plVar13,uVar18,uVar17,((iVar6 - iVar7) + 1) * ((iVar8 - iVar9) + 1));
    puVar12 = *(uchar **)(param_1 + 0x98);
    auVar28 = KisFixedPaintDevice::bounds();
    local_78 = auVar28;
    pQVar16 = (QRect *)KisFixedPaintDevice::data();
    KisPaintDevice::writeBytes(puVar12,pQVar16);
  }
  if (*(int *)local_d0 == 0) {
LAB_001a8d70:
    QArrayData::deallocate((QArrayData *)local_68._0_8_,0x10,8);
  }
  else if (*(int *)local_d0 != -1) {
    LOCK();
    *(int *)local_d0 = *(int *)local_d0 + -1;
    UNLOCK();
    if (*(int *)local_d0 == 0) goto LAB_001a8d70;
  }
                    /* try { // try from 001a8c71 to 001a8c75 has its CatchHandler @ 001a9133 */
  KisOverlayPaintDeviceWrapper::writeRects(*(QVector **)(param_1 + 0xa0),(int)this);
  LOCK();
  *(int *)pKVar4 = *(int *)pKVar4 + -1;
  UNLOCK();
  if (*(int *)pKVar4 == 0) {
    (**(code **)(*(long *)this_01 + 8))(this_01);
  }
  LOCK();
  *(int *)pKVar1 = *(int *)pKVar1 + -1;
  UNLOCK();
  if (*(int *)pKVar1 == 0) {
    (**(code **)(*(long *)this_00 + 8))();
  }
  if (*(int *)local_88 != 0) {
    if (*(int *)local_88 == -1) goto LAB_001a8cc8;
    LOCK();
    *(int *)local_88 = *(int *)local_88 + -1;
    UNLOCK();
    if (*(int *)local_88 != 0) goto LAB_001a8cc8;
  }
  QArrayData::deallocate(local_88,0x10,8);
LAB_001a8cc8:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return this;
}


// ====== paintDab @ 001adae0 ======

/* KisColorSmudgeStrategyWithOverlay::paintDab(QRect const&, QRect const&, KoColor const&, double,
   double, double, double, double, double) */

KisColorSmudgeStrategyWithOverlay * __thiscall
KisColorSmudgeStrategyWithOverlay::paintDab
          (KisColorSmudgeStrategyWithOverlay *this,QRect *param_1,QRect *param_2,KoColor *param_3,
          double param_4,double param_5,double param_6,double param_7,double param_8,double param_9)

{
  int *piVar1;
  long *plVar2;
  QRect QVar3;
  int *piVar4;
  code *pcVar5;
  undefined8 uVar6;
  QArrayData *pQVar7;
  undefined *puVar8;
  QArrayData *pQVar9;
  undefined8 *puVar10;
  long lVar11;
  QFlags QVar12;
  uint uVar13;
  uint uVar14;
  undefined8 *puVar15;
  long lVar16;
  long in_FS_OFFSET;
  QVector<QRect> *local_90;
  undefined8 local_88;
  undefined8 uStack_80;
  QArrayData *local_78;
  QArrayData *local_70;
  long *local_68;
  QRect *local_60;
  undefined8 local_58;
  int *piStack_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisPainter::calculateAllMirroredRects((QRect *)this);
  puVar8 = PTR_shared_null_001eefa0;
  local_78 = (QArrayData *)PTR_shared_null_001eefa0;
  if (*(int *)(PTR_shared_null_001eefa0 + 4) == 0) {
    pQVar9 = *(QArrayData **)this;
    if (pQVar9 == (QArrayData *)PTR_shared_null_001eefa0) {
      uVar13 = 0;
    }
    else {
      if (*(uint *)pQVar9 == 0) {
        if ((char)pQVar9[0xb] < '\0') {
          pQVar9 = (QArrayData *)
                   QArrayData::allocate(0x10,8,(ulong)(*(uint *)(pQVar9 + 8) & 0x7fffffff),0);
          if (pQVar9 == (QArrayData *)0x0) {
            qBadAlloc();
          }
          pQVar9[0xb] = (QArrayData)((byte)pQVar9[0xb] | 0x80);
        }
        else {
          pQVar9 = (QArrayData *)QArrayData::allocate(0x10,8,(long)(int)*(uint *)(pQVar9 + 4),0);
          if (pQVar9 == (QArrayData *)0x0) {
            qBadAlloc();
                    /* WARNING: Does not return */
            pcVar5 = (code *)invalidInstructionException();
            (*pcVar5)();
          }
        }
        if ((*(uint *)(pQVar9 + 8) & 0x7fffffff) != 0) {
          lVar11 = *(long *)this;
          uVar13 = *(uint *)(lVar11 + 4);
          puVar10 = (undefined8 *)(*(long *)(lVar11 + 0x10) + lVar11);
          puVar15 = puVar10 + (long)(int)uVar13 * 2;
          lVar11 = *(long *)(pQVar9 + 0x10) - (long)puVar10;
          for (; puVar10 != puVar15; puVar10 = puVar10 + 2) {
            uVar6 = puVar10[1];
            *(undefined8 *)((long)puVar10 + (long)(pQVar9 + lVar11)) = *puVar10;
            *(undefined8 *)((QArrayData *)((long)puVar10 + (long)(pQVar9 + lVar11)) + 8) = uVar6;
          }
          *(uint *)(pQVar9 + 4) = uVar13;
        }
      }
      else {
        if (*(uint *)pQVar9 != 0xffffffff) {
          LOCK();
          *(uint *)pQVar9 = *(uint *)pQVar9 + 1;
          UNLOCK();
        }
        pQVar9 = *(QArrayData **)this;
      }
      pQVar7 = local_78;
      if (*(int *)local_78 == 0) {
LAB_001ae008:
        local_78 = pQVar9;
        QArrayData::deallocate(pQVar7,0x10,8);
        pQVar9 = local_78;
      }
      else if (*(int *)local_78 != -1) {
        LOCK();
        *(int *)local_78 = *(int *)local_78 + -1;
        UNLOCK();
        if (*(int *)local_78 == 0) goto LAB_001ae008;
      }
      local_78 = pQVar9;
      uVar13 = *(uint *)(local_78 + 4);
      pQVar9 = local_78;
    }
  }
  else {
    uVar13 = *(int *)(PTR_shared_null_001eefa0 + 4) + *(int *)(*(long *)this + 4);
    uVar14 = *(uint *)(PTR_shared_null_001eefa0 + 8) & 0x7fffffff;
    if (*(uint *)PTR_shared_null_001eefa0 < 2) {
      if (uVar14 < uVar13) goto LAB_001ae020;
    }
    else {
      QVar12 = 0;
      if (uVar14 < uVar13) {
LAB_001ae020:
        QVar12 = 8;
        uVar14 = uVar13;
      }
      QVector<QRect>::realloc((QVector<QRect> *)&local_78,uVar14,QVar12);
    }
    pQVar9 = local_78;
    if ((*(uint *)(local_78 + 8) & 0x7fffffff) == 0) {
      uVar13 = *(uint *)(local_78 + 4);
    }
    else {
      lVar11 = *(long *)this;
      lVar16 = *(long *)(lVar11 + 0x10) + lVar11;
      pQVar7 = local_78 + (long)(int)uVar13 * 0x10 + *(long *)(local_78 + 0x10);
      for (lVar11 = (long)*(int *)(lVar11 + 4) * 0x10 + lVar16; lVar16 != lVar11;
          lVar11 = lVar11 + -0x10) {
        uVar6 = *(undefined8 *)(lVar11 + -8);
        *(undefined8 *)(pQVar7 + -0x10) = *(undefined8 *)(lVar11 + -0x10);
        *(undefined8 *)(pQVar7 + -8) = uVar6;
        pQVar7 = pQVar7 + -0x10;
      }
      *(uint *)(local_78 + 4) = uVar13;
    }
  }
  local_90 = (QVector<QRect> *)&local_78;
  if (*(uint *)pQVar9 < 2) {
    if ((*(uint *)(pQVar9 + 8) & 0x7fffffff) < uVar13 + 1) {
      local_88 = *(undefined8 *)param_2;
      uStack_80 = *(undefined8 *)(param_2 + 8);
      goto LAB_001ade61;
    }
    uVar13 = *(uint *)(pQVar9 + 4);
    uVar6 = *(undefined8 *)(param_2 + 8);
    lVar11 = *(long *)(pQVar9 + 0x10);
    *(undefined8 *)(pQVar9 + lVar11 + (long)(int)uVar13 * 0x10) = *(undefined8 *)param_2;
    *(undefined8 *)(pQVar9 + lVar11 + (long)(int)uVar13 * 0x10 + 8) = uVar6;
  }
  else {
    local_88 = *(undefined8 *)param_2;
    uStack_80 = *(undefined8 *)(param_2 + 8);
    if ((*(uint *)(pQVar9 + 8) & 0x7fffffff) < uVar13 + 1) {
LAB_001ade61:
      QVar12 = 8;
      uVar13 = *(uint *)(pQVar9 + 4) + 1;
    }
    else {
      QVar12 = 0;
      uVar13 = *(uint *)(pQVar9 + 8) & 0x7fffffff;
    }
                    /* try { // try from 001adc22 to 001adc7b has its CatchHandler @ 001ae071 */
    QVector<QRect>::realloc(local_90,uVar13,QVar12);
    uVar13 = *(uint *)(local_78 + 4);
    lVar11 = *(long *)(local_78 + 0x10);
    *(undefined8 *)(local_78 + lVar11 + (long)(int)uVar13 * 0x10) = local_88;
    *(undefined8 *)(local_78 + lVar11 + (long)(int)uVar13 * 0x10 + 8) = uStack_80;
    pQVar9 = local_78;
  }
  *(uint *)(pQVar9 + 4) = uVar13 + 1;
  (**(code **)(**(long **)(param_1 + 0x98) + 0x10))(*(long **)(param_1 + 0x98),local_90);
  if (*(long *)(param_1 + 0x90) != 0) {
    KisOverlayPaintDeviceWrapper::readRects(*(QVector **)(param_1 + 0x88));
  }
  local_68 = *(long **)(param_1 + 0x78);
  QVar3 = param_1[0x80];
  if (local_68 != (long *)0x0) {
    LOCK();
    *(int *)(local_68 + 1) = *(int *)(local_68 + 1) + 1;
    UNLOCK();
  }
  local_58 = *(undefined8 *)(param_1 + 0x98);
  piStack_50 = *(int **)(param_1 + 0xa0);
  piVar4 = *(int **)(param_1 + 0xa0);
  if (piVar4 != (int *)0x0) {
    LOCK();
    *piVar4 = *piVar4 + 1;
    UNLOCK();
    LOCK();
    piStack_50[1] = piStack_50[1] + 1;
    UNLOCK();
  }
  local_70 = (QArrayData *)puVar8;
  local_60 = param_1 + 0xa8;
                    /* try { // try from 001adcdc to 001adcfc has its CatchHandler @ 001ae07a */
  QVector<KisPainter*>::append((QVector<KisPainter*> *)&local_70,(KisPainter **)&local_60);
  if (*(QRect **)(param_1 + 0xb8) != (QRect *)0x0) {
    local_60 = *(QRect **)(param_1 + 0xb8);
    QVector<KisPainter*>::append((QVector<KisPainter*> *)&local_70,(KisPainter **)&local_60);
  }
                    /* try { // try from 001add46 to 001add4a has its CatchHandler @ 001ae065 */
  KisColorSmudgeStrategyBase::blendBrush
            (param_4,param_6,param_7,param_5,param_9,param_1,(QVector<KisPainter*> *)&local_70,
             &local_58,&local_68,QVar3,param_2,param_3);
  if (*(int *)local_70 == 0) {
LAB_001ade18:
    QArrayData::deallocate(local_70,8,8);
  }
  else if (*(int *)local_70 != -1) {
    LOCK();
    *(int *)local_70 = *(int *)local_70 + -1;
    UNLOCK();
    if (*(int *)local_70 == 0) goto LAB_001ade18;
  }
  piVar4 = piStack_50;
  if (piStack_50 != (int *)0x0) {
    LOCK();
    piVar1 = piStack_50 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piStack_50 + 2))(piStack_50);
    }
    LOCK();
    *piVar4 = *piVar4 + -1;
    UNLOCK();
    if (*piVar4 == 0) {
      operator_delete(piVar4,0x10);
    }
  }
  if (local_68 != (long *)0x0) {
    LOCK();
    plVar2 = local_68 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*local_68 + 8))();
    }
  }
                    /* try { // try from 001addbc to 001adeb7 has its CatchHandler @ 001ae071 */
  KisOverlayPaintDeviceWrapper::writeRects(*(QVector **)(param_1 + 0x88),(int)this);
  if (*(int *)local_78 != 0) {
    if (*(int *)local_78 == -1) goto LAB_001addda;
    LOCK();
    *(int *)local_78 = *(int *)local_78 + -1;
    UNLOCK();
    if (*(int *)local_78 != 0) goto LAB_001addda;
  }
  QArrayData::deallocate(local_78,0x10,8);
LAB_001addda:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return this;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintBezierCurve @ 001f34f8 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 001f3b38 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


