/* Class KisIndirectPaintingSupport - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisIndirectPaintingSupport @ 002021b0 ======

void __thiscall
KisIndirectPaintingSupport::KisIndirectPaintingSupport(KisIndirectPaintingSupport *this)

{
  (*(code *)PTR_KisIndirectPaintingSupport_00838ba8)();
  return;
}



// ====== KisIndirectPaintingSupport @ 005442a0 ======

/* KisIndirectPaintingSupport::KisIndirectPaintingSupport() */

void __thiscall
KisIndirectPaintingSupport::KisIndirectPaintingSupport(KisIndirectPaintingSupport *this)

{
  undefined *puVar1;
  undefined8 *puVar2;
  
  *(undefined **)this = PTR_vtable_00837460 + 0x10;
  puVar2 = (undefined8 *)operator_new(0x38);
  *puVar2 = 0;
  puVar1 = PTR_shared_null_008377d0;
  puVar2[4] = 0;
  puVar2[1] = puVar1;
  puVar2[3] = puVar1;
                    /* try { // try from 005442ec to 005442f0 has its CatchHandler @ 00544300 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(puVar2 + 5),0);
  *(undefined *)(puVar2 + 6) = 1;
  *(undefined8 **)(this + 8) = puVar2;
  return;
}



