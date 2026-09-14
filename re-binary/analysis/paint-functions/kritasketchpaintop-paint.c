/* Painting functions extracted from kritasketchpaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintLine @ 0011b6a0 ======

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  (*(code *)PTR_paintLine_001784c0)();
  return;
}


// ====== paintAt @ 0012c1e0 ======

/* KisSketchPaintOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisSketchPaintOp::paintAt(KisPaintInformation *param_1)

{
  long lVar1;
  KisPaintInformation *in_RDX;
  KisSketchPaintOp *in_RSI;
  long in_FS_OFFSET;
  
  lVar1 = *(long *)(in_FS_OFFSET + 0x28);
  doPaintLine(in_RSI,in_RDX,in_RDX);
  (**(code **)(*(long *)in_RSI + 0x38))(param_1);
  if (lVar1 == *(long *)(in_FS_OFFSET + 0x28)) {
    return param_1;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 0012c240 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSketchPaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) */

void KisSketchPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  bool bVar1;
  double *pdVar2;
  double *pdVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  
  pdVar2 = (double *)KisPaintInformation::pos();
  pdVar3 = (double *)KisPaintInformation::pos();
  dVar5 = *pdVar2;
  dVar4 = *pdVar3;
  dVar7 = pdVar3[1];
  dVar8 = pdVar2[1];
  if (dVar4 == 0.0) {
LAB_0012c2aa:
    dVar4 = dVar4 - dVar5;
    if (dVar4 < 0.0) {
      dVar4 = (double)((ulong)dVar4 ^ _DAT_00158330);
    }
    if (DAT_001582f0 < dVar4) goto LAB_0012c384;
  }
  else {
    bVar1 = dVar4 == 0.0;
    if (dVar5 == 0.0) {
      bVar1 = !NAN(dVar5);
    }
    if (bVar1) goto LAB_0012c2aa;
    dVar9 = dVar4 - dVar5;
    if (dVar9 < 0.0) {
      dVar9 = (double)((ulong)dVar9 ^ _DAT_00158330);
    }
    if (dVar5 < 0.0) {
      dVar5 = (double)((ulong)dVar5 ^ _DAT_00158330);
      if (dVar4 < 0.0) goto LAB_0012c420;
LAB_0012c376:
      dVar6 = dVar4;
      if (dVar5 <= dVar4) {
        dVar6 = dVar5;
      }
    }
    else {
      if (0.0 <= dVar4) goto LAB_0012c376;
LAB_0012c420:
      dVar6 = (double)((ulong)dVar4 ^ _DAT_00158330);
      if (dVar5 <= (double)((ulong)dVar4 ^ _DAT_00158330)) {
        dVar6 = dVar5;
      }
    }
    if (dVar6 < dVar9 * DAT_001582f8) goto LAB_0012c384;
  }
  dVar5 = dVar7 - dVar8;
  if (dVar7 != 0.0) {
    bVar1 = dVar7 == 0.0;
    if (dVar8 == 0.0) {
      bVar1 = !NAN(dVar8);
    }
    if (!bVar1) {
      if (dVar5 < 0.0) {
        dVar5 = (double)((ulong)dVar5 ^ _DAT_00158330);
      }
      if (dVar8 < 0.0) {
        dVar8 = (double)((ulong)dVar8 ^ _DAT_00158330);
      }
      if (dVar7 < 0.0) {
        dVar7 = (double)((ulong)dVar7 ^ _DAT_00158330);
      }
      if (dVar8 <= dVar7) {
        dVar7 = dVar8;
      }
      if (dVar7 < dVar5 * DAT_001582f8) goto LAB_0012c384;
      goto LAB_0012c31f;
    }
  }
  if (dVar5 < 0.0) {
    dVar5 = (double)((ulong)dVar5 ^ _DAT_00158330);
  }
  if (DAT_001582f0 < dVar5) {
LAB_0012c384:
    doPaintLine((KisSketchPaintOp *)param_1,param_2,(KisPaintInformation *)param_3);
    return;
  }
LAB_0012c31f:
  KisPaintOp::paintLine(param_1,param_2,param_3);
  return;
}


// ====== paintBezierCurve @ 001793e0 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 001798f0 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


