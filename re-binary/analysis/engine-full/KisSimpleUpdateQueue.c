/* Class KisSimpleUpdateQueue - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSimpleUpdateQueue @ 00201f90 ======

void __thiscall KisSimpleUpdateQueue::KisSimpleUpdateQueue(KisSimpleUpdateQueue *this)

{
  (*(code *)PTR_KisSimpleUpdateQueue_00838a98)();
  return;
}



// ====== KisSimpleUpdateQueue @ 004f3ee0 ======

/* KisSimpleUpdateQueue::KisSimpleUpdateQueue() */

void __thiscall KisSimpleUpdateQueue::KisSimpleUpdateQueue(KisSimpleUpdateQueue *this)

{
  undefined *puVar1;
  
  puVar1 = PTR_vtable_00837d70;
  *(undefined8 *)(this + 8) = 0;
  *(undefined4 *)(this + 0x40) = 0xffffffff;
  *(undefined **)this = puVar1 + 0x10;
  puVar1 = PTR_shared_null_00837830;
  *(undefined **)(this + 0x10) = PTR_shared_null_00837830;
  *(undefined **)(this + 0x18) = puVar1;
                    /* try { // try from 004f3f19 to 004f3f1d has its CatchHandler @ 004f3f25 */
  updateSettings(this);
  return;
}



