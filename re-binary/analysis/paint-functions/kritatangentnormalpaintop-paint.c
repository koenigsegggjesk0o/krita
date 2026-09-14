/* Painting functions extracted from kritatangentnormalpaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintLine @ 0011bae0 ======

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  (*(code *)PTR_paintLine_0016f578)();
  return;
}


// ====== paintAt @ 0011c99e ======

/* KisTangentNormalPaintOp::paintAt(KisPaintInformation const&) [clone .cold] */

void KisTangentNormalPaintOp::paintAt(KisPaintInformation *param_1)

{
  long unaff_RBP;
  long in_FS_OFFSET;
  
  QString::~QString(*(QString **)(unaff_RBP + -0x198));
  QMap<QString,QVariant>::~QMap((QMap<QString,QVariant> *)(unaff_RBP + -0xa8));
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 0011caf2 ======

/* KisTangentNormalPaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) [clone .cold] */

void KisTangentNormalPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  long unaff_RBP;
  long in_FS_OFFSET;
  
  QString::~QString(*(QString **)(unaff_RBP + -0x120));
  QString::~QString(*(QString **)(unaff_RBP + -0x148));
  QMap<QString,QVariant>::~QMap((QMap<QString,QVariant> *)(unaff_RBP + -0x88));
  KisPainter::~KisPainter(*(KisPainter **)(unaff_RBP + -0x130));
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 00128580 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTangentNormalPaintOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisTangentNormalPaintOp::paintAt(KisPaintInformation *param_1)

{
  long *plVar1;
  QArrayData *pQVar2;
  int *piVar3;
  long lVar4;
  long lVar5;
  ulong uVar6;
  KoColor *pKVar7;
  int *piVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  KoColorSpace *pKVar11;
  char cVar12;
  byte bVar13;
  int iVar14;
  uint uVar15;
  int iVar16;
  undefined8 *puVar17;
  QString *pQVar18;
  QMapData *pQVar19;
  ulong *puVar20;
  KoColorSpace *pKVar21;
  KisPaintInformation *pKVar22;
  undefined8 uVar23;
  KisPaintInformation *in_RDX;
  KisSpacingOption *in_RSI;
  long *plVar24;
  long *plVar25;
  long in_FS_OFFSET;
  bool bVar26;
  QArrayData *pQVar27;
  double dVar28;
  int *piVar29;
  double dVar30;
  int *local_1a8;
  QVector<float> *local_170;
  QArrayData *local_168;
  QArrayData *local_160;
  double local_158;
  double local_150;
  double local_148;
  QArrayData *local_140;
  undefined8 local_138;
  QArrayData *local_130;
  undefined local_128 [16];
  undefined local_118 [16];
  QArrayData *local_108;
  int *local_100;
  double local_f8;
  KoColorSpace *local_e8;
  undefined local_e0 [40];
  undefined local_b8;
  QMapData *local_b0;
  KoColor local_a8 [56];
  QMapNodeBase *local_70;
  uchar local_68 [40];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisPaintOp::painter();
  puVar17 = (undefined8 *)KisPainter::paintColor();
  local_e8 = (KoColorSpace *)*puVar17;
  local_b8 = *(undefined *)(puVar17 + 6);
  local_b0 = (QMapData *)puVar17[7];
  if (*(int *)local_b0 == 0) {
    pQVar19 = (QMapData *)QMapDataBase::createData();
    local_b0 = pQVar19;
    if (*(QMapNode<QString,QVariant> **)(puVar17[7] + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
      puVar20 = (ulong *)QMapNode<QString,QVariant>::copy
                                   (*(QMapNode<QString,QVariant> **)(puVar17[7] + 0x10),pQVar19);
      uVar6 = *puVar20;
      *(ulong **)(pQVar19 + 0x10) = puVar20;
      *puVar20 = (ulong)((uint)uVar6 & 3) | (ulong)(pQVar19 + 8);
      QMapDataBase::recalcMostLeftNode();
    }
  }
  else if (*(int *)local_b0 != -1) {
    LOCK();
    *(int *)local_b0 = *(int *)local_b0 + 1;
    UNLOCK();
    local_b0 = (QMapData *)puVar17[7];
  }
  __memcpy_chk(local_e0,puVar17 + 1,local_b8,0x38);
                    /* try { // try from 0012862f to 00128631 has its CatchHandler @ 00129aad */
  (**(code **)(*(long *)local_e8 + 0x68))((QRect *)&local_108);
                    /* try { // try from 00128646 to 0012864a has its CatchHandler @ 00129aa1 */
  KoID::id();
  piVar3 = local_100;
  if (local_100 != (int *)0x0) {
    LOCK();
    piVar29 = local_100 + 1;
    *piVar29 = *piVar29 + -1;
    UNLOCK();
    if (*piVar29 == 0) {
      (**(code **)(local_100 + 2))(local_100);
    }
    LOCK();
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      operator_delete(piVar3,0x10);
    }
  }
                    /* try { // try from 00128672 to 00128676 has its CatchHandler @ 00129a68 */
  pQVar18 = (QString *)KoColorSpaceRegistry::instance();
  local_108 = (QArrayData *)PTR_shared_null_0016efc8;
                    /* try { // try from 0012868b to 0012868f has its CatchHandler @ 00129b3d */
  KoColorSpaceRegistry::rgb8(pQVar18);
  if (*(int *)local_108 == 0) {
LAB_00128f68:
    QArrayData::deallocate(local_108,2,8);
  }
  else if (*(int *)local_108 != -1) {
    LOCK();
    *(int *)local_108 = *(int *)local_108 + -1;
    UNLOCK();
    if (*(int *)local_108 == 0) goto LAB_00128f68;
  }
                    /* try { // try from 001286d7 to 001286db has its CatchHandler @ 00129a68 */
  iVar14 = QString::compare_helper
                     ((QChar *)(local_168 + *(long *)(local_168 + 0x10)),*(int *)(local_168 + 4),
                      "RGBA",-1,1);
  pKVar21 = local_e8;
  if (iVar14 != 0) {
                    /* try { // try from 00129338 to 0012933c has its CatchHandler @ 00129a68 */
    pQVar18 = (QString *)KoColorSpaceRegistry::instance();
    local_108 = (QArrayData *)PTR_shared_null_0016efc8;
                    /* try { // try from 00129351 to 00129355 has its CatchHandler @ 00129add */
    pKVar21 = (KoColorSpace *)KoColorSpaceRegistry::rgb8(pQVar18);
    if (*(int *)local_108 == 0) {
LAB_00129380:
      QArrayData::deallocate(local_108,2,8);
    }
    else if (*(int *)local_108 != -1) {
      LOCK();
      *(int *)local_108 = *(int *)local_108 + -1;
      UNLOCK();
      if (*(int *)local_108 == 0) goto LAB_00129380;
    }
  }
  local_160 = (QArrayData *)QArrayData::allocate(4,8,4,0);
  if (local_160 == (QArrayData *)0x0) {
                    /* try { // try from 0011cae0 to 0011cae4 has its CatchHandler @ 0011c99f */
    qBadAlloc();
  }
  pKVar11 = local_e8;
  *(uint *)(local_160 + 4) = 4;
  *(undefined (*) [16])(local_160 + *(long *)(local_160 + 0x10)) = (undefined  [16])0x0;
                    /* try { // try from 0012873e to 00128740 has its CatchHandler @ 00129af5 */
  (**(code **)(*(long *)local_e8 + 0x70))();
                    /* try { // try from 00128759 to 0012875d has its CatchHandler @ 00129ab9 */
  KoID::id();
                    /* try { // try from 0012877e to 00128782 has its CatchHandler @ 00129b25 */
  iVar14 = QString::compare_helper
                     ((QChar *)(local_130 + *(long *)(local_130 + 0x10)),*(int *)(local_130 + 4),
                      "F16",-1,1);
  if (iVar14 == 0) {
    bVar26 = true;
LAB_001288e7:
    iVar14 = *(int *)local_130;
    if (iVar14 == 0) goto LAB_00128850;
LAB_001288f8:
    if (iVar14 != -1) {
      LOCK();
      *(int *)local_130 = *(int *)local_130 + -1;
      UNLOCK();
      if (*(int *)local_130 == 0) goto LAB_00128850;
    }
  }
  else {
                    /* try { // try from 00128794 to 00128796 has its CatchHandler @ 00129b19 */
    (**(code **)(*(long *)pKVar11 + 0x70))((QRect *)&local_108,pKVar11);
                    /* try { // try from 001287a7 to 001287ab has its CatchHandler @ 00129b0d */
    KoID::id();
                    /* try { // try from 001287cc to 001287d0 has its CatchHandler @ 00129b01 */
    iVar14 = QString::compare_helper
                       ((QChar *)(local_128._0_8_ + *(long *)(local_128._0_8_ + 0x10)),
                        *(int *)(local_128._0_8_ + 4),"F32",-1,1);
    bVar26 = iVar14 == 0;
    if (*(int *)local_128._0_8_ == 0) {
LAB_001293b0:
      QArrayData::deallocate((QArrayData *)local_128._0_8_,2,8);
    }
    else if (*(int *)local_128._0_8_ != -1) {
      LOCK();
      *(int *)local_128._0_8_ = *(int *)local_128._0_8_ + -1;
      UNLOCK();
      if (*(int *)local_128._0_8_ == 0) goto LAB_001293b0;
    }
    piVar3 = local_100;
    if (local_100 == (int *)0x0) goto LAB_001288e7;
    LOCK();
    piVar29 = local_100 + 1;
    *piVar29 = *piVar29 + -1;
    UNLOCK();
    if (*piVar29 == 0) {
      (**(code **)(local_100 + 2))(local_100);
    }
    LOCK();
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 != 0) goto LAB_001288e7;
    operator_delete(piVar3,0x10);
    iVar14 = *(int *)local_130;
    if (iVar14 != 0) goto LAB_001288f8;
LAB_00128850:
    QArrayData::deallocate(local_130,2,8);
  }
  uVar23 = local_118._8_8_;
  if ((int *)local_118._8_8_ != (int *)0x0) {
    LOCK();
    piVar3 = (int *)(local_118._8_8_ + 4);
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      (**(code **)(local_118._8_8_ + 8))(local_118._8_8_);
    }
    LOCK();
    *(int *)uVar23 = *(int *)uVar23 + -1;
    UNLOCK();
    if (*(int *)uVar23 == 0) {
      operator_delete((void *)uVar23,0x10);
    }
  }
  if (bVar26) {
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
                    /* try { // try from 001288c6 to 00128bb1 has its CatchHandler @ 00129b31 */
        QVector<float>::realloc
                  ((QVector<float> *)&local_160,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    uVar9 = DAT_00154230;
    local_170 = (QVector<float> *)&local_160;
    *(undefined4 *)(local_160 + *(long *)(local_160 + 0x10)) = DAT_00154230;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    *(undefined4 *)(local_160 + *(long *)(local_160 + 0x10) + 4) = uVar9;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    uVar9 = _DAT_00154234;
    *(undefined4 *)(local_160 + *(long *)(local_160 + 0x10) + 8) = _DAT_00154234;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    *(undefined4 *)(local_160 + *(long *)(local_160 + 0x10) + 0xc) = uVar9;
    KisTangentTiltOption::apply
              ((KisTangentTiltOption *)(in_RSI + 0x1a0),in_RDX,&local_158,&local_150,&local_148);
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_160 + *(long *)(local_160 + 0x10)) = (float)local_158;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_160 + *(long *)(local_160 + 0x10) + 4) = (float)local_150;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_160 + *(long *)(local_160 + 0x10) + 8) = (float)local_148;
  }
  else {
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc
                  ((QVector<float> *)&local_160,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    uVar9 = _DAT_00154234;
    local_170 = (QVector<float> *)&local_160;
    *(undefined4 *)(local_160 + *(long *)(local_160 + 0x10)) = _DAT_00154234;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
                    /* try { // try from 00128dbf to 00128f4e has its CatchHandler @ 00129b31 */
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    uVar10 = DAT_00154230;
    *(undefined4 *)(local_160 + *(long *)(local_160 + 0x10) + 4) = DAT_00154230;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    *(undefined4 *)(local_160 + *(long *)(local_160 + 0x10) + 8) = uVar10;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    *(undefined4 *)(local_160 + *(long *)(local_160 + 0x10) + 0xc) = uVar9;
    KisTangentTiltOption::apply
              ((KisTangentTiltOption *)(in_RSI + 0x1a0),in_RDX,&local_158,&local_150,&local_148);
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_160 + *(long *)(local_160 + 0x10)) = (float)local_148;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_160 + *(long *)(local_160 + 0x10) + 4) = (float)local_150;
    if (1 < *(uint *)local_160) {
      if ((*(uint *)(local_160 + 8) & 0x7fffffff) == 0) {
        local_160 = (QArrayData *)QArrayData::allocate(4,8,0,2);
        *(float *)(local_160 + *(long *)(local_160 + 0x10) + 8) = (float)local_158;
        goto LAB_00128b7f;
      }
      QVector<float>::realloc(local_170,*(uint *)(local_160 + 8) & 0x7fffffff,0);
    }
    *(float *)(local_160 + *(long *)(local_160 + 0x10) + 8) = (float)local_158;
  }
