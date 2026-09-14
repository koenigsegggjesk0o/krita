/* Class KisSurrogateUndoAdapter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSurrogateUndoAdapter @ 00206630 ======

void __thiscall KisSurrogateUndoAdapter::KisSurrogateUndoAdapter(KisSurrogateUndoAdapter *this)

{
  (*(code *)PTR_KisSurrogateUndoAdapter_0083ade8)();
  return;
}



// ====== KisSurrogateUndoAdapter @ 0062b0b0 ======

/* KisSurrogateUndoAdapter::KisSurrogateUndoAdapter() */

void __thiscall KisSurrogateUndoAdapter::KisSurrogateUndoAdapter(KisSurrogateUndoAdapter *this)

{
  KisSurrogateUndoStore *this_00;
  
  this_00 = (KisSurrogateUndoStore *)operator_new(0x18);
                    /* try { // try from 0062b0cd to 0062b0d1 has its CatchHandler @ 0062b0fc */
  KisSurrogateUndoStore::KisSurrogateUndoStore(this_00);
  KisUndoAdapter::KisUndoAdapter((KisUndoAdapter *)this,(KisUndoStore *)this_00,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837650 + 0x10;
  *(undefined8 *)(this + 0x18) = *(undefined8 *)(this + 0x10);
  return;
}



