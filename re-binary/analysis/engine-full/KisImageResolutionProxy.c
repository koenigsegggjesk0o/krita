/* Class KisImageResolutionProxy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageResolutionProxy @ 002023f0 ======

void __thiscall KisImageResolutionProxy::KisImageResolutionProxy(KisImageResolutionProxy *this)

{
  (*(code *)PTR_KisImageResolutionProxy_00838cc8)();
  return;
}



// ====== KisImageResolutionProxy @ 002065a0 ======

void __thiscall
KisImageResolutionProxy::KisImageResolutionProxy
          (KisImageResolutionProxy *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisImageResolutionProxy_0083ada0)();
  return;
}



// ====== KisImageResolutionProxy @ 0020aef0 ======

void __thiscall
KisImageResolutionProxy::KisImageResolutionProxy
          (KisImageResolutionProxy *this,KisImageResolutionProxy *param_1)

{
  (*(code *)PTR_KisImageResolutionProxy_0083d248)();
  return;
}



// ====== KisImageResolutionProxy @ 005308d0 ======

/* KisImageResolutionProxy::KisImageResolutionProxy(KisWeakSharedPtr<KisImage>) */

void __thiscall
KisImageResolutionProxy::KisImageResolutionProxy
          (KisImageResolutionProxy *this,KisWeakSharedPtr param_1)

{
  int iVar1;
  uint uVar2;
  undefined auVar3 [16];
  undefined8 uVar4;
  Private *this_00;
  int *piVar5;
  undefined4 in_register_00000034;
  long *plVar6;
  long in_FS_OFFSET;
  long local_58;
  uint *puStack_50;
  undefined local_48 [8];
  int *piStack_40;
  long local_30;
  
  piStack_40 = (int *)local_48;
  plVar6 = (long *)CONCAT44(in_register_00000034,param_1);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837a88 + 0x10;
                    /* try { // try from 00530911 to 00530915 has its CatchHandler @ 00530b38 */
  this_00 = (Private *)operator_new(0x28);
  if (*plVar6 == 0) {
    local_58 = 0;
LAB_00530ad8:
    puStack_50 = (uint *)0x0;
  }
  else if (((uint *)plVar6[1] == (uint *)0x0) || ((*(uint *)plVar6[1] & 1) == 0)) {
    local_58 = 0;
    puStack_50 = (uint *)0x0;
  }
  else {
    local_58 = *plVar6;
    if (local_58 == 0) goto LAB_00530ad8;
    puStack_50 = *(uint **)(local_58 + 0x58);
    if (puStack_50 == (uint *)0x0) {
                    /* try { // try from 00530b15 to 00530b19 has its CatchHandler @ 00530b68 */
      piVar5 = (int *)operator_new(4);
      *piVar5 = 0;
      *(int **)(local_58 + 0x58) = piVar5;
      LOCK();
      *piVar5 = *piVar5 + 1;
      UNLOCK();
      puStack_50 = *(uint **)(local_58 + 0x58);
    }
    LOCK();
    *puStack_50 = *puStack_50 + 2;
    UNLOCK();
  }
  *(undefined (*) [16])this_00 = (undefined  [16])0x0;
  uVar4 = DAT_007227c0;
  *(undefined8 *)(this_00 + 0x10) = DAT_007227c0;
  *(undefined8 *)(this_00 + 0x18) = uVar4;
                    /* try { // try from 0053095e to 00530962 has its CatchHandler @ 00530b44 */
  QMetaObject::Connection::Connection((Connection *)(this_00 + 0x20));
  if (local_58 == 0) {
    local_48 = (undefined  [8])0x0;
LAB_00530ac1:
    piStack_40 = (int *)0x0;
  }
  else if ((puStack_50 == (uint *)0x0) || ((*puStack_50 & 1) == 0)) {
    local_48 = (undefined  [8])0x0;
    piStack_40 = (int *)0x0;
  }
  else {
    local_48 = (undefined  [8])local_58;
    auVar3 = _local_48;
    if (local_58 == 0) goto LAB_00530ac1;
    piStack_40 = *(int **)(local_58 + 0x58);
    if (piStack_40 == (int *)0x0) {
      _local_48 = auVar3;
                    /* try { // try from 00530af5 to 00530af9 has its CatchHandler @ 00530b50 */
      piVar5 = (int *)operator_new(4);
      *piVar5 = 0;
      *(int **)(local_58 + 0x58) = piVar5;
      LOCK();
      *piVar5 = *piVar5 + 1;
      UNLOCK();
      piStack_40 = *(int **)(local_58 + 0x58);
    }
    LOCK();
    *piStack_40 = *piStack_40 + 2;
    UNLOCK();
  }
                    /* try { // try from 00530996 to 0053099a has its CatchHandler @ 00530b5c */
  Private::setImage(this_00,(KisWeakSharedPtr)local_48);
  piVar5 = piStack_40;
  auVar3._8_8_ = 0;
  auVar3._0_8_ = piStack_40;
  _local_48 = auVar3 << 0x40;
  if (piVar5 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar5;
    *piVar5 = *piVar5 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar5 != (int *)0x0)) {
      operator_delete(piVar5,4);
    }
  }
  *(Private **)(this + 0x10) = this_00;
  if (puStack_50 != (uint *)0x0) {
    LOCK();
    uVar2 = *puStack_50;
    *puStack_50 = *puStack_50 - 2;
    UNLOCK();
    if (((int)uVar2 < 3) && (puStack_50 != (uint *)0x0)) {
      if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
        operator_delete(puStack_50,4);
        return;
      }
      goto LAB_00530b33;
    }
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_00530b33:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisImageResolutionProxy @ 00530b80 ======

