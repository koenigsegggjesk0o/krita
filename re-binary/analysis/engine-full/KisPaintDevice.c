/* Class KisPaintDevice - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintDevice @ 00201ce0 ======

void __thiscall
KisPaintDevice::KisPaintDevice(KisPaintDevice *this,KoColorSpace *param_1,QString *param_2)

{
  (*(code *)PTR_KisPaintDevice_00838940)();
  return;
}



// ====== KisPaintDevice @ 00201f30 ======

void __thiscall
KisPaintDevice::KisPaintDevice
          (KisPaintDevice *this,KisPaintDevice *param_1,DeviceCopyMode param_2,KisNode *param_3)

{
  (*(code *)PTR_KisPaintDevice_00838a68)();
  return;
}



// ====== KisPaintDevice @ 00205ca0 ======

void __thiscall
KisPaintDevice::KisPaintDevice
          (KisPaintDevice *this,KisWeakSharedPtr param_1,KoColorSpace *param_2,KisSharedPtr param_3,
          QString *param_4)

{
  (*(code *)PTR_KisPaintDevice_0083a920)();
  return;
}



// ====== KisPaintDevice @ 0020aa00 ======

void __thiscall
KisPaintDevice::KisPaintDevice
          (KisPaintDevice *this,KisPaintDevice *param_1,DeviceCopyMode param_2,KisNode *param_3)

{
  (*(code *)PTR_KisPaintDevice_0083cfd0)();
  return;
}



// ====== KisPaintDevice @ 0020ade0 ======

void __thiscall
KisPaintDevice::KisPaintDevice
          (KisPaintDevice *this,KisWeakSharedPtr param_1,KoColorSpace *param_2,KisSharedPtr param_3,
          QString *param_4)

{
  (*(code *)PTR_KisPaintDevice_0083d1c0)();
  return;
}



// ====== KisPaintDevice @ 005c1d50 ======

/* KisPaintDevice::KisPaintDevice(KisPaintDevice const&, KritaUtils::DeviceCopyMode, KisNode*) */

void __thiscall
KisPaintDevice::KisPaintDevice
          (KisPaintDevice *this,KisPaintDevice *param_1,DeviceCopyMode param_2,KisNode *param_3)

{
  Private *this_00;
  
  QObject::QObject((QObject *)this,(QObject *)0x0);
                    /* try { // try from 005c1d7c to 005c1d80 has its CatchHandler @ 005c1dd0 */
  KisShared::KisShared((KisShared *)(this + 0x10));
  *(undefined **)this = PTR_vtable_00837b68 + 0x10;
                    /* try { // try from 005c1d94 to 005c1d98 has its CatchHandler @ 005c1de8 */
  this_00 = (Private *)operator_new(0x88);
                    /* try { // try from 005c1da2 to 005c1da6 has its CatchHandler @ 005c1ddc */
  Private::Private(this_00,this);
  *(Private **)(this + 0x20) = this_00;
  if (this != param_1) {
                    /* try { // try from 005c1dbc to 005c1dc0 has its CatchHandler @ 005c1de8 */
    makeFullCopyFrom(this,param_1,param_2,param_3);
  }
  return;
}



// ====== KisPaintDevice @ 005c43c0 ======

/* KisPaintDevice::KisPaintDevice(KoColorSpace const*, QString const&) */

void __thiscall
KisPaintDevice::KisPaintDevice(KisPaintDevice *this,KoColorSpace *param_1,QString *param_2)

