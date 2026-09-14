/* Class KisProjectionLeaf - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisProjectionLeaf @ 00200d90 ======

void __thiscall KisProjectionLeaf::KisProjectionLeaf(KisProjectionLeaf *this,KisNode *param_1)

{
  (*(code *)PTR_KisProjectionLeaf_00838198)();
  return;
}



// ====== KisProjectionLeaf @ 00568890 ======

/* KisProjectionLeaf::KisProjectionLeaf(KisNode*) */

void __thiscall KisProjectionLeaf::KisProjectionLeaf(KisProjectionLeaf *this,KisNode *param_1)

{
  undefined8 *puVar1;
  int *piVar2;
  
  *(undefined **)this = PTR_vtable_00837260 + 0x10;
  puVar1 = (undefined8 *)operator_new(0x18);
  *puVar1 = param_1;
  if (param_1 != (KisNode *)0x0) {
    piVar2 = *(int **)(param_1 + 0x18);
    if (piVar2 == (int *)0x0) {
                    /* try { // try from 00568905 to 00568909 has its CatchHandler @ 0056891e */
      piVar2 = (int *)operator_new(4);
      *piVar2 = 0;
      *(int **)(param_1 + 0x18) = piVar2;
      LOCK();
      *piVar2 = *piVar2 + 1;
      UNLOCK();
      piVar2 = *(int **)(param_1 + 0x18);
    }
    puVar1[1] = piVar2;
    LOCK();
    *piVar2 = *piVar2 + 2;
    UNLOCK();
    *(undefined *)(puVar1 + 2) = 0;
    *(undefined8 **)(this + 8) = puVar1;
    return;
  }
  puVar1[1] = 0;
  *(undefined8 **)(this + 8) = puVar1;
  *(undefined *)(puVar1 + 2) = 0;
  return;
}



