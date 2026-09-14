/* Class KisMaskGenerator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMaskGenerator @ 00205fd0 ======

void __thiscall
KisMaskGenerator::KisMaskGenerator
          (KisMaskGenerator *this,double param_1,double param_2,double param_3,double param_4,
          int param_5,bool param_6,Type param_7,KoID *param_8)

{
  (*(code *)PTR_KisMaskGenerator_0083aab8)();
  return;
}



// ====== KisMaskGenerator @ 0020cd90 ======

void __thiscall KisMaskGenerator::KisMaskGenerator(KisMaskGenerator *this,KisMaskGenerator *param_1)

{
  (*(code *)PTR_KisMaskGenerator_0083e198)();
  return;
}



// ====== KisMaskGenerator @ 00578120 ======

/* KisMaskGenerator::KisMaskGenerator(KisMaskGenerator const&) */

void __thiscall KisMaskGenerator::KisMaskGenerator(KisMaskGenerator *this,KisMaskGenerator *param_1)

{
  undefined8 uVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  undefined8 *puVar4;
  int *piVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  undefined8 *puVar13;
  
  *(undefined **)this = PTR_vtable_00837440 + 0x10;
  puVar13 = (undefined8 *)operator_new(0x68);
  puVar4 = *(undefined8 **)(param_1 + 8);
  uVar1 = *puVar4;
  uVar6 = puVar4[1];
  uVar7 = puVar4[2];
  uVar8 = puVar4[3];
  uVar9 = puVar4[4];
  uVar10 = puVar4[5];
  uVar11 = puVar4[6];
  uVar12 = puVar4[7];
  piVar5 = (int *)puVar4[10];
  *(undefined4 *)(puVar13 + 8) = *(undefined4 *)(puVar4 + 8);
  uVar2 = *(undefined2 *)((long)puVar4 + 0x44);
  *puVar13 = uVar1;
  puVar13[1] = uVar6;
  puVar13[2] = uVar7;
  puVar13[3] = uVar8;
  *(undefined2 *)((long)puVar13 + 0x44) = uVar2;
  uVar3 = *(undefined4 *)(puVar4 + 9);
  puVar13[4] = uVar9;
  puVar13[5] = uVar10;
  *(undefined4 *)(puVar13 + 9) = uVar3;
  puVar13[10] = piVar5;
  puVar13[6] = uVar11;
  puVar13[7] = uVar12;
  if (1 < *piVar5 + 1U) {
    LOCK();
    *piVar5 = *piVar5 + 1;
    UNLOCK();
  }
  uVar6 = puVar4[0xc];
  uVar1 = *(undefined8 *)(param_1 + 0x10);
  puVar13[0xb] = puVar4[0xb];
  puVar13[0xc] = uVar6;
  *(undefined8 **)(this + 8) = puVar13;
  *(undefined8 *)(this + 0x10) = uVar1;
  return;
}



// ====== KisMaskGenerator @ 0057aa60 ======

/* KisMaskGenerator::KisMaskGenerator(double, double, double, double, int, bool,
   KisMaskGenerator::Type, KoID const&) */

void __thiscall
KisMaskGenerator::KisMaskGenerator
          (KisMaskGenerator *this,double param_1,double param_2,double param_3,double param_4,
          int param_5,bool param_6,Type param_7,KoID *param_8)

{
  double dVar1;
  double dVar2;
  undefined *puVar3;
  double *pdVar4;
  
  *(undefined **)this = PTR_vtable_00837440 + 0x10;
  pdVar4 = (double *)operator_new(0x68);
  *(KoID **)(this + 0x10) = param_8;
  dVar1 = DAT_007227c0;
  pdVar4[5] = 0.0;
  *pdVar4 = param_1;
  pdVar4[1] = param_2;
  puVar3 = PTR_shared_null_008377d0;
  dVar2 = DAT_007227c8;
  pdVar4[2] = dVar1;
  pdVar4[6] = 0.0;
  pdVar4[0xb] = dVar1;
  pdVar4[0xc] = dVar1;
  *(undefined *)((long)pdVar4 + 0x44) = 1;
  pdVar4[10] = (double)puVar3;
  *(double **)(this + 8) = pdVar4;
  *(int *)(pdVar4 + 8) = param_5;
  pdVar4[4] = dVar2 * param_4;
  dVar1 = DAT_007231b8;
  *(Type *)(pdVar4 + 9) = param_7;
  *(bool *)((long)pdVar4 + 0x45) = param_6;
  pdVar4[3] = param_3 * dVar2;
  pdVar4[7] = dVar1 / (double)param_5;
                    /* try { // try from 0057ab3a to 0057ab3e has its CatchHandler @ 0057ab4c */
  init(this);
  return;
}