LAB_00128b7f:
  local_170 = (QVector<float> *)&local_160;
  (**(code **)(*(long *)pKVar21 + 0x50))(pKVar21,local_68,local_170);
  KoColor::KoColor(local_a8,local_68,pKVar21);
  piVar3 = *(int **)(in_RSI + 0x30);
  plVar24 = *(long **)(in_RSI + 0x28);
  if (piVar3 != (int *)0x0) {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
    LOCK();
    piVar3[1] = piVar3[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 00128bd2 to 00128be1 has its CatchHandler @ 00129a7d */
  KisPaintOp::painter();
  KisPainter::device();
  if (local_108 == (QArrayData *)0x0) {
LAB_00128f90:
    piVar29 = DAT_00154208;
    *(undefined8 *)param_1 = 1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(int **)(param_1 + 8) = piVar29;
    *(int **)(param_1 + 0x10) = piVar29;
  }
  else {
    if (plVar24 == (long *)0x0) {
LAB_00128f85:
      LOCK();
      pQVar27 = local_108 + 0x10;
      *(int *)pQVar27 = *(int *)pQVar27 + -1;
      UNLOCK();
      if (*(int *)pQVar27 == 0) {
        (**(code **)(*(long *)local_108 + 0x20))();
      }
      goto LAB_00128f90;
    }
                    /* try { // try from 00128c08 to 00128c0d has its CatchHandler @ 00129a89 */
    cVar12 = (**(code **)(*plVar24 + 0xe0))(plVar24,in_RDX);
    if (cVar12 == '\0') {
      if (local_108 == (QArrayData *)0x0) goto LAB_00128f90;
      goto LAB_00128f85;
    }
    if (local_108 != (QArrayData *)0x0) {
      LOCK();
      pQVar27 = local_108 + 0x10;
      *(int *)pQVar27 = *(int *)pQVar27 + -1;
      UNLOCK();
      if (*(int *)pQVar27 == 0) {
        (**(code **)(*(long *)local_108 + 0x20))();
      }
    }
                    /* try { // try from 00128c3e to 00128c71 has its CatchHandler @ 00129a7d */
    cVar12 = KisCurveOption::isChecked();
    local_1a8 = DAT_00154208;
    if (cVar12 != '\0') {
                    /* try { // try from 001293db to 001298ae has its CatchHandler @ 00129a7d */
      local_1a8 = (int *)KisCurveOption::computeSizeLikeValue
                                   ((KisPaintInformation *)(in_RSI + 0x278),SUB81(in_RDX,0));
    }
    KisPaintOp::painter();
    KisPainter::device();
                    /* try { // try from 00128c7c to 00128c80 has its CatchHandler @ 00129a71 */
    KisPaintDevice::defaultBounds();
                    /* try { // try from 00128c8b to 00128c8d has its CatchHandler @ 00129ac5 */
    iVar14 = (**(code **)(*(long *)local_108 + 0x30))();
    piVar8 = DAT_00154208;
    piVar29 = DAT_00154208;
    if (0 < iVar14) {
      piVar29 = (int *)((double)DAT_00154208 / (double)(1 << ((byte)iVar14 & 0x1f)));
    }
    if (local_108 != (QArrayData *)0x0) {
      LOCK();
      pQVar27 = local_108 + 8;
      *(int *)pQVar27 = *(int *)pQVar27 + -1;
      UNLOCK();
      if (*(int *)pQVar27 == 0) {
        (**(code **)(*(long *)local_108 + 8))();
      }
    }
    pQVar27 = (QArrayData *)((double)piVar29 * (double)local_1a8);
    if ((long *)local_118._0_8_ != (long *)0x0) {
      LOCK();
      plVar25 = (long *)(local_118._0_8_ + 0x10);
      *(int *)plVar25 = *(int *)plVar25 + -1;
      UNLOCK();
      if (*(int *)plVar25 == 0) {
        (**(code **)(*(long *)local_118._0_8_ + 0x20))();
      }
    }
                    /* try { // try from 00128d2e to 00128d4a has its CatchHandler @ 00129a7d */
    dVar28 = (double)KisRotationOption::apply((KisPaintInformation *)(in_RSI + 0x3a8));
    cVar12 = KisBrushBasedPaintOp::checkSizeTooSmall((double)pQVar27);
    if (cVar12 != '\0') {
      *(undefined8 *)param_1 = 1;
      *(undefined8 *)(param_1 + 0x18) = 0;
      param_1[0x20] = (KisPaintInformation)0x0;
      *(undefined (*) [16])(param_1 + 8) = (undefined  [16])0x0;
      goto LAB_00128fb9;
    }
    local_100 = piVar8;
    local_108 = pQVar27;
    local_f8 = dVar28;
    iVar14 = (**(code **)(*plVar24 + 0xc0))(0,plVar24,(QRect *)&local_108,in_RDX);
    iVar16 = (**(code **)(*plVar24 + 0xb8))(0,plVar24,(QRect *)&local_108,in_RDX);
    local_128 = KisScatterOption::apply
                          ((KisPaintInformation *)(in_RSI + 0x368),(double)iVar16,(double)iVar14);
    pKVar7 = *(KoColor **)(in_RSI + 0x20);
    cVar12 = KisCurveOption::isChecked();
    piVar29 = piVar8;
    if (cVar12 != '\0') {
      piVar29 = (int *)KisCurveOption::computeSizeLikeValue
                                 ((KisPaintInformation *)(in_RSI + 0x2f0),SUB81(in_RDX,0));
    }
    KisDabCache::fetchDab
              ((KoColorSpace *)local_118,pKVar7,(QPointF *)pKVar21,(KisDabShape *)local_a8,
               (KisPaintInformation *)local_128,(double)piVar29,(QRect *)&local_108,(double)piVar8);
    plVar24 = *(long **)(in_RSI + 0x440);
    plVar25 = plVar24;
    if ((long *)local_118._0_8_ != plVar24) {
      if ((long *)local_118._0_8_ != (long *)0x0) {
        LOCK();
        *(int *)(local_118._0_8_ + 8) = *(int *)(local_118._0_8_ + 8) + 1;
        UNLOCK();
        plVar24 = *(long **)(in_RSI + 0x440);
      }
      *(undefined8 *)(in_RSI + 0x440) = local_118._0_8_;
      plVar25 = (long *)local_118._0_8_;
      if (plVar24 != (long *)0x0) {
        LOCK();
        plVar1 = plVar24 + 1;
        *(int *)plVar1 = *(int *)plVar1 + -1;
        UNLOCK();
        if (*(int *)plVar1 == 0) {
          (**(code **)(*plVar24 + 8))();
          plVar25 = (long *)local_118._0_8_;
        }
      }
    }
    if (plVar25 != (long *)0x0) {
      LOCK();
      plVar24 = plVar25 + 1;
      *(int *)plVar24 = *(int *)plVar24 + -1;
      UNLOCK();
      if (*(int *)plVar24 == 0) {
        (**(code **)(*plVar25 + 8))();
      }
    }
    if ((*(int *)(in_RSI + 0x458) < *(int *)(in_RSI + 0x450)) ||
       (*(int *)(in_RSI + 0x45c) < *(int *)(in_RSI + 0x454))) goto LAB_00128f90;
    KisFixedPaintDevice::bounds();
    KisPaintOp::painter();
    dVar30 = (double)KisPainter::opacityF();
    KisPaintOp::painter();
    KisPainter::compositeOpId();
                    /* try { // try from 001298b9 to 001298e6 has its CatchHandler @ 00129ae9 */
    pKVar22 = (KisPaintInformation *)KisPaintOp::painter();
    KisFlowOpacityOption2::apply((KisPainter *)(in_RSI + 0x1c0),pKVar22);
    uVar23 = KisPaintOp::painter();
    local_118 = KisFixedPaintDevice::bounds();
    local_130 = *(QArrayData **)(in_RSI + 0x440);
    if (local_130 != (QArrayData *)0x0) {
      LOCK();
      *(int *)(local_130 + 8) = *(int *)(local_130 + 8) + 1;
      UNLOCK();
    }
    local_138 = *(undefined8 *)(in_RSI + 0x450);
                    /* try { // try from 00129933 to 00129937 has its CatchHandler @ 00129ad1 */
    KisPainter::bltFixed(uVar23,&local_138,&local_130,(KoColorSpace *)local_118);
    if (local_130 != (QArrayData *)0x0) {
      LOCK();
      pQVar2 = local_130 + 8;
      *(int *)pQVar2 = *(int *)pQVar2 + -1;
      UNLOCK();
      if (*(int *)pQVar2 == 0) {
        (**(code **)(*(long *)local_130 + 8))();
      }
    }
                    /* try { // try from 00129954 to 00129964 has its CatchHandler @ 00129ae9 */
    uVar23 = KisPaintOp::painter();
    bVar13 = KisDabCache::needSeparateOriginal();
    local_118._0_8_ = *(undefined8 *)(in_RSI + 0x440);
    if ((long *)local_118._0_8_ != (long *)0x0) {
      LOCK();
      *(int *)(local_118._0_8_ + 8) = *(int *)(local_118._0_8_ + 8) + 1;
      UNLOCK();
    }
                    /* try { // try from 0012999c to 001299a0 has its CatchHandler @ 00129a95 */
    KisPainter::renderMirrorMaskSafe
              (uVar23,*(undefined8 *)(in_RSI + 0x450),*(undefined8 *)(in_RSI + 0x458),
               (KoColorSpace *)local_118,bVar13 ^ 1);
    if ((long *)local_118._0_8_ != (long *)0x0) {
      LOCK();
      plVar24 = (long *)(local_118._0_8_ + 8);
      *(int *)plVar24 = *(int *)plVar24 + -1;
      UNLOCK();
      if (*(int *)plVar24 == 0) {
        (**(code **)(*(long *)local_118._0_8_ + 8))();
      }
    }
                    /* try { // try from 001299bd to 00129a11 has its CatchHandler @ 00129ae9 */
    KisPaintOp::painter();
    KisPainter::setOpacityF(dVar30);
    pQVar18 = (QString *)KisPaintOp::painter();
    KisPainter::setCompositeOpId(pQVar18);
    KisBrushBasedPaintOp::effectiveSpacing
              ((double)pQVar27,dVar28,(KisAirbrushOptionData *)param_1,in_RSI,
               (KisPaintInformation *)(in_RSI + 0x3f0));
    if (*(int *)local_140 == 0) {
LAB_00129a39:
      QArrayData::deallocate(local_140,2,8);
    }
    else if (*(int *)local_140 != -1) {
      LOCK();
      *(int *)local_140 = *(int *)local_140 + -1;
      UNLOCK();
      if (*(int *)local_140 == 0) goto LAB_00129a39;
    }
  }
LAB_00128fb9:
  if (piVar3 != (int *)0x0) {
    LOCK();
    piVar29 = piVar3 + 1;
    *piVar29 = *piVar29 + -1;
    UNLOCK();
    if (*piVar29 == 0) {
      (**(code **)(piVar3 + 2))(piVar3);
    }
    LOCK();
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      operator_delete(piVar3,0x10);
    }
  }
  if (*(int *)local_70 == 0) {
LAB_001291c0:
    lVar4 = *(long *)(local_70 + 0x10);
    if (lVar4 != 0) {
      pQVar27 = *(QArrayData **)(lVar4 + 0x18);
      if (*(int *)pQVar27 == 0) {
LAB_0012967d:
        QArrayData::deallocate(pQVar27,2,8);
      }
      else if (*(int *)pQVar27 != -1) {
        LOCK();
        *(int *)pQVar27 = *(int *)pQVar27 + -1;
        UNLOCK();
        if (*(int *)pQVar27 == 0) {
          pQVar27 = *(QArrayData **)(lVar4 + 0x18);
          goto LAB_0012967d;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
      lVar5 = *(long *)(lVar4 + 8);
      if (lVar5 != 0) {
        pQVar27 = *(QArrayData **)(lVar5 + 0x18);
        if (*(int *)pQVar27 == 0) {
LAB_00129218:
          QArrayData::deallocate(pQVar27,2,8);
        }
        else if (*(int *)pQVar27 != -1) {
          LOCK();
          *(int *)pQVar27 = *(int *)pQVar27 + -1;
          UNLOCK();
          if (*(int *)pQVar27 == 0) {
            pQVar27 = *(QArrayData **)(lVar5 + 0x18);
            goto LAB_00129218;
          }
        }
        QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
        if (*(QMapNode<QString,QVariant> **)(lVar5 + 8) != (QMapNode<QString,QVariant> *)0x0) {
          QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar5 + 8));
        }
        if (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
          QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar5 + 0x10))
          ;
        }
      }
      lVar4 = *(long *)(lVar4 + 0x10);
      if (lVar4 != 0) {
        pQVar27 = *(QArrayData **)(lVar4 + 0x18);
        if (*(int *)pQVar27 == 0) {
LAB_00129271:
          QArrayData::deallocate(pQVar27,2,8);
        }
        else if (*(int *)pQVar27 != -1) {
          LOCK();
          *(int *)pQVar27 = *(int *)pQVar27 + -1;
          UNLOCK();
          if (*(int *)pQVar27 == 0) {
            pQVar27 = *(QArrayData **)(lVar4 + 0x18);
            goto LAB_00129271;
          }
        }
        QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
        if (*(QMapNode<QString,QVariant> **)(lVar4 + 8) != (QMapNode<QString,QVariant> *)0x0) {
          QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar4 + 8));
        }
        if (*(QMapNode<QString,QVariant> **)(lVar4 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
          QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar4 + 0x10))
          ;
        }
      }
      QMapDataBase::freeTree(local_70,(int)*(undefined8 *)(local_70 + 0x10));
    }
    QMapDataBase::freeData((QMapDataBase *)local_70);
    uVar15 = *(uint *)local_160;
    if (uVar15 != 0) goto LAB_00129015;
