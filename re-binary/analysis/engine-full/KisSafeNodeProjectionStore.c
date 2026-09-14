/* Class KisSafeNodeProjectionStore - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSafeNodeProjectionStore @ 00201ca0 ======

void __thiscall
KisSafeNodeProjectionStore::KisSafeNodeProjectionStore(KisSafeNodeProjectionStore *this)

{
  (*(code *)PTR_KisSafeNodeProjectionStore_00838920)();
  return;
}



// ====== KisSafeNodeProjectionStore @ 002034c0 ======

void __thiscall
KisSafeNodeProjectionStore::KisSafeNodeProjectionStore
          (KisSafeNodeProjectionStore *this,KisSafeNodeProjectionStore *param_1)

{
  (*(code *)PTR_KisSafeNodeProjectionStore_00839530)();
  return;
}



// ====== KisSafeNodeProjectionStore @ 00570660 ======

/* KisSafeNodeProjectionStore::KisSafeNodeProjectionStore() */

void __thiscall
KisSafeNodeProjectionStore::KisSafeNodeProjectionStore(KisSafeNodeProjectionStore *this)

{
  undefined *puVar1;
  StoreImplementationInterface *pSVar2;
  
  pSVar2 = (StoreImplementationInterface *)operator_new(0x20);
  *(undefined8 *)(pSVar2 + 8) = 0;
  puVar1 = PTR_shared_null_008377d0;
  *(undefined **)(pSVar2 + 0x10) = PTR_shared_null_008377d0;
  *(undefined **)(pSVar2 + 0x18) = puVar1;
  *(undefined ***)pSVar2 = &PTR_FUN_00820420;
  KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase
            ((KisSafeNodeProjectionStoreBase *)this,pSVar2);
  *(undefined **)this = PTR_vtable_00837248 + 0x10;
  return;
}



// ====== KisSafeNodeProjectionStore @ 00570970 ======

/* KisSafeNodeProjectionStore::KisSafeNodeProjectionStore(KisSafeNodeProjectionStore const&) */

void __thiscall
KisSafeNodeProjectionStore::KisSafeNodeProjectionStore
          (KisSafeNodeProjectionStore *this,KisSafeNodeProjectionStore *param_1)

{
  KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase
            ((KisSafeNodeProjectionStoreBase *)this,(KisSafeNodeProjectionStoreBase *)param_1);
  *(undefined **)this = PTR_vtable_00837248 + 0x10;
  return;
}



