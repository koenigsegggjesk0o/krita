/* Class KisSafeSelectionNodeProjectionStore - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSafeSelectionNodeProjectionStore @ 00207640 ======

void __thiscall
KisSafeSelectionNodeProjectionStore::KisSafeSelectionNodeProjectionStore
          (KisSafeSelectionNodeProjectionStore *this,KisSafeSelectionNodeProjectionStore *param_1)

{
  (*(code *)PTR_KisSafeSelectionNodeProjectionStore_0083b5f0)();
  return;
}



// ====== KisSafeSelectionNodeProjectionStore @ 00208220 ======

void __thiscall
KisSafeSelectionNodeProjectionStore::KisSafeSelectionNodeProjectionStore
          (KisSafeSelectionNodeProjectionStore *this)

{
  (*(code *)PTR_KisSafeSelectionNodeProjectionStore_0083bbe0)();
  return;
}



// ====== KisSafeSelectionNodeProjectionStore @ 005706b0 ======

/* KisSafeSelectionNodeProjectionStore::KisSafeSelectionNodeProjectionStore() */

void __thiscall
KisSafeSelectionNodeProjectionStore::KisSafeSelectionNodeProjectionStore
          (KisSafeSelectionNodeProjectionStore *this)

{
  undefined *puVar1;
  StoreImplementationInterface *pSVar2;
  
  pSVar2 = (StoreImplementationInterface *)operator_new(0x20);
  *(undefined8 *)(pSVar2 + 8) = 0;
  puVar1 = PTR_shared_null_008377d0;
  *(undefined **)(pSVar2 + 0x10) = PTR_shared_null_008377d0;
  *(undefined **)(pSVar2 + 0x18) = puVar1;
  *(undefined ***)pSVar2 = &PTR_FUN_00820460;
  KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase
            ((KisSafeNodeProjectionStoreBase *)this,pSVar2);
  *(undefined **)this = PTR_vtable_00836e08 + 0x10;
  return;
}



// ====== KisSafeSelectionNodeProjectionStore @ 00570990 ======

/* KisSafeSelectionNodeProjectionStore::KisSafeSelectionNodeProjectionStore(KisSafeSelectionNodeProjectionStore
   const&) */

void __thiscall
KisSafeSelectionNodeProjectionStore::KisSafeSelectionNodeProjectionStore
          (KisSafeSelectionNodeProjectionStore *this,KisSafeSelectionNodeProjectionStore *param_1)

{
  KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase
            ((KisSafeNodeProjectionStoreBase *)this,(KisSafeNodeProjectionStoreBase *)param_1);
  *(undefined **)this = PTR_vtable_00836e08 + 0x10;
  return;
}