LAB_001292cf:
    QArrayData::deallocate(local_160,4,8);
  }
  else {
    if (*(int *)local_70 != -1) {
      LOCK();
      *(int *)local_70 = *(int *)local_70 + -1;
      UNLOCK();
      if (*(int *)local_70 == 0) goto LAB_001291c0;
    }
    uVar15 = *(uint *)local_160;
    if (uVar15 == 0) goto LAB_001292cf;
LAB_00129015:
    if (uVar15 != 0xffffffff) {
      LOCK();
      *(uint *)local_160 = *(uint *)local_160 - 1;
      UNLOCK();
      if (*(uint *)local_160 == 0) goto LAB_001292cf;
    }
  }
  if (*(int *)local_168 == 0) {
LAB_001291a8:
    QArrayData::deallocate(local_168,2,8);
  }
  else if (*(int *)local_168 != -1) {
    LOCK();
    *(int *)local_168 = *(int *)local_168 + -1;
    UNLOCK();
    if (*(int *)local_168 == 0) goto LAB_001291a8;
  }
  pQVar19 = local_b0;
  if (*(int *)local_b0 != 0) {
    if (*(int *)local_b0 == -1) goto LAB_00129071;
    LOCK();
    *(int *)local_b0 = *(int *)local_b0 + -1;
    UNLOCK();
    if (*(int *)local_b0 != 0) goto LAB_00129071;
  }
  lVar4 = *(long *)(local_b0 + 0x10);
  if (lVar4 != 0) {
    pQVar27 = *(QArrayData **)(lVar4 + 0x18);
    if (*(int *)pQVar27 == 0) {
LAB_00129695:
      QArrayData::deallocate(pQVar27,2,8);
    }
    else if (*(int *)pQVar27 != -1) {
      LOCK();
      *(int *)pQVar27 = *(int *)pQVar27 + -1;
      UNLOCK();
      if (*(int *)pQVar27 == 0) {
        pQVar27 = *(QArrayData **)(lVar4 + 0x18);
        goto LAB_00129695;
      }
    }
    QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
    lVar5 = *(long *)(lVar4 + 8);
    if (lVar5 != 0) {
      pQVar27 = *(QArrayData **)(lVar5 + 0x18);
      if (*(int *)pQVar27 == 0) {
LAB_001290f8:
        QArrayData::deallocate(pQVar27,2,8);
      }
      else if (*(int *)pQVar27 != -1) {
        LOCK();
        *(int *)pQVar27 = *(int *)pQVar27 + -1;
        UNLOCK();
        if (*(int *)pQVar27 == 0) {
          pQVar27 = *(QArrayData **)(lVar5 + 0x18);
          goto LAB_001290f8;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
      if (*(QMapNode<QString,QVariant> **)(lVar5 + 8) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar5 + 8));
      }
      if (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar5 + 0x10));
      }
    }
    lVar4 = *(long *)(lVar4 + 0x10);
    if (lVar4 != 0) {
      pQVar27 = *(QArrayData **)(lVar4 + 0x18);
      if (*(int *)pQVar27 == 0) {
LAB_00129151:
        QArrayData::deallocate(pQVar27,2,8);
      }
      else if (*(int *)pQVar27 != -1) {
        LOCK();
        *(int *)pQVar27 = *(int *)pQVar27 + -1;
        UNLOCK();
        if (*(int *)pQVar27 == 0) {
          pQVar27 = *(QArrayData **)(lVar4 + 0x18);
          goto LAB_00129151;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
      if (*(QMapNode<QString,QVariant> **)(lVar4 + 8) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar4 + 8));
      }
      if (*(QMapNode<QString,QVariant> **)(lVar4 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar4 + 0x10));
      }
    }
    QMapDataBase::freeTree((QMapNodeBase *)pQVar19,(int)*(undefined8 *)(pQVar19 + 0x10));
  }
  QMapDataBase::freeData((QMapDataBase *)pQVar19);
