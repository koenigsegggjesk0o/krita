/* Class KisRectangleMaskGenerator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisRectangleMaskGenerator @ 0020a6f0 ======

void __thiscall
KisRectangleMaskGenerator::KisRectangleMaskGenerator
          (KisRectangleMaskGenerator *this,KisRectangleMaskGenerator *param_1)

{
  (*(code *)PTR_KisRectangleMaskGenerator_0083ce48)();
  return;
}



// ====== KisRectangleMaskGenerator @ 0020b2a0 ======

void __thiscall
KisRectangleMaskGenerator::KisRectangleMaskGenerator
          (KisRectangleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,bool param_6)

{
  (*(code *)PTR_KisRectangleMaskGenerator_0083d420)();
  return;
}



// ====== KisRectangleMaskGenerator @ 0057afa0 ======

/* KisRectangleMaskGenerator::KisRectangleMaskGenerator(KisRectangleMaskGenerator const&) */

void __thiscall
KisRectangleMaskGenerator::KisRectangleMaskGenerator
          (KisRectangleMaskGenerator *this,KisRectangleMaskGenerator *param_1)

{
  undefined uVar1;
  undefined8 *puVar2;
  long *plVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 *puVar10;
  long *plVar11;
  long in_FS_OFFSET;
  KisRectangleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator((KisMaskGenerator *)this,(KisMaskGenerator *)param_1);
  *(undefined **)this = PTR_vtable_00837188 + 0x10;
                    /* try { // try from 0057afdb to 0057afdf has its CatchHandler @ 0057b082 */
  puVar10 = (undefined8 *)operator_new(0x40);
  puVar2 = *(undefined8 **)(param_1 + 0x18);
  uVar4 = *puVar2;
  uVar5 = puVar2[1];
  uVar6 = puVar2[2];
  uVar7 = puVar2[3];
  uVar8 = puVar2[4];
  uVar9 = puVar2[5];
  uVar1 = *(undefined *)(puVar2 + 6);
  *(undefined8 **)(this + 0x18) = puVar10;
  puVar10[7] = 0;
  *(undefined *)(puVar10 + 6) = uVar1;
  *puVar10 = uVar4;
  puVar10[1] = uVar5;
  puVar10[2] = uVar6;
  puVar10[3] = uVar7;
  puVar10[4] = uVar8;
  puVar10[5] = uVar9;
  local_28 = this;
                    /* try { // try from 0057b01b to 0057b01f has its CatchHandler @ 0057b076 */
  plVar11 = (long *)FUN_0057c640(&local_28);
  plVar3 = (long *)puVar10[7];
  if ((plVar11 == plVar3) || (puVar10[7] = plVar11, plVar3 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0057b051. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar3 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisRectangleMaskGenerator @ 0057b2e0 ======

/* KisRectangleMaskGenerator::KisRectangleMaskGenerator(double, double, double, double, int, bool)
    */

void __thiscall
KisRectangleMaskGenerator::KisRectangleMaskGenerator
          (KisRectangleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,bool param_6)

{
  long lVar1;
  long *plVar2;
  double dVar3;
  undefined (*pauVar4) [16];
  long *plVar5;
  long in_FS_OFFSET;
  KisRectangleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator
            ((KisMaskGenerator *)this,param_1,param_2,param_3,param_4,param_5,param_6,1,
             (KoID *)&DAT_00852cb0);
  *(undefined **)this = PTR_vtable_00837188 + 0x10;
                    /* try { // try from 0057b328 to 0057b32c has its CatchHandler @ 0057b3da */
  pauVar4 = (undefined (*) [16])operator_new(0x40);
  pauVar4[3][0] = 0;
  *pauVar4 = (undefined  [16])0x0;
  pauVar4[1] = (undefined  [16])0x0;
  pauVar4[2] = (undefined  [16])0x0;
  dVar3 = DAT_007227c0;
  *(undefined8 *)(pauVar4[3] + 8) = 0;
  *(undefined (**) [16])(this + 0x18) = pauVar4;
                    /* try { // try from 0057b35b to 0057b374 has its CatchHandler @ 0057b3ce */
  setScale(this,dVar3,dVar3);
  lVar1 = *(long *)(this + 0x18);
  *(bool *)(lVar1 + 0x30) = param_6;
  local_28 = this;
  plVar5 = (long *)FUN_0057c640(&local_28);
  plVar2 = *(long **)(lVar1 + 0x38);
  if ((plVar5 == plVar2) || (*(long **)(lVar1 + 0x38) = plVar5, plVar2 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0057b3a8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar2 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



