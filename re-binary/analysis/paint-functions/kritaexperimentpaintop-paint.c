/* Painting functions extracted from kritaexperimentpaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintLine @ 00111530 ======

/* KisExperimentPaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) [clone .cold] */

void KisExperimentPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3,undefined param_4,undefined param_5,undefined param_6
               ,QPainterPath *param_7,undefined param_8,QPainterPath *param_9,undefined param_10,
               long param_11)

{
  QVector<QRect> *unaff_R12;
  QVector<QRect> *unaff_R14;
  QPainterPath *unaff_R15;
  long in_FS_OFFSET;
  
  QVector<QRect>::~QVector(unaff_R12);
  QPainterPath::~QPainterPath(unaff_R15);
  QPainterPath::~QPainterPath(param_9);
  QPainterPath::~QPainterPath(param_7);
  QVector<QRect>::~QVector(unaff_R14);
  if (param_11 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 0011cff0 ======

/* KisExperimentPaintOp::paintAt(KisPaintInformation const&) */

KisExperimentPaintOp * __thiscall
KisExperimentPaintOp::paintAt(KisExperimentPaintOp *this,KisPaintInformation *param_1)

{
  long lVar1;
  undefined8 uVar2;
  long in_FS_OFFSET;
  
  uVar2 = DAT_0013c650;
  lVar1 = *(long *)(in_FS_OFFSET + 0x28);
  if (*(code **)(*(long *)param_1 + 0x38) == updateSpacingImpl) {
    *(undefined8 *)this = 1;
    *(undefined8 *)(this + 0x18) = 0;
    this[0x20] = (KisExperimentPaintOp)0x0;
    *(undefined8 *)(this + 8) = uVar2;
    *(undefined8 *)(this + 0x10) = uVar2;
  }
  else {
    (**(code **)(*(long *)param_1 + 0x38))(this);
  }
  if (lVar1 == *(long *)(in_FS_OFFSET + 0x28)) {
    return this;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 0011ddf0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisExperimentPaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) */

void KisExperimentPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  QVector<QPointF> *this;
  QPointF *pQVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  int iVar4;
  int iVar5;
  long lVar6;
  double *pdVar7;
  undefined (*pauVar8) [16];
  undefined8 *puVar9;
  ulong uVar10;
  uint *puVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  uint uVar15;
  QFlags QVar16;
  QArrayData *pQVar17;
  QPainterPath *pQVar18;
  long in_FS_OFFSET;
  double dVar19;
  double dVar20;
  double dVar21;
  double dVar22;
  double dVar23;
  undefined auVar24 [16];
  QPainterPath *local_108;
  QArrayData *local_f8;
  double dStack_f0;
  double local_e8;
  double dStack_e0;
  QArrayData *local_d0;
  double local_c8;
  double dStack_c0;
  undefined local_b8 [16];
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined8 local_98;
  undefined8 uStack_90;
  QArrayData *local_88;
  double dStack_80;
  double local_78;
  double dStack_70;
  QArrayData *local_68;
  double dStack_60;
  double local_58;
  double local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  lVar6 = KisPaintOp::painter();
  if (lVar6 == 0) goto LAB_0011e002;
  if (param_1[0x7c] != (KisPaintInformation)0x0) {
    param_1[0x7c] = (KisPaintInformation)0x0;
    KisPaintInformation::pos();
    QPainterPath::moveTo((QPointF *)(param_1 + 0x90));
    KisPaintInformation::pos();
    QPainterPath::lineTo((QPointF *)(param_1 + 0x90));
    puVar9 = (undefined8 *)KisPaintInformation::pos();
    uVar2 = *puVar9;
    uVar3 = puVar9[1];
    *(undefined4 *)(param_1 + 0x78) = 0;
    *(undefined8 *)(param_1 + 0x38) = 0;
    *(undefined8 *)(param_1 + 0x68) = 0;
    *(undefined8 *)(param_1 + 0x80) = uVar2;
    *(undefined8 *)(param_1 + 0x88) = uVar3;
    *(undefined8 *)(param_1 + 0x40) = uVar2;
    *(undefined8 *)(param_1 + 0x48) = uVar3;
    *(undefined8 *)(param_1 + 0x58) = uVar2;
    *(undefined8 *)(param_1 + 0x60) = uVar3;
    goto LAB_0011e002;
  }
  pdVar7 = (double *)KisPaintInformation::pos();
  local_c8 = *pdVar7;
  dStack_c0 = pdVar7[1];
  pauVar8 = (undefined (*) [16])KisPaintInformation::pos();
  local_b8._0_8_ = *(undefined8 *)*pauVar8;
  local_b8._8_8_ = *(undefined8 *)(*pauVar8 + 8);
  auVar24 = *pauVar8;
  if (param_1[0x32] != (KisPaintInformation)0x0) {
    auVar24 = speedCorrectedPosition(param_1,param_2);
  }
  local_b8._8_8_ = auVar24._8_8_;
  local_b8._0_8_ = auVar24._0_8_;
  dVar21 = (double)local_b8._8_8_ - dStack_c0;
  dVar19 = (double)local_b8._0_8_ - local_c8;
  if (dVar19 < 0.0) {
    dVar19 = (double)((ulong)dVar19 ^ _DAT_0013c660);
  }
  if (dVar21 < 0.0) {
    dVar21 = (double)((ulong)dVar21 ^ _DAT_0013c660);
  }
  *(int *)(param_1 + 0x6c) = *(int *)(param_1 + 0x6c) + (int)(dVar19 + dVar21);
  local_b8 = auVar24;
  if (param_1[0x50] == (KisPaintInformation)0x0) {
    QPainterPath::lineTo((QPointF *)(param_1 + 0x90));
    QVector<QPointF>::append((QVector<QPointF> *)(param_1 + 0x70),(QPointF *)&local_c8);
    QVector<QPointF>::append((QVector<QPointF> *)(param_1 + 0x70),(QPointF *)local_b8);
    auVar24 = local_b8;
  }
  else {
    iVar4 = *(int *)(param_1 + 0x68) + (int)(dVar19 + dVar21);
    *(int *)(param_1 + 0x68) = iVar4;
    if (*(int *)(param_1 + 0x54) < iVar4) {
      this = (QVector<QPointF> *)(param_1 + 0x70);
      local_68 = (QArrayData *)
                 ((*(double *)(param_1 + 0x58) + (double)local_b8._0_8_) * DAT_0013c6a8);
      dStack_60 = (*(double *)(param_1 + 0x60) + (double)local_b8._8_8_) * DAT_0013c6a8;
      auVar24 = QPainterPath::currentPosition();
      puVar11 = *(uint **)(param_1 + 0x70);
      uVar15 = puVar11[2] & 0x7fffffff;
      if (*puVar11 < 2) {
        if (uVar15 < puVar11[1] + 1) goto LAB_0011e8f8;
      }
      else {
        QVar16 = 0;
        if (uVar15 < puVar11[1] + 1) {
LAB_0011e8f8:
          QVar16 = 8;
          uVar15 = puVar11[1] + 1;
        }
        QVector<QPointF>::realloc(this,uVar15,QVar16);
        puVar11 = *(uint **)(param_1 + 0x70);
      }
      uVar15 = puVar11[1];
      *(undefined (*) [16])((long)puVar11 + *(long *)(puVar11 + 4) + (long)(int)uVar15 * 0x10) =
           auVar24;
      puVar11[1] = uVar15 + 1;
      pQVar1 = (QPointF *)(param_1 + 0x58);
      QVector<QPointF>::append(this,pQVar1);
      QVector<QPointF>::append(this,pQVar1);
      QVector<QPointF>::append(this,(QPointF *)&local_68);
      QPainterPath::quadTo((QPointF *)(param_1 + 0x90),pQVar1);
      *(undefined4 *)(param_1 + 0x68) = 0;
      *(undefined (*) [16])(param_1 + 0x58) = local_b8;
      auVar24 = local_b8;
    }
  }
  pQVar18 = (QPainterPath *)(param_1 + 0x90);
  local_b8 = auVar24;
  if (param_1[0x20] != (KisPaintInformation)0x0) {
    uVar10 = QPainterPath::elementCount();
    if ((uVar10 & 0xf) == 0) {
      QPainterPath::boundingRect();
      applyDisplace((QPainterPath *)&local_68,(int)pQVar18);
      pQVar17 = *(QArrayData **)(param_1 + 0x90);
      *(QArrayData **)(param_1 + 0x90) = local_68;
      local_68 = pQVar17;
      QPainterPath::~QPainterPath((QPainterPath *)&local_68);
      QPainterPath::boundingRect();
      QRectF::operator|((QRectF *)&local_f8,(QRectF *)&local_88);
      local_78 = local_e8;
      dStack_70 = dStack_e0;
      if (dStack_e0 <= local_e8) {
        dStack_e0 = local_e8;
      }
      local_88 = local_f8;
      dStack_80 = dStack_f0;
      dVar19 = DAT_0013c650;
      if (DAT_0013c650 <= dStack_e0 * DAT_0013c688) {
        dVar19 = dStack_e0 * DAT_0013c688;
      }
      KisAlgebra2D::trySimplifyPath((QPainterPath *)&local_68,dVar19);
    }
    else {
      applyDisplace((QPainterPath *)&local_68,(int)pQVar18);
    }
    pQVar17 = *(QArrayData **)(param_1 + 0x90);
    *(QArrayData **)(param_1 + 0x90) = local_68;
    local_68 = pQVar17;
    QPainterPath::~QPainterPath((QPainterPath *)&local_68);
  }
  dVar21 = (double)KisPaintInformation::currentTime();
  iVar4 = *(int *)(param_1 + 0x78);
  QPainterPath::boundingRect();
  dVar19 = DAT_0013c6a8;
  dVar20 = local_50 + dStack_60;
  if (dVar20 < 0.0) {
    iVar5 = (int)((dVar20 - (double)(int)(dVar20 - DAT_0013c650)) + DAT_0013c6a8) +
            (int)(dVar20 - DAT_0013c650);
  }
  else {
    iVar5 = (int)(dVar20 + DAT_0013c6a8);
  }
  dVar20 = local_58 + (double)local_68;
  if (dVar20 < 0.0) {
    iVar13 = (int)((dVar20 - (double)(int)(dVar20 - DAT_0013c650)) + DAT_0013c6a8) +
             (int)(dVar20 - DAT_0013c650);
  }
  else {
    iVar13 = (int)(dVar20 + DAT_0013c6a8);
  }
  if (dStack_60 < 0.0) {
    iVar14 = (int)((dStack_60 - (double)(int)(dStack_60 - DAT_0013c650)) + DAT_0013c6a8) +
             (int)(dStack_60 - DAT_0013c650);
  }
  else {
    iVar14 = (int)(dStack_60 + DAT_0013c6a8);
  }
  if ((double)local_68 < 0.0) {
    iVar12 = (int)(((double)local_68 - (double)(int)((double)local_68 - DAT_0013c650)) +
                  DAT_0013c6a8) + (int)((double)local_68 - DAT_0013c650);
  }
  else {
    iVar12 = (int)((double)local_68 + DAT_0013c6a8);
  }
  iVar12 = (iVar13 + -1) - iVar12;
  iVar14 = (iVar5 + -1) - iVar14;
  if (iVar12 < iVar14) {
    iVar12 = iVar14;
  }
  iVar5 = iVar12 + 1;
  if ((int)(dVar21 - (double)iVar4) < 0x29) {
    if (param_1[0x20] != (KisPaintInformation)0x0) goto LAB_0011e002;
    if (iVar5 < 0) {
      iVar5 = iVar12 + 8;
    }
    if (*(int *)(param_1 + 0x6c) <= iVar5 >> 3) goto LAB_0011e002;
LAB_0011dfdd:
    if (*(int *)(*(long *)(param_1 + 0x70) + 4) != 0) {
      KritaUtils::splitTriangles((QPointF *)&local_68,(QVector *)(param_1 + 0x80));
                    /* try { // try from 0011e949 to 0011e94d has its CatchHandler @ 0011ebee */
      paintRegion((KisRegion *)param_1);
      pQVar17 = local_68;
      if (*(int *)local_68 == 0) goto LAB_0011e980;
      if (*(int *)local_68 != -1) {
        LOCK();
        *(int *)local_68 = *(int *)local_68 + -1;
        iVar4 = *(int *)local_68;
        UNLOCK();
joined_r0x0011e811:
        if (iVar4 == 0) goto LAB_0011e980;
      }
      goto LAB_0011e817;
    }
  }
  else {
    if (param_1[0x20] == (KisPaintInformation)0x0) goto LAB_0011dfdd;
    local_d0 = (QArrayData *)PTR_shared_null_0014afb8;
    if (iVar5 < 0x80) {
                    /* try { // try from 0011e0d1 to 0011e36d has its CatchHandler @ 0011ebe2 */
      QPainterPath::boundingRect();
      local_50 = local_50 + dStack_60;
      if (local_50 < 0.0) {
        iVar4 = (int)((local_50 - (double)(int)(local_50 - DAT_0013c650)) + dVar19) +
                (int)(local_50 - DAT_0013c650);
      }
      else {
        iVar4 = (int)(local_50 + dVar19);
      }
      local_58 = local_58 + (double)local_68;
      if (local_58 < 0.0) {
        iVar5 = (int)((local_58 - (double)(int)(local_58 - DAT_0013c650)) + dVar19) +
                (int)(local_58 - DAT_0013c650);
      }
      else {
        iVar5 = (int)(local_58 + dVar19);
      }
      if (dStack_60 < 0.0) {
        iVar13 = (int)((dStack_60 - (double)(int)(dStack_60 - DAT_0013c650)) + dVar19) +
                 (int)(dStack_60 - DAT_0013c650);
      }
      else {
        iVar13 = (int)(dStack_60 + dVar19);
      }
      if ((double)local_68 < 0.0) {
        iVar14 = (int)(((double)local_68 - (double)(int)((double)local_68 - DAT_0013c650)) + dVar19)
                 + (int)((double)local_68 - DAT_0013c650);
      }
      else {
        iVar14 = (int)((double)local_68 + dVar19);
      }
      local_98 = CONCAT44(iVar13,iVar14);
      uStack_90 = CONCAT44(iVar4 + -1,iVar5 + -1);
      QPainterPath::boundingRect();
      dVar21 = DAT_0013c650;
      dVar20 = dStack_70 + dStack_80;
      if (dVar20 < 0.0) {
        iVar4 = (int)((dVar20 - (double)(int)(dVar20 - DAT_0013c650)) + dVar19) +
                (int)(dVar20 - DAT_0013c650);
      }
      else {
        iVar4 = (int)(dVar20 + dVar19);
      }
      dVar20 = local_78 + (double)local_88;
      if (dVar20 < 0.0) {
        iVar5 = (int)((dVar20 - (double)(int)(dVar20 - DAT_0013c650)) + dVar19) +
                (int)(dVar20 - DAT_0013c650);
      }
      else {
        iVar5 = (int)(dVar20 + dVar19);
      }
      if (dStack_80 < 0.0) {
        iVar13 = (int)((dStack_80 - (double)(int)(dStack_80 - DAT_0013c650)) + dVar19) +
                 (int)(dStack_80 - DAT_0013c650);
      }
      else {
        iVar13 = (int)(dStack_80 + dVar19);
      }
      if ((double)local_88 < 0.0) {
        iVar14 = (int)(((double)local_88 - (double)(int)((double)local_88 - DAT_0013c650)) + dVar19)
                 + (int)((double)local_88 - DAT_0013c650);
      }
      else {
        iVar14 = (int)((double)local_88 + dVar19);
      }
      local_a8 = CONCAT44(iVar13,iVar14);
      uStack_a0 = CONCAT44(iVar4 + -1,iVar5 + -1);
      auVar24 = QRect::operator|((QRect *)&local_a8,(QRect *)&local_98);
      dVar22 = (double)auVar24._4_4_ - dVar21;
      dVar23 = (double)auVar24._0_4_ - dVar21;
      dVar20 = (double)((auVar24._12_4_ - auVar24._4_4_) + 1) + _DAT_0013c6c0 + dVar22;
      if (dVar20 < 0.0) {
        iVar4 = (int)((dVar20 - (double)(int)(dVar20 - dVar21)) + dVar19) + (int)(dVar20 - dVar21);
      }
      else {
        iVar4 = (int)(dVar20 + dVar19);
      }
      dVar20 = (double)((auVar24._8_4_ - auVar24._0_4_) + 1) + _DAT_0013c6c0 + dVar23;
      if (dVar20 < 0.0) {
        iVar5 = (int)((dVar20 - (double)(int)(dVar20 - dVar21)) + dVar19) + (int)(dVar20 - dVar21);
      }
      else {
        iVar5 = (int)(dVar20 + dVar19);
      }
      if (dVar22 < 0.0) {
        iVar13 = (int)((dVar22 - (double)(int)(dVar22 - dVar21)) + dVar19) + (int)(dVar22 - dVar21);
      }
      else {
        iVar13 = (int)(dVar22 + dVar19);
      }
      if (dVar23 < 0.0) {
        iVar14 = (int)((dVar23 - (double)(int)(dVar23 - dVar21)) + dVar19) + (int)(dVar23 - dVar21);
      }
      else {
        iVar14 = (int)(dVar23 + dVar19);
      }
      local_68 = (QArrayData *)CONCAT44(iVar13,iVar14);
      dStack_60 = (double)CONCAT44(iVar4 + -1,iVar5 + -1);
      KisRegion::KisRegion((KisRegion *)&local_88,(QRect *)&local_68);
                    /* try { // try from 0011e374 to 0011e378 has its CatchHandler @ 0011ebfa */
      KisRegion::operator=((KisRegion *)&local_d0,(KisRegion *)&local_88);
      if (*(int *)local_88 == 0) {
LAB_0011e3a8:
        QArrayData::deallocate(local_88,0x10,8);
      }
      else if (*(int *)local_88 != -1) {
        LOCK();
        *(int *)local_88 = *(int *)local_88 + -1;
        UNLOCK();
        if (*(int *)local_88 == 0) goto LAB_0011e3a8;
      }
    }
    else {
                    /* try { // try from 0011e741 to 0011e745 has its CatchHandler @ 0011ebe2 */
      QPainterPath::operator-((QPainterPath *)&local_a8,pQVar18);
                    /* try { // try from 0011e75f to 0011e763 has its CatchHandler @ 0011ec12 */
      QPainterPath::operator-((QPainterPath *)&local_98,(QPainterPath *)(param_1 + 0x28));
                    /* try { // try from 0011e777 to 0011e77b has its CatchHandler @ 0011ec34 */
      QPainterPath::operator|((QPainterPath *)&local_88,(QPainterPath *)&local_a8);
                    /* try { // try from 0011e782 to 0011e786 has its CatchHandler @ 0011ec23 */
      KritaUtils::splitPath((QPainterPath *)&local_68);
                    /* try { // try from 0011e792 to 0011e796 has its CatchHandler @ 0011ec06 */
      KisRegion::operator=((KisRegion *)&local_d0,(KisRegion *)&local_68);
      if (*(int *)local_68 == 0) {
LAB_0011e8d8:
        QArrayData::deallocate(local_68,0x10,8);
      }
      else if (*(int *)local_68 != -1) {
        LOCK();
        *(int *)local_68 = *(int *)local_68 + -1;
        UNLOCK();
        if (*(int *)local_68 == 0) goto LAB_0011e8d8;
      }
      QPainterPath::~QPainterPath((QPainterPath *)&local_88);
      QPainterPath::~QPainterPath((QPainterPath *)&local_98);
      QPainterPath::~QPainterPath((QPainterPath *)&local_a8);
    }
    local_108 = (QPainterPath *)(param_1 + 0x28);
                    /* try { // try from 0011e7e2 to 0011e7f3 has its CatchHandler @ 0011ebe2 */
    paintRegion((KisRegion *)param_1);
    QPainterPath::operator=(local_108,pQVar18);
    pQVar17 = local_d0;
    if (*(int *)local_d0 != 0) {
      if (*(int *)local_d0 != -1) {
        LOCK();
        *(int *)local_d0 = *(int *)local_d0 + -1;
        iVar4 = *(int *)local_d0;
        UNLOCK();
        goto joined_r0x0011e811;
      }
      goto LAB_0011e817;
    }
LAB_0011e980:
    QArrayData::deallocate(pQVar17,0x10,8);
LAB_0011e817:
    puVar11 = *(uint **)(param_1 + 0x70);
    if (puVar11[1] != 0) {
      if (1 < *puVar11) {
        if ((puVar11[2] & 0x7fffffff) == 0) {
          puVar11 = (uint *)QArrayData::allocate(0x10,8,0,2);
          *(uint **)(param_1 + 0x70) = puVar11;
        }
        else {
          QVector<QPointF>::realloc((QVector<QPointF> *)(param_1 + 0x70),puVar11[2] & 0x7fffffff,0);
          puVar11 = *(uint **)(param_1 + 0x70);
        }
      }
      if (1 < *puVar11) {
        if ((puVar11[2] & 0x7fffffff) == 0) {
          puVar11 = (uint *)QArrayData::allocate(0x10,8,0,2);
          *(uint **)(param_1 + 0x70) = puVar11;
        }
        else {
          QVector<QPointF>::realloc((QVector<QPointF> *)(param_1 + 0x70),puVar11[2] & 0x7fffffff,0);
          puVar11 = *(uint **)(param_1 + 0x70);
        }
      }
      puVar11[1] = 0;
    }
  }
  *(undefined4 *)(param_1 + 0x6c) = 0;
  dVar19 = (double)KisPaintInformation::currentTime();
  *(int *)(param_1 + 0x78) = (int)dVar19;
LAB_0011e002:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintBezierCurve @ 0014c328 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


