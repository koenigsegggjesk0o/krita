/* Class KisPaintInformation - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintInformation @ 002019f0 ======

void __thiscall
KisPaintInformation::KisPaintInformation(KisPaintInformation *this,KisPaintInformation *param_1)

{
  (*(code *)PTR_KisPaintInformation_008387c8)();
  return;
}



// ====== KisPaintInformation @ 0020a3c0 ======

void __thiscall
KisPaintInformation::KisPaintInformation
          (KisPaintInformation *this,QPointF *param_1,double param_2,double param_3,double param_4,
          double param_5,double param_6,double param_7,double param_8,double param_9)

{
  (*(code *)PTR_KisPaintInformation_0083ccb0)();
  return;
}



// ====== KisPaintInformation @ 0020d710 ======

void __thiscall
KisPaintInformation::KisPaintInformation(KisPaintInformation *this,QPointF *param_1,double param_2)

{
  (*(code *)PTR_KisPaintInformation_0083e658)();
  return;
}



// ====== KisPaintInformation @ 003301f0 ======

/* KisPaintInformation::KisPaintInformation(QPointF const&, double, double, double, double, double,
   double, double, double) */

void __thiscall
KisPaintInformation::KisPaintInformation
          (KisPaintInformation *this,QPointF *param_1,double param_2,double param_3,double param_4,
          double param_5,double param_6,double param_7,double param_8,double param_9)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 *puVar3;
  
  puVar3 = (undefined8 *)operator_new(0xe8);
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  *(undefined *)(puVar3 + 10) = 0;
  puVar3[2] = param_2;
  puVar3[3] = param_3;
  puVar3[0xd] = 0;
  *(undefined2 *)(puVar3 + 0xe) = 0;
  puVar3[0xf] = 0;
  *(undefined *)(puVar3 + 0x10) = 0;
  puVar3[0x11] = 0;
  *(undefined *)(puVar3 + 0x12) = 0;
  *(undefined *)(puVar3 + 0x13) = 0;
  *(undefined4 *)(puVar3 + 0x1c) = 0;
  *(undefined8 **)this = puVar3;
  *puVar3 = uVar1;
  puVar3[1] = uVar2;
  puVar3[4] = param_4;
  puVar3[5] = param_5;
  puVar3[6] = param_6;
  puVar3[7] = param_7;
  puVar3[8] = param_8;
  puVar3[9] = param_9;
  *(undefined (*) [16])(puVar3 + 0xb) = (undefined  [16])0x0;
  return;
}



// ====== KisPaintInformation @ 003302c0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisPaintInformation::KisPaintInformation(QPointF const&, double, double, double, double) */

void __thiscall
KisPaintInformation::KisPaintInformation
          (KisPaintInformation *this,QPointF *param_1,double param_2,double param_3,double param_4,
          double param_5)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
  
  puVar5 = (undefined8 *)operator_new(0xe8);
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  *(undefined *)(puVar5 + 10) = 0;
  puVar5[2] = param_2;
  puVar5[3] = param_3;
  uVar4 = DAT_007231a8;
  uVar3 = _DAT_007231a0;
  puVar5[0xd] = 0;
  puVar5[6] = uVar3;
  puVar5[7] = uVar4;
  *(undefined (*) [16])(puVar5 + 8) = (undefined  [16])0x0;
  *(undefined2 *)(puVar5 + 0xe) = 0;
  puVar5[0xf] = 0;
  *(undefined *)(puVar5 + 0x10) = 0;
  puVar5[0x11] = 0;
  *(undefined *)(puVar5 + 0x12) = 0;
  *(undefined *)(puVar5 + 0x13) = 0;
  *(undefined4 *)(puVar5 + 0x1c) = 0;
  *(undefined8 **)this = puVar5;
  *puVar5 = uVar1;
  puVar5[1] = uVar2;
  puVar5[4] = param_4;
  puVar5[5] = param_5;
  *(undefined (*) [16])(puVar5 + 0xb) = (undefined  [16])0x0;
  return;
}



// ====== KisPaintInformation @ 00330370 ======

/* KisPaintInformation::KisPaintInformation(QPointF const&, double) */

