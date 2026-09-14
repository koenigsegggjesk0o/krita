/* Class KisRunnableStrokeJobDataBase - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisRunnableStrokeJobDataBase @ 00208590 ======

void __thiscall
KisRunnableStrokeJobDataBase::KisRunnableStrokeJobDataBase
          (KisRunnableStrokeJobDataBase *this,Sequentiality param_1,Exclusivity param_2)

{
  (*(code *)PTR_KisRunnableStrokeJobDataBase_0083bd98)();
  return;
}



// ====== KisRunnableStrokeJobDataBase @ 004eaaf0 ======

/* KisRunnableStrokeJobDataBase::KisRunnableStrokeJobDataBase(KisStrokeJobData::Sequentiality,
   KisStrokeJobData::Exclusivity) */

void __thiscall
KisRunnableStrokeJobDataBase::KisRunnableStrokeJobDataBase
          (KisRunnableStrokeJobDataBase *this,Sequentiality param_1,Exclusivity param_2)

{
  undefined *puVar1;
  
  KisStrokeJobData::KisStrokeJobData((KisStrokeJobData *)this,param_1,param_2);
  puVar1 = PTR_vtable_00836c98 + 0x38;
  *(undefined **)this = PTR_vtable_00836c98 + 0x10;
  *(undefined **)(this + 0x18) = puVar1;
  return;
}



