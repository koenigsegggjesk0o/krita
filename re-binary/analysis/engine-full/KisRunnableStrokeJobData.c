/* Class KisRunnableStrokeJobData - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisRunnableStrokeJobData @ 0020d760 ======

void __thiscall
KisRunnableStrokeJobData::KisRunnableStrokeJobData
          (KisRunnableStrokeJobData *this,QRunnable *param_1,Sequentiality param_2,
          Exclusivity param_3)

{
  (*(code *)PTR_KisRunnableStrokeJobData_0083e680)();
  return;
}



// ====== KisRunnableStrokeJobData @ 0020d9f0 ======

void __thiscall
KisRunnableStrokeJobData::KisRunnableStrokeJobData
          (KisRunnableStrokeJobData *this,function param_1,Sequentiality param_2,Exclusivity param_3
          )

{
  (*(code *)PTR_KisRunnableStrokeJobData_0083e7c8)();
  return;
}



// ====== KisRunnableStrokeJobData @ 004eacc0 ======

/* KisRunnableStrokeJobData::KisRunnableStrokeJobData(QRunnable*, KisStrokeJobData::Sequentiality,
   KisStrokeJobData::Exclusivity) */

void __thiscall
KisRunnableStrokeJobData::KisRunnableStrokeJobData
          (KisRunnableStrokeJobData *this,QRunnable *param_1,Sequentiality param_2,
          Exclusivity param_3)

{
  undefined *puVar1;
  
  KisRunnableStrokeJobDataBase::KisRunnableStrokeJobDataBase
            ((KisRunnableStrokeJobDataBase *)this,param_2,param_3);
  puVar1 = PTR_vtable_00836c78;
  *(QRunnable **)(this + 0x20) = param_1;
  *(undefined8 *)(this + 0x38) = 0;
  *(undefined8 *)(this + 0x40) = 0;
  *(undefined **)this = puVar1 + 0x10;
  *(undefined **)(this + 0x18) = puVar1 + 0x40;
  *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
  return;
}



// ====== KisRunnableStrokeJobData @ 004ead20 ======

/* KisRunnableStrokeJobData::KisRunnableStrokeJobData(std::function<void ()>,
   KisStrokeJobData::Sequentiality, KisStrokeJobData::Exclusivity) */

void __thiscall
KisRunnableStrokeJobData::KisRunnableStrokeJobData
          (KisRunnableStrokeJobData *this,function param_1,Sequentiality param_2,Exclusivity param_3
          )

{
  code *pcVar1;
  undefined8 uVar2;
  undefined *puVar3;
  undefined4 in_register_00000034;
  long lVar4;
  
  lVar4 = CONCAT44(in_register_00000034,param_1);
  KisRunnableStrokeJobDataBase::KisRunnableStrokeJobDataBase
            ((KisRunnableStrokeJobDataBase *)this,param_2,param_3);
  puVar3 = PTR_vtable_00836c78;
  *(undefined8 *)(this + 0x20) = 0;
  *(undefined8 *)(this + 0x38) = 0;
  *(undefined8 *)(this + 0x40) = 0;
  *(undefined **)(this + 0x18) = puVar3 + 0x40;
  pcVar1 = *(code **)(lVar4 + 0x10);
  *(undefined **)this = puVar3 + 0x10;
  *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
  if (pcVar1 != (code *)0x0) {
                    /* try { // try from 004ead85 to 004ead86 has its CatchHandler @ 004ead95 */
    (*pcVar1)(this + 0x28,lVar4,2);
    uVar2 = *(undefined8 *)(lVar4 + 0x18);
    *(undefined8 *)(this + 0x38) = *(undefined8 *)(lVar4 + 0x10);
    *(undefined8 *)(this + 0x40) = uVar2;
  }
  return;
}



