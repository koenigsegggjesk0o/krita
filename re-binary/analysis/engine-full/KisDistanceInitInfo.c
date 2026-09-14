/* Class KisDistanceInitInfo - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisDistanceInitInfo @ 00203a60 ======

void __thiscall
KisDistanceInitInfo::KisDistanceInitInfo
          (KisDistanceInitInfo *this,double param_1,double param_2,int param_3)

{
  (*(code *)PTR_KisDistanceInitInfo_00839800)();
  return;
}



// ====== KisDistanceInitInfo @ 00207280 ======

void __thiscall
KisDistanceInitInfo::KisDistanceInitInfo
          (KisDistanceInitInfo *this,QPointF *param_1,double param_2,double param_3,double param_4,
          int param_5)

{
  (*(code *)PTR_KisDistanceInitInfo_0083b410)();
  return;
}



// ====== KisDistanceInitInfo @ 00309da0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisDistanceInitInfo::KisDistanceInitInfo() */

void __thiscall KisDistanceInitInfo::KisDistanceInitInfo(KisDistanceInitInfo *this)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  
  puVar4 = (undefined *)operator_new(0x38);
  uVar2 = DAT_00722788;
  *(undefined (*) [16])(puVar4 + 8) = (undefined  [16])0x0;
  uVar3 = DAT_00722788;
  uVar1 = _DAT_00722780;
  *puVar4 = 0;
  *(undefined8 *)(puVar4 + 0x28) = uVar2;
  *(undefined4 *)(puVar4 + 0x30) = 0;
  *(undefined **)this = puVar4;
  *(undefined8 *)(puVar4 + 0x18) = uVar1;
  *(undefined8 *)(puVar4 + 0x20) = uVar3;
  return;
}



// ====== KisDistanceInitInfo @ 00309de0 ======

/* KisDistanceInitInfo::KisDistanceInitInfo(double, double, int) */

void __thiscall
KisDistanceInitInfo::KisDistanceInitInfo
          (KisDistanceInitInfo *this,double param_1,double param_2,int param_3)

{
  undefined *puVar1;
  
  puVar1 = (undefined *)operator_new(0x38);
  *puVar1 = 0;
  *(undefined8 *)(puVar1 + 0x18) = 0;
  *(undefined **)this = puVar1;
  *(int *)(puVar1 + 0x30) = param_3;
  *(undefined (*) [16])(puVar1 + 8) = (undefined  [16])0x0;
  *(double *)(puVar1 + 0x20) = param_1;
  *(double *)(puVar1 + 0x28) = param_2;
  return;
}



// ====== KisDistanceInitInfo @ 00309e30 ======

/* KisDistanceInitInfo::KisDistanceInitInfo(QPointF const&, double, int) */

void __thiscall
KisDistanceInitInfo::KisDistanceInitInfo
          (KisDistanceInitInfo *this,QPointF *param_1,double param_2,int param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  
  puVar4 = (undefined *)operator_new(0x38);
  uVar3 = DAT_00722788;
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  *(undefined **)this = puVar4;
  *puVar4 = 1;
  *(int *)(puVar4 + 0x30) = param_3;
  *(undefined8 *)(puVar4 + 0x20) = uVar3;
  *(undefined8 *)(puVar4 + 0x28) = uVar3;
  *(undefined8 *)(puVar4 + 8) = uVar1;
  *(undefined8 *)(puVar4 + 0x10) = uVar2;
  *(double *)(puVar4 + 0x18) = param_2;
  return;
}



// ====== KisDistanceInitInfo @ 00309e90 ======

/* KisDistanceInitInfo::KisDistanceInitInfo(QPointF const&, double, double, double, int) */

void __thiscall
KisDistanceInitInfo::KisDistanceInitInfo
          (KisDistanceInitInfo *this,QPointF *param_1,double param_2,double param_3,double param_4,
          int param_5)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined *puVar3;
  
  puVar3 = (undefined *)operator_new(0x38);
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  *(undefined **)this = puVar3;
  *puVar3 = 1;
  *(int *)(puVar3 + 0x30) = param_5;
  *(undefined8 *)(puVar3 + 8) = uVar1;
  *(undefined8 *)(puVar3 + 0x10) = uVar2;
  *(double *)(puVar3 + 0x18) = param_2;
  *(double *)(puVar3 + 0x20) = param_3;
  *(double *)(puVar3 + 0x28) = param_4;
  return;
}



// ====== KisDistanceInitInfo @ 00309ef0 ======

/* KisDistanceInitInfo::KisDistanceInitInfo(KisDistanceInitInfo const&) */

void __thiscall
KisDistanceInitInfo::KisDistanceInitInfo(KisDistanceInitInfo *this,KisDistanceInitInfo *param_1)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  undefined8 *puVar3;
  
  puVar3 = (undefined8 *)operator_new(0x38);
  puVar1 = *(undefined8 **)param_1;
  *(undefined8 **)this = puVar3;
  uVar2 = puVar1[1];
  *puVar3 = *puVar1;
  puVar3[1] = uVar2;
  uVar2 = puVar1[3];
  puVar3[2] = puVar1[2];
  puVar3[3] = uVar2;
  uVar2 = puVar1[5];
  puVar3[4] = puVar1[4];
  puVar3[5] = uVar2;
  puVar3[6] = puVar1[6];
  return;
}



