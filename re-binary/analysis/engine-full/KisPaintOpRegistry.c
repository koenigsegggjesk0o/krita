/* Class KisPaintOpRegistry - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintOpRegistry @ 00208fc0 ======

void __thiscall KisPaintOpRegistry::KisPaintOpRegistry(KisPaintOpRegistry *this)

{
  (*(code *)PTR_KisPaintOpRegistry_0083c2b0)();
  return;
}



// ====== KisPaintOpRegistry @ 00342c60 ======

/* KisPaintOpRegistry::KisPaintOpRegistry() */

void __thiscall KisPaintOpRegistry::KisPaintOpRegistry(KisPaintOpRegistry *this)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  
  puVar3 = PTR_vtable_00837ad0;
  puVar2 = PTR_shared_null_00837830;
  puVar1 = PTR_vtable_00837ad0 + 0x80;
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = puVar3 + 0x10;
  *(undefined **)(this + 0x10) = puVar1;
  *(undefined **)(this + 0x18) = puVar2;
  puVar1 = PTR_shared_null_00836c40;
  *(undefined **)(this + 0x20) = PTR_shared_null_00836c40;
  *(undefined **)(this + 0x28) = puVar1;
  return;
}



