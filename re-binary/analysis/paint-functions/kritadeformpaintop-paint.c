/* Painting functions extracted from kritadeformpaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintAt @ 00119eaa ======

/* KisDeformPaintOp::paintAt(KisPaintInformation const&) [clone .cold] */

void __thiscall KisDeformPaintOp::paintAt(KisDeformPaintOp *this,KisPaintInformation *param_1)

{
  long unaff_RBP;
  long in_FS_OFFSET;
  
  KisSharedPtr<KisRandomSource>::deref
            (*(KisSharedPtr **)(unaff_RBP + -0x40),(KisRandomSource *)param_1);
  KisSharedPtr<KisFixedPaintDevice>::deref
            (*(KisSharedPtr **)(unaff_RBP + -0x70),(KisFixedPaintDevice *)param_1);
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 0012bc20 ======

/* KisDeformPaintOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisDeformPaintOp::paintAt(KisPaintInformation *param_1)

{
  KisRandomSource *pKVar1;
  long *plVar2;
  double dVar3;
  double dVar4;
  KisSharedPtr KVar5;
  uint uVar6;
  char cVar7;
  int iVar8;
  int iVar9;
  QRect QVar10;
  long lVar11;
  double *pdVar12;
  KisPaintInformation *pKVar13;
  bool in_DL;
  int extraout_EDX;
  int extraout_EDX_00;
  undefined8 extraout_RDX;
  long *in_RSI;
  long in_FS_OFFSET;
  double dVar14;
  double dVar15;
  undefined auVar16 [16];
  ulong uVar17;
  double local_b0;
  double local_a8;
  double local_98;
  KisSharedPtr local_80;
  uint local_7c;
  long *local_78;
  double local_70;
  double local_68;
  KisRandomSource *local_60;
  KisRandomSource *local_58;
  long *local_50;
  KisRandomSource *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  lVar11 = KisPaintOp::painter();
  if ((lVar11 == 0) || (in_RSI[5] == 0)) {
    lVar11 = in_RSI[0x40];
    *param_1 = (KisPaintInformation)0x1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(long *)(param_1 + 8) = lVar11;
    *(long *)(param_1 + 0x10) = lVar11;
  }
  else {
    KisPaintOp::source();
                    /* try { // try from 0012bc86 to 0012bc97 has its CatchHandler @ 0012c2fb */
    (**(code **)(*(long *)local_48 + 0x70))();
    KisPaintOp::cachedDab((KoColorSpace *)&local_78);
    if (local_48 != (KisRandomSource *)0x0) {
      LOCK();
      pKVar1 = local_48 + 0x10;
      *(int *)pKVar1 = *(int *)pKVar1 + -1;
      UNLOCK();
      if (*(int *)pKVar1 == 0) {
        (**(code **)(*(long *)local_48 + 0x20))();
      }
    }
                    /* try { // try from 0012bcaf to 0012bcf4 has its CatchHandler @ 0012c2e3 */
    pdVar12 = (double *)KisPaintInformation::pos();
    local_a8 = *pdVar12;
    local_98 = pdVar12[1];
    if (*(char *)(in_RSI + 0x1b) != '\0') {
      dVar15 = (double)in_RSI[0x14];
      KisPaintInformation::randomSource();
                    /* try { // try from 0012bcf9 to 0012bcfd has its CatchHandler @ 0012c2d7 */
      dVar14 = (double)KisRandomSource::generateNormalized();
      pKVar1 = local_48;
      local_b0 = (double)in_RSI[0x14];
      local_a8 = (dVar14 * dVar15 - DAT_00162d80 * local_b0) * (double)in_RSI[0x1a] + local_a8;
      if (local_48 != (KisRandomSource *)0x0) {
        LOCK();
        *(int *)local_48 = *(int *)local_48 + -1;
        UNLOCK();
        if (*(int *)local_48 == 0) {
          KisRandomSource::~KisRandomSource(local_48);
          operator_delete(pKVar1,0x18);
          local_b0 = (double)in_RSI[0x14];
        }
        else {
          local_b0 = (double)in_RSI[0x14];
        }
      }
                    /* try { // try from 0012bd69 to 0012bd6d has its CatchHandler @ 0012c2e3 */
      KisPaintInformation::randomSource();
                    /* try { // try from 0012bd72 to 0012bd76 has its CatchHandler @ 0012c2a7 */
      dVar15 = (double)KisRandomSource::generateNormalized();
      local_98 = (dVar15 * local_b0 - DAT_00162d80 * (double)in_RSI[0x14]) * (double)in_RSI[0x1a] +
                 local_98;
      if (local_48 != (KisRandomSource *)0x0) {
        LOCK();
        *(int *)local_48 = *(int *)local_48 + -1;
        UNLOCK();
        if (*(int *)local_48 == 0) {
          KisRandomSource::~KisRandomSource(local_48);
          operator_delete(local_48,0x18);
        }
      }
    }
                    /* try { // try from 0012bdca to 0012beb0 has its CatchHandler @ 0012c2e3 */
    dVar14 = (double)KisRotationOption::apply((KisPaintInformation *)(in_RSI + 0x2e));
    cVar7 = KisCurveOption::isChecked();
    dVar15 = DAT_00162da8;
    if (cVar7 != '\0') {
                    /* try { // try from 0012c1ab to 0012c1af has its CatchHandler @ 0012c2e3 */
      dVar15 = (double)KisCurveOption::computeSizeLikeValue
                                 ((KisPaintInformation *)(in_RSI + 0x1f),in_DL);
    }
    dVar3 = (double)in_RSI[0x17];
    dVar4 = (double)in_RSI[0x16];
    auVar16 = DeformBrush::hotSpot((DeformBrush *)(in_RSI + 6),dVar15 * dVar3,dVar14 + dVar4);
    KisPaintOp::splitCoordinate(local_a8 - auVar16._0_8_,(int *)&local_80,&local_70);
    KisPaintOp::splitCoordinate(local_98 - auVar16._8_8_,(int *)&local_7c,&local_68);
    uVar6 = local_7c;
    KVar5 = local_80;
    pdVar12 = (double *)KisPaintInformation::pos();
    KisPaintInformation::randomSource();
    local_48 = (KisRandomSource *)in_RSI[5];
    if (local_48 != (KisRandomSource *)0x0) {
      LOCK();
      *(int *)(local_48 + 0x10) = *(int *)(local_48 + 0x10) + 1;
      UNLOCK();
    }
    local_50 = local_78;
    if (local_78 != (long *)0x0) {
      LOCK();
      *(int *)(local_78 + 1) = *(int *)(local_78 + 1) + 1;
      UNLOCK();
    }
    uVar17 = (ulong)uVar6;
                    /* try { // try from 0012bf2f to 0012bf33 has its CatchHandler @ 0012c2b3 */
    DeformBrush::paintMask
              ((KisSharedPtr)&local_60,(KisSharedPtr)(DeformBrush *)(in_RSI + 6),
               (KisSharedPtr)&local_50,dVar15 * dVar3,dVar14 + dVar4,(QPointF)&local_48,*pdVar12,
               pdVar12[1],(int)&local_58,KVar5,uVar6);
    if (local_50 != (long *)0x0) {
      LOCK();
      plVar2 = local_50 + 1;
      *(int *)plVar2 = *(int *)plVar2 + -1;
      UNLOCK();
      if (*(int *)plVar2 == 0) {
        (**(code **)(*local_50 + 8))(local_50,0x12beb1,extraout_RDX,uVar17);
      }
    }
    if (local_48 != (KisRandomSource *)0x0) {
      LOCK();
      pKVar1 = local_48 + 0x10;
      *(int *)pKVar1 = *(int *)pKVar1 + -1;
      UNLOCK();
      if (*(int *)pKVar1 == 0) {
        (**(code **)(*(long *)local_48 + 0x20))();
      }
    }
    if (local_58 != (KisRandomSource *)0x0) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + -1;
      UNLOCK();
      if (*(int *)local_58 == 0) {
        KisRandomSource::~KisRandomSource(local_58);
        operator_delete(local_58,0x18);
      }
    }
    if (local_60 != (KisRandomSource *)0x0) {
                    /* try { // try from 0012bf88 to 0012bfc9 has its CatchHandler @ 0012c2bf */
      pKVar13 = (KisPaintInformation *)KisPaintOp::painter();
      KisOpacityOption::apply((KisPainter *)(in_RSI + 0x26),pKVar13);
      iVar8 = KisPaintOp::painter();
      KisFixedPaintDevice::bounds();
      iVar9 = KisFixedPaintDevice::bounds();
      local_48 = local_60;
      if (local_60 != (KisRandomSource *)0x0) {
        LOCK();
        *(int *)(local_60 + 8) = *(int *)(local_60 + 8) + 1;
        UNLOCK();
      }
      local_50 = local_78;
      if (local_78 != (long *)0x0) {
        LOCK();
        *(int *)(local_78 + 1) = *(int *)(local_78 + 1) + 1;
        UNLOCK();
      }
                    /* try { // try from 0012c00d to 0012c011 has its CatchHandler @ 0012c2ef */
      KisPainter::bltFixedWithFixedSelection
                (iVar8,local_80,local_7c,(KisSharedPtr)&local_50,(QPointF)&local_48,
                 (extraout_EDX - iVar9) + 1);
      if (local_50 != (long *)0x0) {
        LOCK();
        plVar2 = local_50 + 1;
        *(int *)plVar2 = *(int *)plVar2 + -1;
        UNLOCK();
        if (*(int *)plVar2 == 0) {
          (**(code **)(*local_50 + 8))();
        }
      }
      if (local_48 != (KisRandomSource *)0x0) {
        LOCK();
        pKVar1 = local_48 + 8;
        *(int *)pKVar1 = *(int *)pKVar1 + -1;
        UNLOCK();
        if (*(int *)pKVar1 == 0) {
          (**(code **)(*(long *)local_48 + 8))();
        }
      }
                    /* try { // try from 0012c03f to 0012c043 has its CatchHandler @ 0012c2bf */
      QVar10 = KisPaintOp::painter();
      local_48 = local_60;
      if (local_60 != (KisRandomSource *)0x0) {
        LOCK();
        *(int *)(local_60 + 8) = *(int *)(local_60 + 8) + 1;
        UNLOCK();
      }
      local_50 = local_78;
      if (local_78 != (long *)0x0) {
        LOCK();
        *(int *)(local_78 + 1) = *(int *)(local_78 + 1) + 1;
        UNLOCK();
      }
                    /* try { // try from 0012c06f to 0012c0e5 has its CatchHandler @ 0012c2cb */
      KisFixedPaintDevice::bounds();
      iVar8 = KisFixedPaintDevice::bounds();
      KisPainter::renderMirrorMask(QVar10,local_80,local_80 + (extraout_EDX_00 - iVar8));
      if (local_50 != (long *)0x0) {
        LOCK();
        plVar2 = local_50 + 1;
        *(int *)plVar2 = *(int *)plVar2 + -1;
        UNLOCK();
        if (*(int *)plVar2 == 0) {
          (**(code **)(*local_50 + 8))();
        }
      }
      if (local_48 != (KisRandomSource *)0x0) {
        LOCK();
        pKVar1 = local_48 + 8;
        *(int *)pKVar1 = *(int *)pKVar1 + -1;
        UNLOCK();
        if (*(int *)pKVar1 == 0) {
          (**(code **)(*(long *)local_48 + 8))();
        }
      }
    }
                    /* try { // try from 0012c120 to 0012c122 has its CatchHandler @ 0012c2bf */
    (**(code **)(*in_RSI + 0x38))(param_1);
    if (local_60 != (KisRandomSource *)0x0) {
      LOCK();
      pKVar1 = local_60 + 8;
      *(int *)pKVar1 = *(int *)pKVar1 + -1;
      UNLOCK();
      if (*(int *)pKVar1 == 0) {
        (**(code **)(*(long *)local_60 + 8))();
      }
    }
    if (local_78 != (long *)0x0) {
      LOCK();
      plVar2 = local_78 + 1;
      *(int *)plVar2 = *(int *)plVar2 + -1;
      UNLOCK();
      if (*(int *)plVar2 == 0) {
        (**(code **)(*local_78 + 8))();
      }
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return param_1;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintBezierCurve @ 001803c0 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 00180918 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


