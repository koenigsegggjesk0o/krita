/* Class KisUpdateOutlineJob - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisUpdateOutlineJob @ 005fdd50 ======

/* KisUpdateOutlineJob::KisUpdateOutlineJob(KisSharedPtr<KisSelection>, bool, QColor const&) */

void __thiscall
KisUpdateOutlineJob::KisUpdateOutlineJob
          (KisUpdateOutlineJob *this,KisSharedPtr param_1,bool param_2,QColor *param_3)

{
  undefined4 uVar1;
  long lVar2;
  undefined *puVar3;
  undefined4 in_register_00000034;
  
  puVar3 = PTR_vtable_00837fc8;
  this[8] = (KisUpdateOutlineJob)0x0;
  *(undefined **)this = puVar3 + 0x10;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x10) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 8) = *(int *)(lVar2 + 8) + 1;
    UNLOCK();
  }
  uVar1 = *(undefined4 *)param_3;
  this[0x18] = (KisUpdateOutlineJob)param_2;
  *(undefined4 *)(this + 0x1c) = uVar1;
  *(undefined8 *)(this + 0x20) = *(undefined8 *)(param_3 + 4);
  *(undefined2 *)(this + 0x28) = *(undefined2 *)(param_3 + 0xc);
  return;
}



