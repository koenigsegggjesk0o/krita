/* Class KisGreenCoordinatesMath - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGreenCoordinatesMath @ 00208030 ======

void __thiscall KisGreenCoordinatesMath::KisGreenCoordinatesMath(KisGreenCoordinatesMath *this)

{
  (*(code *)PTR_KisGreenCoordinatesMath_0083bae8)();
  return;
}



// ====== KisGreenCoordinatesMath @ 00628ee0 ======

/* KisGreenCoordinatesMath::KisGreenCoordinatesMath() */

void __thiscall KisGreenCoordinatesMath::KisGreenCoordinatesMath(KisGreenCoordinatesMath *this)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined8 *puVar3;
  
  puVar1 = PTR_shared_null_008377d0;
  puVar3 = (undefined8 *)operator_new(0x20);
  puVar2 = PTR_shared_null_008377d0;
  *(undefined4 *)(puVar3 + 2) = 0;
  puVar3[3] = puVar2;
  *(undefined8 **)this = puVar3;
  *puVar3 = puVar1;
  puVar3[1] = puVar1;
  return;
}



