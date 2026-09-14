/* Class KisUpdateSelectionJob - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisUpdateSelectionJob @ 00208790 ======

void __thiscall
KisUpdateSelectionJob::KisUpdateSelectionJob
          (KisUpdateSelectionJob *this,KisSharedPtr param_1,QRect *param_2)

{
  (*(code *)PTR_KisUpdateSelectionJob_0083be98)();
  return;
}



// ====== KisUpdateSelectionJob @ 005fe180 ======

/* KisUpdateSelectionJob::KisUpdateSelectionJob(KisSharedPtr<KisSelection>, QRect const&) */

void __thiscall
KisUpdateSelectionJob::KisUpdateSelectionJob
          (KisUpdateSelectionJob *this,KisSharedPtr param_1,QRect *param_2)

{
  long lVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined4 in_register_00000034;
  
  puVar4 = PTR_vtable_008373c8;
  this[8] = (KisUpdateSelectionJob)0x0;
  *(undefined **)this = puVar4 + 0x10;
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x10) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 8) = *(int *)(lVar1 + 8) + 1;
    UNLOCK();
  }
  uVar2 = *(undefined8 *)param_2;
  uVar3 = *(undefined8 *)(param_2 + 8);
  this[8] = (KisUpdateSelectionJob)0x1;
  *(undefined8 *)(this + 0x18) = uVar2;
  *(undefined8 *)(this + 0x20) = uVar3;
  return;
}



