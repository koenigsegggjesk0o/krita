/* Class KisGrowUntilDarkestPixelSelectionFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGrowUntilDarkestPixelSelectionFilter @ 0020c630 ======

void __thiscall
KisGrowUntilDarkestPixelSelectionFilter::KisGrowUntilDarkestPixelSelectionFilter
          (KisGrowUntilDarkestPixelSelectionFilter *this,int param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisGrowUntilDarkestPixelSelectionFilter_0083dde8)();
  return;
}



// ====== KisGrowUntilDarkestPixelSelectionFilter @ 00648ba0 ======

/* KisGrowUntilDarkestPixelSelectionFilter::KisGrowUntilDarkestPixelSelectionFilter(int,
   KisSharedPtr<KisPaintDevice>) */

void __thiscall
KisGrowUntilDarkestPixelSelectionFilter::KisGrowUntilDarkestPixelSelectionFilter
          (KisGrowUntilDarkestPixelSelectionFilter *this,int param_1,KisSharedPtr param_2)

{
  long lVar1;
  undefined *puVar2;
  undefined4 in_register_00000014;
  
  puVar2 = PTR_vtable_00836da8;
  *(int *)(this + 8) = param_1;
  *(undefined **)this = puVar2 + 0x10;
  lVar1 = *(long *)CONCAT44(in_register_00000014,param_2);
  *(long *)(this + 0x10) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  return;
}



