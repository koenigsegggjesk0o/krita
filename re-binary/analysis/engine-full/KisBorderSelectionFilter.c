/* Class KisBorderSelectionFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisBorderSelectionFilter @ 006487d0 ======

/* KisBorderSelectionFilter::KisBorderSelectionFilter(int, int, bool) */

void __thiscall
KisBorderSelectionFilter::KisBorderSelectionFilter
          (KisBorderSelectionFilter *this,int param_1,int param_2,bool param_3)

{
  undefined *puVar1;
  
  puVar1 = PTR_vtable_00836fa8;
  *(int *)(this + 8) = param_1;
  *(int *)(this + 0xc) = param_2;
  this[0x10] = (KisBorderSelectionFilter)param_3;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



