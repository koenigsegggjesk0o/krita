/* Painting functions extracted from kritadefaultpaintops
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintLine @ 001265c0 ======

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  (*(code *)PTR_paintLine_001a97a0)();
  return;
}


// ====== paintAt @ 0012724a ======

/* KisBrushOp::paintAt(KisPaintInformation const&) [clone .cold] */

void __thiscall
KisBrushOp::paintAt(KisBrushOp *this,KisPaintInformation *param_2,undefined param_3,
                   undefined param_4,undefined param_5,undefined param_6,undefined param_7,
                   undefined param_8,undefined param_9,undefined param_10,KisSharedPtr *param_11,
                   KisSharedPtr *param_12,long param_13)

{
  ExternalRefCountData *unaff_RBP;
  long in_FS_OFFSET;
  
  KisSharedPtr<KisDefaultBoundsBase>::deref(param_12,(KisDefaultBoundsBase *)param_2);
  KisSharedPtr<KisPaintDevice>::deref(param_11,(KisPaintDevice *)param_2);
  if (unaff_RBP != (ExternalRefCountData *)0x0) {
    QSharedPointer<KisBrush>::deref(unaff_RBP);
  }
  if (param_13 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 00127cd6 ======

/* KisBrushOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) [clone .cold] */

void KisBrushOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  long unaff_RBP;
  long in_FS_OFFSET;
  
  KisSharedPtr<KisPaintDevice>::deref
            (*(KisSharedPtr **)(unaff_RBP + -0x58),(KisPaintDevice *)param_2);
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 00128f92 ======

/* KisDuplicateOp::paintAt(KisPaintInformation const&) [clone .cold] */

void __thiscall KisDuplicateOp::paintAt(KisDuplicateOp *this,KisPaintInformation *param_1)

{
  int *piVar1;
  long unaff_RBP;
  long in_FS_OFFSET;
  
  KisSharedPtr<KisPaintDevice>::deref
            (*(KisSharedPtr **)(unaff_RBP + -0xb8),(KisPaintDevice *)param_1);
  KisPainter::~KisPainter(*(KisPainter **)(unaff_RBP + -0x100));
  KisSharedPtr<KisFixedPaintDevice>::deref
            (*(KisSharedPtr **)(unaff_RBP + -0xd8),(KisFixedPaintDevice *)param_1);
  KisSharedPtr<KisPaintDevice>::deref
            (*(KisSharedPtr **)(unaff_RBP + -0x128),(KisPaintDevice *)param_1);
  if (*(long *)(unaff_RBP + -0xf8) != 0) {
    LOCK();
    piVar1 = (int *)(*(long *)(unaff_RBP + -0xf8) + 4);
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(*(long *)(unaff_RBP + -0xf8) + 8))(*(long *)(unaff_RBP + -0xf8));
    }
    piVar1 = *(int **)(unaff_RBP + -0xf8);
    LOCK();
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      operator_delete(*(void **)(unaff_RBP + -0xf8),0x10);
    }
  }
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 00138ff0 ======

/* KisBrushOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisBrushOp::paintAt(KisPaintInformation *param_1)

{
  int *piVar1;
  long *plVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  double *in_RDX;
  int *piVar6;
  bool bVar7;
  KisSpacingOption *in_RSI;
  long *plVar8;
  long in_FS_OFFSET;
  float fVar9;
  double dVar10;
  double dVar11;
  double local_120;
  double local_118;
  double local_f8;
  double local_e8;
  double local_e0;
  undefined local_d8 [16];
  double local_c8;
  double dStack_c0;
  double local_b8;
  long *local_a8;
  double dStack_a0;
  double dStack_98;
  undefined8 uStack_90;
  undefined8 local_88;
  long *local_78;
  undefined *local_70;
  double *local_68;
  double local_58;
  double dStack_50;
  undefined8 local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisPaintOp::painter();
  KisPainter::device();
  if (local_78 == (long *)0x0) {
LAB_001390e8:
    dVar10 = DAT_00181ff8;
    *(undefined8 *)param_1 = 1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(double *)(param_1 + 8) = dVar10;
    *(double *)(param_1 + 0x10) = dVar10;
    goto LAB_0013910b;
  }
  LOCK();
  plVar8 = local_78 + 2;
  *(int *)plVar8 = *(int *)plVar8 + -1;
  UNLOCK();
  if (*(int *)plVar8 == 0) {
    (**(code **)(*local_78 + 0x20))();
    piVar6 = *(int **)(in_RSI + 0x30);
    plVar8 = *(long **)(in_RSI + 0x28);
    if (piVar6 == (int *)0x0) goto LAB_001390e0;
LAB_00139067:
    LOCK();
    *piVar6 = *piVar6 + 1;
    UNLOCK();
    LOCK();
    piVar6[1] = piVar6[1] + 1;
    dVar10 = DAT_00181ff8;
    UNLOCK();
    if (plVar8 != (long *)0x0) goto LAB_0013907a;
    *(undefined8 *)param_1 = 1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(double *)(param_1 + 8) = dVar10;
    *(double *)(param_1 + 0x10) = dVar10;
  }
  else {
    piVar6 = *(int **)(in_RSI + 0x30);
    plVar8 = *(long **)(in_RSI + 0x28);
    if (piVar6 != (int *)0x0) goto LAB_00139067;
LAB_001390e0:
    if (plVar8 == (long *)0x0) goto LAB_001390e8;
LAB_0013907a:
                    /* try { // try from 00139084 to 00139089 has its CatchHandler @ 001395a7 */
    cVar3 = (**(code **)(*plVar8 + 0xe0))(plVar8);
    dVar10 = DAT_00181ff8;
    if (cVar3 == '\0') {
      *(undefined8 *)param_1 = 1;
      *(undefined8 *)(param_1 + 0x18) = 0;
      param_1[0x20] = (KisPaintInformation)0x0;
      *(double *)(param_1 + 8) = dVar10;
      *(double *)(param_1 + 0x10) = dVar10;
    }
    else {
                    /* try { // try from 00139170 to 001391be has its CatchHandler @ 001395a7 */
      cVar3 = KisCurveOption::isChecked();
      local_120 = DAT_00181ff8;
      bVar7 = SUB81(in_RDX,0);
      if (cVar3 != '\0') {
        local_120 = (double)KisCurveOption::computeSizeLikeValue
                                      ((KisPaintInformation *)(in_RSI + 0x1c8),bVar7);
      }
      KisPaintOp::painter();
      KisPainter::device();
                    /* try { // try from 001391ca to 001391ce has its CatchHandler @ 001395bf */
      KisPaintDevice::defaultBounds();
                    /* try { // try from 001391da to 001391dc has its CatchHandler @ 001395b3 */
      iVar4 = (**(code **)(*local_78 + 0x30))();
      local_f8 = DAT_00181ff8;
      dVar10 = DAT_00181ff8;
      if (0 < iVar4) {
        dVar10 = DAT_00181ff8 / (double)(1 << ((byte)iVar4 & 0x1f));
      }
      local_118 = DAT_00181ff8;
      if (local_78 != (long *)0x0) {
        LOCK();
        plVar2 = local_78 + 1;
        *(int *)plVar2 = *(int *)plVar2 + -1;
        UNLOCK();
        if (*(int *)plVar2 == 0) {
          (**(code **)(*local_78 + 8))();
        }
      }
      dVar10 = dVar10 * local_120;
      if (local_a8 != (long *)0x0) {
        LOCK();
        plVar2 = local_a8 + 2;
        *(int *)plVar2 = *(int *)plVar2 + -1;
        UNLOCK();
        if (*(int *)plVar2 == 0) {
          (**(code **)(*local_a8 + 0x20))();
        }
      }
                    /* try { // try from 00139269 to 00139598 has its CatchHandler @ 001395a7 */
      cVar3 = KisBrushBasedPaintOp::checkSizeTooSmall(dVar10);
      if (cVar3 == '\0') {
        dVar11 = (double)KisRotationOption::apply((KisPaintInformation *)(in_RSI + 0x3a0));
        cVar3 = KisCurveOption::isChecked();
        dStack_c0 = local_f8;
        if (cVar3 != '\0') {
          dStack_c0 = (double)KisCurveOption::computeSizeLikeValue
                                        ((KisPaintInformation *)(in_RSI + 0x200),bVar7);
        }
        local_c8 = dVar10;
        local_b8 = dVar11;
        iVar4 = (**(code **)(*plVar8 + 0xc0))(0,plVar8,&local_c8);
        iVar5 = (**(code **)(*plVar8 + 0xb8))(0,plVar8,&local_c8);
        local_d8 = KisScatterOption::apply
                             ((KisPaintInformation *)(in_RSI + 800),(double)iVar5,(double)iVar4);
        local_e8 = local_f8;
        local_e0 = local_f8;
        KisFlowOpacityOption2::apply((KisPaintInformation *)(in_RSI + 1000),in_RDX,&local_e8);
        cVar3 = KisCurveOption::isChecked();
        if (cVar3 != '\0') {
          local_f8 = (double)KisCurveOption::computeSizeLikeValue
                                       ((KisPaintInformation *)(in_RSI + 0x2a8),bVar7);
        }
        cVar3 = KisCurveOption::isChecked();
        if (cVar3 != '\0') {
          local_118 = (double)KisCurveOption::computeSizeLikeValue
                                        ((KisPaintInformation *)(in_RSI + 0x270),bVar7);
        }
        KisPaintOp::painter();
        local_78 = (long *)KisPainter::paintColor();
        local_70 = local_d8;
        local_58 = local_118;
        dStack_50 = local_f8;
        local_68 = &local_c8;
        KisDabRenderingExecutor::addDab(*(DabRequestInfo **)(in_RSI + 0x470),local_e8,local_e0);
        KisBrushBasedPaintOp::effectiveSpacing
                  (dVar10,dVar11,(KisAirbrushOptionData *)&local_a8,in_RSI,
                   (KisPaintInformation *)(in_RSI + 0x1b0));
        dVar10 = dStack_a0;
        if (dStack_98 != dStack_a0) {
          local_48 = CONCAT44((float)dStack_98,(float)dStack_a0);
          fVar9 = (float)QVector2D::length();
          dVar10 = (double)fVar9;
        }
        KisRollingMeanAccumulatorWrapper::operator()
                  ((KisRollingMeanAccumulatorWrapper *)(in_RSI + 0x480),dVar10);
        *(undefined8 *)(param_1 + 0x20) = local_88;
        *(long **)param_1 = local_a8;
        *(double *)(param_1 + 8) = dStack_a0;
        *(double *)(param_1 + 0x10) = dStack_98;
        *(undefined8 *)(param_1 + 0x18) = uStack_90;
      }
      else {
        *(undefined8 *)param_1 = 1;
        *(undefined8 *)(param_1 + 0x18) = 0;
        param_1[0x20] = (KisPaintInformation)0x0;
        *(undefined (*) [16])(param_1 + 8) = (undefined  [16])0x0;
      }
    }
    if (piVar6 == (int *)0x0) goto LAB_0013910b;
  }
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
LAB_0013910b:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return param_1;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 0013ddd0 ======

/* KisBrushOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) */

void KisBrushOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  long *plVar1;
  char cVar2;
  int iVar3;
  QPointF *pQVar4;
  ulong uVar5;
  undefined8 uVar6;
  long *plVar7;
  long *plVar8;
  long in_FS_OFFSET;
  undefined auVar9 [16];
  long *local_60;
  long *local_58 [3];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  cVar2 = KisCurveOption::isChecked();
  if ((((cVar2 == '\0') || (*(long *)(param_1 + 0x28) == 0)) ||
      (iVar3 = KisBrush::width(), iVar3 != 1)) || (iVar3 = KisBrush::height(), iVar3 != 1)) {
    if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
      KisPaintOp::paintLine(param_1,param_2,param_3);
      return;
    }
  }
  else {
    if (*(long **)(param_1 + 0x468) == (long *)0x0) {
      KisPaintOp::source();
                    /* try { // try from 0013e014 to 0013e018 has its CatchHandler @ 0013e091 */
      KisPaintDevice::createCompositionSourceDevice();
      plVar7 = *(long **)(param_1 + 0x468);
      plVar8 = plVar7;
      if (local_58[0] != plVar7) {
        if (local_58[0] != (long *)0x0) {
          LOCK();
          *(int *)(local_58[0] + 2) = *(int *)(local_58[0] + 2) + 1;
          UNLOCK();
          plVar7 = *(long **)(param_1 + 0x468);
        }
        *(long **)(param_1 + 0x468) = local_58[0];
        plVar8 = local_58[0];
        if (plVar7 != (long *)0x0) {
          LOCK();
          plVar1 = plVar7 + 2;
          *(int *)plVar1 = *(int *)plVar1 + -1;
          UNLOCK();
          if (*(int *)plVar1 == 0) {
            (**(code **)(*plVar7 + 0x20))();
            plVar8 = local_58[0];
          }
        }
      }
      if (plVar8 != (long *)0x0) {
        LOCK();
        plVar7 = plVar8 + 2;
        *(int *)plVar7 = *(int *)plVar7 + -1;
        UNLOCK();
        if (*(int *)plVar7 == 0) {
          (**(code **)(*plVar8 + 0x20))();
        }
      }
      if (local_60 != (long *)0x0) {
        LOCK();
        plVar7 = local_60 + 2;
        *(int *)plVar7 = *(int *)plVar7 + -1;
        UNLOCK();
        if (*(int *)plVar7 == 0) {
          (**(code **)(*local_60 + 0x20))();
        }
      }
    }
    else {
      (**(code **)(**(long **)(param_1 + 0x468) + 0x68))();
    }
    local_60 = *(long **)(param_1 + 0x468);
    if (local_60 != (long *)0x0) {
      LOCK();
      *(int *)(local_60 + 2) = *(int *)(local_60 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 0013de9f to 0013dea3 has its CatchHandler @ 0013e0c1 */
    KisPainter::KisPainter((KisPainter *)local_58,&local_60);
    if (local_60 != (long *)0x0) {
      LOCK();
      plVar7 = local_60 + 2;
      *(int *)plVar7 = *(int *)plVar7 + -1;
      UNLOCK();
      if (*(int *)plVar7 == 0) {
        (**(code **)(*local_60 + 0x20))();
      }
    }
                    /* try { // try from 0013dec3 to 0013df24 has its CatchHandler @ 0013e0a9 */
    KisPaintOp::painter();
    KisPainter::paintColor();
    KisPainter::setPaintColor((KoColor *)local_58);
    KisPaintInformation::pos();
    pQVar4 = (QPointF *)KisPaintInformation::pos();
    KisPainter::drawDDALine((QPointF *)local_58,pQVar4);
    auVar9 = KisPaintDevice::extent();
    uVar5 = auVar9._0_8_;
    uVar6 = KisPaintOp::painter();
    local_60 = *(long **)(param_1 + 0x468);
    if (local_60 != (long *)0x0) {
      LOCK();
      *(int *)(local_60 + 2) = *(int *)(local_60 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 0013df67 to 0013df6b has its CatchHandler @ 0013e0b5 */
    KisPainter::bitBlt(uVar6,uVar5 & 0xffffffff,auVar9._4_4_,&local_60,uVar5 & 0xffffffff,
                       (long)uVar5 >> 0x20 & 0xffffffff,(auVar9._8_4_ - auVar9._0_4_) + 1);
    if (local_60 != (long *)0x0) {
      LOCK();
      plVar7 = local_60 + 2;
      *(int *)plVar7 = *(int *)plVar7 + -1;
      UNLOCK();
      if (*(int *)plVar7 == 0) {
        (**(code **)(*local_60 + 0x20))();
      }
    }
                    /* try { // try from 0013df8b to 0013df8f has its CatchHandler @ 0013e0a9 */
    uVar6 = KisPaintOp::painter();
    local_60 = *(long **)(param_1 + 0x468);
    if (local_60 != (long *)0x0) {
      LOCK();
      *(int *)(local_60 + 2) = *(int *)(local_60 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 0013dfb2 to 0013dfb6 has its CatchHandler @ 0013e09d */
    KisPainter::renderMirrorMask(uVar6,uVar5,auVar9._8_8_,&local_60);
    if (local_60 != (long *)0x0) {
      LOCK();
      plVar7 = local_60 + 2;
      *(int *)plVar7 = *(int *)plVar7 + -1;
      UNLOCK();
      if (*(int *)plVar7 == 0) {
        (**(code **)(*local_60 + 0x20))();
      }
    }
    KisPainter::~KisPainter((KisPainter *)local_58);
    if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 00164360 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisDuplicateOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisDuplicateOp::paintAt(KisPaintInformation *param_1)

{
  KisSharedPtr *pKVar1;
  KisSharedPtr *pKVar2;
  size_t __n;
  int *piVar3;
  double *pdVar4;
  double *pdVar5;
  double dVar6;
  KisDabShape *pKVar7;
  int *piVar8;
  KoColor *pKVar9;
  code *pcVar10;
  KisSharedPtr *pKVar11;
  long lVar12;
  KoColorSpace *pKVar13;
  QTextStream *this;
  char cVar14;
  int iVar15;
  uint uVar16;
  KisPaintInformation *pKVar17;
  undefined8 *puVar18;
  ulong uVar19;
  long *plVar20;
  long *plVar21;
  double *pdVar22;
  long lVar23;
  int iVar24;
  long lVar25;
  bool in_DL;
  int iVar26;
  long in_RSI;
  KisPaintDevice *pKVar27;
  double *pdVar28;
  double *pdVar29;
  double *pdVar30;
  int iVar31;
  int iVar32;
  KisSharedPtr *pKVar33;
  long in_FS_OFFSET;
  bool bVar34;
  undefined8 uVar35;
  double dVar36;
  double dVar37;
  double dVar38;
  double dVar39;
  double *pdVar40;
  double *pdVar41;
  double dVar42;
  undefined auVar43 [16];
  undefined auVar44 [16];
  uint local_184;
  double *local_160;
  int local_148;
  KisSharedPtr *local_130;
  double local_128;
  int local_11c;
  double *local_118;
  double *local_f0;
  KisSharedPtr *local_e0;
  long *local_d8;
  long *local_d0;
  QTextStream *local_c8;
  KisSharedPtr *local_c0;
  undefined8 local_b8;
  undefined8 uStack_b0;
  double local_a8;
  double local_a0;
  undefined8 local_98;
  KisSharedPtr *local_88;
  undefined local_80 [16];
  undefined8 local_70;
  ushort local_60;
  ushort local_5e;
  ushort local_5c;
  ushort local_58;
  ushort local_56;
  ushort local_54;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisPaintOp::painter();
  KisPainter::device();
  if (local_88 == (KisSharedPtr *)0x0) {
LAB_001644a0:
    dVar36 = DAT_0018a6c8;
    *(undefined8 *)param_1 = 1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(double *)(param_1 + 8) = dVar36;
    *(double *)(param_1 + 0x10) = dVar36;
    goto LAB_001644c9;
  }
  LOCK();
  pKVar1 = local_88 + 0x10;
  *(int *)pKVar1 = *(int *)pKVar1 + -1;
  UNLOCK();
  if (*(int *)pKVar1 == 0) {
    (**(code **)(*(long *)local_88 + 0x20))();
  }
  pKVar7 = *(KisDabShape **)(in_RSI + 0x28);
  piVar8 = *(int **)(in_RSI + 0x30);
  if (piVar8 == (int *)0x0) {
    if (pKVar7 == (KisDabShape *)0x0) goto LAB_001644a0;
LAB_00164402:
                    /* try { // try from 0016440f to 00164414 has its CatchHandler @ 00165a1f */
    cVar14 = (**(code **)(*(long *)pKVar7 + 0xe0))();
    dVar36 = DAT_0018a6c8;
    if (cVar14 == '\0') {
      *(undefined8 *)param_1 = 1;
      *(undefined8 *)(param_1 + 0x18) = 0;
      param_1[0x20] = (KisPaintInformation)0x0;
      *(double *)(param_1 + 8) = dVar36;
      *(double *)(param_1 + 0x10) = dVar36;
    }
    else {
      if (*(char *)(in_RSI + 0x1e0) == '\0') {
        *(undefined *)(in_RSI + 0x1e0) = 1;
                    /* try { // try from 00164755 to 00164759 has its CatchHandler @ 00165a1f */
        puVar18 = (undefined8 *)KisPaintInformation::pos();
        uVar35 = puVar18[1];
        *(undefined8 *)(in_RSI + 0x1d0) = *puVar18;
        *(undefined8 *)(in_RSI + 0x1d8) = uVar35;
      }
      if ((*(char *)(in_RSI + 0x1b4) == '\0') || (*(long *)(in_RSI + 0x1a0) == 0)) {
                    /* try { // try from 00164595 to 00164599 has its CatchHandler @ 001659fe */
        KisDuplicateOpSettings::sourceNode();
        pKVar1 = local_88;
        if (((uint *)local_80._0_8_ == (uint *)0x0) ||
           ((local_88 == (KisSharedPtr *)0x0 || ((*(uint *)local_80._0_8_ & 1) == 0)))) {
          local_88 = (KisSharedPtr *)0x0;
          if ((uint *)local_80._0_8_ != (uint *)0x0) {
            LOCK();
            uVar16 = *(uint *)local_80._0_8_;
            *(uint *)local_80._0_8_ = *(uint *)local_80._0_8_ - 2;
            UNLOCK();
            if (((int)uVar16 < 3) && ((uint *)local_80._0_8_ != (uint *)0x0)) {
              operator_delete((void *)local_80._0_8_,4);
            }
          }
          pKVar33 = *(KisSharedPtr **)(in_RSI + 0x1a8);
          if (pKVar33 != (KisSharedPtr *)0x0) {
            LOCK();
            *(int *)(pKVar33 + 0x10) = *(int *)(pKVar33 + 0x10) + 1;
            UNLOCK();
          }
        }
        else {
          pKVar2 = local_88 + 0x10;
          LOCK();
          *(int *)(local_88 + 0x10) = *(int *)(local_88 + 0x10) + 1;
          UNLOCK();
          local_88 = (KisSharedPtr *)0x0;
          if ((uint *)local_80._0_8_ != (uint *)0x0) {
            LOCK();
            uVar16 = *(uint *)local_80._0_8_;
            *(uint *)local_80._0_8_ = *(uint *)local_80._0_8_ - 2;
            UNLOCK();
            if (((int)uVar16 < 3) && ((uint *)local_80._0_8_ != (uint *)0x0)) {
              operator_delete((void *)local_80._0_8_,4);
            }
          }
                    /* try { // try from 00164603 to 00164621 has its CatchHandler @ 00165a5b */
          lVar23 = KisNode::graphListener();
          pKVar33 = pKVar1;
          if ((lVar23 == 0) && (pKVar11 = *(KisSharedPtr **)(in_RSI + 0x1a8), pKVar11 != pKVar1)) {
            if (pKVar11 != (KisSharedPtr *)0x0) {
              LOCK();
              *(int *)(pKVar11 + 0x10) = *(int *)(pKVar11 + 0x10) + 1;
              UNLOCK();
            }
            LOCK();
            *(int *)pKVar2 = *(int *)pKVar2 + -1;
            UNLOCK();
            pKVar33 = pKVar11;
            if (*(int *)pKVar2 == 0) {
              (**(code **)(*(long *)pKVar1 + 0x20))(pKVar1);
            }
          }
        }
        (**(code **)(*(long *)pKVar33 + 0x70))((KisPainter *)&local_88,pKVar33);
        local_130 = local_88;
        if (local_88 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_88 + 0x10) = *(int *)(local_88 + 0x10) + 1;
          UNLOCK();
          if (local_88 != (KisSharedPtr *)0x0) {
            LOCK();
            pKVar1 = local_88 + 0x10;
            *(int *)pKVar1 = *(int *)pKVar1 + -1;
            UNLOCK();
            if (*(int *)pKVar1 == 0) {
              (**(code **)(*(long *)local_88 + 0x20))();
            }
          }
        }
        LOCK();
        pKVar1 = pKVar33 + 0x10;
        *(int *)pKVar1 = *(int *)pKVar1 + -1;
        UNLOCK();
        if (*(int *)pKVar1 == 0) {
          (**(code **)(*(long *)pKVar33 + 0x20))(pKVar33);
        }
      }
      else {
                    /* try { // try from 00164527 to 0016452b has its CatchHandler @ 00165a0a */
        KisImage::projection();
        local_130 = local_88;
        if (local_88 != (KisSharedPtr *)0x0) {
          LOCK();
          *(int *)(local_88 + 0x10) = *(int *)(local_88 + 0x10) + 1;
          UNLOCK();
          if (local_88 != (KisSharedPtr *)0x0) {
            LOCK();
            pKVar1 = local_88 + 0x10;
            *(int *)pKVar1 = *(int *)pKVar1 + -1;
            UNLOCK();
            if (*(int *)pKVar1 == 0) {
              (**(code **)(*(long *)local_88 + 0x20))();
            }
          }
        }
      }
                    /* try { // try from 00164679 to 001646ee has its CatchHandler @ 00165a73 */
      uVar35 = KisRotationOption::apply((KisPaintInformation *)(in_RSI + 0x260));
      pKVar17 = (KisPaintInformation *)KisPaintOp::painter();
      KisOpacityOption::apply((KisPainter *)(in_RSI + 0x220),pKVar17);
      cVar14 = KisCurveOption::isChecked();
      local_128 = DAT_0018a6c8;
      if (cVar14 != '\0') {
        local_128 = (double)KisCurveOption::computeSizeLikeValue
                                      ((KisPaintInformation *)(in_RSI + 0x1e8),in_DL);
      }
      cVar14 = KisBrushBasedPaintOp::checkSizeTooSmall(local_128);
      if (cVar14 == '\0') {
        local_a0 = DAT_0018a6c8;
        local_a8 = local_128;
        local_98 = uVar35;
        if ((paintAt(KisPaintInformation_const&)::cs == '\0') &&
           (iVar15 = __cxa_guard_acquire(&paintAt(KisPaintInformation_const&)::cs), iVar15 != 0)) {
                    /* try { // try from 001647f3 to 001647ff has its CatchHandler @ 00165a7f */
          KoColorSpaceRegistry::instance();
          paintAt(KisPaintInformation_const&)::cs = (KoColorSpace *)KoColorSpaceRegistry::alpha8();
          __cxa_guard_release(&paintAt(KisPaintInformation_const&)::cs);
        }
        if ((paintAt(KisPaintInformation_const&)::color == '\0') &&
           (iVar15 = __cxa_guard_acquire(&paintAt(KisPaintInformation_const&)::color),
           pKVar13 = paintAt(KisPaintInformation_const&)::cs, iVar15 != 0)) {
          QColor::QColor((QColor *)&local_58,2);
                    /* try { // try from 00164856 to 0016485a has its CatchHandler @ 00165a67 */
          KoColor::KoColor((KoColor *)paintAt(KisPaintInformation_const&)::color,(QColor *)&local_58
                           ,pKVar13);
          __cxa_atexit(KoColor::~KoColor,paintAt(KisPaintInformation_const&)::color,&__dso_handle);
          __cxa_guard_release(&paintAt(KisPaintInformation_const&)::color);
        }
        pKVar9 = *(KoColor **)(in_RSI + 0x20);
        local_b8 = _DAT_0018a6f0;
        uStack_b0 = _UNK_0018a6f8;
                    /* try { // try from 0016489d to 001648e0 has its CatchHandler @ 00165a73 */
        pKVar17 = (KisPaintInformation *)KisPaintInformation::pos();
        KisDabCache::fetchDab
                  ((KoColorSpace *)&local_e0,pKVar9,
                   (QPointF *)paintAt(KisPaintInformation_const&)::cs,
                   (KisDabShape *)paintAt(KisPaintInformation_const&)::color,pKVar17,DAT_0018a6c8,
                   (QRect *)&local_a8,DAT_0018a6c8);
        dVar36 = DAT_0018a6c8;
        if ((int)uStack_b0 < (int)local_b8) {
LAB_00164d58:
          *(undefined8 *)param_1 = 1;
          *(undefined8 *)(param_1 + 0x18) = 0;
          param_1[0x20] = (KisPaintInformation)0x0;
          *(double *)(param_1 + 8) = dVar36;
          *(double *)(param_1 + 0x10) = dVar36;
        }
        else {
          if (uStack_b0._4_4_ < (int)local_b8._4_4_) goto LAB_00164d58;
          if (*(char *)(in_RSI + 0x1b2) == '\0') {
                    /* try { // try from 0016540f to 00165436 has its CatchHandler @ 00165a16 */
            auVar43 = KisBrush::hotSpot(pKVar7,(KisPaintInformation *)&local_a8);
            auVar44 = KisDuplicateOpSettings::position(*(KisDuplicateOpSettings **)(in_RSI + 0x1b8))
            ;
            dVar42 = auVar44._8_8_ - auVar43._8_8_;
            dVar36 = auVar44._0_8_ - auVar43._0_8_;
            if (dVar42 < 0.0) {
              iVar15 = (int)((dVar42 - (double)(int)(dVar42 - DAT_0018a6c8)) + DAT_0018a6d0) +
                       (int)(dVar42 - DAT_0018a6c8);
            }
            else {
              iVar15 = (int)(dVar42 + DAT_0018a6d0);
            }
            if (dVar36 < 0.0) {
              iVar32 = (int)((dVar36 - (double)(int)(dVar36 - DAT_0018a6c8)) + DAT_0018a6d0) +
                       (int)(dVar36 - DAT_0018a6c8);
            }
            else {
              iVar32 = (int)(dVar36 + DAT_0018a6d0);
            }
          }
          else {
                    /* try { // try from 00164922 to 00164926 has its CatchHandler @ 00165a16 */
            auVar43 = KisDuplicateOpSettings::offset(*(KisDuplicateOpSettings **)(in_RSI + 0x1b8));
            dVar42 = (double)(int)local_b8._4_4_ - auVar43._8_8_;
            dVar36 = (double)(int)local_b8 - auVar43._0_8_;
            if (dVar42 < 0.0) {
              iVar15 = (int)((dVar42 - (double)(int)(dVar42 - DAT_0018a6c8)) + DAT_0018a6d0) +
                       (int)(dVar42 - DAT_0018a6c8);
            }
            else {
              iVar15 = (int)(dVar42 + DAT_0018a6d0);
            }
            if (dVar36 < 0.0) {
              iVar32 = (int)((dVar36 - (double)(int)(dVar36 - DAT_0018a6c8)) + DAT_0018a6d0) +
                       (int)(dVar36 - DAT_0018a6c8);
            }
            else {
              iVar32 = (int)(dVar36 + DAT_0018a6d0);
            }
          }
          iVar26 = (int)uStack_b0 - (int)local_b8;
          local_c0 = *(KisSharedPtr **)(in_RSI + 0x1c0);
          if (local_c0 != (KisSharedPtr *)0x0) {
            LOCK();
            *(int *)((long)local_c0 + 0x10) = *(int *)((long)local_c0 + 0x10) + 1;
            UNLOCK();
          }
                    /* try { // try from 001649d5 to 001649d9 has its CatchHandler @ 001659f2 */
          KisPainter::KisPainter((KisPainter *)&local_88,(QString *)&local_c0);
          if (local_c0 != (KisSharedPtr *)0x0) {
            LOCK();
            plVar20 = (long *)((long)local_c0 + 0x10);
            *(int *)plVar20 = *(int *)plVar20 + -1;
            UNLOCK();
            if (*(int *)plVar20 == 0) {
              (**(code **)(*(long *)local_c0 + 0x20))();
            }
          }
                    /* try { // try from 00164a01 to 00164a05 has its CatchHandler @ 001659e6 */
          KisPainter::setCompositeOpId((QString *)&local_88);
          local_c0 = local_130;
          if (local_130 != (KisSharedPtr *)0x0) {
            LOCK();
            *(int *)(local_130 + 0x10) = *(int *)(local_130 + 0x10) + 1;
            UNLOCK();
          }
                    /* try { // try from 00164a39 to 00164a3d has its CatchHandler @ 00165ac7 */
          KisPainter::bitBltOldData
                    ((KisPainter *)&local_88,0,0,(QString *)&local_c0,iVar32,iVar15,iVar26 + 1);
          if (local_c0 != (KisSharedPtr *)0x0) {
            LOCK();
            pKVar1 = local_c0 + 0x10;
            *(int *)pKVar1 = *(int *)pKVar1 + -1;
            UNLOCK();
            if (*(int *)pKVar1 == 0) {
              (**(code **)(*(long *)local_c0 + 0x20))();
            }
          }
                    /* try { // try from 00164a63 to 00164a67 has its CatchHandler @ 001659e6 */
          KisPainter::end();
          KisPainter::~KisPainter((KisPainter *)&local_88);
          if (*(char *)(in_RSI + 0x1b0) != '\0') {
            pKVar27 = (KisPaintDevice *)(ulong)local_b8._4_4_;
            local_184 = (int)uStack_b0 - (int)local_b8;
            local_11c = uStack_b0._4_4_ - local_b8._4_4_;
            if (((int)local_184 < 2) || (iVar15 = (int)local_b8, local_11c < 2)) {
              iVar15 = (int)local_b8 + -1;
              local_11c = (uStack_b0._4_4_ + 1) - (local_b8._4_4_ - 1);
              local_184 = ((int)uStack_b0 + 1) - iVar15;
            }
            iVar32 = local_184 + 1;
            iVar26 = local_11c + 1;
            uVar16 = iVar32 * iVar26 * 3;
            if ((ulong)(long)(int)uVar16 >> 0x3c != 0) {
              if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
                __stack_chk_fail();
              }
                    /* try { // try from 00129056 to 0012905a has its CatchHandler @ 0012905b */
              uVar35 = __cxa_throw_bad_array_new_length();
              KisSharedPtr<KisFixedPaintDevice>::deref(local_e0,(KisFixedPaintDevice *)pKVar27);
              KisSharedPtr<KisPaintDevice>::deref(local_130,pKVar27);
                    /* catch() { ... } // from try @ 00129056 with catch @ 0012905b */
              if (piVar8 != (int *)0x0) {
                LOCK();
                piVar3 = piVar8 + 1;
                *piVar3 = *piVar3 + -1;
                UNLOCK();
                if (*piVar3 == 0) {
                  (**(code **)(piVar8 + 2))(piVar8);
                }
                LOCK();
                *piVar8 = *piVar8 + -1;
                UNLOCK();
                if (*piVar8 == 0) {
                  operator_delete(piVar8,0x10);
                }
              }
              if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
                __stack_chk_fail();
              }
                    /* WARNING: Subroutine does not return */
              _Unwind_Resume(uVar35);
            }
            uVar19 = (long)(int)uVar16 << 3;
                    /* try { // try from 00164b17 to 00164b1b has its CatchHandler @ 00165a16 */
            local_160 = (double *)operator_new__(uVar19);
                    /* try { // try from 00164b2a to 00164b6a has its CatchHandler @ 00165aaf */
            plVar20 = (long *)KisPaintDevice::colorSpace();
            plVar21 = (long *)KisPaintDevice::colorSpace();
            KisPaintDevice::createHLineConstIteratorNG((int)&local_d8,(int)local_130,iVar15);
                    /* try { // try from 00164b84 to 00164b88 has its CatchHandler @ 00165aa3 */
            KisPaintDevice::createHLineIteratorNG
                      ((int)&local_d0,(int)*(undefined8 *)(in_RSI + 0x1c0),0);
            if (0 < iVar26) {
              local_f0 = local_160;
              iVar15 = 0;
              do {
                pdVar22 = local_f0 + (ulong)local_184 * 3 + 3;
                if (0 < iVar32) {
                  do {
                    pcVar10 = *(code **)(*plVar20 + 0xd8);
                    /* try { // try from 00164c14 to 00164d33 has its CatchHandler @ 00165a97 */
                    uVar35 = (**(code **)(*(long *)((long)local_d8 + *(long *)(*local_d8 + -0x18)) +
                                         0x10))();
                    (*pcVar10)(plVar20,uVar35,&local_60,1);
                    pcVar10 = *(code **)(*plVar21 + 0xd8);
                    uVar35 = (**(code **)(local_d0[1] + 0x10))(local_d0 + 1);
                    (*pcVar10)(plVar21,uVar35,&local_58,1);
                    dVar36 = (double)local_60;
                    if (local_58 != 0) {
                      dVar36 = dVar36 / (double)local_58;
                    }
                    *local_f0 = dVar36;
                    dVar36 = (double)local_5e;
                    if (local_56 != 0) {
                      dVar36 = dVar36 / (double)local_56;
                    }
                    local_f0[1] = dVar36;
                    dVar36 = (double)local_5c;
                    if (local_54 != 0) {
                      dVar36 = dVar36 / (double)local_54;
                    }
                    lVar23 = *local_d8;
                    local_f0[2] = dVar36;
                    (**(code **)(*(long *)((long)local_d8 + *(long *)(lVar23 + -0x18)) + 0x30))();
                    (**(code **)(*(long *)((long)local_d0 + *(long *)(*local_d0 + -0x18)) + 0x30))()
                    ;
                    local_f0 = local_f0 + 3;
                  } while (local_f0 != pdVar22);
                }
                (**(code **)(*local_d8 + 0x10))();
                (**(code **)(*local_d0 + 0x10))();
                bVar34 = local_11c != iVar15;
                iVar15 = iVar15 + 1;
              } while (bVar34);
            }
                    /* try { // try from 00165035 to 00165039 has its CatchHandler @ 00165a97 */
            local_118 = (double *)operator_new__(uVar19);
            dVar36 = DAT_0018a6c8;
            if ((iVar26 < 3) || (iVar32 < 3)) {
              if (iVar32 * iVar26 != 0) {
                pdVar22 = local_118;
                if (((ulong)(uVar16 >> 1) * 0x10 & 0x10) == 0) goto LAB_001650a0;
                *local_118 = DAT_0018a6c8;
                local_118[1] = dVar36;
                for (pdVar22 = local_118 + 2; local_118 + (ulong)(uVar16 >> 1) * 2 != pdVar22;
                    pdVar22 = pdVar22 + 4) {
LAB_001650a0:
                  *pdVar22 = dVar36;
                  pdVar22[1] = dVar36;
                  pdVar22[2] = dVar36;
                  pdVar22[3] = dVar36;
                }
                if ((uVar16 & 1) != 0) {
                  local_118[uVar16 & 0xfffffffe] = DAT_0018a6c8;
                }
              }
                    /* try { // try from 001650d2 to 001650d6 has its CatchHandler @ 00165ad3 */
              lVar23 = _41000();
              if (*(char *)(lVar23 + 0x11) != '\0') {
                    /* try { // try from 0016534c to 0016537e has its CatchHandler @ 00165ad3 */
                lVar23 = _41000();
                local_70 = *(undefined8 *)(lVar23 + 8);
                local_88 = (KisSharedPtr *)0x2;
                local_80 = (undefined  [16])0x0;
                QMessageLogger::warning();
                this = local_c8;
                    /* try { // try from 0016539c to 001653a0 has its CatchHandler @ 00165a43 */
                QString::fromUtf8_helper((char *)&local_c0,0x18a650);
                    /* try { // try from 001653a7 to 001653ab has its CatchHandler @ 00165a2b */
                QTextStream::operator<<(this,(QString *)&local_c0);
                if (*(int *)local_c0 == 0) {
LAB_001653e5:
                  QArrayData::deallocate((QArrayData *)local_c0,2,8);
                }
                else if (*(int *)local_c0 != -1) {
                  LOCK();
                  *(int *)local_c0 = *(int *)local_c0 + -1;
                  UNLOCK();
                  if (*(int *)local_c0 == 0) goto LAB_001653e5;
                }
                if (local_c8[0x20] != (QTextStream)0x0) {
                    /* try { // try from 001653fb to 001653ff has its CatchHandler @ 00165a43 */
                  QTextStream::operator<<(local_c8,' ');
                }
                QDebug::~QDebug((QDebug *)&local_c8);
              }
            }
            else {
              iVar15 = iVar32 * 3;
              local_148 = 0;
              __n = (long)iVar32 * 0x18;
              lVar23 = (long)iVar15;
              uVar16 = 1;
              if (6 < iVar15) {
                uVar16 = iVar15 - 6;
              }
              pdVar22 = local_118;
              local_118 = local_160;
              while( true ) {
                local_160 = local_118;
                local_118 = pdVar22;
                memcpy(local_118,local_160,__n);
                lVar12 = _UNK_0018a708;
                dVar36 = DAT_0018a6d8;
                pdVar22 = local_160 + lVar23;
                pdVar29 = local_118 + lVar23;
                if (local_11c < 2) break;
                dVar42 = 0.0;
                iVar31 = 1;
                do {
                  dVar6 = pdVar22[1];
                  *pdVar29 = *pdVar22;
                  pdVar29[1] = dVar6;
                  pdVar29[2] = pdVar22[2];
                  if (((local_184 == 0xaaaaaaac || iVar15 < 7) ||
                      ((long)pdVar29 - (long)(pdVar22 + (4 - lVar23)) == -0x18 ||
                       (long)pdVar29 - (long)(pdVar22 + lVar23 + 4) == -0x18)) ||
                     ((ulong)((long)pdVar29 + (0x10 - (long)pdVar22)) < 0x31)) {
                    iVar24 = 3;
                    pdVar41 = pdVar22 + 3;
                    pdVar40 = pdVar29 + 3;
                    do {
                      pdVar30 = pdVar40;
                      pdVar28 = pdVar41;
                      iVar24 = iVar24 + 1;
                      pdVar40 = pdVar30 + 1;
                      dVar6 = *pdVar30;
                      pdVar41 = pdVar28 + 1;
                      dVar38 = (pdVar28[3] + pdVar28[-3] + pdVar28[-lVar23] + pdVar28[lVar23] +
                               *pdVar28 + *pdVar28) / dVar36;
                      *pdVar30 = dVar38;
                      dVar38 = dVar38 - dVar6;
                      dVar42 = dVar42 + dVar38 * dVar38;
                    } while (iVar24 < iVar15 + -3);
                  }
                  else {
                    lVar25 = 0;
                    pdVar41 = pdVar29 + 4;
                    pdVar40 = pdVar22 + 4;
                    do {
                      pdVar28 = pdVar40;
                      pdVar30 = pdVar41;
                      pdVar4 = (double *)((long)pdVar22 + lVar25 + 0x30);
                      pdVar41 = (double *)((long)pdVar22 + lVar25 + lVar23 * -8 + 0x18);
                      pdVar40 = (double *)((long)pdVar22 + lVar25 + lVar23 * 8 + 0x18);
                      pdVar5 = (double *)((long)pdVar22 + lVar25 + 0x18);
                      dVar6 = *pdVar5;
                      dVar38 = pdVar5[1];
                      pdVar5 = (double *)((long)pdVar29 + lVar25 + 0x18);
                      dVar37 = *pdVar5;
                      dVar39 = pdVar5[1];
                      auVar44._0_8_ =
                           *pdVar4 + *(double *)((long)pdVar22 + lVar25) + *pdVar41 + *pdVar40 +
                           dVar6 + dVar6;
                      auVar44._8_8_ =
                           pdVar4[1] + ((double *)((long)pdVar22 + lVar25))[1] + pdVar41[1] +
                           pdVar40[1] + dVar38 + dVar38;
                      auVar43._8_8_ = dVar36;
                      auVar43._0_8_ = dVar36;
                      auVar43 = divpd(auVar44,auVar43);
                      *(undefined (*) [16])((long)pdVar29 + lVar25 + 0x18) = auVar43;
                      dVar37 = auVar43._0_8_ - dVar37;
                      dVar39 = auVar43._8_8_ - dVar39;
                      lVar25 = lVar25 + 0x10;
                      dVar42 = dVar42 + dVar37 * dVar37 + dVar39 * dVar39;
                      pdVar41 = (double *)((long)pdVar30 + lVar12);
                      pdVar40 = (double *)((long)pdVar28 + lVar12);
                    } while (lVar25 != (ulong)(uVar16 >> 1) << 4);
                    pdVar29 = pdVar29 + 3 + (uVar16 & 0xfffffffe);
                    pdVar22 = pdVar22 + 3 + (uVar16 & 0xfffffffe);
                    if ((uVar16 & 1) == 0) {
                      pdVar40 = (double *)(_UNK_0018a718 + (long)pdVar30);
                      pdVar41 = (double *)(_UNK_0018a718 + (long)pdVar28);
                    }
                    else {
                      dVar6 = *pdVar29;
                      pdVar41 = pdVar22 + 1;
                      pdVar40 = pdVar29 + 1;
                      dVar38 = (pdVar22[3] + pdVar22[-3] + pdVar22[-lVar23] + pdVar22[lVar23] +
                               *pdVar22 + *pdVar22) / dVar36;
                      *pdVar29 = dVar38;
                      dVar38 = dVar38 - dVar6;
                      dVar42 = dVar42 + dVar38 * dVar38;
                      pdVar28 = pdVar22;
                      pdVar30 = pdVar29;
                    }
                  }
                  dVar6 = pdVar41[1];
                  pdVar22 = pdVar28 + 4;
                  pdVar29 = pdVar30 + 4;
                  iVar31 = iVar31 + 1;
                  *pdVar40 = *pdVar41;
                  pdVar40[1] = dVar6;
                  pdVar40[2] = pdVar41[2];
                } while (local_11c != iVar31);
                memcpy(pdVar29,pdVar22,__n);
                local_148 = local_148 + 1;
                if ((dVar42 <= _DAT_0018a6e0) || (pdVar22 = local_160, local_148 == 100))
                goto LAB_001650e3;
              }
              memcpy(pdVar29,pdVar22,__n);
            }
LAB_001650e3:
            operator_delete__(local_160);
                    /* try { // try from 0016510f to 00165113 has its CatchHandler @ 00165a4f */
            KisPaintDevice::createHLineIteratorNG
                      ((int)(KisPainter *)&local_88,(int)*(undefined8 *)(in_RSI + 0x1c0),0);
            if (0 < iVar26) {
              local_f0 = local_118;
              iVar15 = 0;
              do {
                pdVar22 = local_f0 + (ulong)local_184 * 3 + 3;
                if (0 < iVar32) {
                  do {
                    pcVar10 = *(code **)(*plVar21 + 0xd8);
                    uVar35 = (**(code **)(*(long *)(local_88 + 8) + 0x10))(local_88 + 8);
                    (*pcVar10)(plVar21,uVar35,&local_58,1);
                    dVar36 = *local_f0;
                    if (local_58 != 0) {
                      dVar36 = dVar36 * (double)local_58;
                    }
                    local_58 = 0;
                    if ((0.0 <= dVar36) && (local_58 = 0xffff, dVar36 <= _DAT_0018a6e8)) {
                      local_58 = (ushort)(int)dVar36;
                    }
                    dVar36 = local_f0[1];
                    if (local_56 != 0) {
                      dVar36 = dVar36 * (double)local_56;
                    }
                    local_56 = 0;
                    if ((0.0 <= dVar36) && (local_56 = 0xffff, dVar36 <= _DAT_0018a6e8)) {
                      local_56 = (ushort)(int)dVar36;
                    }
                    dVar36 = local_f0[2];
                    if (local_54 != 0) {
                      dVar36 = dVar36 * (double)local_54;
                    }
                    local_54 = 0;
                    if ((0.0 <= dVar36) && (local_54 = 0xffff, dVar36 <= _DAT_0018a6e8)) {
                      local_54 = (ushort)(int)dVar36;
                    }
                    pcVar10 = *(code **)(*plVar21 + 0xe0);
                    /* try { // try from 00165253 to 00165329 has its CatchHandler @ 00165a37 */
                    uVar35 = (**(code **)(*(long *)(local_88 + 8) + 0x10))(local_88 + 8);
                    (*pcVar10)(plVar21,&local_58,uVar35,1);
                    (**(code **)(*(long *)(local_88 + *(long *)(*(long *)local_88 + -0x18)) + 0x30))
                              ();
                    local_f0 = local_f0 + 3;
                  } while (local_f0 != pdVar22);
                }
                (**(code **)(*(long *)local_88 + 0x10))();
                bVar34 = iVar15 != local_11c;
                iVar15 = iVar15 + 1;
              } while (bVar34);
            }
            if (local_88 != (KisSharedPtr *)0x0) {
              LOCK();
              pKVar1 = local_88 + *(long *)(*(long *)local_88 + -0x18) + 8;
              *(int *)pKVar1 = *(int *)pKVar1 + -1;
              UNLOCK();
              if (*(int *)pKVar1 == 0) {
                (**(code **)(*(long *)local_88 + 8))();
              }
            }
            if (local_d0 != (long *)0x0) {
              LOCK();
              piVar3 = (int *)((long)local_d0 + *(long *)(*local_d0 + -0x18) + 8);
              *piVar3 = *piVar3 + -1;
              UNLOCK();
              if (*piVar3 == 0) {
                (**(code **)(*local_d0 + 8))();
              }
            }
            if (local_d8 != (long *)0x0) {
              LOCK();
              piVar3 = (int *)((long)local_d8 + *(long *)(*local_d8 + -0x18) + 8);
              *piVar3 = *piVar3 + -1;
              UNLOCK();
              if (*piVar3 == 0) {
                (**(code **)(*local_d8 + 8))();
              }
            }
            operator_delete__(local_118);
          }
                    /* try { // try from 00164eb0 to 00164eb4 has its CatchHandler @ 00165a16 */
          uVar35 = KisPaintOp::painter();
          local_88 = local_e0;
          uVar19 = (ulong)((uStack_b0._4_4_ - local_b8._4_4_) + 1);
          if (local_e0 != (KisSharedPtr *)0x0) {
            LOCK();
            *(int *)(local_e0 + 8) = *(int *)(local_e0 + 8) + 1;
            UNLOCK();
          }
          local_c0 = *(KisSharedPtr **)(in_RSI + 0x1c0);
          if (local_c0 != (KisSharedPtr *)0x0) {
            LOCK();
            *(int *)((long)local_c0 + 0x10) = *(int *)((long)local_c0 + 0x10) + 1;
            UNLOCK();
          }
                    /* try { // try from 00164f2c to 00164f30 has its CatchHandler @ 00165abb */
          KisPainter::bitBltWithFixedSelection
                    (uVar35,local_b8 & 0xffffffff,local_b8._4_4_,(QString *)&local_c0,
                     (KisPainter *)&local_88,((int)uStack_b0 - (int)local_b8) + 1);
          if (local_c0 != (KisSharedPtr *)0x0) {
            LOCK();
            plVar20 = (long *)((long)local_c0 + 0x10);
            *(int *)plVar20 = *(int *)plVar20 + -1;
            UNLOCK();
            if (*(int *)plVar20 == 0) {
              (**(code **)(*(long *)local_c0 + 0x20))(local_c0,uVar19);
            }
          }
          if (local_88 != (KisSharedPtr *)0x0) {
            LOCK();
            pKVar1 = local_88 + 8;
            *(int *)pKVar1 = *(int *)pKVar1 + -1;
            UNLOCK();
            if (*(int *)pKVar1 == 0) {
              (**(code **)(*(long *)local_88 + 8))();
            }
          }
                    /* try { // try from 00164f6c to 00164f7c has its CatchHandler @ 00165a16 */
          uVar35 = KisPaintOp::painter();
          KisDabCache::needSeparateOriginal();
          local_88 = local_e0;
          if (local_e0 != (KisSharedPtr *)0x0) {
            LOCK();
            *(int *)(local_e0 + 8) = *(int *)(local_e0 + 8) + 1;
            UNLOCK();
          }
          local_c0 = *(KisSharedPtr **)(in_RSI + 0x1c0);
          if (local_c0 != (KisSharedPtr *)0x0) {
            LOCK();
            *(int *)((long)local_c0 + 0x10) = *(int *)((long)local_c0 + 0x10) + 1;
            UNLOCK();
          }
                    /* try { // try from 00164fdc to 00164fe0 has its CatchHandler @ 00165a8b */
          KisPainter::renderMirrorMaskSafe(uVar35,local_b8,uStack_b0,(QString *)&local_c0,0,0);
          if (local_c0 != (KisSharedPtr *)0x0) {
            LOCK();
            plVar20 = (long *)((long)local_c0 + 0x10);
            *(int *)plVar20 = *(int *)plVar20 + -1;
            UNLOCK();
            if (*(int *)plVar20 == 0) {
              (**(code **)(*(long *)local_c0 + 0x20))();
            }
          }
          if (local_88 != (KisSharedPtr *)0x0) {
            LOCK();
            pKVar1 = local_88 + 8;
            *(int *)pKVar1 = *(int *)pKVar1 + -1;
            UNLOCK();
            if (*(int *)pKVar1 == 0) {
              (**(code **)(*(long *)local_88 + 8))();
            }
          }
                    /* try { // try from 00165024 to 00165028 has its CatchHandler @ 00165a16 */
          KisBrushBasedPaintOp::effectiveSpacing(local_128);
        }
        if (local_e0 != (KisSharedPtr *)0x0) {
          LOCK();
          pKVar1 = local_e0 + 8;
          *(int *)pKVar1 = *(int *)pKVar1 + -1;
          UNLOCK();
          if (*(int *)pKVar1 == 0) {
            (**(code **)(*(long *)local_e0 + 8))();
          }
        }
      }
      else {
        *(undefined8 *)param_1 = 1;
        *(undefined8 *)(param_1 + 0x18) = 0;
        param_1[0x20] = (KisPaintInformation)0x0;
        *(undefined (*) [16])(param_1 + 8) = (undefined  [16])0x0;
      }
      if (local_130 != (KisSharedPtr *)0x0) {
        LOCK();
        pKVar1 = local_130 + 0x10;
        *(int *)pKVar1 = *(int *)pKVar1 + -1;
        UNLOCK();
        if (*(int *)pKVar1 == 0) {
          (**(code **)(*(long *)local_130 + 0x20))();
        }
      }
    }
    if (piVar8 == (int *)0x0) goto LAB_001644c9;
  }
  else {
    LOCK();
    *piVar8 = *piVar8 + 1;
    UNLOCK();
    LOCK();
    piVar8[1] = piVar8[1] + 1;
    dVar36 = DAT_0018a6c8;
    UNLOCK();
    if (pKVar7 != (KisDabShape *)0x0) goto LAB_00164402;
    *(undefined8 *)param_1 = 1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(double *)(param_1 + 8) = dVar36;
    *(double *)(param_1 + 0x10) = dVar36;
  }
  LOCK();
  piVar3 = piVar8 + 1;
  *piVar3 = *piVar3 + -1;
  UNLOCK();
  if (*piVar3 == 0) {
    (**(code **)(piVar8 + 2))(piVar8);
  }
  LOCK();
  *piVar8 = *piVar8 + -1;
  UNLOCK();
  if (*piVar8 == 0) {
    operator_delete(piVar8,0x10);
  }
LAB_001644c9:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return param_1;
}


// ====== paintBezierCurve @ 001ac638 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 001acd30 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


