/* Class KisInterstrokeData - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisInterstrokeData @ 00601a50 ======

/* KisInterstrokeData::KisInterstrokeData(KisSharedPtr<KisPaintDevice>) */

void __thiscall
KisInterstrokeData::KisInterstrokeData(KisInterstrokeData *this,KisSharedPtr param_1)

{
  KisPaintDevice *this_00;
  long lVar1;
  undefined8 uVar2;
  int *piVar3;
  undefined4 in_register_00000034;
  
  *(undefined **)this = PTR_vtable_00837ed8 + 0x10;
  uVar2 = KisPaintDevice::offset(*(KisPaintDevice **)CONCAT44(in_register_00000034,param_1));
  this_00 = *(KisPaintDevice **)CONCAT44(in_register_00000034,param_1);
  *(undefined8 *)(this + 8) = uVar2;
  uVar2 = KisPaintDevice::colorSpace(this_00);
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(undefined8 *)(this + 0x10) = uVar2;
  *(long *)(this + 0x18) = lVar1;
  if (lVar1 != 0) {
    piVar3 = *(int **)(lVar1 + 0x18);
    if (piVar3 == (int *)0x0) {
      piVar3 = (int *)operator_new(4);
      *piVar3 = 0;
      *(int **)(lVar1 + 0x18) = piVar3;
      LOCK();
      *piVar3 = *piVar3 + 1;
      UNLOCK();
      piVar3 = *(int **)(lVar1 + 0x18);
    }
    *(int **)(this + 0x20) = piVar3;
    LOCK();
    *piVar3 = *piVar3 + 2;
    UNLOCK();
    return;
  }
  *(undefined8 *)(this + 0x20) = 0;
  return;
}



