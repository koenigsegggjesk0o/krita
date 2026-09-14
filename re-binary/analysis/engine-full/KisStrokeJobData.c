/* Class KisStrokeJobData - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisStrokeJobData @ 00208e40 ======

void __thiscall
KisStrokeJobData::KisStrokeJobData(KisStrokeJobData *this,Sequentiality param_1,Exclusivity param_2)

{
  (*(code *)PTR_KisStrokeJobData_0083c1f0)();
  return;
}



// ====== KisStrokeJobData @ 0020a310 ======

void __thiscall
KisStrokeJobData::KisStrokeJobData(KisStrokeJobData *this,Sequentiality param_1,Exclusivity param_2)

{
  (*(code *)PTR_KisStrokeJobData_0083cc58)();
  return;
}



// ====== KisStrokeJobData @ 004eb180 ======

/* KisStrokeJobData::KisStrokeJobData(KisStrokeJobData::Sequentiality,
   KisStrokeJobData::Exclusivity) */

void __thiscall
KisStrokeJobData::KisStrokeJobData(KisStrokeJobData *this,Sequentiality param_1,Exclusivity param_2)

{
  undefined *puVar1;
  
  puVar1 = PTR_vtable_008376e8;
  *(Sequentiality *)(this + 8) = param_1;
  *(Exclusivity *)(this + 0xc) = param_2;
  this[0x10] = (KisStrokeJobData)0x1;
  *(undefined **)this = puVar1 + 0x10;
  *(undefined4 *)(this + 0x14) = 0xffffffff;
  return;
}



// ====== KisStrokeJobData @ 004eb1b0 ======

/* KisStrokeJobData::KisStrokeJobData(KisStrokeJobData const&) */

void __thiscall KisStrokeJobData::KisStrokeJobData(KisStrokeJobData *this,KisStrokeJobData *param_1)

{
  *(undefined **)this = PTR_vtable_008376e8 + 0x10;
  *(undefined8 *)(this + 8) = *(undefined8 *)(param_1 + 8);
  this[0x10] = param_1[0x10];
  *(undefined4 *)(this + 0x14) = *(undefined4 *)(param_1 + 0x14);
  return;
}



