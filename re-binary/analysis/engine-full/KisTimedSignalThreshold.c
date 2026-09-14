/* Class KisTimedSignalThreshold - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTimedSignalThreshold @ 0053b230 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTimedSignalThreshold::KisTimedSignalThreshold(int, int, QObject*) */

void __thiscall
KisTimedSignalThreshold::KisTimedSignalThreshold
          (KisTimedSignalThreshold *this,int param_1,int param_2,QObject *param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 *puVar3;
  
  QObject::QObject((QObject *)this,param_3);
  *(undefined **)this = PTR_vtable_00837cb0 + 0x10;
                    /* try { // try from 0053b25b to 0053b25f has its CatchHandler @ 0053b286 */
  puVar3 = (undefined8 *)operator_new(0x20);
  *(int *)(puVar3 + 2) = param_1;
  uVar2 = _UNK_0072d018;
  uVar1 = _DAT_0072d010;
  if (param_2 < 0) {
    param_2 = param_1 * 2;
  }
  *(undefined *)(puVar3 + 3) = 1;
  *(undefined8 **)(this + 0x10) = puVar3;
  *(int *)((long)puVar3 + 0x14) = param_2;
  *puVar3 = uVar1;
  puVar3[1] = uVar2;
  return;
}



