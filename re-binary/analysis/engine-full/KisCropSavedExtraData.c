/* Class KisCropSavedExtraData - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCropSavedExtraData @ 00209460 ======

void __thiscall KisCropSavedExtraData::KisCropSavedExtraData(void)

{
  (*(code *)PTR_KisCropSavedExtraData_0083c500)();
  return;
}



// ====== KisCropSavedExtraData @ 0053b1a0 ======

/* KisCropSavedExtraData::KisCropSavedExtraData(KisCropSavedExtraData::Type, QRect,
   KisSharedPtr<KisNode>) */

void __thiscall
KisCropSavedExtraData::KisCropSavedExtraData
          (KisCropSavedExtraData *this,undefined4 param_1,undefined8 param_3,undefined8 param_4,
          long *param_5)

{
  long lVar1;
  undefined *puVar2;
  
  puVar2 = PTR_vtable_00837f10;
  *(undefined4 *)(this + 8) = param_1;
  *(undefined8 *)(this + 0xc) = param_3;
  lVar1 = *param_5;
  *(undefined8 *)(this + 0x14) = param_4;
  *(undefined **)this = puVar2 + 0x10;
  *(long *)(this + 0x20) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  return;
}



