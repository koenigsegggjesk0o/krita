/* Class KisGaussRectangleMaskGenerator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGaussRectangleMaskGenerator @ 00200d40 ======

void __thiscall
KisGaussRectangleMaskGenerator::KisGaussRectangleMaskGenerator
          (KisGaussRectangleMaskGenerator *this,KisGaussRectangleMaskGenerator *param_1)

{
  (*(code *)PTR_KisGaussRectangleMaskGenerator_00838170)();
  return;
}



// ====== KisGaussRectangleMaskGenerator @ 00201460 ======

void __thiscall
KisGaussRectangleMaskGenerator::KisGaussRectangleMaskGenerator
          (KisGaussRectangleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,bool param_6)

{
  (*(code *)PTR_KisGaussRectangleMaskGenerator_00838500)();
  return;
}



// ====== KisGaussRectangleMaskGenerator @ 00582840 ======

/* KisGaussRectangleMaskGenerator::KisGaussRectangleMaskGenerator(KisGaussRectangleMaskGenerator
   const&) */

void __thiscall
KisGaussRectangleMaskGenerator::KisGaussRectangleMaskGenerator
          (KisGaussRectangleMaskGenerator *this,KisGaussRectangleMaskGenerator *param_1)

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
  undefined8 uVar11;
  undefined8 uVar12;
  undefined8 *puVar13;
  long *plVar14;
  long in_FS_OFFSET;
  KisGaussRectangleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator((KisMaskGenerator *)this,(KisMaskGenerator *)param_1);
  *(undefined **)this = PTR_vtable_00837588 + 0x10;
                    /* try { // try from 0058287b to 0058287f has its CatchHandler @ 00582942 */
  puVar13 = (undefined8 *)operator_new(0x70);
  puVar3 = *(undefined8 **)(param_1 + 0x18);
  uVar1 = puVar3[1];
  uVar5 = puVar3[2];
  uVar6 = puVar3[3];
  uVar7 = puVar3[4];
  uVar8 = puVar3[5];
  uVar9 = puVar3[6];
  uVar10 = puVar3[7];
  uVar11 = puVar3[8];
  uVar12 = puVar3[9];
  *puVar13 = *puVar3;
  puVar13[1] = uVar1;
  puVar13[2] = uVar5;
  puVar13[3] = uVar6;
  puVar13[4] = uVar7;
  puVar13[5] = uVar8;
  puVar13[6] = uVar9;
  puVar13[7] = uVar10;
  puVar13[8] = uVar11;
  puVar13[9] = uVar12;
  uVar1 = puVar3[10];
  uVar2 = *(undefined *)(puVar3 + 0xb);
  puVar13[0xc] = puVar13;
  *(undefined *)(puVar13 + 0xb) = uVar2;
  puVar13[0xd] = 0;
  *(undefined8 **)(this + 0x18) = puVar13;
  puVar13[10] = uVar1;
  local_28 = this;
                    /* try { // try from 005828db to 005828df has its CatchHandler @ 00582936 */
  plVar14 = (long *)FUN_00582cc0(&local_28);
  plVar4 = (long *)puVar13[0xd];
  if ((plVar14 == plVar4) || (puVar13[0xd] = plVar14, plVar4 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x00582911. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar4 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisGaussRectangleMaskGenerator @ 00582bb0 ======

/* KisGaussRectangleMaskGenerator::KisGaussRectangleMaskGenerator(double, double, double, double,
   int, bool) */

void __thiscall
KisGaussRectangleMaskGenerator::KisGaussRectangleMaskGenerator
          (KisGaussRectangleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,bool param_6)

{
  long lVar1;
  long *plVar2;
  double dVar3;
  undefined (*pauVar4) [16];
  long *plVar5;
  long in_FS_OFFSET;
  KisGaussRectangleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator
            ((KisMaskGenerator *)this,param_1,param_2,param_3,param_4,param_5,param_6,1,
             (KoID *)&DAT_00852dc0);
  *(undefined **)this = PTR_vtable_00837588 + 0x10;
                    /* try { // try from 00582bf6 to 00582bfa has its CatchHandler @ 00582cb0 */
  pauVar4 = (undefined (*) [16])operator_new(0x70);
  *(undefined8 *)pauVar4[5] = 0;
  *pauVar4 = (undefined  [16])0x0;
  pauVar4[1] = (undefined  [16])0x0;
  pauVar4[2] = (undefined  [16])0x0;
  pauVar4[3] = (undefined  [16])0x0;
  pauVar4[4] = (undefined  [16])0x0;
  dVar3 = DAT_007227c0;
  pauVar4[5][8] = param_6;
  *(undefined (**) [16])pauVar4[6] = pauVar4;
  *(undefined8 *)(pauVar4[6] + 8) = 0;
  *(undefined (**) [16])(this + 0x18) = pauVar4;
                    /* try { // try from 00582c3d to 00582c51 has its CatchHandler @ 00582ca4 */
  setScale(this,dVar3,dVar3);
  lVar1 = *(long *)(this + 0x18);
  local_28 = this;
  plVar5 = (long *)FUN_00582cc0(&local_28);
  plVar2 = *(long **)(lVar1 + 0x68);
  if ((plVar5 == plVar2) || (*(long **)(lVar1 + 0x68) = plVar5, plVar2 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x00582c81. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar2 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



