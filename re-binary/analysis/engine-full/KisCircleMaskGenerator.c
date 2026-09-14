/* Class KisCircleMaskGenerator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCircleMaskGenerator @ 00202270 ======

void __thiscall
KisCircleMaskGenerator::KisCircleMaskGenerator
          (KisCircleMaskGenerator *this,double param_1,double param_2,double param_3,double param_4,
          int param_5,bool param_6)

{
  (*(code *)PTR_KisCircleMaskGenerator_00838c08)();
  return;
}



// ====== KisCircleMaskGenerator @ 00202cf0 ======

void __thiscall
KisCircleMaskGenerator::KisCircleMaskGenerator
          (KisCircleMaskGenerator *this,KisCircleMaskGenerator *param_1)

{
  (*(code *)PTR_KisCircleMaskGenerator_00839148)();
  return;
}



// ====== KisCircleMaskGenerator @ 0057e2b0 ======

/* KisCircleMaskGenerator::KisCircleMaskGenerator(KisCircleMaskGenerator const&) */

void __thiscall
KisCircleMaskGenerator::KisCircleMaskGenerator
          (KisCircleMaskGenerator *this,KisCircleMaskGenerator *param_1)

{
  undefined8 uVar1;
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
  KisCircleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator((KisMaskGenerator *)this,(KisMaskGenerator *)param_1);
  *(undefined **)this = PTR_vtable_00837038 + 0x10;
                    /* try { // try from 0057e2eb to 0057e2ef has its CatchHandler @ 0057e39a */
  puVar11 = (undefined8 *)operator_new(0x48);
  puVar3 = *(undefined8 **)(param_1 + 0x18);
  uVar5 = *puVar3;
  uVar6 = puVar3[1];
  uVar7 = puVar3[2];
  uVar8 = puVar3[3];
  uVar9 = puVar3[4];
  uVar10 = puVar3[5];
  uVar1 = puVar3[6];
  uVar2 = *(undefined *)(puVar3 + 7);
  puVar11[8] = 0;
  *(undefined8 **)(this + 0x18) = puVar11;
  *(undefined *)(puVar11 + 7) = uVar2;
  *puVar11 = uVar5;
  puVar11[1] = uVar6;
  puVar11[2] = uVar7;
  puVar11[3] = uVar8;
  puVar11[4] = uVar9;
  puVar11[5] = uVar10;
  puVar11[6] = uVar1;
  local_28 = this;
                    /* try { // try from 0057e335 to 0057e339 has its CatchHandler @ 0057e38e */
  plVar12 = (long *)FUN_0057e6d0(&local_28);
  plVar4 = (long *)puVar11[8];
  if ((plVar12 == plVar4) || (puVar11[8] = plVar12, plVar4 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0057e36b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar4 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCircleMaskGenerator @ 0057e540 ======

/* KisCircleMaskGenerator::KisCircleMaskGenerator(double, double, double, double, int, bool) */

void __thiscall
KisCircleMaskGenerator::KisCircleMaskGenerator
          (KisCircleMaskGenerator *this,double param_1,double param_2,double param_3,double param_4,
          int param_5,bool param_6)

{
  long lVar1;
  long *plVar2;
  double dVar3;
  undefined (*pauVar4) [16];
  long *plVar5;
  long in_FS_OFFSET;
  KisCircleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator
            ((KisMaskGenerator *)this,param_1,param_2,param_3,param_4,param_5,param_6,0,
             (KoID *)&DAT_00852d80);
  *(undefined **)this = PTR_vtable_00837038 + 0x10;
                    /* try { // try from 0057e585 to 0057e589 has its CatchHandler @ 0057e64a */
  pauVar4 = (undefined (*) [16])operator_new(0x48);
  pauVar4[3][8] = 0;
  *pauVar4 = (undefined  [16])0x0;
  pauVar4[1] = (undefined  [16])0x0;
  dVar3 = DAT_0072eb60;
  *(undefined8 *)pauVar4[3] = 0;
  *(double *)pauVar4[2] = dVar3;
  *(undefined8 *)(pauVar4[2] + 8) = 0;
  dVar3 = DAT_0072eb60;
  *(undefined8 *)pauVar4[4] = 0;
  *(undefined (**) [16])(this + 0x18) = pauVar4;
                    /* try { // try from 0057e5c8 to 0057e5e1 has its CatchHandler @ 0057e63e */
  setScale(this,dVar3,dVar3);
  lVar1 = *(long *)(this + 0x18);
  *(bool *)(lVar1 + 0x38) = param_6;
  local_28 = this;
  plVar5 = (long *)FUN_0057e6d0(&local_28);
  plVar2 = *(long **)(lVar1 + 0x40);
  if ((plVar5 == plVar2) || (*(long **)(lVar1 + 0x40) = plVar5, plVar2 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0057e615. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar2 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



