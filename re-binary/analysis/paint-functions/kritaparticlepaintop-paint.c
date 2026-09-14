/* Painting functions extracted from kritaparticlepaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintLine @ 00112260 ======

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  (*(code *)PTR_paintLine_001543a0)();
  return;
}


// ====== paintAt @ 0011d070 ======

/* KisParticlePaintOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisParticlePaintOp::paintAt(KisPaintInformation *param_1)

{
  long lVar1;
  KisPaintInformation *in_RDX;
  KisPaintInformation *in_RSI;
  long in_FS_OFFSET;
  
  lVar1 = *(long *)(in_FS_OFFSET + 0x28);
  doPaintLine(in_RSI,in_RDX);
  (**(code **)(*(long *)in_RSI + 0x38))(param_1);
  if (lVar1 == *(long *)(in_FS_OFFSET + 0x28)) {
    return param_1;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 0011d0d0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisParticlePaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) */

void KisParticlePaintOp::paintLine
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
LAB_0011d13a:
    dVar4 = dVar4 - dVar5;
    if (dVar4 < 0.0) {
      dVar4 = (double)((ulong)dVar4 ^ _DAT_00140f50);
    }
    if (DAT_00140f20 < dVar4) goto LAB_0011d214;
  }
  else {
    bVar1 = dVar4 == 0.0;
    if (dVar5 == 0.0) {
      bVar1 = !NAN(dVar5);
    }
    if (bVar1) goto LAB_0011d13a;
    dVar9 = dVar4 - dVar5;
    if (dVar9 < 0.0) {
      dVar9 = (double)((ulong)dVar9 ^ _DAT_00140f50);
    }
    if (dVar5 < 0.0) {
      dVar5 = (double)((ulong)dVar5 ^ _DAT_00140f50);
      if (dVar4 < 0.0) goto LAB_0011d2b0;
LAB_0011d206:
      dVar6 = dVar4;
      if (dVar5 <= dVar4) {
        dVar6 = dVar5;
      }
    }
    else {
      if (0.0 <= dVar4) goto LAB_0011d206;
LAB_0011d2b0:
      dVar6 = (double)((ulong)dVar4 ^ _DAT_00140f50);
      if (dVar5 <= (double)((ulong)dVar4 ^ _DAT_00140f50)) {
        dVar6 = dVar5;
      }
    }
    if (dVar6 < dVar9 * DAT_00140f28) goto LAB_0011d214;
  }
  dVar5 = dVar7 - dVar8;
  if (dVar7 != 0.0) {
    bVar1 = dVar7 == 0.0;
    if (dVar8 == 0.0) {
      bVar1 = !NAN(dVar8);
    }
    if (!bVar1) {
      if (dVar5 < 0.0) {
        dVar5 = (double)((ulong)dVar5 ^ _DAT_00140f50);
      }
      if (dVar8 < 0.0) {
        dVar8 = (double)((ulong)dVar8 ^ _DAT_00140f50);
      }
      if (dVar7 < 0.0) {
        dVar7 = (double)((ulong)dVar7 ^ _DAT_00140f50);
      }
      if (dVar8 <= dVar7) {
        dVar7 = dVar8;
      }
      if (dVar7 < dVar5 * DAT_00140f28) goto LAB_0011d214;
      goto LAB_0011d1af;
    }
  }
  if (dVar5 < 0.0) {
    dVar5 = (double)((ulong)dVar5 ^ _DAT_00140f50);
  }
  if (DAT_00140f20 < dVar5) {
LAB_0011d214:
    doPaintLine(param_1,param_2);
    return;
  }
LAB_0011d1af:
  KisPaintOp::paintLine(param_1,param_2,param_3);
  return;
}


// ====== paintBezierCurve @ 00155318 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 00155750 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


