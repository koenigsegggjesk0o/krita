/* Painting functions extracted from kritaspraypaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintAt @ 00126052 ======

/* KisSprayPaintOp::paintAt(KisPaintInformation const&) [clone .cold] */

void __thiscall
KisSprayPaintOp::paintAt
          (KisSprayPaintOp *this,KisPaintInformation *param_2,undefined param_3,undefined param_4,
          undefined param_5,undefined param_6,undefined param_7,undefined param_8,undefined param_9,
          undefined param_10,KisSharedPtr *param_11,long param_12)

{
  long in_FS_OFFSET;
  
  KisSharedPtr<KisPaintDevice>::deref(param_11,(KisPaintDevice *)param_2);
  if (param_12 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 00142000 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSprayPaintOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisSprayPaintOp::paintAt(KisPaintInformation *param_1)

{
  long *plVar1;
  double dVar2;
  char cVar3;
  int iVar4;
  long lVar5;
  KisPaintInformation *pKVar6;
  bool in_DL;
  byte bVar7;
  long in_RSI;
  long *plVar8;
  long *plVar9;
  long in_FS_OFFSET;
  undefined8 uVar10;
  double local_90;
  double local_88;
  long *local_68;
  long *local_60;
  undefined local_58 [16];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  lVar5 = KisPaintOp::painter();
  if ((lVar5 == 0) || (*(char *)(in_RSI + 0x270) == '\0')) {
    uVar10 = *(undefined8 *)(in_RSI + 0x268);
    *param_1 = (KisPaintInformation)0x1;
    *(undefined8 *)(param_1 + 0x18) = 0;
    param_1[0x20] = (KisPaintInformation)0x0;
    *(undefined8 *)(param_1 + 8) = uVar10;
    *(undefined8 *)(param_1 + 0x10) = uVar10;
  }
  else {
    if (*(long **)(in_RSI + 0x160) == (long *)0x0) {
      KisPaintOp::source();
                    /* try { // try from 001423dd to 001423e1 has its CatchHandler @ 00142469 */
      KisPaintDevice::createCompositionSourceDevice();
      plVar8 = *(long **)(in_RSI + 0x160);
      plVar9 = plVar8;
      if ((long *)local_58._0_8_ != plVar8) {
        if ((long *)local_58._0_8_ != (long *)0x0) {
          LOCK();
          *(int *)(local_58._0_8_ + 0x10) = *(int *)(local_58._0_8_ + 0x10) + 1;
          UNLOCK();
          plVar8 = *(long **)(in_RSI + 0x160);
        }
        *(undefined8 *)(in_RSI + 0x160) = local_58._0_8_;
        plVar9 = (long *)local_58._0_8_;
        if (plVar8 != (long *)0x0) {
          LOCK();
          plVar1 = plVar8 + 2;
          *(int *)plVar1 = *(int *)plVar1 + -1;
          UNLOCK();
          if (*(int *)plVar1 == 0) {
            (**(code **)(*plVar8 + 0x20))();
            plVar9 = (long *)local_58._0_8_;
          }
        }
      }
      if (plVar9 != (long *)0x0) {
        LOCK();
        plVar8 = plVar9 + 2;
        *(int *)plVar8 = *(int *)plVar8 + -1;
        UNLOCK();
        if (*(int *)plVar8 == 0) {
          (**(code **)(*plVar9 + 0x20))();
        }
      }
      if (local_60 != (long *)0x0) {
        LOCK();
        plVar8 = local_60 + 2;
        *(int *)plVar8 = *(int *)plVar8 + -1;
        UNLOCK();
        if (*(int *)plVar8 == 0) {
          (**(code **)(*local_60 + 0x20))();
        }
      }
    }
    else {
      (**(code **)(**(long **)(in_RSI + 0x160) + 0x68))();
    }
    uVar10 = KisRotationOption::apply((KisPaintInformation *)(in_RSI + 0x290));
    pKVar6 = (KisPaintInformation *)KisPaintOp::painter();
    KisOpacityOption::apply((KisPainter *)(in_RSI + 0x310),pKVar6);
    cVar3 = KisCurveOption::isChecked();
    local_88 = _DAT_001b2d98;
    if (cVar3 != '\0') {
      local_88 = (double)KisCurveOption::computeSizeLikeValue
                                   ((KisPaintInformation *)(in_RSI + 0x2d8),in_DL);
    }
    KisPaintOp::painter();
    KisPainter::device();
                    /* try { // try from 0014211f to 00142123 has its CatchHandler @ 00142499 */
    KisPaintDevice::defaultBounds();
                    /* try { // try from 0014212c to 0014212e has its CatchHandler @ 001424a5 */
    iVar4 = (**(code **)(*(long *)local_58._0_8_ + 0x30))();
    dVar2 = _DAT_001b2d98;
    if (iVar4 < 1) {
      local_90 = _DAT_001b2d98;
    }
    else {
      local_90 = _DAT_001b2d98 / (double)(1 << ((byte)iVar4 & 0x1f));
    }
    if ((long *)local_58._0_8_ != (long *)0x0) {
      LOCK();
      plVar8 = (long *)(local_58._0_8_ + 8);
      *(int *)plVar8 = *(int *)plVar8 + -1;
      UNLOCK();
      if (*(int *)plVar8 == 0) {
        (**(code **)(*(long *)local_58._0_8_ + 8))();
      }
    }
    if (local_60 != (long *)0x0) {
      LOCK();
      plVar8 = local_60 + 2;
      *(int *)plVar8 = *(int *)plVar8 + -1;
      UNLOCK();
      if (*(int *)plVar8 == 0) {
        (**(code **)(*local_60 + 0x20))();
      }
    }
    KisPaintOp::painter();
    KisPainter::backgroundColor();
    KisPaintOp::painter();
    KisPainter::paintColor();
    (**(code **)(**(long **)(in_RSI + 0x388) + 0x60))(&local_60);
    local_58._0_8_ = *(long *)(in_RSI + 0x160);
    if ((long *)local_58._0_8_ != (long *)0x0) {
      LOCK();
      *(int *)(local_58._0_8_ + 0x10) = *(int *)(local_58._0_8_ + 0x10) + 1;
      UNLOCK();
    }
                    /* try { // try from 0014221f to 00142223 has its CatchHandler @ 00142475 */
    SprayBrush::paint(uVar10,local_88,local_90,in_RSI + 0x168,local_58,&local_60);
    if ((long *)local_58._0_8_ != (long *)0x0) {
      LOCK();
      plVar8 = (long *)(local_58._0_8_ + 0x10);
      *(int *)plVar8 = *(int *)plVar8 + -1;
      UNLOCK();
      if (*(int *)plVar8 == 0) {
        (**(code **)(*(long *)local_58._0_8_ + 0x20))();
      }
    }
    if (local_60 != (long *)0x0) {
      LOCK();
      plVar8 = local_60 + 2;
      *(int *)plVar8 = *(int *)plVar8 + -1;
      UNLOCK();
      if (*(int *)plVar8 == 0) {
        (**(code **)(*local_60 + 0x20))();
      }
    }
    local_58 = KisPaintDevice::extent();
    uVar10 = KisPaintOp::painter();
    local_60 = *(long **)(in_RSI + 0x160);
    if (local_60 != (long *)0x0) {
      LOCK();
      *(int *)(local_60 + 2) = *(int *)(local_60 + 2) + 1;
      UNLOCK();
    }
    local_68 = (long *)local_58._0_8_;
                    /* try { // try from 001422ac to 001422b0 has its CatchHandler @ 00142481 */
    KisPainter::bitBlt(uVar10,&local_68,&local_60,local_58);
    if (local_60 != (long *)0x0) {
      LOCK();
      plVar8 = local_60 + 2;
      *(int *)plVar8 = *(int *)plVar8 + -1;
      UNLOCK();
      if (*(int *)plVar8 == 0) {
        (**(code **)(*local_60 + 0x20))();
      }
    }
    uVar10 = KisPaintOp::painter();
    local_60 = *(long **)(in_RSI + 0x160);
    if (local_60 != (long *)0x0) {
      LOCK();
      *(int *)(local_60 + 2) = *(int *)(local_60 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 001422fe to 00142302 has its CatchHandler @ 0014248d */
    KisPainter::renderMirrorMask(uVar10,local_58._0_8_,local_58._8_8_);
    if (local_60 != (long *)0x0) {
      LOCK();
      plVar8 = local_60 + 2;
      *(int *)plVar8 = *(int *)plVar8 + -1;
      UNLOCK();
      if (*(int *)plVar8 == 0) {
        (**(code **)(*local_60 + 0x20))();
      }
    }
    bVar7 = true;
    if (*(char *)(in_RSI + 0x278) != '\0') {
      bVar7 = *(byte *)(in_RSI + 0x288) ^ 1;
    }
    KisPaintOpUtils::effectiveSpacing
              (dVar2,dVar2,dVar2,SUB81(param_1,0),(bool)bVar7,0.0,true,
               local_90 * *(double *)(in_RSI + 0x268),false,dVar2,local_90);
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return param_1;
}


// ====== paintBezierCurve @ 001e94f8 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 001e9b40 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


