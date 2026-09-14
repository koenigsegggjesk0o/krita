/* Class KisBusyWaitBroker - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisBusyWaitBroker @ 00207190 ======

void __thiscall KisBusyWaitBroker::KisBusyWaitBroker(KisBusyWaitBroker *this)

{
  (*(code *)PTR_KisBusyWaitBroker_0083b398)();
  return;
}



// ====== KisBusyWaitBroker @ 0046aba0 ======

/* KisBusyWaitBroker::KisBusyWaitBroker() */

void __thiscall KisBusyWaitBroker::KisBusyWaitBroker(KisBusyWaitBroker *this)

{
  undefined *puVar1;
  undefined8 *puVar2;
  
  puVar2 = (undefined8 *)operator_new(0x38);
  puVar1 = PTR_shared_null_00836c40;
  *puVar2 = 0;
  puVar2[1] = puVar1;
  *(undefined4 *)(puVar2 + 2) = 0;
  puVar2[5] = 0;
  puVar2[6] = 0;
  *(undefined8 **)this = puVar2;
  *(undefined (*) [16])(puVar2 + 3) = (undefined  [16])0x0;
  return;
}



