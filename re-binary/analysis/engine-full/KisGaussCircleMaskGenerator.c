/* Class KisGaussCircleMaskGenerator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGaussCircleMaskGenerator @ 00204ca0 ======

void __thiscall
KisGaussCircleMaskGenerator::KisGaussCircleMaskGenerator
          (KisGaussCircleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,bool param_6)

{
  (*(code *)PTR_KisGaussCircleMaskGenerator_0083a120)();
  return;
}



// ====== KisGaussCircleMaskGenerator @ 00208fb0 ======

void __thiscall
KisGaussCircleMaskGenerator::KisGaussCircleMaskGenerator
          (KisGaussCircleMaskGenerator *this,KisGaussCircleMaskGenerator *param_1)

{
  (*(code *)PTR_KisGaussCircleMaskGenerator_0083c2a8)();
  return;
}



// ====== KisGaussCircleMaskGenerator @ 00580360 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisGaussCircleMaskGenerator::KisGaussCircleMaskGenerator(double, double, double, double, int,
   bool) */

void __thiscall
KisGaussCircleMaskGenerator::KisGaussCircleMaskGenerator
          (KisGaussCircleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,bool param_6)

{
  long *plVar1;
  double *pdVar2;
  long *plVar3;
  long in_FS_OFFSET;
  double dVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  KisGaussCircleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator
            ((KisMaskGenerator *)this,param_1,param_2,param_3,param_4,param_5,param_6,0,
             (KoID *)&DAT_00852d90);
  *(undefined **)this = PTR_vtable_00837e60 + 0x10;
                    /* try { // try from 005803b9 to 005803bd has its CatchHandler @ 0058057e */
  pdVar2 = (double *)operator_new(0x60);
  dVar8 = DAT_007227c0 / param_2;
  dVar5 = DAT_007227c0 - (param_3 + param_4) * DAT_007227c8;
  dVar6 = DAT_0072ea40;
  dVar7 = DAT_00735d48;
  dVar4 = DAT_00735d40;
  if ((dVar5 != 0.0) &&
     (dVar6 = DAT_0072ea48, dVar7 = DAT_00735d58, dVar4 = DAT_00735d50, dVar5 != DAT_007227c0)) {
    dVar7 = ((DAT_00735d60 * dVar5 - _DAT_00735d68) * DAT_007338a0) / (dVar5 * _DAT_00735d70);
    dVar4 = erf(dVar7);
    dVar6 = dVar5;
    dVar4 = _DAT_00722cb8 / (dVar4 + dVar4);
  }
  *(undefined *)(pdVar2 + 6) = 0;
  pdVar2[3] = 0.0;
  pdVar2[5] = 0.0;
  *(bool *)(pdVar2 + 9) = param_6;
  pdVar2[10] = (double)pdVar2;
  pdVar2[0xb] = 0.0;
  *(double **)(this + 0x18) = pdVar2;
  *(undefined (*) [16])(pdVar2 + 7) = (undefined  [16])0x0;
  *pdVar2 = dVar8;
  pdVar2[1] = dVar6;
  pdVar2[2] = dVar7;
  pdVar2[4] = dVar4;
  local_28 = this;
                    /* try { // try from 00580468 to 0058046c has its CatchHandler @ 00580572 */
  plVar3 = (long *)FUN_00580860(&local_28);
  plVar1 = (long *)pdVar2[0xb];
  if ((plVar3 == plVar1) || (pdVar2[0xb] = (double)plVar3, plVar1 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x005804aa. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar1 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisGaussCircleMaskGenerator @ 00580590 ======

/* KisGaussCircleMaskGenerator::KisGaussCircleMaskGenerator(KisGaussCircleMaskGenerator const&) */

void __thiscall
KisGaussCircleMaskGenerator::KisGaussCircleMaskGenerator
          (KisGaussCircleMaskGenerator *this,KisGaussCircleMaskGenerator *param_1)

{
  undefined uVar1;
  undefined uVar2;
  undefined8 *puVar3;
  long *plVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 *puVar11;
  long *plVar12;
  long in_FS_OFFSET;
  KisGaussCircleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator((KisMaskGenerator *)this,(KisMaskGenerator *)param_1);
  *(undefined **)this = PTR_vtable_00837e60 + 0x10;
                    /* try { // try from 005805cb to 005805cf has its CatchHandler @ 0058068a */
  puVar11 = (undefined8 *)operator_new(0x60);
  puVar3 = *(undefined8 **)(param_1 + 0x18);
  uVar5 = *puVar3;
  uVar6 = puVar3[1];
  uVar7 = puVar3[2];
  uVar8 = puVar3[3];
  uVar9 = puVar3[4];
  uVar10 = puVar3[5];
  puVar11[10] = puVar11;
  *puVar11 = uVar5;
  puVar11[1] = uVar6;
  puVar11[2] = uVar7;
  puVar11[3] = uVar8;
  puVar11[4] = uVar9;
  puVar11[5] = uVar10;
  uVar1 = *(undefined *)(puVar3 + 6);
  uVar5 = puVar3[7];
  uVar6 = puVar3[8];
  uVar2 = *(undefined *)(puVar3 + 9);
  puVar11[0xb] = 0;
  *(undefined *)(puVar11 + 6) = uVar1;
  *(undefined *)(puVar11 + 9) = uVar2;
  *(undefined8 **)(this + 0x18) = puVar11;
  puVar11[7] = uVar5;
  puVar11[8] = uVar6;
  local_28 = this;
                    /* try { // try from 0058061f to 00580623 has its CatchHandler @ 0058067e */
  plVar12 = (long *)FUN_00580860(&local_28);
  plVar4 = (long *)puVar11[0xb];
  if ((plVar12 == plVar4) || (puVar11[0xb] = plVar12, plVar4 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x00580655. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar4 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