/* KisImageResolutionProxy::KisImageResolutionProxy() */

void __thiscall KisImageResolutionProxy::KisImageResolutionProxy(KisImageResolutionProxy *this)

{
  int iVar1;
  undefined auVar2 [16];
  undefined8 uVar3;
  long in_FS_OFFSET;
  undefined local_38 [24];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_38._0_16_ = (undefined  [16])0x0;
                    /* try { // try from 00530ba8 to 00530bac has its CatchHandler @ 00530c03 */
  KisImageResolutionProxy(this,(KisWeakSharedPtr)local_38);
  uVar3 = local_38._8_8_;
  auVar2._8_8_ = 0;
  auVar2._0_8_ = local_38._8_8_;
  local_38._0_16_ = auVar2 << 0x40;
  if ((int *)uVar3 != (int *)0x0) {
    LOCK();
    iVar1 = *(int *)uVar3;
    *(int *)uVar3 = *(int *)uVar3 + -2;
    UNLOCK();
    if ((iVar1 < 3) && ((int *)uVar3 != (int *)0x0)) {
      operator_delete((void *)uVar3,4);
    }
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisImageResolutionProxy @ 00530d10 ======

/* KisImageResolutionProxy::KisImageResolutionProxy(KisImageResolutionProxy const&) */

void __thiscall
KisImageResolutionProxy::KisImageResolutionProxy
          (KisImageResolutionProxy *this,KisImageResolutionProxy *param_1)

{
  int iVar1;
  long *plVar2;
  undefined auVar3 [8];
  long lVar4;
  undefined auVar5 [16];
  Private *this_00;
  int *piVar6;
  long in_FS_OFFSET;
  undefined local_48 [8];
  int *piStack_40;
  long local_30;
  
  piStack_40 = (int *)local_48;
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837a88 + 0x10;
                    /* try { // try from 00530d51 to 00530d55 has its CatchHandler @ 00530e75 */
  this_00 = (Private *)operator_new(0x28);
  plVar2 = *(long **)(param_1 + 0x10);
  *(undefined (*) [16])this_00 = (undefined  [16])0x0;
  lVar4 = plVar2[3];
  *(long *)(this_00 + 0x10) = plVar2[2];
  *(long *)(this_00 + 0x18) = lVar4;
                    /* try { // try from 00530d77 to 00530d7b has its CatchHandler @ 00530e8d */
  QMetaObject::Connection::Connection((Connection *)(this_00 + 0x20));
  if (*plVar2 == 0) {
    local_48 = (undefined  [8])0x0;
    auVar3 = local_48;
  }
  else {
    if (((uint *)plVar2[1] == (uint *)0x0) || ((*(uint *)plVar2[1] & 1) == 0)) {
      local_48 = (undefined  [8])0x0;
      piStack_40 = (int *)0x0;
      goto LAB_00530d9f;
    }
    auVar3 = (undefined  [8])*plVar2;
    local_48 = auVar3;
    if (auVar3 != (undefined  [8])0x0) {
      piVar6 = *(int **)((long)auVar3 + 0x58);
      if (piVar6 == (int *)0x0) {
                    /* try { // try from 00530e55 to 00530e59 has its CatchHandler @ 00530e99 */
        piVar6 = (int *)operator_new(4);
        *piVar6 = 0;
        *(int **)((long)auVar3 + 0x58) = piVar6;
        LOCK();
        *piVar6 = *piVar6 + 1;
        UNLOCK();
        piVar6 = *(int **)((long)auVar3 + 0x58);
        auVar3 = local_48;
      }
      local_48 = auVar3;
      LOCK();
      *piVar6 = *piVar6 + 2;
      UNLOCK();
      piStack_40 = piVar6;
      goto LAB_00530d9f;
    }
  }
  local_48 = auVar3;
  piStack_40 = (int *)0x0;
LAB_00530d9f:
                    /* try { // try from 00530da8 to 00530dac has its CatchHandler @ 00530e81 */
  Private::setImage(this_00,(KisWeakSharedPtr)local_48);
  piVar6 = piStack_40;
  auVar5._8_8_ = 0;
  auVar5._0_8_ = piStack_40;
  _local_48 = auVar5 << 0x40;
  if (piVar6 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar6;
    *piVar6 = *piVar6 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar6 != (int *)0x0)) {
      operator_delete(piVar6,4);
    }
  }
  *(Private **)(this + 0x10) = this_00;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



