/* Class KisGrowSelectionFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGrowSelectionFilter @ 00209500 ======

void __thiscall
KisGrowSelectionFilter::KisGrowSelectionFilter(KisGrowSelectionFilter *this,int param_1,int param_2)

{
  (*(code *)PTR_KisGrowSelectionFilter_0083c550)();
  return;
}



// ====== KisGrowSelectionFilter @ 00648810 ======

/* KisGrowSelectionFilter::KisGrowSelectionFilter(int, int) */

void __thiscall
KisGrowSelectionFilter::KisGrowSelectionFilter(KisGrowSelectionFilter *this,int param_1,int param_2)

{
  undefined *puVar1;
  
  puVar1 = PTR_vtable_008371b8;
  *(int *)(this + 8) = param_1;
  *(int *)(this + 0xc) = param_2;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



