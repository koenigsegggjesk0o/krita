/* Class KisRecycleProjectionsJob - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisRecycleProjectionsJob @ 00209fd0 ======

void __thiscall
KisRecycleProjectionsJob::KisRecycleProjectionsJob
          (KisRecycleProjectionsJob *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisRecycleProjectionsJob_0083cab8)();
  return;
}



// ====== KisRecycleProjectionsJob @ 0064b560 ======

/* KisRecycleProjectionsJob::KisRecycleProjectionsJob(KisWeakSharedPtr<KisSafeNodeProjectionStoreBase>)
    */

void __thiscall
KisRecycleProjectionsJob::KisRecycleProjectionsJob
          (KisRecycleProjectionsJob *this,KisWeakSharedPtr param_1)

{
  uint *puVar1;
  long lVar2;
  undefined *puVar3;
  int *piVar4;
  undefined4 in_register_00000034;
  
  puVar3 = PTR_vtable_008373b0;
  this[8] = (KisRecycleProjectionsJob)0x0;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(undefined **)this = puVar3 + 0x10;
  puVar1 = *(uint **)(CONCAT44(in_register_00000034,param_1) + 8);
  if (lVar2 == 0) {
    *(undefined8 *)(this + 0x10) = 0;
  }
  else {
    if ((puVar1 == (uint *)0x0) || ((*puVar1 & 1) == 0)) {
      this[8] = (KisRecycleProjectionsJob)0x1;
      *(undefined (*) [16])(this + 0x10) = (undefined  [16])0x0;
      return;
    }
    lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
    *(long *)(this + 0x10) = lVar2;
    if (lVar2 != 0) {
      piVar4 = *(int **)(lVar2 + 0x18);
      if (piVar4 == (int *)0x0) {
        piVar4 = (int *)operator_new(4);
        *piVar4 = 0;
        *(int **)(lVar2 + 0x18) = piVar4;
        LOCK();
        *piVar4 = *piVar4 + 1;
        UNLOCK();
        piVar4 = *(int **)(lVar2 + 0x18);
      }
      *(int **)(this + 0x18) = piVar4;
      LOCK();
      *piVar4 = *piVar4 + 2;
      UNLOCK();
      this[8] = (KisRecycleProjectionsJob)0x1;
      return;
    }
  }
  *(undefined8 *)(this + 0x18) = 0;
  this[8] = (KisRecycleProjectionsJob)0x1;
  return;
}



