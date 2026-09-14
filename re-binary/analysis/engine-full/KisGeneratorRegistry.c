/* Class KisGeneratorRegistry - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGeneratorRegistry @ 0020cb70 ======

void __thiscall
KisGeneratorRegistry::KisGeneratorRegistry(KisGeneratorRegistry *this,QObject *param_1)

{
  (*(code *)PTR_KisGeneratorRegistry_0083e088)();
  return;
}



// ====== KisGeneratorRegistry @ 003883a0 ======

/* KisGeneratorRegistry::KisGeneratorRegistry(QObject*) */

void __thiscall
KisGeneratorRegistry::KisGeneratorRegistry(KisGeneratorRegistry *this,QObject *param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  
  puVar3 = PTR_shared_null_00837830;
  puVar2 = PTR_vtable_008377f0;
  puVar1 = PTR_vtable_008377f0 + 0x80;
  QObject::QObject((QObject *)this,param_1);
  *(undefined **)this = puVar2 + 0x10;
  *(undefined **)(this + 0x10) = puVar1;
  *(undefined **)(this + 0x18) = puVar3;
  puVar1 = PTR_shared_null_00836c40;
  *(undefined **)(this + 0x20) = PTR_shared_null_00836c40;
  *(undefined **)(this + 0x28) = puVar1;
  return;
}



