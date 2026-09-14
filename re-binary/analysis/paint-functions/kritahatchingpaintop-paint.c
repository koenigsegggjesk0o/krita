/* Painting functions extracted from kritahatchingpaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintAt @ 00121de2 ======

/* KisHatchingPaintOp::paintAt(KisPaintInformation const&) [clone .cold] */

void __thiscall KisHatchingPaintOp::paintAt(KisHatchingPaintOp *this,KisPaintInformation *param_1)

{
  long unaff_RBP;
  ExternalRefCountData *unaff_R15;
  long in_FS_OFFSET;
  
  __cxa_guard_abort(&paintAt(KisPaintInformation_const&)::cs);
  KisSharedPtr<KisPaintDevice>::deref
            (*(KisSharedPtr **)(unaff_RBP + -0xd0),(KisPaintDevice *)param_1);
  if (unaff_R15 != (ExternalRefCountData *)0x0) {
    QSharedPointer<KisBrush>::deref(unaff_R15);
  }
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 00131c60 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisHatchingPaintOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisHatchingPaintOp::paintAt(KisPaintInformation *param_1)

{
  uint uVar1;
  int *piVar2;
  long *plVar3;
  KisSharedPtr *pKVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  KoColor *pKVar8;
  int iVar9;
  int iVar10;
  KoColorSpace *pKVar11;
  char cVar12;
  int iVar13;
  KisPaintInformation *pKVar14;
  undefined8 *puVar15;
  undefined8 uVar16;
  undefined8 uVar17;
  undefined8 uVar18;
  QMapData *pQVar19;
  ulong uVar20;
  bool in_DL;
  undefined8 extraout_RDX;
  uint uVar21;
  KisHatchingPaintOp *in_RSI;
  KisPaintDevice *pKVar22;
  KisPaintDevice *pKVar23;
  long *plVar24;
  long *plVar25;
  long in_FS_OFFSET;
  ushort in_FPUStatusWord;
  double dVar26;
  double dVar27;
  double dVar28;
  double dVar29;
  undefined auVar30 [16];
  long *local_f8;
  long *local_f0;
  long *local_d8;
  KisSharedPtr *local_d0;
  long *local_c8;
  KisSharedPtr *local_c0;
  undefined8 local_b8;
  undefined8 uStack_b0;
  long *local_a8;
  long *local_a0;
  undefined8 local_98;
  undefined8 local_88;
  undefined local_80 [40];
  undefined local_58;
  QMapData *local_50 [2];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisPaintOp::painter();
  KisPainter::device();
  plVar24 = DAT_00170600;
  if (local_a8 == (long *)0x0) {
    *(undefined8 *)param_1 = 1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(long **)(param_1 + 8) = plVar24;
    *(long **)(param_1 + 0x10) = plVar24;
    goto LAB_00131d92;
  }
  LOCK();
  plVar24 = local_a8 + 2;
  *(int *)plVar24 = *(int *)plVar24 + -1;
  UNLOCK();
  if (*(int *)plVar24 == 0) {
    (**(code **)(*local_a8 + 0x20))();
    plVar24 = *(long **)(in_RSI + 0x1e8);
    if (plVar24 != (long *)0x0) goto LAB_00131cd5;
LAB_00131e06:
    KisPaintOp::source();
                    /* try { // try from 00131e1f to 00131e23 has its CatchHandler @ 0013365d */
    KisPaintDevice::createCompositionSourceDevice();
    plVar24 = *(long **)(in_RSI + 0x1e8);
    plVar25 = plVar24;
    if (local_a8 != plVar24) {
      if (local_a8 != (long *)0x0) {
        LOCK();
        *(int *)(local_a8 + 2) = *(int *)(local_a8 + 2) + 1;
        UNLOCK();
        plVar24 = *(long **)(in_RSI + 0x1e8);
      }
      *(long **)(in_RSI + 0x1e8) = local_a8;
      plVar25 = local_a8;
      if (plVar24 != (long *)0x0) {
        LOCK();
        plVar3 = plVar24 + 2;
        *(int *)plVar3 = *(int *)plVar3 + -1;
        UNLOCK();
        if (*(int *)plVar3 == 0) {
          (**(code **)(*plVar24 + 0x20))();
          plVar25 = local_a8;
        }
      }
    }
    if (plVar25 != (long *)0x0) {
      LOCK();
      plVar24 = plVar25 + 2;
      *(int *)plVar24 = *(int *)plVar24 + -1;
      UNLOCK();
      if (*(int *)plVar24 == 0) {
        (**(code **)(*plVar25 + 0x20))();
      }
    }
    if (local_b8 != (long *)0x0) {
      LOCK();
      plVar24 = local_b8 + 2;
      *(int *)plVar24 = *(int *)plVar24 + -1;
      UNLOCK();
      if (*(int *)plVar24 == 0) {
        (**(code **)(*local_b8 + 0x20))();
      }
    }
  }
  else {
    plVar24 = *(long **)(in_RSI + 0x1e8);
    if (plVar24 == (long *)0x0) goto LAB_00131e06;
LAB_00131cd5:
    (**(code **)(*plVar24 + 0x68))();
  }
  plVar24 = *(long **)(in_RSI + 0x28);
  piVar7 = *(int **)(in_RSI + 0x30);
  if (piVar7 != (int *)0x0) {
    LOCK();
    *piVar7 = *piVar7 + 1;
    UNLOCK();
    LOCK();
    piVar7[1] = piVar7[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 00131cfd to 00131d10 has its CatchHandler @ 0013368d */
  KisPaintOp::painter();
  KisPainter::device();
                    /* try { // try from 00131d28 to 00131d2d has its CatchHandler @ 00133681 */
  if ((plVar24 == (long *)0x0) || (cVar12 = (**(code **)(*plVar24 + 0xe0))(), cVar12 == '\0')) {
LAB_00131d36:
    plVar24 = DAT_00170600;
    *(undefined8 *)param_1 = 1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(long **)(param_1 + 8) = plVar24;
    *(long **)(param_1 + 0x10) = plVar24;
  }
  else {
                    /* try { // try from 00131eb9 to 00131fdc has its CatchHandler @ 00133681 */
    cVar12 = KisCurveOption::isChecked();
    dVar26 = DAT_00170590;
    if (cVar12 != '\0') {
      dVar26 = (double)KisCurveOption::computeSizeLikeValue
                                 ((KisPaintInformation *)(in_RSI + 0x1f0),in_DL);
    }
    *(double *)(*(long *)(in_RSI + 0x1a0) + 0x90) = dVar26;
    cVar12 = KisCurveOption::isChecked();
    dVar26 = DAT_00170590;
    if (cVar12 != '\0') {
      dVar26 = (double)KisCurveOption::computeSizeLikeValue
                                 ((KisPaintInformation *)(in_RSI + 0x228),in_DL);
    }
    *(double *)(*(long *)(in_RSI + 0x1a0) + 0x98) = dVar26;
    cVar12 = KisCurveOption::isChecked();
    dVar26 = DAT_00170590;
    if (cVar12 != '\0') {
      dVar26 = (double)KisCurveOption::computeSizeLikeValue
                                 ((KisPaintInformation *)(in_RSI + 0x260),in_DL);
    }
    *(double *)(*(long *)(in_RSI + 0x1a0) + 0xa0) = dVar26;
    cVar12 = KisCurveOption::isChecked();
    dVar26 = DAT_00170590;
    if (cVar12 != '\0') {
      dVar26 = (double)KisCurveOption::computeSizeLikeValue
                                 ((KisPaintInformation *)(in_RSI + 0x298),in_DL);
    }
    *(double *)(*(long *)(in_RSI + 0x1a0) + 0xa8) = dVar26;
    KisPaintOp::painter();
    KisPainter::device();
                    /* try { // try from 00131fe7 to 00131feb has its CatchHandler @ 001336a5 */
    KisPaintDevice::defaultBounds();
                    /* try { // try from 00131ff6 to 00131ff8 has its CatchHandler @ 00133675 */
    iVar13 = (**(code **)(*local_a8 + 0x30))();
    plVar24 = DAT_00170600;
    if (iVar13 < 1) {
      local_f8 = DAT_00170600;
    }
    else {
      local_f8 = (long *)((double)DAT_00170600 / (double)(1 << ((byte)iVar13 & 0x1f)));
    }
    if (local_a8 != (long *)0x0) {
      LOCK();
      plVar25 = local_a8 + 1;
      *(int *)plVar25 = *(int *)plVar25 + -1;
      UNLOCK();
      if (*(int *)plVar25 == 0) {
        (**(code **)(*local_a8 + 8))();
      }
    }
    if (local_b8 != (long *)0x0) {
      LOCK();
      plVar25 = local_b8 + 2;
      *(int *)plVar25 = *(int *)plVar25 + -1;
      UNLOCK();
      if (*(int *)plVar25 == 0) {
        (**(code **)(*local_b8 + 0x20))();
      }
    }
                    /* try { // try from 00132081 to 00132206 has its CatchHandler @ 00133681 */
    cVar12 = KisCurveOption::isChecked();
    local_f0 = local_f8;
    if (cVar12 != '\0') {
      dVar26 = (double)KisCurveOption::computeSizeLikeValue
                                 ((KisPaintInformation *)(in_RSI + 0x310),in_DL);
      local_f0 = (long *)(dVar26 * (double)local_f8);
    }
    iVar13 = KisBrush::width();
    if (((double)iVar13 * (double)local_f0 <= DAT_00170598) ||
       (iVar13 = KisBrush::height(), (double)iVar13 * (double)local_f0 <= DAT_00170598))
    goto LAB_00131d36;
    local_98 = 0;
    local_a8 = local_f0;
    local_a0 = plVar24;
    pKVar14 = (KisPaintInformation *)KisPaintOp::painter();
    KisOpacityOption::apply((KisPainter *)(in_RSI + 0x2d0),pKVar14);
    if ((paintAt(KisPaintInformation_const&)::cs == '\0') &&
       (iVar13 = __cxa_guard_acquire(&paintAt(KisPaintInformation_const&)::cs), iVar13 != 0)) {
                    /* try { // try from 001329ea to 001329f6 has its CatchHandler @ 00133669 */
      KoColorSpaceRegistry::instance();
      paintAt(KisPaintInformation_const&)::cs = (KoColorSpace *)KoColorSpaceRegistry::alpha8();
      __cxa_guard_release(&paintAt(KisPaintInformation_const&)::cs);
    }
    if ((paintAt(KisPaintInformation_const&)::color == '\0') &&
       (iVar13 = __cxa_guard_acquire(&paintAt(KisPaintInformation_const&)::color),
       pKVar11 = paintAt(KisPaintInformation_const&)::cs, iVar13 != 0)) {
      QColor::QColor((QColor *)&local_88,2);
                    /* try { // try from 00132a4a to 00132a4e has its CatchHandler @ 00133705 */
      KoColor::KoColor((KoColor *)paintAt(KisPaintInformation_const&)::color,(QColor *)&local_88,
                       pKVar11);
      __cxa_atexit(KoColor::~KoColor,paintAt(KisPaintInformation_const&)::color,&__dso_handle);
      __cxa_guard_release(&paintAt(KisPaintInformation_const&)::color);
    }
    pKVar8 = *(KoColor **)(in_RSI + 0x20);
    local_b8 = _DAT_00170620;
    uStack_b0 = _UNK_00170628;
    pKVar14 = (KisPaintInformation *)KisPaintInformation::pos();
    KisDabCache::fetchDab
              ((KoColorSpace *)&local_d0,pKVar8,(QPointF *)paintAt(KisPaintInformation_const&)::cs,
               (KisDabShape *)paintAt(KisPaintInformation_const&)::color,pKVar14,(double)plVar24,
               (QRect *)&local_a8,(double)plVar24);
                    /* try { // try from 00132211 to 001322ce has its CatchHandler @ 0013371d */
    auVar30 = KisFixedPaintDevice::bounds();
    uVar21 = uStack_b0._4_4_ - local_b8._4_4_;
    iVar13 = (int)uStack_b0 - (int)local_b8;
    if ((auVar30._12_4_ - auVar30._4_4_ != uVar21) ||
       (iVar9 = (int)local_b8, iVar10 = local_b8._4_4_, auVar30._8_4_ - auVar30._0_4_ != iVar13)) {
      kis_assert_recoverable
                ("dstRect.size() == maskDab->bounds().size()",
                 "/builds/graphics/krita/plugins/paintops/hatching/kis_hatching_paintop.cpp",0x69);
      iVar13 = (int)uStack_b0 - (int)local_b8;
      uVar21 = uStack_b0._4_4_ - local_b8._4_4_;
      iVar9 = (int)local_b8;
      iVar10 = local_b8._4_4_;
    }
    iVar5 = iVar13 + 1;
    uVar1 = uVar21 + 1;
    if (in_RSI[0x1d9] != (KisHatchingPaintOp)0x0) {
      KisPaintOp::painter();
      puVar15 = (undefined8 *)KisPainter::backgroundColor();
      local_88 = *puVar15;
      local_58 = *(undefined *)(puVar15 + 6);
      local_50[0] = (QMapData *)puVar15[7];
      if (*(int *)local_50[0] == 0) {
                    /* try { // try from 00132989 to 001329e4 has its CatchHandler @ 0013371d */
        pQVar19 = (QMapData *)QMapDataBase::createData();
        local_50[0] = pQVar19;
        if (*(QMapNode<QString,QVariant> **)(puVar15[7] + 0x10) != (QMapNode<QString,QVariant> *)0x0
           ) {
          uVar16 = QMapNode<QString,QVariant>::copy
                             (*(QMapNode<QString,QVariant> **)(puVar15[7] + 0x10),pQVar19);
          *(undefined8 *)(pQVar19 + 0x10) = uVar16;
          **(ulong **)(local_50[0] + 0x10) =
               (ulong)((uint)**(ulong **)(local_50[0] + 0x10) & 3) | (ulong)(local_50[0] + 8);
          QMapDataBase::recalcMostLeftNode();
        }
      }
      else if (*(int *)local_50[0] != -1) {
        LOCK();
        *(int *)local_50[0] = *(int *)local_50[0] + 1;
        UNLOCK();
        local_50[0] = (QMapData *)puVar15[7];
      }
      __memcpy_chk(local_80,puVar15 + 1,local_58,0x38);
                    /* try { // try from 0013233a to 0013233e has its CatchHandler @ 00133729 */
      KisPaintDevice::fill((int)*(undefined8 *)(in_RSI + 0x1e8),0,0,iVar13,(uchar *)(ulong)uVar21);
      QMap<QString,QVariant>::~QMap((QMap<QString,QVariant> *)local_50);
    }
                    /* try { // try from 0013234f to 001323c9 has its CatchHandler @ 0013371d */
    cVar12 = KisCurveOption::isChecked();
    iVar6 = *(int *)(in_RSI + 0x1d0);
    if (cVar12 == '\0') {
      if (iVar6 == 1) {
        uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 00132c2e to 00132c3a has its CatchHandler @ 0013371d */
        KisPaintOp::painter();
        uVar17 = KisPainter::paintColor();
        dVar29 = *(double *)(in_RSI + 0x1a8) + DAT_00170588;
        dVar26 = (double)(DAT_00170578 & -(ulong)(dVar29 < 0.0) |
                         ~-(ulong)(dVar29 < 0.0) & (ulong)plVar24);
        dVar27 = dVar29;
        do {
          dVar27 = dVar27 - (dVar27 / DAT_00170580) * DAT_00170580;
        } while ((in_FPUStatusWord & 0x400) != 0);
        if (NAN(dVar27)) {
          fmod(dVar29,DAT_00170580);
LAB_00132cda:
          dVar26 = 0.0;
        }
        else {
          dVar27 = (double)((ulong)dVar27 & DAT_001705d0);
          if (dVar27 <= DAT_00170588) {
            dVar26 = dVar26 * dVar27;
          }
          else {
            if (DAT_00170580 < dVar27) goto LAB_00132cda;
            dVar26 = dVar26 * (double)((ulong)(DAT_00170580 - dVar27) ^ DAT_001705e0);
          }
        }
        local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
          UNLOCK();
        }
                    /* try { // try from 00132d3b to 00132d3f has its CatchHandler @ 001336bd */
        HatchingBrush::hatch
                  ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,dVar26,local_f8,
                   uVar16,&local_c0,uVar17);
      }
      else if (iVar6 == 2) {
        uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 00132d70 to 00132d7c has its CatchHandler @ 0013371d */
        KisPaintOp::painter();
        uVar17 = KisPainter::paintColor();
        dVar29 = DAT_001705b8;
        dVar28 = *(double *)(in_RSI + 0x1a8) - DAT_001705b8;
        dVar26 = (double)(DAT_00170578 & -(ulong)(dVar28 < 0.0) |
                         ~-(ulong)(dVar28 < 0.0) & (ulong)plVar24);
        dVar27 = dVar28;
        do {
          dVar27 = dVar27 - (dVar27 / DAT_00170580) * DAT_00170580;
        } while ((in_FPUStatusWord & 0x400) != 0);
        if (NAN(dVar27)) {
          fmod(dVar28,DAT_00170580);
LAB_00132e30:
          dVar26 = 0.0;
        }
        else {
          dVar27 = (double)((ulong)dVar27 & DAT_001705d0);
          if (dVar27 <= DAT_00170588) {
            dVar26 = dVar26 * dVar27;
          }
          else {
            if (DAT_00170580 < dVar27) goto LAB_00132e30;
            dVar26 = dVar26 * (double)((ulong)(DAT_00170580 - dVar27) ^ DAT_001705e0);
          }
        }
        local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
          UNLOCK();
        }
                    /* try { // try from 00132ed8 to 00132edc has its CatchHandler @ 001336d5 */
        HatchingBrush::hatch
                  ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,dVar26,local_f8,
                   uVar16,&local_c0,uVar17);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          pKVar4 = local_c0 + 0x10;
          *(int *)pKVar4 = *(int *)pKVar4 + -1;
          UNLOCK();
          if (*(int *)pKVar4 == 0) {
            (**(code **)(*(long *)local_c0 + 0x20))();
          }
        }
        uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 00132f00 to 00132f0c has its CatchHandler @ 0013371d */
        KisPaintOp::painter();
        uVar17 = KisPainter::paintColor();
        dVar29 = dVar29 + *(double *)(in_RSI + 0x1a8);
        dVar26 = (double)(DAT_00170578 & -(ulong)(dVar29 < 0.0) |
                         ~-(ulong)(dVar29 < 0.0) & (ulong)plVar24);
        dVar27 = dVar29;
        do {
          dVar27 = dVar27 - (dVar27 / DAT_00170580) * DAT_00170580;
        } while ((in_FPUStatusWord & 0x400) != 0);
        if (NAN(dVar27)) {
          fmod(dVar29,DAT_00170580);
LAB_00132fb4:
          dVar26 = 0.0;
        }
        else {
          dVar27 = (double)((ulong)dVar27 & DAT_001705d0);
          if (dVar27 <= DAT_00170588) {
            dVar26 = dVar26 * dVar27;
          }
          else {
            if (DAT_00170580 < dVar27) goto LAB_00132fb4;
            dVar26 = dVar26 * (double)((ulong)(DAT_00170580 - dVar27) ^ DAT_001705e0);
          }
        }
        local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
          UNLOCK();
        }
                    /* try { // try from 00133005 to 00133009 has its CatchHandler @ 00133711 */
        HatchingBrush::hatch
                  ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,dVar26,local_f8,
                   uVar16,&local_c0,uVar17);
      }
      else {
        if (iVar6 != 3) {
          if (iVar6 == 4) {
            uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
            KisPaintOp::painter();
            uVar17 = KisPainter::paintColor();
            uVar18 = spinAngle(in_RSI,DAT_001705c8);
            local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
            if (local_c0 != (KisSharedPtr *)0x0) {
              LOCK();
              *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
              UNLOCK();
            }
            pKVar22 = (KisPaintDevice *)&local_c0;
                    /* try { // try from 001326c2 to 001326c6 has its CatchHandler @ 001336e1 */
            HatchingBrush::hatch
                      ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,uVar18,local_f8
                       ,uVar16,pKVar22,uVar17);
            goto LAB_001326c7;
          }
          goto LAB_00132390;
        }
        uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 001331cc to 001331d8 has its CatchHandler @ 0013371d */
        KisPaintOp::painter();
        uVar17 = KisPainter::paintColor();
        dVar29 = DAT_001705b8;
        dVar28 = DAT_001705b8 + *(double *)(in_RSI + 0x1a8);
        dVar26 = (double)(DAT_00170578 & -(ulong)(dVar28 < 0.0) |
                         ~-(ulong)(dVar28 < 0.0) & (ulong)plVar24);
        dVar27 = dVar28;
        do {
          dVar27 = dVar27 - (dVar27 / DAT_00170580) * DAT_00170580;
        } while ((in_FPUStatusWord & 0x400) != 0);
        if (NAN(dVar27)) {
          fmod(dVar28,DAT_00170580);
LAB_0013328c:
          dVar26 = 0.0;
        }
        else {
          dVar27 = (double)((ulong)dVar27 & DAT_001705d0);
          if (dVar27 <= DAT_00170588) {
            dVar26 = dVar26 * dVar27;
          }
          else {
            if (DAT_00170580 < dVar27) goto LAB_0013328c;
            dVar26 = dVar26 * (double)((ulong)(DAT_00170580 - dVar27) ^ DAT_001705e0);
          }
        }
        local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
          UNLOCK();
        }
                    /* try { // try from 00133334 to 00133338 has its CatchHandler @ 00133759 */
        HatchingBrush::hatch
                  ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,dVar26,local_f8,
                   uVar16,&local_c0,uVar17);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          pKVar4 = local_c0 + 0x10;
          *(int *)pKVar4 = *(int *)pKVar4 + -1;
          UNLOCK();
          if (*(int *)pKVar4 == 0) {
            (**(code **)(*(long *)local_c0 + 0x20))();
          }
        }
        uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 0013335c to 00133368 has its CatchHandler @ 0013371d */
        KisPaintOp::painter();
        uVar17 = KisPainter::paintColor();
        dVar29 = *(double *)(in_RSI + 0x1a8) - dVar29;
        dVar26 = (double)(DAT_00170578 & -(ulong)(dVar29 < 0.0) |
                         ~-(ulong)(dVar29 < 0.0) & (ulong)plVar24);
        dVar27 = dVar29;
        do {
          dVar27 = dVar27 - (dVar27 / DAT_00170580) * DAT_00170580;
        } while ((in_FPUStatusWord & 0x400) != 0);
        if (NAN(dVar27)) {
          fmod(dVar29,DAT_00170580);
LAB_00133410:
          dVar26 = 0.0;
        }
        else {
          dVar27 = (double)((ulong)dVar27 & DAT_001705d0);
          if (dVar27 <= DAT_00170588) {
            dVar26 = dVar26 * dVar27;
          }
          else {
            if (DAT_00170580 < dVar27) goto LAB_00133410;
            dVar26 = dVar26 * (double)((ulong)(DAT_00170580 - dVar27) ^ DAT_001705e0);
          }
        }
        local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
          UNLOCK();
        }
                    /* try { // try from 00133461 to 00133465 has its CatchHandler @ 00133699 */
        HatchingBrush::hatch
                  ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,dVar26,local_f8,
                   uVar16,&local_c0,uVar17);
      }
      if (local_c0 != (KisSharedPtr *)0x0) {
        LOCK();
        pKVar4 = local_c0 + 0x10;
        *(int *)pKVar4 = *(int *)pKVar4 + -1;
        UNLOCK();
        if (*(int *)pKVar4 == 0) {
          (**(code **)(*(long *)local_c0 + 0x20))();
        }
      }
    }
    else if (iVar6 == 1) {
      if (DAT_00170590 < *(double *)(*(long *)(in_RSI + 0x1a0) + 0x98)) {
        uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 001328fe to 0013290a has its CatchHandler @ 0013371d */
        KisPaintOp::painter();
        uVar17 = KisPainter::paintColor();
        uVar18 = spinAngle(in_RSI,DAT_00170588);
        local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
          UNLOCK();
        }
        pKVar22 = (KisPaintDevice *)&local_c0;
                    /* try { // try from 0013297f to 00132983 has its CatchHandler @ 00133741 */
        HatchingBrush::hatch
                  ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,uVar18,local_f8,
                   uVar16,pKVar22,uVar17);
        goto LAB_001326c7;
      }
    }
    else if (iVar6 == 2) {
      if (_DAT_001705a0 < *(double *)(*(long *)(in_RSI + 0x1a0) + 0x98)) {
        uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 00132aa1 to 00132aad has its CatchHandler @ 0013371d */
        KisPaintOp::painter();
        uVar17 = KisPainter::paintColor();
        uVar18 = spinAngle(in_RSI,DAT_001705a8);
        local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
          UNLOCK();
        }
        pKVar22 = (KisPaintDevice *)&local_c0;
        pKVar23 = pKVar22;
                    /* try { // try from 00132b6d to 00132b71 has its CatchHandler @ 00133651 */
        HatchingBrush::hatch
                  ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,uVar18,local_f8,
                   uVar16,pKVar22,uVar17);
        KisSharedPtr<KisPaintDevice>::deref(local_c0,pKVar23);
        if (_DAT_001705b0 < *(double *)(*(long *)(in_RSI + 0x1a0) + 0x98)) {
          uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 00132ba5 to 00132bb1 has its CatchHandler @ 0013371d */
          KisPaintOp::painter();
          uVar17 = KisPainter::paintColor();
          uVar18 = spinAngle(in_RSI,DAT_001705b8);
          local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
          if (local_c0 != (KisSharedPtr *)0x0) {
            LOCK();
            *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
            UNLOCK();
          }
                    /* try { // try from 00132c1a to 00132c1e has its CatchHandler @ 001336f9 */
          HatchingBrush::hatch
                    ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,uVar18,local_f8,
                     uVar16,pKVar22,uVar17);
          goto LAB_001326c7;
        }
      }
    }
    else if (iVar6 == 3) {
      if (_DAT_001705a0 < *(double *)(*(long *)(in_RSI + 0x1a0) + 0x98)) {
        uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 00133036 to 00133042 has its CatchHandler @ 0013371d */
        KisPaintOp::painter();
        uVar17 = KisPainter::paintColor();
        uVar18 = spinAngle(in_RSI,DAT_001705b8);
        local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
        if (local_c0 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
          UNLOCK();
        }
        pKVar22 = (KisPaintDevice *)&local_c0;
        pKVar23 = pKVar22;
                    /* try { // try from 00133102 to 00133106 has its CatchHandler @ 0013374d */
        HatchingBrush::hatch
                  ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,uVar18,local_f8,
                   uVar16,pKVar22,uVar17);
        KisSharedPtr<KisPaintDevice>::deref(local_c0,pKVar23);
        if (_DAT_001705b0 < *(double *)(*(long *)(in_RSI + 0x1a0) + 0x98)) {
          uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 0013313a to 00133146 has its CatchHandler @ 0013371d */
          KisPaintOp::painter();
          uVar17 = KisPainter::paintColor();
          uVar18 = spinAngle(in_RSI,DAT_001705a8);
          local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
          if (local_c0 != (KisSharedPtr *)0x0) {
            LOCK();
            *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
            UNLOCK();
          }
                    /* try { // try from 001331af to 001331b3 has its CatchHandler @ 001336c9 */
          HatchingBrush::hatch
                    ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,uVar18,local_f8,
                     uVar16,pKVar22,uVar17);
          goto LAB_001326c7;
        }
      }
    }
    else if (iVar6 == 4) {
      uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 0013348a to 00133496 has its CatchHandler @ 0013371d */
      KisPaintOp::painter();
      uVar17 = KisPainter::paintColor();
      uVar18 = spinAngle(in_RSI,DAT_001705c0 * *(double *)(*(long *)(in_RSI + 0x1a0) + 0x98));
      local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
      if (local_c0 != (KisSharedPtr *)0x0) {
        LOCK();
        *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
        UNLOCK();
      }
      pKVar22 = (KisPaintDevice *)&local_c0;
                    /* try { // try from 0013351a to 0013351e has its CatchHandler @ 001336b1 */
      HatchingBrush::hatch
                ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,uVar18,local_f8,
                 uVar16,pKVar22,uVar17);