void __thiscall
KisPaintInformation::KisPaintInformation(KisPaintInformation *this,QPointF *param_1,double param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 *puVar4;
  
  puVar4 = (undefined8 *)operator_new(0xe8);
  uVar3 = DAT_007231a8;
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  puVar4[3] = 0;
  puVar4[2] = param_2;
  puVar4[7] = uVar3;
  puVar4[4] = 0;
  puVar4[5] = 0;
  puVar4[6] = 0;
  puVar4[8] = 0;
  puVar4[9] = 0;
  *(undefined *)(puVar4 + 10) = 0;
  puVar4[0xd] = 0;
  *(undefined2 *)(puVar4 + 0xe) = 0;
  puVar4[0xf] = 0;
  *(undefined *)(puVar4 + 0x10) = 0;
  puVar4[0x11] = 0;
  *(undefined *)(puVar4 + 0x12) = 0;
  *(undefined *)(puVar4 + 0x13) = 0;
  *(undefined4 *)(puVar4 + 0x1c) = 0;
  *(undefined8 **)this = puVar4;
  *puVar4 = uVar1;
  puVar4[1] = uVar2;
  *(undefined (*) [16])(puVar4 + 0xb) = (undefined  [16])0x0;
  return;
}



// ====== KisPaintInformation @ 00330440 ======

/* KisPaintInformation::KisPaintInformation(KisPaintInformation const&) */

void __thiscall
KisPaintInformation::KisPaintInformation(KisPaintInformation *this,KisPaintInformation *param_1)

{
  undefined8 uVar1;
  char cVar2;
  undefined uVar3;
  undefined4 uVar4;
  undefined8 *puVar5;
  int *piVar6;
  KisRandomSource *this_00;
  undefined8 uVar7;
  undefined (*pauVar8) [16];
  KisPerStrokeRandomSource *pKVar9;
  KisPerStrokeRandomSource *this_01;
  
  pauVar8 = (undefined (*) [16])operator_new(0xe8);
  puVar5 = *(undefined8 **)param_1;
  *pauVar8 = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar8[5] + 8) = (undefined  [16])0x0;
  *(undefined8 *)(pauVar8[6] + 8) = 0;
  *(undefined8 *)(pauVar8[7] + 8) = 0;
  *(undefined8 *)(pauVar8[8] + 8) = 0;
  uVar1 = *puVar5;
  uVar7 = puVar5[1];
  *(undefined2 *)pauVar8[7] = 0;
  *(undefined8 *)*pauVar8 = uVar1;
  *(undefined8 *)(*pauVar8 + 8) = uVar7;
  uVar1 = puVar5[2];
  pauVar8[8][0] = 0;
  *(undefined8 *)pauVar8[1] = uVar1;
  uVar1 = puVar5[3];
  pauVar8[9][0] = 0;
  *(undefined8 *)(pauVar8[1] + 8) = uVar1;
  uVar1 = puVar5[4];
  pauVar8[9][8] = 0;
  *(undefined8 *)pauVar8[2] = uVar1;
  *(undefined8 *)(pauVar8[2] + 8) = puVar5[5];
  *(undefined8 *)pauVar8[3] = puVar5[6];
  *(undefined8 *)(pauVar8[3] + 8) = puVar5[7];
  *(undefined8 *)pauVar8[4] = puVar5[8];
  *(undefined8 *)(pauVar8[4] + 8) = puVar5[9];
  pauVar8[5][0] = *(undefined *)(puVar5 + 10);
  piVar6 = (int *)puVar5[0xb];
  if (piVar6 == (int *)0x0) {
    pKVar9 = (KisPerStrokeRandomSource *)puVar5[0xc];
    if (pKVar9 == (KisPerStrokeRandomSource *)0x0) goto LAB_00330562;
LAB_00330545:
    LOCK();
    *(int *)pKVar9 = *(int *)pKVar9 + 1;
    UNLOCK();
    this_01 = *(KisPerStrokeRandomSource **)pauVar8[6];
  }
  else {
    LOCK();
    *piVar6 = *piVar6 + 1;
    UNLOCK();
    this_00 = *(KisRandomSource **)(pauVar8[5] + 8);
    *(int **)(pauVar8[5] + 8) = piVar6;
    if (this_00 != (KisRandomSource *)0x0) {
      LOCK();
      *(int *)this_00 = *(int *)this_00 + -1;
      UNLOCK();
      if (*(int *)this_00 == 0) {
        KisRandomSource::~KisRandomSource(this_00);
        operator_delete(this_00,0x18);
      }
    }
    this_01 = *(KisPerStrokeRandomSource **)pauVar8[6];
    pKVar9 = (KisPerStrokeRandomSource *)puVar5[0xc];
    if (pKVar9 == this_01) goto LAB_00330562;
    if (pKVar9 != (KisPerStrokeRandomSource *)0x0) goto LAB_00330545;
  }
  *(KisPerStrokeRandomSource **)pauVar8[6] = pKVar9;
  if (this_01 != (KisPerStrokeRandomSource *)0x0) {
    LOCK();
    *(int *)this_01 = *(int *)this_01 + -1;
    UNLOCK();
    if (*(int *)this_01 == 0) {
      KisPerStrokeRandomSource::~KisPerStrokeRandomSource(this_01);
      operator_delete(this_01,0x18);
    }
  }
