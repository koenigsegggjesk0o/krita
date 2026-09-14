/* Class KisShrinkSelectionFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisShrinkSelectionFilter @ 0020a4e0 ======

void __thiscall
KisShrinkSelectionFilter::KisShrinkSelectionFilter
          (KisShrinkSelectionFilter *this,int param_1,int param_2,bool param_3)

{
  (*(code *)PTR_KisShrinkSelectionFilter_0083cd40)();
  return;
}



// ====== KisShrinkSelectionFilter @ 00648830 ======

/* KisShrinkSelectionFilter::KisShrinkSelectionFilter(int, int, bool) */

void __thiscall
KisShrinkSelectionFilter::KisShrinkSelectionFilter
          (KisShrinkSelectionFilter *this,int param_1,int param_2,bool param_3)

{
  undefined *puVar1;
  
  puVar1 = PTR_vtable_00837d58;
  *(int *)(this + 8) = param_1;
  *(int *)(this + 0xc) = param_2;
  *(uint *)(this + 0x10) = (uint)param_3;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