{
  int iVar1;
  undefined auVar2 [16];
  undefined8 uVar3;
  Private *this_00;
  KisDefaultBounds *pKVar4;
  long in_FS_OFFSET;
  KisDefaultBounds *local_50;
  undefined local_48 [24];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
                    /* try { // try from 005c43f7 to 005c43fb has its CatchHandler @ 005c4522 */
  KisShared::KisShared((KisShared *)(this + 0x10));
  *(undefined **)this = PTR_vtable_00837b68 + 0x10;
                    /* try { // try from 005c440f to 005c4413 has its CatchHandler @ 005c4516 */
  this_00 = (Private *)operator_new(0x88);
                    /* try { // try from 005c441d to 005c4421 has its CatchHandler @ 005c452e */
  Private::Private(this_00,this);
  *(Private **)(this + 0x20) = this_00;
  local_48._0_16_ = (undefined  [16])0x0;
                    /* try { // try from 005c4434 to 005c4438 has its CatchHandler @ 005c44ed */
  pKVar4 = (KisDefaultBounds *)operator_new(0x20);
                    /* try { // try from 005c443f to 005c4443 has its CatchHandler @ 005c450a */
  KisDefaultBounds::KisDefaultBounds(pKVar4);
  LOCK();
  *(int *)(pKVar4 + 8) = *(int *)(pKVar4 + 8) + 1;
  UNLOCK();
  local_50 = pKVar4;
                    /* try { // try from 005c4464 to 005c4468 has its CatchHandler @ 005c44fe */
  init((KoColorSpace *)this,(KisSharedPtr)param_1,(KisWeakSharedPtr)&local_50,(QString *)local_48);
  if (local_50 != (KisDefaultBounds *)0x0) {
    LOCK();
    pKVar4 = local_50 + 8;
    *(int *)pKVar4 = *(int *)pKVar4 + -1;
    UNLOCK();
    if (*(int *)pKVar4 == 0) {
      (**(code **)(*(long *)local_50 + 8))();
    }
  }
  uVar3 = local_48._8_8_;
  auVar2._8_8_ = 0;
  auVar2._0_8_ = local_48._8_8_;
  local_48._0_16_ = auVar2 << 0x40;
  if ((int *)uVar3 != (int *)0x0) {
    LOCK();
    iVar1 = *(int *)uVar3;
    *(int *)uVar3 = *(int *)uVar3 + -2;
    UNLOCK();
    if ((iVar1 < 3) && ((int *)uVar3 != (int *)0x0)) {
      operator_delete((void *)uVar3,4);
    }
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPaintDevice @ 005c5e20 ======

/* KisPaintDevice::KisPaintDevice(KisWeakSharedPtr<KisNode>, KoColorSpace const*,
   KisSharedPtr<KisDefaultBoundsBase>, QString const&) */

void __thiscall
KisPaintDevice::KisPaintDevice
          (KisPaintDevice *this,KisWeakSharedPtr param_1,KoColorSpace *param_2,KisSharedPtr param_3,
          QString *param_4)

{
  int iVar1;
  uint *puVar2;
  undefined auVar3 [8];
  long lVar4;
  undefined auVar5 [16];
  Private *this_00;
  int *piVar6;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000034;
  long *plVar7;
  long in_FS_OFFSET;
  long *local_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  piStack_50 = (int *)local_58;
  plVar7 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
                    /* try { // try from 005c5e61 to 005c5e65 has its CatchHandler @ 005c5fdf */
  KisShared::KisShared((KisShared *)(this + 0x10));
  *(undefined **)this = PTR_vtable_00837b68 + 0x10;
                    /* try { // try from 005c5e79 to 005c5e7d has its CatchHandler @ 005c5feb */
  this_00 = (Private *)operator_new(0x88);
                    /* try { // try from 005c5e87 to 005c5e8b has its CatchHandler @ 005c5ff7 */
  Private::Private(this_00,this);
  lVar4 = *plVar7;
  puVar2 = (uint *)plVar7[1];
  *(Private **)(this + 0x20) = this_00;
  if (lVar4 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar3 = local_58;
  }
  else {
    if ((puVar2 == (uint *)0x0) || ((*puVar2 & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
      goto LAB_005c5eb7;
    }
    auVar3 = (undefined  [8])*plVar7;
    local_58 = auVar3;
    if (auVar3 != (undefined  [8])0x0) {
      piVar6 = *(int **)((long)auVar3 + 0x18);
      if (piVar6 == (int *)0x0) {
                    /* try { // try from 005c5fb5 to 005c5fb9 has its CatchHandler @ 005c5feb */
        piVar6 = (int *)operator_new(4);
        *piVar6 = 0;
        *(int **)((long)auVar3 + 0x18) = piVar6;
        LOCK();
        *piVar6 = *piVar6 + 1;
        UNLOCK();
        piVar6 = *(int **)((long)auVar3 + 0x18);
        auVar3 = local_58;
      }
      local_58 = auVar3;
      LOCK();
      *piVar6 = *piVar6 + 2;
      UNLOCK();
      piStack_50 = piVar6;
      goto LAB_005c5eb7;
    }
  }
  local_58 = auVar3;
  piStack_50 = (int *)0x0;
LAB_005c5eb7:
  local_60 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (local_60 != (long *)0x0) {
    LOCK();
    *(int *)(local_60 + 1) = *(int *)(local_60 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 005c5ee2 to 005c5ee6 has its CatchHandler @ 005c5fd3 */
  init((KoColorSpace *)this,(KisSharedPtr)param_2,(KisWeakSharedPtr)&local_60,(QString *)local_58);
  if (local_60 != (long *)0x0) {
    LOCK();
    plVar7 = local_60 + 1;
    *(int *)plVar7 = *(int *)plVar7 + -1;
    UNLOCK();
    if (*(int *)plVar7 == 0) {
      (**(code **)(*local_60 + 8))();
    }
  }
  piVar6 = piStack_50;
  auVar5._8_8_ = 0;
  auVar5._0_8_ = piStack_50;
  _local_58 = auVar5 << 0x40;
  if (piVar6 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar6;
    *piVar6 = *piVar6 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar6 != (int *)0x0)) {
      operator_delete(piVar6,4);
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



