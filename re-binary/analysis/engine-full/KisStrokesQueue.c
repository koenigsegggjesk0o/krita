/* Class KisStrokesQueue - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisStrokesQueue @ 00209910 ======

void __thiscall KisStrokesQueue::KisStrokesQueue(KisStrokesQueue *this)

{
  (*(code *)PTR_KisStrokesQueue_0083c758)();
  return;
}



// ====== KisStrokesQueue @ 004f1e60 ======

/* KisStrokesQueue::KisStrokesQueue() */

void __thiscall KisStrokesQueue::KisStrokesQueue(KisStrokesQueue *this)

{
  undefined8 uVar1;
  undefined *puVar2;
  undefined8 *puVar3;
  
  *(undefined **)this = PTR_vtable_00836bc8 + 0x10;
                    /* try { // try from 004f1e88 to 004f1e8c has its CatchHandler @ 004f1f91 */
  puVar3 = (undefined8 *)operator_new(0xf8);
  *puVar3 = this;
  puVar2 = PTR_shared_null_00837830;
  *(undefined4 *)(puVar3 + 2) = 0;
  *(undefined2 *)(puVar3 + 4) = 0x100;
  puVar3[1] = puVar2;
  *(undefined2 *)((long)puVar3 + 0x14) = 0;
  uVar1 = DAT_007227d0;
  *(undefined8 *)((long)puVar3 + 0x24) = 0;
  puVar3[3] = uVar1;
  puVar3[6] = 0;
  puVar3[9] = 0;
  puVar3[10] = 0;
  puVar3[0xd] = 0;
  puVar3[0xe] = 0;
  puVar3[0x11] = 0;
  puVar3[0x12] = 0;
  puVar3[0x15] = 0;
  puVar3[0x16] = 0;
  *(undefined (*) [16])(puVar3 + 7) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar3 + 0xb) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar3 + 0xf) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar3 + 0x13) = (undefined  [16])0x0;
                    /* try { // try from 004f1f3c to 004f1f40 has its CatchHandler @ 004f1fa9 */
  KisSurrogateUndoStore::KisSurrogateUndoStore((KisSurrogateUndoStore *)(puVar3 + 0x17));
  puVar2 = PTR_vtable_00837d48;
  puVar3[0x1b] = this;
  puVar3[0x1a] = puVar2 + 0x10;
                    /* try { // try from 004f1f6e to 004f1f72 has its CatchHandler @ 004f1f9d */
  KisPostExecutionUndoAdapter::KisPostExecutionUndoAdapter
            ((KisPostExecutionUndoAdapter *)(puVar3 + 0x1c),(KisUndoStore *)(puVar3 + 0x17),
             (KisStrokesFacade *)(puVar3 + 0x1a));
  puVar3[0x1e] = 0;
  *(undefined8 **)(this + 8) = puVar3;
  return;
}