LAB_00330562:
  cVar2 = pauVar8[9][8];
  pauVar8[9][0] = 0;
  if (cVar2 == '\0') {
    if (*(char *)(puVar5 + 0x13) != '\0') {
      uVar1 = puVar5[0x15];
      *(undefined8 *)pauVar8[10] = puVar5[0x14];
      *(undefined8 *)(pauVar8[10] + 8) = uVar1;
      uVar1 = puVar5[0x17];
      *(undefined8 *)pauVar8[0xb] = puVar5[0x16];
      *(undefined8 *)(pauVar8[0xb] + 8) = uVar1;
      uVar1 = puVar5[0x19];
      *(undefined8 *)pauVar8[0xc] = puVar5[0x18];
      *(undefined8 *)(pauVar8[0xc] + 8) = uVar1;
      uVar1 = puVar5[0x1a];
      uVar7 = puVar5[0x1b];
      pauVar8[9][8] = 1;
      *(undefined8 *)pauVar8[0xd] = uVar1;
      *(undefined8 *)(pauVar8[0xd] + 8) = uVar7;
    }
  }
  else if (*(char *)(puVar5 + 0x13) == '\0') {
    pauVar8[9][8] = 0;
  }
  else {
    uVar1 = puVar5[0x15];
    *(undefined8 *)pauVar8[10] = puVar5[0x14];
    *(undefined8 *)(pauVar8[10] + 8) = uVar1;
    uVar1 = puVar5[0x17];
    *(undefined8 *)pauVar8[0xb] = puVar5[0x16];
    *(undefined8 *)(pauVar8[0xb] + 8) = uVar1;
    uVar1 = puVar5[0x19];
    *(undefined8 *)pauVar8[0xc] = puVar5[0x18];
    *(undefined8 *)(pauVar8[0xc] + 8) = uVar1;
    uVar1 = puVar5[0x1b];
    *(undefined8 *)pauVar8[0xd] = puVar5[0x1a];
    *(undefined8 *)(pauVar8[0xd] + 8) = uVar1;
  }
  uVar1 = puVar5[0xd];
  pauVar8[7][0] = *(undefined *)(puVar5 + 0xe);
  uVar3 = *(undefined *)((long)puVar5 + 0x71);
  *(undefined8 *)(pauVar8[6] + 8) = uVar1;
  uVar1 = puVar5[0xf];
  pauVar8[7][1] = uVar3;
  cVar2 = *(char *)(puVar5 + 0x10);
  *(undefined8 *)(pauVar8[7] + 8) = uVar1;
  if (cVar2 != '\0') {
    uVar1 = puVar5[0x11];
    pauVar8[8][0] = 1;
    *(undefined8 *)(pauVar8[8] + 8) = uVar1;
  }
  uVar4 = *(undefined4 *)(puVar5 + 0x1c);
  *(undefined (**) [16])this = pauVar8;
  *(undefined4 *)pauVar8[0xe] = uVar4;
  return;
}



