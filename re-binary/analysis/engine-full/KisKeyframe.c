/* Class KisKeyframe - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisKeyframe @ 00206930 ======

void __thiscall KisKeyframe::KisKeyframe(KisKeyframe *this)

{
  (*(code *)PTR_KisKeyframe_0083af68)();
  return;
}



// ====== KisKeyframe @ 0064b6e0 ======

/* KisKeyframe::KisKeyframe() */

void __thiscall KisKeyframe::KisKeyframe(KisKeyframe *this)

{
  undefined4 *puVar1;
  
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837678 + 0x10;
                    /* try { // try from 0064b707 to 0064b70b has its CatchHandler @ 0064b71d */
  puVar1 = (undefined4 *)operator_new(4);
  *puVar1 = 0;
  *(undefined4 **)(this + 0x10) = puVar1;
  return;
}



