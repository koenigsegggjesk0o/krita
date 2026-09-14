/* Class KisFixedPaintDevice - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisFixedPaintDevice @ 00203c10 ======

void __thiscall
KisFixedPaintDevice::KisFixedPaintDevice
          (KisFixedPaintDevice *this,KoColorSpace *param_1,QSharedPointer param_2)

{
  (*(code *)PTR_KisFixedPaintDevice_008398d8)();
  return;
}



// ====== KisFixedPaintDevice @ 00206a30 ======

void __thiscall
KisFixedPaintDevice::KisFixedPaintDevice(KisFixedPaintDevice *this,KisFixedPaintDevice *param_1)

{
  (*(code *)PTR_KisFixedPaintDevice_0083afe8)();
  return;
}



// ====== KisFixedPaintDevice @ 005d72c0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisFixedPaintDevice::KisFixedPaintDevice(KoColorSpace const*,
   QSharedPointer<KisOptimizedByteArray::MemoryAllocator>) */

void __thiscall
KisFixedPaintDevice::KisFixedPaintDevice
          (KisFixedPaintDevice *this,KoColorSpace *param_1,QSharedPointer param_2)

{
  int *piVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  int *piVar5;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  undefined8 local_48;
  int *piStack_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)(this + 8));
  puVar4 = PTR_vtable_008371e0;
  *(KoColorSpace **)(this + 0x18) = param_1;
  uVar3 = DAT_00721778;
  uVar2 = _DAT_00721770;
  *(undefined **)this = puVar4 + 0x10;
  *(undefined8 *)(this + 0x20) = uVar2;
  *(undefined8 *)(this + 0x28) = uVar3;
  local_48 = *(undefined8 *)CONCAT44(in_register_00000014,param_2);
  piStack_40 = (int *)((undefined8 *)CONCAT44(in_register_00000014,param_2))[1];
  if (piStack_40 != (int *)0x0) {
    LOCK();
    *piStack_40 = *piStack_40 + 1;
    UNLOCK();
    LOCK();
    piStack_40[1] = piStack_40[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 005d7339 to 005d733d has its CatchHandler @ 005d7394 */
  KisOptimizedByteArray::KisOptimizedByteArray
            ((KisOptimizedByteArray *)(this + 0x30),(QSharedPointer)&local_48);
  piVar5 = piStack_40;
  if (piStack_40 != (int *)0x0) {
    LOCK();
    piVar1 = piStack_40 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piStack_40 + 2))(piStack_40);
    }
    LOCK();
    *piVar5 = *piVar5 + -1;
    UNLOCK();
    if (*piVar5 == 0) {
      operator_delete(piVar5,0x10);
    }
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisFixedPaintDevice @ 005d73a0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisFixedPaintDevice::KisFixedPaintDevice(KisFixedPaintDevice const&) */

void __thiscall
KisFixedPaintDevice::KisFixedPaintDevice(KisFixedPaintDevice *this,KisFixedPaintDevice *param_1)

{
  int *piVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  long in_FS_OFFSET;
  undefined local_48 [24];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)(this + 8));
  uVar3 = DAT_00721778;
  uVar2 = _DAT_00721770;
  *(undefined **)this = PTR_vtable_008371e0 + 0x10;
  *(undefined8 *)(this + 0x20) = uVar2;
  *(undefined8 *)(this + 0x28) = uVar3;
  local_48._0_16_ = (undefined  [16])0x0;
                    /* try { // try from 005d73fe to 005d7402 has its CatchHandler @ 005d7480 */
  KisOptimizedByteArray::KisOptimizedByteArray
            ((KisOptimizedByteArray *)(this + 0x30),(QSharedPointer)local_48);
  uVar2 = local_48._8_8_;
  if ((int *)local_48._8_8_ != (int *)0x0) {
    LOCK();
    piVar1 = (int *)(local_48._8_8_ + 4);
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_48._8_8_ + 8))(local_48._8_8_);
    }
    LOCK();
    *(int *)uVar2 = *(int *)uVar2 + -1;
    UNLOCK();
    if (*(int *)uVar2 == 0) {
      operator_delete((void *)uVar2,0x10);
    }
  }
  uVar2 = *(undefined8 *)(param_1 + 0x20);
  uVar3 = *(undefined8 *)(param_1 + 0x28);
  *(undefined8 *)(this + 0x18) = *(undefined8 *)(param_1 + 0x18);
  *(undefined8 *)(this + 0x20) = uVar2;
  *(undefined8 *)(this + 0x28) = uVar3;
                    /* try { // try from 005d743e to 005d7442 has its CatchHandler @ 005d7474 */
  KisOptimizedByteArray::operator=
            ((KisOptimizedByteArray *)(this + 0x30),(KisOptimizedByteArray *)(param_1 + 0x30));
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



