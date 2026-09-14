/* Class KisRandomSubAccessor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisRandomSubAccessor @ 00208470 ======

void __thiscall
KisRandomSubAccessor::KisRandomSubAccessor(KisRandomSubAccessor *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisRandomSubAccessor_0083bd08)();
  return;
}



// ====== KisRandomSubAccessor @ 005f2a00 ======

/* KisRandomSubAccessor::KisRandomSubAccessor(KisSharedPtr<KisPaintDevice>) */

void __thiscall
KisRandomSubAccessor::KisRandomSubAccessor(KisRandomSubAccessor *this,KisSharedPtr param_1)

{
  long lVar1;
  long lVar2;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  
  lVar1 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)this);
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x10) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
  *(undefined (*) [16])(this + 0x18) = (undefined  [16])0x0;
                    /* try { // try from 005f2a47 to 005f2a4b has its CatchHandler @ 005f2a68 */
  KisPaintDevice::createRandomConstAccessorNG();
  if (lVar1 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



