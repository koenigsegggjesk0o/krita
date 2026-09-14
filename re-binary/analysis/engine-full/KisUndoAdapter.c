/* Class KisUndoAdapter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisUndoAdapter @ 002030f0 ======

void __thiscall
KisUndoAdapter::KisUndoAdapter(KisUndoAdapter *this,KisUndoStore *param_1,QObject *param_2)

{
  (*(code *)PTR_KisUndoAdapter_00839348)();
  return;
}



// ====== KisUndoAdapter @ 0062ad30 ======

/* KisUndoAdapter::KisUndoAdapter(KisUndoStore*, QObject*) */

void __thiscall
KisUndoAdapter::KisUndoAdapter(KisUndoAdapter *this,KisUndoStore *param_1,QObject *param_2)

{
  undefined *puVar1;
  
  QObject::QObject((QObject *)this,param_2);
  puVar1 = PTR_vtable_00837a78;
  *(KisUndoStore **)(this + 0x10) = param_1;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



