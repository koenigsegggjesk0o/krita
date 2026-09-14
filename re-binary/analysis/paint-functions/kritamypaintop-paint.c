/* Painting functions extracted from kritamypaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintAt @ 0015bca0 ======

/* KisMyPaintPaintOp::paintAt(KisPaintInformation const&) [clone .cold] */

void __thiscall
KisMyPaintPaintOp::paintAt
          (KisMyPaintPaintOp *this,KisPaintInformation *param_2,undefined param_3,undefined param_4,
          undefined param_5,undefined param_6,undefined param_7,undefined param_8,undefined param_9,
          undefined param_10,KisSharedPtr *param_11,KisSharedPtr *param_12,long param_13)

{
  long in_FS_OFFSET;
  
  KisSharedPtr<KisDefaultBoundsBase>::deref(param_12,(KisDefaultBoundsBase *)param_2);
  KisSharedPtr<KisPaintDevice>::deref(param_11,(KisPaintDevice *)param_2);
  if (param_13 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 0018e8b0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisMyPaintPaintOp::paintAt(KisPaintInformation const&) */

KisMyPaintPaintOp * __thiscall
KisMyPaintPaintOp::paintAt(KisMyPaintPaintOp *this,KisPaintInformation *param_1)

{
  long *plVar1;
  long lVar2;
  double dVar3;
  int iVar4;
  long lVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  byte bVar8;
  long in_FS_OFFSET;
  float fVar9;
  double dVar10;
  double dVar11;
  long *local_40;
  long *local_38;
  
  lVar2 = *(long *)(in_FS_OFFSET + 0x28);
  lVar5 = KisPaintOp::painter();
  dVar3 = DAT_00225750;
  if (lVar5 == 0) {
    *(undefined8 *)this = 1;
    *(undefined8 *)(this + 0x18) = 0;
    this[0x20] = (KisMyPaintPaintOp)0x0;
    *(double *)(this + 8) = dVar3;
    *(double *)(this + 0x10) = dVar3;
  }
  else {
    KisPaintOp::painter();
    KisPainter::device();
                    /* try { // try from 0018e909 to 0018e90d has its CatchHandler @ 0018ecae */
    KisPaintDevice::defaultBounds();
                    /* try { // try from 0018e916 to 0018e918 has its CatchHandler @ 0018eca2 */
    iVar4 = (**(code **)(*local_38 + 0x30))();
    dVar3 = DAT_00225750;
    dVar11 = DAT_00225750;
    if (0 < iVar4) {
      dVar11 = DAT_00225750 / (double)(1 << ((byte)iVar4 & 0x1f));
    }
    if (local_38 != (long *)0x0) {
      LOCK();
      plVar1 = local_38 + 1;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*local_38 + 8))();
      }
    }
    if (local_40 != (long *)0x0) {
      LOCK();
      plVar1 = local_40 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*local_40 + 0x20))();
      }
    }
    log(dVar11 * *(double *)(param_1 + 0x68));
    uVar6 = KisMyPaintPaintOpPreset::brush(*(KisMyPaintPaintOpPreset **)(param_1 + 0x20));
    mypaint_brush_set_base_value(uVar6,3);
    uVar6 = KisMyPaintPaintOpPreset::brush(*(KisMyPaintPaintOpPreset **)(param_1 + 0x20));
    fVar9 = (float)mypaint_brush_get_state(uVar6,0x15);
    param_1[0x78] = (KisPaintInformation)(fVar9 != 0.0);
    if (fVar9 != 0.0) {
      dVar10 = (double)KisPaintInformation::currentTime();
      dVar10 = (double)((ulong)(dVar10 - *(double *)(param_1 + 0x70)) & _DAT_0022f5b0) *
               _DAT_0022f588;
    }
    else {
      KisPaintInformation::yTilt();
      KisPaintInformation::xTilt();
      KisPaintInformation::pressure();
      KisPaintInformation::pos();
      KisPaintInformation::pos();
      uVar6 = KisMyPaintSurface::surface(*(KisMyPaintSurface **)(param_1 + 0x28));
      uVar7 = KisMyPaintPaintOpPreset::brush(*(KisMyPaintPaintOpPreset **)(param_1 + 0x20));
      mypaint_brush_stroke_to(uVar7,uVar6);
      dVar10 = DAT_0022f580;
    }
    *(double *)(param_1 + 0x60) = dVar10;
    KisPaintInformation::yTilt();
    KisPaintInformation::xTilt();
    KisPaintInformation::pressure();
    KisPaintInformation::pos();
    KisPaintInformation::pos();
    uVar6 = KisMyPaintSurface::surface(*(KisMyPaintSurface **)(param_1 + 0x28));
    uVar7 = KisMyPaintPaintOpPreset::brush(*(KisMyPaintPaintOpPreset **)(param_1 + 0x20));
    mypaint_brush_stroke_to(uVar7,uVar6);
    uVar6 = KisPaintInformation::currentTime();
    bVar8 = true;
    *(undefined8 *)(param_1 + 0x70) = uVar6;
    dVar10 = *(double *)(param_1 + 0x68) + *(double *)(param_1 + 0x68);
    if (param_1[0x38] != (KisPaintInformation)0x0) {
      bVar8 = (byte)param_1[0x48] ^ 1;
    }
    KisPaintOpUtils::effectiveSpacing
              (dVar10,dVar10,dVar3,SUB81(this,0),(bool)bVar8,0.0,false,dVar10,false,dVar3,dVar11);
  }
  if (lVar2 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return this;
}


// ====== paintBezierCurve @ 002bf4f8 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 002bfae0 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


