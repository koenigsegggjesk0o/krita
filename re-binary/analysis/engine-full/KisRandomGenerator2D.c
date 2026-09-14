/* Class KisRandomGenerator2D - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisRandomGenerator2D @ 005f2790 ======

/* KisRandomGenerator2D::KisRandomGenerator2D(unsigned long long) */

void __thiscall
KisRandomGenerator2D::KisRandomGenerator2D(KisRandomGenerator2D *this,ulonglong param_1)

{
  ulonglong *puVar1;
  
  puVar1 = (ulonglong *)operator_new(8);
  *(ulonglong **)this = puVar1;
  *puVar1 = param_1;
  return;
}