LAB_001326c7:
      KisSharedPtr<KisPaintDevice>::deref(local_c0,pKVar22);
    }
LAB_00132390:
    cVar12 = KisCurveOption::isChecked();
    if (cVar12 != '\0') {
      uVar16 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 001326e2 to 001326ee has its CatchHandler @ 0013371d */
      KisPaintOp::painter();
      uVar17 = KisPainter::paintColor();
      dVar29 = DAT_001705c0 * *(double *)(*(long *)(in_RSI + 0x1a0) + 0x90) +
               *(double *)(in_RSI + 0x1a8) + *(double *)(in_RSI + 0x1a8);
      dVar26 = (double)(DAT_00170578 & -(ulong)(dVar29 < 0.0) |
                       ~-(ulong)(dVar29 < 0.0) & (ulong)plVar24);
      dVar27 = dVar29;
      do {
        dVar27 = dVar27 - (dVar27 / DAT_00170580) * DAT_00170580;
      } while ((in_FPUStatusWord & 0x400) != 0);
      if (NAN(dVar27)) {
        fmod(dVar29,DAT_00170580);
LAB_001327a6:
        dVar26 = 0.0;
      }
      else {
        dVar27 = (double)((ulong)dVar27 & DAT_001705d0);
        if (dVar27 <= DAT_00170588) {
          dVar26 = dVar26 * dVar27;
        }
        else {
          if (DAT_00170580 < dVar27) goto LAB_001327a6;
          dVar26 = dVar26 * (double)((ulong)(DAT_00170580 - dVar27) ^ DAT_001705e0);
        }
      }
      local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
      if (local_c0 != (KisSharedPtr *)0x0) {
        LOCK();
        *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
        UNLOCK();
      }
                    /* try { // try from 00132804 to 00132808 has its CatchHandler @ 00133735 */
      HatchingBrush::hatch
                ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,dVar26,local_f8,
                 uVar16,&local_c0,uVar17);
      if (local_c0 != (KisSharedPtr *)0x0) {
        LOCK();
        pKVar4 = local_c0 + 0x10;
        *(int *)pKVar4 = *(int *)pKVar4 + -1;
        UNLOCK();
        if (*(int *)pKVar4 == 0) {
          (**(code **)(*(long *)local_c0 + 0x20))();
        }
      }
    }
    if ((*(int *)(in_RSI + 0x1d0) != 4) && (cVar12 = KisCurveOption::isChecked(), cVar12 == '\0')) {
      uVar17 = *(undefined8 *)(in_RSI + 0x1e0);
                    /* try { // try from 00132839 to 00132845 has its CatchHandler @ 0013371d */
      KisPaintOp::painter();
      uVar18 = KisPainter::paintColor();
      local_c0 = *(KisSharedPtr **)(in_RSI + 0x1e8);
      uVar16 = *(undefined8 *)(in_RSI + 0x1a8);
      if (local_c0 != (KisSharedPtr *)0x0) {
        LOCK();
        *(int *)(local_c0 + 0x10) = *(int *)(local_c0 + 0x10) + 1;
        UNLOCK();
      }
                    /* try { // try from 001328ac to 001328b0 has its CatchHandler @ 001336ed */
      HatchingBrush::hatch
                ((double)iVar9,(double)iVar10,(double)iVar5,(double)(int)uVar1,uVar16,local_f8,
                 uVar17,&local_c0,uVar18);
      if (local_c0 != (KisSharedPtr *)0x0) {
        LOCK();
        pKVar4 = local_c0 + 0x10;
        *(int *)pKVar4 = *(int *)pKVar4 + -1;
        UNLOCK();
        if (*(int *)pKVar4 == 0) {
          (**(code **)(*(long *)local_c0 + 0x20))();
        }
      }
    }
    uVar16 = KisPaintOp::painter();
    local_c0 = local_d0;
    if (local_d0 != (KisSharedPtr *)0x0) {
      LOCK();
      *(int *)(local_d0 + 8) = *(int *)(local_d0 + 8) + 1;
      UNLOCK();
    }
    local_c8 = *(long **)(in_RSI + 0x1e8);
    if (local_c8 != (long *)0x0) {
      LOCK();
      *(int *)(local_c8 + 2) = *(int *)(local_c8 + 2) + 1;
      UNLOCK();
    }
    uVar20 = (ulong)uVar1;
                    /* try { // try from 00132436 to 0013243a has its CatchHandler @ 00133771 */
    KisPainter::bitBltWithFixedSelection(uVar16,iVar9,iVar10,&local_c8,&local_c0,iVar5);
    if (local_c8 != (long *)0x0) {
      LOCK();
      plVar24 = local_c8 + 2;
      *(int *)plVar24 = *(int *)plVar24 + -1;
      UNLOCK();
      if (*(int *)plVar24 == 0) {
        (**(code **)(*local_c8 + 0x20))(local_c8,0x1323ca,extraout_RDX,uVar20);
      }
    }
    if (local_c0 != (KisSharedPtr *)0x0) {
      LOCK();
      pKVar4 = local_c0 + 8;
      *(int *)pKVar4 = *(int *)pKVar4 + -1;
      UNLOCK();
      if (*(int *)pKVar4 == 0) {
        (**(code **)(*(long *)local_c0 + 8))();
      }
    }
                    /* try { // try from 00132483 to 00132493 has its CatchHandler @ 0013371d */
    uVar16 = KisPaintOp::painter();
    KisDabCache::needSeparateOriginal();
    local_c0 = local_d0;
    if (local_d0 != (KisSharedPtr *)0x0) {
      LOCK();
      *(int *)(local_d0 + 8) = *(int *)(local_d0 + 8) + 1;
      UNLOCK();
    }
    local_c8 = *(long **)(in_RSI + 0x1e8);
    if (local_c8 != (long *)0x0) {
      LOCK();
      *(int *)(local_c8 + 2) = *(int *)(local_c8 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 00132532 to 00132536 has its CatchHandler @ 00133765 */
    KisPainter::renderMirrorMaskSafe
              (uVar16,CONCAT44(iVar10,iVar9),CONCAT44(uVar21 + iVar10,iVar13 + iVar9),&local_c8,0,0,
               &local_c0);
    if (local_c8 != (long *)0x0) {
      LOCK();
      plVar24 = local_c8 + 2;
      *(int *)plVar24 = *(int *)plVar24 + -1;
      UNLOCK();
      if (*(int *)plVar24 == 0) {
        (**(code **)(*local_c8 + 0x20))();
      }
    }
    if (local_c0 != (KisSharedPtr *)0x0) {
      LOCK();
      pKVar4 = local_c0 + 8;
      *(int *)pKVar4 = *(int *)pKVar4 + -1;
      UNLOCK();
      if (*(int *)pKVar4 == 0) {
        (**(code **)(*(long *)local_c0 + 8))();
      }
    }
                    /* try { // try from 00132586 to 0013264d has its CatchHandler @ 0013371d */
    KisBrushBasedPaintOp::effectiveSpacing((double)local_f0);
    if (local_d0 != (KisSharedPtr *)0x0) {
      LOCK();
      pKVar4 = local_d0 + 8;
      *(int *)pKVar4 = *(int *)pKVar4 + -1;
      UNLOCK();
      if (*(int *)pKVar4 == 0) {
        (**(code **)(*(long *)local_d0 + 8))();
      }
    }
  }
  if (local_d8 != (long *)0x0) {
    LOCK();
    plVar24 = local_d8 + 2;
    *(int *)plVar24 = *(int *)plVar24 + -1;
    UNLOCK();
    if (*(int *)plVar24 == 0) {
      (**(code **)(*local_d8 + 0x20))();
    }
  }
  if (piVar7 != (int *)0x0) {
    LOCK();
    piVar2 = piVar7 + 1;
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (*piVar2 == 0) {
      (**(code **)(piVar7 + 2))(piVar7);
    }
    LOCK();
    *piVar7 = *piVar7 + -1;
    UNLOCK();
    if (*piVar7 == 0) {
      operator_delete(piVar7,0x10);
    }
  }
LAB_00131d92:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return param_1;
}


// ====== paintBezierCurve @ 00198450 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 001989f0 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