LAB_00129071:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return param_1;
}


// ====== paintLine @ 00129b50 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTangentNormalPaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) */

void KisTangentNormalPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  QArrayData *pQVar1;
  int *piVar2;
  int *piVar3;
  long *plVar4;
  long *plVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined *puVar8;
  KoColorSpace *pKVar9;
  char cVar10;
  int iVar11;
  undefined8 *puVar12;
  QString *pQVar13;
  QPointF *pQVar14;
  ulong uVar15;
  undefined8 uVar16;
  QMapData *pQVar17;
  ulong *puVar18;
  KoColorSpace *pKVar19;
  long in_FS_OFFSET;
  undefined auVar20 [16];
  QVector<float> *local_130;
  int local_120;
  QArrayData *local_118;
  QArrayData *local_110;
  QArrayData *local_108;
  QArrayData *local_100;
  KisPainter local_f8 [16];
  double local_e8;
  int *local_e0;
  QArrayData *local_d8;
  int *local_d0;
  KoColorSpace *local_c8;
  undefined local_c0 [40];
  undefined local_98;
  QMapData *local_90;
  KoColor local_88 [56];
  QMap<QString,QVariant> local_50 [12];
  uchar local_44 [4];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  cVar10 = KisCurveOption::isChecked();
  if ((((cVar10 == '\0') || (*(long *)(param_1 + 0x28) == 0)) ||
      (iVar11 = KisBrush::width(), iVar11 != 1)) || (iVar11 = KisBrush::height(), iVar11 != 1)) {
    if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
      KisPaintOp::paintLine(param_1,param_2,param_3);
      return;
    }
    goto LAB_0012a94e;
  }
  if (*(long **)(param_1 + 0x460) == (long *)0x0) {
    local_d8 = *(QArrayData **)(param_1 + 0x448);
    if (local_d8 != (QArrayData *)0x0) {
      LOCK();
      *(int *)(local_d8 + 0x10) = *(int *)(local_d8 + 0x10) + 1;
      UNLOCK();
      plVar5 = *(long **)(param_1 + 0x460);
      *(QArrayData **)(param_1 + 0x460) = local_d8;
      if (plVar5 != (long *)0x0) {
        LOCK();
        plVar4 = plVar5 + 2;
        *(int *)plVar4 = *(int *)plVar4 + -1;
        UNLOCK();
        if (*(int *)plVar4 != 0) goto LAB_00129c04;
        (**(code **)(*plVar5 + 0x20))();
        local_d8 = *(QArrayData **)(param_1 + 0x460);
        goto LAB_00129c0b;
      }
      goto LAB_00129c17;
    }
    local_d8 = (QArrayData *)0x0;
  }
  else {
    (**(code **)(**(long **)(param_1 + 0x460) + 0x68))();
LAB_00129c04:
    local_d8 = *(QArrayData **)(param_1 + 0x460);
LAB_00129c0b:
    if (local_d8 != (QArrayData *)0x0) {
LAB_00129c17:
      LOCK();
      *(int *)(local_d8 + 0x10) = *(int *)(local_d8 + 0x10) + 1;
      UNLOCK();
    }
  }
                    /* try { // try from 00129c3b to 00129c3f has its CatchHandler @ 0012a98f */
  KisPainter::KisPainter(local_f8);
  if (local_d8 != (QArrayData *)0x0) {
    LOCK();
    pQVar1 = local_d8 + 0x10;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (*(int *)pQVar1 == 0) {
      (**(code **)(*(long *)local_d8 + 0x20))();
    }
  }
                    /* try { // try from 00129c63 to 00129c6f has its CatchHandler @ 0012a983 */
  KisPaintOp::painter();
  puVar12 = (undefined8 *)KisPainter::paintColor();
  local_c8 = (KoColorSpace *)*puVar12;
  local_98 = *(undefined *)(puVar12 + 6);
  local_90 = (QMapData *)puVar12[7];
  if (*(int *)local_90 == 0) {
                    /* try { // try from 0012a71b to 0012a75e has its CatchHandler @ 0012a983 */
    pQVar17 = (QMapData *)QMapDataBase::createData();
    local_90 = pQVar17;
    if (*(QMapNode<QString,QVariant> **)(puVar12[7] + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
      puVar18 = (ulong *)QMapNode<QString,QVariant>::copy
                                   (*(QMapNode<QString,QVariant> **)(puVar12[7] + 0x10),pQVar17);
      uVar15 = *puVar18;
      *(ulong **)(pQVar17 + 0x10) = puVar18;
      *puVar18 = (ulong)((uint)uVar15 & 3) | (ulong)(pQVar17 + 8);
      QMapDataBase::recalcMostLeftNode();
    }
  }
  else if (*(int *)local_90 != -1) {
    LOCK();
    *(int *)local_90 = *(int *)local_90 + 1;
    UNLOCK();
    local_90 = (QMapData *)puVar12[7];
  }
  __memcpy_chk(local_c0,puVar12 + 1,local_98,0x38);
                    /* try { // try from 00129cda to 00129cdc has its CatchHandler @ 0012a953 */
  (**(code **)(*(long *)local_c8 + 0x68))(&local_d8);
                    /* try { // try from 00129cf1 to 00129cf5 has its CatchHandler @ 0012a977 */
  KoID::id();
  piVar3 = local_d0;
  if (local_d0 != (int *)0x0) {
    LOCK();
    piVar2 = local_d0 + 1;
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (*piVar2 == 0) {
      (**(code **)(local_d0 + 2))(local_d0);
    }
    LOCK();
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      operator_delete(piVar3,0x10);
    }
  }
                    /* try { // try from 00129d1c to 00129d20 has its CatchHandler @ 0012aa13 */
  pQVar13 = (QString *)KoColorSpaceRegistry::instance();
  puVar8 = PTR_shared_null_0016efc8;
  local_d8 = (QArrayData *)PTR_shared_null_0016efc8;
                    /* try { // try from 00129d39 to 00129d3d has its CatchHandler @ 0012a9fb */
  KoColorSpaceRegistry::rgb8(pQVar13);
  if (*(int *)local_d8 == 0) {
LAB_0012a61f:
    QArrayData::deallocate(local_d8,2,8);
  }
  else if (*(int *)local_d8 != -1) {
    LOCK();
    *(int *)local_d8 = *(int *)local_d8 + -1;
    UNLOCK();
    if (*(int *)local_d8 == 0) goto LAB_0012a61f;
  }
                    /* try { // try from 00129d85 to 00129d89 has its CatchHandler @ 0012aa13 */
  iVar11 = QString::compare_helper
                     ((QChar *)(local_118 + *(long *)(local_118 + 0x10)),*(int *)(local_118 + 4),
                      "RGBA",-1,1);
  pKVar19 = local_c8;
  if (iVar11 != 0) {
                    /* try { // try from 0012a764 to 0012a768 has its CatchHandler @ 0012aa13 */
    pQVar13 = (QString *)KoColorSpaceRegistry::instance();
    local_d8 = (QArrayData *)puVar8;
                    /* try { // try from 0012a77a to 0012a77e has its CatchHandler @ 0012a95f */
    pKVar19 = (KoColorSpace *)KoColorSpaceRegistry::rgb8(pQVar13);
    if (*(int *)local_d8 == 0) {
LAB_0012a7a9:
      QArrayData::deallocate(local_d8,2,8);
    }
    else if (*(int *)local_d8 != -1) {
      LOCK();
      *(int *)local_d8 = *(int *)local_d8 + -1;
      UNLOCK();
      if (*(int *)local_d8 == 0) goto LAB_0012a7a9;
    }
  }
  local_110 = (QArrayData *)QArrayData::allocate(4,8,4,0);
  if (local_110 == (QArrayData *)0x0) {
    qBadAlloc();
  }
  pKVar9 = local_c8;
  *(uint *)(local_110 + 4) = 4;
  *(undefined (*) [16])(local_110 + *(long *)(local_110 + 0x10)) = (undefined  [16])0x0;
                    /* try { // try from 00129def to 00129df1 has its CatchHandler @ 0012a9ef */
  (**(code **)(*(long *)local_c8 + 0x70))(&local_e8,local_c8);
                    /* try { // try from 00129e0a to 00129e0e has its CatchHandler @ 0012a9e3 */
  KoID::id();
                    /* try { // try from 00129e2f to 00129e33 has its CatchHandler @ 0012a9d7 */
  iVar11 = QString::compare_helper
                     ((QChar *)(local_108 + *(long *)(local_108 + 0x10)),*(int *)(local_108 + 4),
                      "F16",-1,1);
  if (iVar11 != 0) {
                    /* try { // try from 00129e49 to 00129e4b has its CatchHandler @ 0012a96b */
    (**(code **)(*(long *)pKVar9 + 0x70))(&local_d8,pKVar9);
                    /* try { // try from 00129e5d to 00129e61 has its CatchHandler @ 0012a9bf */
    KoID::id();
                    /* try { // try from 00129e82 to 00129e86 has its CatchHandler @ 0012a9a7 */
    iVar11 = QString::compare_helper
                       ((QChar *)(local_100 + *(long *)(local_100 + 0x10)),*(int *)(local_100 + 4),
                        "F32",-1,1);
    cVar10 = iVar11 == 0;
    if (*(int *)local_100 == 0) {
LAB_0012a80d:
      QArrayData::deallocate(local_100,2,8);
    }
    else if (*(int *)local_100 != -1) {
      LOCK();
      *(int *)local_100 = *(int *)local_100 + -1;
      UNLOCK();
      if (*(int *)local_100 == 0) goto LAB_0012a80d;
    }
    if (local_d0 != (int *)0x0) {
      LOCK();
      piVar3 = local_d0 + 1;
      *piVar3 = *piVar3 + -1;
      UNLOCK();
      if (*piVar3 == 0) {
        (**(code **)(local_d0 + 2))(local_d0);
      }
      LOCK();
      *local_d0 = *local_d0 + -1;
      UNLOCK();
      if (*local_d0 == 0) {
        operator_delete(local_d0,0x10);
      }
    }
  }
  if (*(int *)local_108 == 0) {
LAB_0012a60b:
    QArrayData::deallocate(local_108,2,8);
  }
  else if (*(int *)local_108 != -1) {
    LOCK();
    *(int *)local_108 = *(int *)local_108 + -1;
    UNLOCK();
    if (*(int *)local_108 == 0) goto LAB_0012a60b;
  }
  if (local_e0 != (int *)0x0) {
    LOCK();
    piVar3 = local_e0 + 1;
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      (**(code **)(local_e0 + 2))(local_e0);
    }
    LOCK();
    *local_e0 = *local_e0 + -1;
    UNLOCK();
    if (*local_e0 == 0) {
      operator_delete(local_e0,0x10);
    }
  }
  if (cVar10 == '\0') {
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc
                  ((QVector<float> *)&local_110,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    uVar6 = _DAT_00154234;
    local_130 = (QVector<float> *)&local_110;
    *(undefined4 *)(local_110 + *(long *)(local_110 + 0x10)) = _DAT_00154234;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    uVar7 = DAT_00154230;
    *(undefined4 *)(local_110 + *(long *)(local_110 + 0x10) + 4) = DAT_00154230;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(undefined4 *)(local_110 + *(long *)(local_110 + 0x10) + 8) = uVar7;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(undefined4 *)(local_110 + *(long *)(local_110 + 0x10) + 0xc) = uVar6;
                    /* try { // try from 0012a4db to 0012a70b has its CatchHandler @ 0012aa07 */
    KisTangentTiltOption::apply
              ((KisTangentTiltOption *)(param_1 + 0x1a0),(KisPaintInformation *)param_3,
               (double *)&local_108,(double *)&local_100,&local_e8);
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_110 + *(long *)(local_110 + 0x10)) = (float)local_e8;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_110 + *(long *)(local_110 + 0x10) + 4) = (float)(double)local_100;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_110 + *(long *)(local_110 + 0x10) + 8) = (float)(double)local_108;
  }
  else {
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc
                  ((QVector<float> *)&local_110,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    uVar6 = DAT_00154230;
    local_130 = (QVector<float> *)&local_110;
    *(undefined4 *)(local_110 + *(long *)(local_110 + 0x10)) = DAT_00154230;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(undefined4 *)(local_110 + *(long *)(local_110 + 0x10) + 4) = uVar6;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    uVar6 = _DAT_00154234;
    *(undefined4 *)(local_110 + *(long *)(local_110 + 0x10) + 8) = _DAT_00154234;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
                    /* try { // try from 0012a873 to 0012a910 has its CatchHandler @ 0012aa07 */
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(undefined4 *)(local_110 + *(long *)(local_110 + 0x10) + 0xc) = uVar6;
                    /* try { // try from 0012a093 to 0012a1c1 has its CatchHandler @ 0012aa07 */
    KisTangentTiltOption::apply
              ((KisTangentTiltOption *)(param_1 + 0x1a0),(KisPaintInformation *)param_3,
               (double *)&local_108,(double *)&local_100,&local_e8);
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_110 + *(long *)(local_110 + 0x10)) = (float)(double)local_108;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_110 + *(long *)(local_110 + 0x10) + 4) = (float)(double)local_100;
    if (1 < *(uint *)local_110) {
      if ((*(uint *)(local_110 + 8) & 0x7fffffff) == 0) {
        local_110 = (QArrayData *)QArrayData::allocate(4,8,0,2);
      }
      else {
        QVector<float>::realloc(local_130,*(uint *)(local_110 + 8) & 0x7fffffff,0);
      }
    }
    *(float *)(local_110 + *(long *)(local_110 + 0x10) + 8) = (float)local_e8;
  }
  local_130 = (QVector<float> *)&local_110;
  (**(code **)(*(long *)pKVar19 + 0x50))(pKVar19,local_44,local_130);
  KoColor::KoColor(local_88,local_44,pKVar19);
                    /* try { // try from 0012a1cf to 0012a21f has its CatchHandler @ 0012a9cb */
  KisPainter::setPaintColor((KoColor *)local_f8);
  KisPaintInformation::pos();
  pQVar14 = (QPointF *)KisPaintInformation::pos();
  KisPainter::drawDDALine((QPointF *)local_f8,pQVar14);
  auVar20 = KisPaintDevice::extent();
  uVar15 = auVar20._0_8_;
  uVar16 = KisPaintOp::painter();
  local_120 = auVar20._0_4_;
  local_d8 = *(QArrayData **)(param_1 + 0x460);
  if (local_d8 != (QArrayData *)0x0) {
    LOCK();
    *(int *)(local_d8 + 0x10) = *(int *)(local_d8 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 0012a272 to 0012a276 has its CatchHandler @ 0012a9b3 */
  KisPainter::bitBlt(uVar16,uVar15 & 0xffffffff,(long)uVar15 >> 0x20 & 0xffffffff,&local_d8,
                     uVar15 & 0xffffffff,(long)uVar15 >> 0x20 & 0xffffffff,
                     (auVar20._8_4_ - local_120) + 1);
  if (local_d8 != (QArrayData *)0x0) {
    LOCK();
    pQVar1 = local_d8 + 0x10;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (*(int *)pQVar1 == 0) {
      (**(code **)(*(long *)local_d8 + 0x20))();
    }
  }
                    /* try { // try from 0012a29b to 0012a29f has its CatchHandler @ 0012a9cb */
  uVar16 = KisPaintOp::painter();
  local_d8 = *(QArrayData **)(param_1 + 0x460);
  if (local_d8 != (QArrayData *)0x0) {
    LOCK();
    *(int *)(local_d8 + 0x10) = *(int *)(local_d8 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 0012a2cc to 0012a2d0 has its CatchHandler @ 0012a99b */
  KisPainter::renderMirrorMask(uVar16,uVar15,auVar20._8_8_,&local_d8);
  if (local_d8 != (QArrayData *)0x0) {
    LOCK();
    pQVar1 = local_d8 + 0x10;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (*(int *)pQVar1 == 0) {
      (**(code **)(*(long *)local_d8 + 0x20))();
    }
  }
  QMap<QString,QVariant>::~QMap(local_50);
  if (*(int *)local_110 == 0) {
LAB_0012a5f7:
    QArrayData::deallocate(local_110,4,8);
  }
  else if (*(int *)local_110 != -1) {
    LOCK();
    *(int *)local_110 = *(int *)local_110 + -1;
    UNLOCK();
    if (*(int *)local_110 == 0) goto LAB_0012a5f7;
  }
  if (*(int *)local_118 == 0) {
LAB_0012a5e3:
    QArrayData::deallocate(local_118,2,8);
  }
  else if (*(int *)local_118 != -1) {
    LOCK();
    *(int *)local_118 = *(int *)local_118 + -1;
    UNLOCK();
    if (*(int *)local_118 == 0) goto LAB_0012a5e3;
  }
  QMap<QString,QVariant>::~QMap((QMap<QString,QVariant> *)&local_90);
  KisPainter::~KisPainter(local_f8);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_0012a94e:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintBezierCurve @ 00170490 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 00170a50 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


