/* Class KisOutlineGenerator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisOutlineGenerator @ 0020bc00 ======

void __thiscall
KisOutlineGenerator::KisOutlineGenerator
          (KisOutlineGenerator *this,KoColorSpace *param_1,uchar param_2)

{
  (*(code *)PTR_KisOutlineGenerator_0083d8d0)();
  return;
}



// ====== KisOutlineGenerator @ 0063ba30 ======

/* KisOutlineGenerator::KisOutlineGenerator(KoColorSpace const*, unsigned char) */

void __thiscall
KisOutlineGenerator::KisOutlineGenerator
          (KisOutlineGenerator *this,KoColorSpace *param_1,uchar param_2)

{
  *(KoColorSpace **)this = param_1;
  this[8] = (KisOutlineGenerator)param_2;
  this[9] = (KisOutlineGenerator)0x0;
  *(undefined8 *)(this + 0x10) = 0;
  return;
}



