/* Painting functions extracted from kritaroundmarkerpaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintAt @ 00112c42 ======

/* KisRoundMarkerOp::paintAt(KisPaintInformation const&) [clone .cold] */

void KisRoundMarkerOp::paintAt(KisPaintInformation *param_1)

{
  QArrayData *pQVar1;
  int iVar2;
  long lVar3;
  undefined8 uVar4;
  long lVar5;
  QVector *pQVar6;
  QArrayData *pQVar7;
  KisPaintInformation *unaff_RBX;
  undefined (*unaff_RBP) [16];
  double *pdVar8;
  KisRoundMarkerOp *unaff_R14;
  double *pdVar9;
  long lVar10;
  long in_FS_OFFSET;
  QPointF *unaff_retaddr;
  double in_stack_00000008;
  double dStack0000000000000010;
  double dStack0000000000000018;
  double in_stack_00000030;
  double in_stack_00000038;
  int *in_stack_00000040;
  double in_stack_00000060;
  double in_stack_00000068;
  QArrayData *in_stack_00000070;
  double in_stack_00000080;
  double in_stack_00000088;
  double dStack0000000000000090;
  double dStack0000000000000098;
  QArrayData *in_stack_000000a0;
  double *pdStack00000000000000a8;
  double *pdStack00000000000000b0;
  undefined4 uStack00000000000000b8;
  undefined4 in_stack_000000bc;
  undefined8 in_stack_000000c0;
  long in_stack_000000c8;
  
                    /* try { // try from 00112c42 to 00112c50 has its CatchHandler @ 00112c8b */
  qBadAlloc();
  lVar3 = *(long *)(in_stack_00000040 + 4);
  if ((in_stack_00000040[2] & 0x7fffffffU) == 0) {
    lVar10 = (long)in_stack_00000040[1] << 4;
  }
  else {
    iVar2 = *(int *)(in_stack_00000070 + 4);
    pQVar7 = in_stack_00000070 + *(long *)(in_stack_00000070 + 0x10);
    lVar10 = (long)iVar2 * 0x10;
    pQVar1 = pQVar7 + lVar10;
    lVar5 = lVar3 - (long)pQVar7;
    for (; pQVar7 != pQVar1; pQVar7 = pQVar7 + 0x10) {
      uVar4 = *(undefined8 *)(pQVar7 + 8);
      *(undefined8 *)(pQVar7 + (long)in_stack_00000040 + lVar5) = *(undefined8 *)pQVar7;
      *(undefined8 *)(pQVar7 + (long)in_stack_00000040 + lVar5 + 8) = uVar4;
    }
    in_stack_00000040[1] = iVar2;
  }
  pdVar8 = (double *)(lVar3 + (long)in_stack_00000040);
  pdVar9 = (double *)(lVar10 + (long)pdVar8);
  uStack00000000000000b8 = 1;
  pdStack00000000000000b0 = pdVar9;
  dStack0000000000000010 = DAT_0013f700;
  dStack0000000000000018 = DAT_0013f700;
  for (; pdStack00000000000000a8 = pdVar8, pdVar8 != pdVar9; pdVar8 = pdVar8 + 2) {
    in_stack_00000080 = *pdVar8 + dStack0000000000000010;
    in_stack_00000088 = pdVar8[1] + dStack0000000000000018;
    KisMarkerPainter::fillFullCircle(unaff_retaddr,in_stack_00000008);
  }
  if (*in_stack_00000040 == 0) {
LAB_0011e750:
    QArrayData::deallocate(in_stack_000000a0,0x10,8);
  }
  else if (*in_stack_00000040 != -1) {
    LOCK();
    *in_stack_00000040 = *in_stack_00000040 + -1;
    UNLOCK();
    if (*in_stack_00000040 == 0) goto LAB_0011e750;
  }
  if (*(int *)in_stack_00000070 == 0) {
LAB_0011e738:
    QArrayData::deallocate(in_stack_00000070,0x10,8);
  }
  else if (*(int *)in_stack_00000070 != -1) {
    LOCK();
    *(int *)in_stack_00000070 = *(int *)in_stack_00000070 + -1;
    UNLOCK();
    if (*(int *)in_stack_00000070 == 0) goto LAB_0011e738;
  }
  unaff_RBX[0x20] = (KisPaintInformation)0x0;
  *(double *)(unaff_RBX + 0xb8) = in_stack_00000008;
  *(double *)(unaff_RBX + 0xa8) = in_stack_00000060;
  *(double *)(unaff_RBX + 0xb0) = in_stack_00000068;
  in_stack_00000080 = (in_stack_00000060 - in_stack_00000008) + DAT_0013f708;
  in_stack_00000088 = (in_stack_00000068 - in_stack_00000008) + DAT_0013f708;
  dStack0000000000000090 =
       in_stack_00000008 + in_stack_00000008 + in_stack_00000038 + in_stack_00000038;
  dStack0000000000000098 = dStack0000000000000090;
  KisPaintOp::painter();
  _in_stack_000000a0 = QRectF::toAlignedRect();
  KisPainter::calculateAllMirroredRects((QRect *)&stack0x00000070);
  pQVar6 = (QVector *)KisPaintOp::painter();
  KisPainter::addDirtyRects(pQVar6);
  computeSpacing(unaff_R14,unaff_RBX,in_stack_00000030);
  if (unaff_RBX[0x20] != (KisPaintInformation)0x0) {
    unaff_RBX[0x20] = (KisPaintInformation)0x0;
  }
  *(undefined8 *)unaff_RBP[2] = in_stack_000000c0;
  *unaff_RBP = _in_stack_000000a0;
  *(double **)unaff_RBP[1] = pdStack00000000000000b0;
  *(ulong *)(unaff_RBP[1] + 8) = CONCAT44(in_stack_000000bc,uStack00000000000000b8);
  if (*(int *)in_stack_00000070 != 0) {
    if (*(int *)in_stack_00000070 == -1) goto LAB_0011e4cd;
    LOCK();
    *(int *)in_stack_00000070 = *(int *)in_stack_00000070 + -1;
    UNLOCK();
    if (*(int *)in_stack_00000070 != 0) goto LAB_0011e4cd;
  }
  QArrayData::deallocate(in_stack_00000070,0x10,8);
LAB_0011e4cd:
  KisMarkerPainter::~KisMarkerPainter((KisMarkerPainter *)unaff_retaddr);
  if (in_stack_000000c8 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}


// ====== paintAt @ 0011e0c0 ======

/* KisRoundMarkerOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisRoundMarkerOp::paintAt(KisPaintInformation *param_1)

{
  long *plVar1;
  QArrayData *pQVar2;
  double dVar3;
  double dVar4;
  char cVar5;
  int iVar6;
  double *pdVar7;
  undefined8 uVar8;
  QVector *pQVar9;
  QArrayData *pQVar10;
  bool in_DL;
  KisPaintInformation *in_RSI;
  long lVar11;
  QArrayData *pQVar12;
  long lVar13;
  QArrayData *pQVar14;
  long in_FS_OFFSET;
  double dVar15;
  double dVar16;
  QArrayData *local_c8;
  KisMarkerPainter local_b8 [8];
  QArrayData *local_b0;
  double local_a8;
  double dStack_a0;
  QArrayData *local_98;
  double dStack_90;
  long *local_88;
  double dStack_80;
  double local_78;
  double dStack_70;
  undefined local_68 [16];
  QArrayData *local_58;
  double dStack_50;
  undefined8 local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisPaintOp::painter();
  KisPainter::device();
  dVar4 = DAT_0013f710;
  if ((long *)local_68._0_8_ == (long *)0x0) {
    *(undefined8 *)param_1 = 1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(double *)(param_1 + 8) = dVar4;
    *(double *)(param_1 + 0x10) = dVar4;
    goto LAB_0011e524;
  }
  LOCK();
  plVar1 = (long *)(local_68._0_8_ + 0x10);
  *(int *)plVar1 = *(int *)plVar1 + -1;
  UNLOCK();
  if (*(int *)plVar1 == 0) {
    (**(code **)(*(long *)local_68._0_8_ + 0x20))();
  }
  KisPaintOp::painter();
  KisPainter::device();
                    /* try { // try from 0011e150 to 0011e154 has its CatchHandler @ 0011e9e0 */
  KisPaintDevice::defaultBounds();
                    /* try { // try from 0011e160 to 0011e162 has its CatchHandler @ 0011e9bc */
  iVar6 = (**(code **)(*(long *)local_68._0_8_ + 0x30))();
  dVar4 = DAT_0013f710;
  dVar16 = DAT_0013f710;
  if (0 < iVar6) {
    dVar16 = DAT_0013f710 / (double)(1 << ((byte)iVar6 & 0x1f));
  }
  if ((long *)local_68._0_8_ != (long *)0x0) {
    LOCK();
    plVar1 = (long *)(local_68._0_8_ + 8);
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*(long *)local_68._0_8_ + 8))();
    }
  }
  if (local_88 != (long *)0x0) {
    LOCK();
    plVar1 = local_88 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_88 + 0x20))();
    }
  }
  cVar5 = KisCurveOption::isChecked();
  if (cVar5 != '\0') {
    dVar15 = (double)KisCurveOption::computeSizeLikeValue(in_RSI + 0x30,in_DL);
    dVar16 = dVar16 * dVar15;
  }
  dVar15 = *(double *)(in_RSI + 0xc0) * dVar16;
  if (dVar16 * dVar15 < DAT_0013f6f0) {
    *(undefined8 *)param_1 = 1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(undefined (*) [16])(param_1 + 8) = (undefined  [16])0x0;
    goto LAB_0011e524;
  }
  pdVar7 = (double *)KisPaintInformation::pos();
  local_a8 = *pdVar7;
  dStack_a0 = pdVar7[1];
  KisPaintOp::painter();
  uVar8 = KisPainter::paintColor();
  KisPaintOp::painter();
  KisPainter::device();
                    /* try { // try from 0011e267 to 0011e26b has its CatchHandler @ 0011ea04 */
  KisMarkerPainter::KisMarkerPainter(local_b8,(KisRoundMarkerOp *)local_68,uVar8);
  if ((long *)local_68._0_8_ != (long *)0x0) {
    LOCK();
    plVar1 = (long *)(local_68._0_8_ + 0x10);
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*(long *)local_68._0_8_ + 0x20))();
    }
  }
  dVar16 = dVar15 * DAT_0013f6f8;
  if (in_RSI[0x20] == (KisPaintInformation)0x0) {
                    /* try { // try from 0011e580 to 0011e5ba has its CatchHandler @ 0011e9f8 */
    KisPaintOp::painter();
    local_68._0_8_ = *(undefined8 *)(in_RSI + 0xa8);
    local_68._8_8_ = *(undefined8 *)(in_RSI + 0xb0);
    local_58 = (QArrayData *)local_a8;
    dStack_50 = dStack_a0;
    KisPainter::calculateAllMirroredPoints((QPair *)&local_b0);
    if (*(int *)local_b0 == 0) {
      if ((char)local_b0[0xb] < '\0') {
        local_c8 = (QArrayData *)
                   QArrayData::allocate(0x20,8,(ulong)(*(uint *)(local_b0 + 8) & 0x7fffffff),0);
        local_68._0_8_ = local_c8;
        if (local_c8 == (QArrayData *)0x0) {
          qBadAlloc();
        }
        local_c8[0xb] = (QArrayData)((byte)local_c8[0xb] | 0x80);
      }
      else {
        local_c8 = (QArrayData *)QArrayData::allocate(0x20,8,(long)*(int *)(local_b0 + 4),0);
        local_68._0_8_ = local_c8;
        if (local_c8 == (QArrayData *)0x0) {
          qBadAlloc();
        }
      }
      if ((*(uint *)(local_c8 + 8) & 0x7fffffff) == 0) {
        pQVar12 = local_c8 + *(long *)(local_c8 + 0x10);
        lVar13 = (long)*(int *)(local_c8 + 4) << 5;
      }
      else {
        pQVar12 = local_c8 + *(long *)(local_c8 + 0x10);
        iVar6 = *(int *)(local_b0 + 4);
        pQVar10 = local_b0 + *(long *)(local_b0 + 0x10);
        lVar13 = (long)iVar6 * 0x20;
        pQVar14 = pQVar10 + lVar13;
        lVar11 = (long)pQVar12 - (long)pQVar10;
        for (; pQVar10 != pQVar14; pQVar10 = pQVar10 + 0x20) {
          pQVar2 = pQVar10 + lVar11;
          uVar8 = *(undefined8 *)(pQVar10 + 8);
          *(undefined8 *)pQVar2 = *(undefined8 *)pQVar10;
          *(undefined8 *)(pQVar2 + 8) = uVar8;
          uVar8 = *(undefined8 *)(pQVar10 + 0x18);
          *(undefined8 *)(pQVar2 + 0x10) = *(undefined8 *)(pQVar10 + 0x10);
          *(undefined8 *)(pQVar2 + 0x18) = uVar8;
        }
        *(int *)(local_c8 + 4) = iVar6;
      }
    }
    else {
      if (*(int *)local_b0 != -1) {
        LOCK();
        *(int *)local_b0 = *(int *)local_b0 + 1;
        UNLOCK();
      }
      local_c8 = local_b0;
      local_68._0_8_ = local_b0;
      lVar13 = (long)*(int *)(local_b0 + 4) << 5;
      pQVar12 = local_b0 + *(long *)(local_b0 + 0x10);
    }
    dVar3 = DAT_0013f700;
    pQVar14 = pQVar12 + lVar13;
    dStack_50 = (double)CONCAT44(dStack_50._4_4_,1);
    local_58 = pQVar14;
    for (; local_68._8_8_ = pQVar12, pQVar12 != pQVar14; pQVar12 = pQVar12 + 0x20) {
      local_88 = (long *)(*(double *)(pQVar12 + 0x10) + dVar3);
      dStack_80 = *(double *)(pQVar12 + 0x18) + dVar3;
      local_98 = (QArrayData *)(*(double *)pQVar12 + dVar3);
      dStack_90 = *(double *)(pQVar12 + 8) + dVar3;
                    /* try { // try from 0011e672 to 0011e676 has its CatchHandler @ 0011e9d4 */
      KisMarkerPainter::fillCirclesDiff
                ((QPointF *)local_b8,*(double *)(in_RSI + 0xb8),(QPointF *)&local_98,dVar16);
    }
    if (*(int *)local_c8 == 0) {
LAB_0011e718:
      QArrayData::deallocate((QArrayData *)local_68._0_8_,0x20,8);
    }
    else if (*(int *)local_c8 != -1) {
      LOCK();
      *(int *)local_c8 = *(int *)local_c8 + -1;
      UNLOCK();
      if (*(int *)local_c8 == 0) goto LAB_0011e718;
    }
    if (*(int *)local_b0 == 0) {
LAB_0011e6d0:
      QArrayData::deallocate(local_b0,0x20,8);
    }
    else if (*(int *)local_b0 != -1) {
      LOCK();
      *(int *)local_b0 = *(int *)local_b0 + -1;
      UNLOCK();
      if (*(int *)local_b0 == 0) goto LAB_0011e6d0;
    }
  }
  else {
                    /* try { // try from 0011e2a5 to 0011e2c3 has its CatchHandler @ 0011e9f8 */
    KisPaintOp::painter();
    KisPainter::calculateAllMirroredPoints((QPointF *)&local_98);
    if (*(int *)local_98 == 0) {
      if ((char)local_98[0xb] < '\0') {
        local_c8 = (QArrayData *)
                   QArrayData::allocate(0x10,8,(ulong)(*(uint *)(local_98 + 8) & 0x7fffffff),0);
        local_68._0_8_ = local_c8;
        if (local_c8 == (QArrayData *)0x0) {
          qBadAlloc();
        }
        local_c8[0xb] = (QArrayData)((byte)local_c8[0xb] | 0x80);
      }
      else {
        local_c8 = (QArrayData *)QArrayData::allocate(0x10,8,(long)*(int *)(local_98 + 4),0);
        local_68._0_8_ = local_c8;
        if (local_c8 == (QArrayData *)0x0) {
          qBadAlloc();
        }
      }
      if ((*(uint *)(local_c8 + 8) & 0x7fffffff) == 0) {
        pQVar12 = local_c8 + *(long *)(local_c8 + 0x10);
        lVar13 = (long)*(int *)(local_c8 + 4) << 4;
      }
      else {
        pQVar12 = local_c8 + *(long *)(local_c8 + 0x10);
        iVar6 = *(int *)(local_98 + 4);
        pQVar10 = local_98 + *(long *)(local_98 + 0x10);
        lVar13 = (long)iVar6 * 0x10;
        pQVar14 = pQVar10 + lVar13;
        lVar11 = (long)pQVar12 - (long)pQVar10;
        for (; pQVar10 != pQVar14; pQVar10 = pQVar10 + 0x10) {
          uVar8 = *(undefined8 *)(pQVar10 + 8);
          *(undefined8 *)(pQVar10 + lVar11) = *(undefined8 *)pQVar10;
          *(undefined8 *)(pQVar10 + lVar11 + 8) = uVar8;
        }
        *(int *)(local_c8 + 4) = iVar6;
      }
    }
    else {
      if (*(int *)local_98 != -1) {
        LOCK();
        *(int *)local_98 = *(int *)local_98 + 1;
        UNLOCK();
      }
      local_c8 = local_98;
      local_68._0_8_ = local_98;
      lVar13 = (long)*(int *)(local_98 + 4) << 4;
      pQVar12 = local_98 + *(long *)(local_98 + 0x10);
    }
    dVar3 = DAT_0013f700;
    pQVar14 = pQVar12 + lVar13;
    dStack_50 = (double)CONCAT44(dStack_50._4_4_,1);
    local_58 = pQVar14;
    for (; local_68._8_8_ = pQVar12, pQVar12 != pQVar14; pQVar12 = pQVar12 + 0x10) {
      local_88 = (long *)(*(double *)pQVar12 + dVar3);
      dStack_80 = *(double *)(pQVar12 + 8) + dVar3;
                    /* try { // try from 0011e351 to 0011e355 has its CatchHandler @ 0011e9c8 */
      KisMarkerPainter::fillFullCircle((QPointF *)local_b8,dVar16);
    }
    if (*(int *)local_c8 == 0) {
LAB_0011e750:
      QArrayData::deallocate((QArrayData *)local_68._0_8_,0x10,8);
    }
    else if (*(int *)local_c8 != -1) {
      LOCK();
      *(int *)local_c8 = *(int *)local_c8 + -1;
      UNLOCK();
      if (*(int *)local_c8 == 0) goto LAB_0011e750;
    }
    if (*(int *)local_98 == 0) {
LAB_0011e738:
      QArrayData::deallocate(local_98,0x10,8);
    }
    else if (*(int *)local_98 != -1) {
      LOCK();
      *(int *)local_98 = *(int *)local_98 + -1;
      UNLOCK();
      if (*(int *)local_98 == 0) goto LAB_0011e738;
    }
  }
  in_RSI[0x20] = (KisPaintInformation)0x0;
  *(double *)(in_RSI + 0xb8) = dVar16;
  *(double *)(in_RSI + 0xa8) = local_a8;
  *(double *)(in_RSI + 0xb0) = dStack_a0;
  local_88 = (long *)((local_a8 - dVar16) + DAT_0013f708);
  dStack_80 = (dStack_a0 - dVar16) + DAT_0013f708;
  local_78 = dVar16 + dVar16 + dVar4 + dVar4;
  dStack_70 = local_78;
                    /* try { // try from 0011e41e to 0011e450 has its CatchHandler @ 0011e9f8 */
  KisPaintOp::painter();
  local_68 = QRectF::toAlignedRect();
  KisPainter::calculateAllMirroredRects((QRect *)&local_98);
                    /* try { // try from 0011e454 to 0011e479 has its CatchHandler @ 0011e9ec */
  pQVar9 = (QVector *)KisPaintOp::painter();
  KisPainter::addDirtyRects(pQVar9);
  computeSpacing((KisRoundMarkerOp *)local_68,in_RSI,dVar15);
  if (in_RSI[0x20] != (KisPaintInformation)0x0) {
    in_RSI[0x20] = (KisPaintInformation)0x0;
  }
  *(undefined8 *)(param_1 + 0x20) = local_48;
  *(undefined (*) [16])param_1 = local_68;
  *(QArrayData **)(param_1 + 0x10) = local_58;
  *(double *)(param_1 + 0x18) = dStack_50;
  if (*(int *)local_98 == 0) {
LAB_0011e770:
    QArrayData::deallocate(local_98,0x10,8);
  }
  else if (*(int *)local_98 != -1) {
    LOCK();
    *(int *)local_98 = *(int *)local_98 + -1;
    UNLOCK();
    if (*(int *)local_98 == 0) goto LAB_0011e770;
  }
  KisMarkerPainter::~KisMarkerPainter(local_b8);
LAB_0011e524:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return param_1;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintBezierCurve @ 00152330 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 00152798 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


