/* Class KisImageAnimationInterface - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageAnimationInterface @ 00209e20 ======

void __thiscall
KisImageAnimationInterface::KisImageAnimationInterface
          (KisImageAnimationInterface *this,KisImage *param_1)

{
  (*(code *)PTR_KisImageAnimationInterface_0083c9e0)();
  return;
}



// ====== KisImageAnimationInterface @ 0020aff0 ======

void __thiscall
KisImageAnimationInterface::KisImageAnimationInterface
          (KisImageAnimationInterface *this,KisImageAnimationInterface *param_1,KisImage *param_2)

{
  (*(code *)PTR_KisImageAnimationInterface_0083d2c8)();
  return;
}



// ====== KisImageAnimationInterface @ 0050cb70 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisImageAnimationInterface::KisImageAnimationInterface(KisImage*) */

void __thiscall
KisImageAnimationInterface::KisImageAnimationInterface
          (KisImageAnimationInterface *this,KisImage *param_1)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined *puVar6;
  undefined8 *puVar7;
  undefined4 *puVar8;
  long in_FS_OFFSET;
  QObject local_40 [8];
  undefined *local_38;
  undefined8 local_30;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)param_1);
  *(undefined **)this = PTR_vtable_00837390 + 0x10;
                    /* try { // try from 0050cbac to 0050cbb0 has its CatchHandler @ 0050ccad */
  puVar7 = (undefined8 *)operator_new(0x70);
  puVar5 = PTR_shared_null_008377d0;
  puVar4 = PTR_shared_null_00836c40;
  *(undefined4 *)(puVar7 + 8) = 0xffffffff;
  *(undefined2 *)(puVar7 + 1) = 0;
  puVar6 = PTR_shared_null_008377d0;
  uVar3 = DAT_00732170;
  puVar7[7] = puVar5;
  puVar7[5] = puVar4;
  puVar7[6] = puVar6;
  *(undefined (*) [16])(puVar7 + 9) = (undefined  [16])0x0;
  uVar2 = DAT_00732168;
  uVar1 = _DAT_00732160;
  *(undefined4 *)(puVar7 + 0xb) = 0;
  puVar7[0xc] = 0;
  puVar7[0xd] = 0;
  *(undefined8 **)(this + 0x10) = puVar7;
  *puVar7 = param_1;
  *(undefined8 *)((long)puVar7 + 0x1c) = uVar3;
  *(undefined8 *)((long)puVar7 + 0xc) = uVar1;
  *(undefined8 *)((long)puVar7 + 0x14) = uVar2;
  local_30 = 0;
  local_38 = PTR_sigInternalRequestTimeSwitch_00837968;
                    /* try { // try from 0050cc31 to 0050cc77 has its CatchHandler @ 0050cca1 */
  puVar8 = (undefined4 *)operator_new(0x18);
  *puVar8 = 1;
  *(KisImageAnimationInterface **)(puVar8 + 4) = this;
  *(code **)(puVar8 + 2) = FUN_0050c740;
  QObject::connectImpl
            (local_40,(void **)this,(QObject *)&local_38,(void **)this,(QSlotObjectBase *)0x0,
             (ConnectionType)puVar8,(int *)0x0,(QMetaObject *)0x0);
  QMetaObject::Connection::~Connection((Connection *)local_40);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisImageAnimationInterface @ 0050ccc0 ======

/* KisImageAnimationInterface::KisImageAnimationInterface(KisImageAnimationInterface const&,
   KisImage*) */

void __thiscall
KisImageAnimationInterface::KisImageAnimationInterface
          (KisImageAnimationInterface *this,KisImageAnimationInterface *param_1,KisImage *param_2)

{
  undefined4 uVar1;
  long lVar2;
  undefined8 uVar3;
  int *piVar4;
  undefined8 *puVar5;
  undefined4 *puVar6;
  long in_FS_OFFSET;
  QObject local_50 [8];
  undefined *local_48;
  undefined8 local_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837390 + 0x10;
                    /* try { // try from 0050cd03 to 0050cd07 has its CatchHandler @ 0050ce4a */
  puVar5 = (undefined8 *)operator_new(0x70);
  lVar2 = *(long *)(param_1 + 0x10);
  *puVar5 = param_2;
  *(undefined2 *)(puVar5 + 1) = 0;
  uVar3 = *(undefined8 *)(lVar2 + 0xc);
  piVar4 = *(int **)(lVar2 + 0x30);
  *(undefined4 *)(puVar5 + 4) = 0xffffffff;
  *(undefined8 *)((long)puVar5 + 0xc) = uVar3;
  *(undefined8 *)((long)puVar5 + 0x14) = *(undefined8 *)(lVar2 + 0x14);
  uVar1 = *(undefined4 *)(lVar2 + 0x1c);
  puVar5[6] = piVar4;
  *(undefined4 *)((long)puVar5 + 0x1c) = uVar1;
  puVar5[5] = PTR_shared_null_00836c40;
  if (1 < *piVar4 + 1U) {
    LOCK();
    *piVar4 = *piVar4 + 1;
    UNLOCK();
  }
  piVar4 = *(int **)(lVar2 + 0x38);
  puVar5[7] = piVar4;
  if (1 < *piVar4 + 1U) {
    LOCK();
    *piVar4 = *piVar4 + 1;
    UNLOCK();
  }
  uVar1 = *(undefined4 *)(lVar2 + 0x40);
  *(undefined4 *)(puVar5 + 0xb) = 0;
  *(undefined (*) [16])(puVar5 + 9) = (undefined  [16])0x0;
  uVar3 = *(undefined8 *)(lVar2 + 0x68);
  *(undefined4 *)(puVar5 + 8) = uVar1;
  puVar5[0xc] = 0;
  puVar5[0xd] = uVar3;
  *(undefined8 **)(this + 0x10) = puVar5;
  local_40 = 0;
  local_48 = PTR_sigInternalRequestTimeSwitch_00837968;
                    /* try { // try from 0050cdab to 0050cdf1 has its CatchHandler @ 0050ce3e */
  puVar6 = (undefined4 *)operator_new(0x18);
  *puVar6 = 1;
  *(KisImageAnimationInterface **)(puVar6 + 4) = this;
  *(code **)(puVar6 + 2) = FUN_0050c6f0;
  QObject::connectImpl
            (local_50,(void **)this,(QObject *)&local_48,(void **)this,(QSlotObjectBase *)0x0,
             (ConnectionType)puVar6,(int *)0x0,(QMetaObject *)0x0);
  QMetaObject::Connection::~Connection((Connection *)local_50);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



