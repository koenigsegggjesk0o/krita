/* Class KisStrokeSpeedMeasurer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisStrokeSpeedMeasurer @ 00357be0 ======

/* KisStrokeSpeedMeasurer::KisStrokeSpeedMeasurer(int) */

void __thiscall
KisStrokeSpeedMeasurer::KisStrokeSpeedMeasurer(KisStrokeSpeedMeasurer *this,int param_1)

{
  undefined *puVar1;
  int *piVar2;
  
  piVar2 = (int *)operator_new(0x30);
  puVar1 = PTR_shared_null_00837830;
  piVar2[8] = 0;
  piVar2[10] = 0;
  piVar2[0xb] = 0;
  *(undefined **)(piVar2 + 2) = puVar1;
  *(int **)this = piVar2;
  *piVar2 = param_1;
  *(undefined (*) [16])(piVar2 + 4) = (undefined  [16])0x0;
  return;
}



