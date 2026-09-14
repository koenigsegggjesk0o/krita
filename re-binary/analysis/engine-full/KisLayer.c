/* Class KisLayer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLayer @ 00203740 ======

void __thiscall
KisLayer::KisLayer(KisLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3)

{
  (*(code *)PTR_KisLayer_00839670)();
  return;
}



// ====== KisLayer @ 0020ae50 ======

void __thiscall KisLayer::KisLayer(KisLayer *this,KisLayer *param_1)

{
  (*(code *)PTR_KisLayer_0083d1f8)();
  return;
}



// ====== KisLayer @ 0053d440 ======

/* KisLayer::KisLayer(KisWeakSharedPtr<KisImage>, QString const&, unsigned char) */

void __thiscall
KisLayer::KisLayer(KisLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3)

{
  int *piVar1;
  long *plVar2;
  int iVar3;
  long lVar4;
  long *plVar5;
  undefined auVar6 [8];
  undefined auVar7 [16];
  undefined auVar8 [16];
  undefined *puVar9;
  undefined8 *puVar10;
  Store *this_00;
  void *pvVar11;
  undefined4 *puVar12;
  KisSafeNodeProjectionStore *this_01;
  int *piVar13;
  undefined4 in_register_00000034;
  long *plVar14;
  KisSafeNodeProjectionStore *this_02;
  long in_FS_OFFSET;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  plVar14 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar14 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar6 = local_58;
LAB_0053d798:
    local_58 = auVar6;
    piStack_50 = (int *)0x0;
  }
  else if (((uint *)plVar14[1] == (uint *)0x0) || ((*(uint *)plVar14[1] & 1) == 0)) {
    local_58 = (undefined  [8])0x0;
    piStack_50 = (int *)0x0;
  }
  else {
    auVar6 = (undefined  [8])*plVar14;
    piStack_50 = (int *)local_58;
    local_58 = auVar6;
    if (auVar6 == (undefined  [8])0x0) goto LAB_0053d798;
    piVar13 = *(int **)((long)auVar6 + 0x58);
    if (piVar13 == (int *)0x0) {
      piVar13 = (int *)operator_new(4);
      *piVar13 = 0;
      *(int **)((long)auVar6 + 0x58) = piVar13;
      LOCK();
      *piVar13 = *piVar13 + 1;
      UNLOCK();
      piVar13 = *(int **)((long)auVar6 + 0x58);
      auVar6 = local_58;
    }
    local_58 = auVar6;
    LOCK();
    *piVar13 = *piVar13 + 2;
    UNLOCK();
    piStack_50 = piVar13;
  }
                    /* try { // try from 0053d49a to 0053d49e has its CatchHandler @ 0053d84e */
  KisNode::KisNode((KisNode *)this,(KisWeakSharedPtr)local_58);
  piVar13 = piStack_50;
  auVar7._8_8_ = 0;
  auVar7._0_8_ = piStack_50;
  _local_58 = auVar7 << 0x40;
  if (piVar13 != (int *)0x0) {
    LOCK();
    iVar3 = *piVar13;
    *piVar13 = *piVar13 + -2;
    UNLOCK();
    if ((iVar3 < 3) && (piVar13 != (int *)0x0)) {
      operator_delete(piVar13,4);
    }
  }
  *(undefined **)this = PTR_vtable_00837f40 + 0x10;
                    /* try { // try from 0053d4d7 to 0053d4db has its CatchHandler @ 0053d812 */
  puVar10 = (undefined8 *)operator_new(0x78);
  puVar9 = PTR_shared_null_008377d0;
  puVar10[1] = 0;
  *puVar10 = puVar9;
  puVar9 = PTR_shared_null_00837830;
  puVar10[3] = 0;
  puVar10[2] = puVar9;
  puVar10[4] = 0;
  puVar10[5] = 0;
  puVar10[6] = 0;
  puVar10[7] = 0;
  puVar10[8] = 0;
  puVar10[9] = 0;
  puVar10[10] = this;
                    /* try { // try from 0053d53e to 0053d542 has its CatchHandler @ 0053d82a */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(puVar10 + 0xb),0);
  puVar10[0xd] = 0;
  *(undefined2 *)(puVar10 + 0xc) = 0;
  puVar9 = PTR_shared_null_00837830;
  *(undefined8 **)(this + 0x30) = puVar10;
  puVar10[0xe] = puVar9;
                    /* try { // try from 0053d566 to 0053d58d has its CatchHandler @ 0053d812 */
  QObject::setObjectName((QString *)this);
  (**(code **)(*(long *)this + 0x148))(this);
  KisBaseNode::setOpacity((KisBaseNode *)this,param_3);
  this_00 = (Store *)operator_new(8);
                    /* try { // try from 0053d594 to 0053d598 has its CatchHandler @ 0053d806 */
  KisMetaData::Store::Store(this_00);
  *(Store **)(*(long *)(this + 0x30) + 8) = this_00;
                    /* try { // try from 0053d5a6 to 0053d5aa has its CatchHandler @ 0053d812 */
  pvVar11 = operator_new(0x10);
                    /* try { // try from 0053d5b4 to 0053d5b8 has its CatchHandler @ 0053d81e */
  FUN_00545e10(pvVar11,this);
                    /* try { // try from 0053d5be to 0053d5c2 has its CatchHandler @ 0053d812 */
  puVar12 = (undefined4 *)operator_new(0x18);
  *(void **)(puVar12 + 4) = pvVar11;
  *(code **)(puVar12 + 2) = FUN_00542500;
  puVar12[1] = 1;
  *puVar12 = 1;
  lVar4 = *(long *)(this + 0x30);
  piVar13 = *(int **)(lVar4 + 0x40);
  *(void **)(lVar4 + 0x38) = pvVar11;
  *(undefined4 **)(lVar4 + 0x40) = puVar12;
  if (piVar13 != (int *)0x0) {
    LOCK();
    piVar1 = piVar13 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piVar13 + 2))(piVar13);
    }
    LOCK();
    *piVar13 = *piVar13 + -1;
    UNLOCK();
    if (*piVar13 == 0) {
      operator_delete(piVar13,0x10);
    }
  }
                    /* try { // try from 0053d614 to 0053d618 has its CatchHandler @ 0053d812 */
  this_01 = (KisSafeNodeProjectionStore *)operator_new(0x28);
                    /* try { // try from 0053d61f to 0053d623 has its CatchHandler @ 0053d842 */
  KisSafeNodeProjectionStore::KisSafeNodeProjectionStore(this_01);
  lVar4 = *(long *)(this + 0x30);
  this_02 = *(KisSafeNodeProjectionStore **)(lVar4 + 0x48);
  if (this_01 != this_02) {
    LOCK();
    *(int *)(this_01 + 0x10) = *(int *)(this_01 + 0x10) + 1;
    UNLOCK();
    plVar5 = *(long **)(lVar4 + 0x48);
    *(KisSafeNodeProjectionStore **)(lVar4 + 0x48) = this_01;
    if (plVar5 != (long *)0x0) {
      LOCK();
      plVar2 = plVar5 + 2;
      *(int *)plVar2 = *(int *)plVar2 + -1;
      UNLOCK();
      if (*(int *)plVar2 == 0) {
        (**(code **)(*plVar5 + 0x20))();
      }
    }
    this_02 = *(KisSafeNodeProjectionStore **)(*(long *)(this + 0x30) + 0x48);
  }
  if (*plVar14 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar6 = local_58;
  }
  else {
    if (((uint *)plVar14[1] == (uint *)0x0) || ((*(uint *)plVar14[1] & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
      goto LAB_0053d67d;
    }
    auVar6 = (undefined  [8])*plVar14;
    local_58 = auVar6;
    if (auVar6 != (undefined  [8])0x0) {
      piVar13 = *(int **)((long)auVar6 + 0x58);
      if (piVar13 == (int *)0x0) {
                    /* try { // try from 0053d7e5 to 0053d7e9 has its CatchHandler @ 0053d812 */
        piVar13 = (int *)operator_new(4);
        *piVar13 = 0;
        *(int **)((long)auVar6 + 0x58) = piVar13;
        LOCK();
        *piVar13 = *piVar13 + 1;
        UNLOCK();
        piVar13 = *(int **)((long)auVar6 + 0x58);
        auVar6 = local_58;
      }
      local_58 = auVar6;
      LOCK();
      *piVar13 = *piVar13 + 2;
      UNLOCK();
      piStack_50 = piVar13;
      goto LAB_0053d67d;
    }
  }
  local_58 = auVar6;
  piStack_50 = (int *)0x0;
LAB_0053d67d:
                    /* try { // try from 0053d683 to 0053d687 has its CatchHandler @ 0053d836 */
  KisSafeNodeProjectionStoreBase::setImage
            ((KisSafeNodeProjectionStoreBase *)this_02,(KisWeakSharedPtr)local_58);
  piVar13 = piStack_50;
  auVar8._8_8_ = 0;
  auVar8._0_8_ = piStack_50;
  _local_58 = auVar8 << 0x40;
  if (piVar13 != (int *)0x0) {
    LOCK();
    iVar3 = *piVar13;
    *piVar13 = *piVar13 + -2;
    UNLOCK();
    if ((iVar3 < 3) && (piVar13 != (int *)0x0)) {
      operator_delete(piVar13,4);
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisLayer @ 0053d860 ======

/* KisLayer::KisLayer(KisLayer const&) */

void __thiscall KisLayer::KisLayer(KisLayer *this,KisLayer *param_1)

{
  long *plVar1;
  int iVar2;
  int iVar3;
  QByteArray *this_00;
  QByteArray *pQVar4;
  long lVar5;
  long *plVar6;
  int *piVar7;
  undefined *puVar8;
  undefined *puVar9;
  undefined8 *puVar10;
  Store *this_01;
  void *pvVar11;
  undefined4 *puVar12;
  KisSafeNodeProjectionStore *this_02;
  QArrayData *pQVar13;
  KisLayerStyleProjectionPlane *this_03;
  KisSafeNodeProjectionStore *this_04;
  int *piVar14;
  long in_FS_OFFSET;
  QArrayData *local_48;
  int *piStack_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisNode::KisNode((KisNode *)this,(KisNode *)param_1);
  *(undefined **)this = PTR_vtable_00837f40 + 0x10;
                    /* try { // try from 0053d89f to 0053d8a3 has its CatchHandler @ 0053dd5b */
  puVar10 = (undefined8 *)operator_new(0x78);
  puVar9 = PTR_shared_null_00837830;
  puVar8 = PTR_shared_null_008377d0;
  puVar10[1] = 0;
  *puVar10 = puVar8;
  puVar10[2] = puVar9;
  puVar10[3] = 0;
  puVar10[4] = 0;
  puVar10[5] = 0;
  puVar10[6] = 0;
  puVar10[7] = 0;
  puVar10[8] = 0;
  puVar10[9] = 0;
  puVar10[10] = this;
                    /* try { // try from 0053d906 to 0053d90a has its CatchHandler @ 0053dd4f */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(puVar10 + 0xb),0);
  puVar10[0xd] = 0;
  *(undefined2 *)(puVar10 + 0xc) = 0;
  puVar10[0xe] = puVar9;
  *(undefined8 **)(this + 0x30) = puVar10;
  if (this == param_1) goto LAB_0053dc40;
                    /* try { // try from 0053d92f to 0053d933 has its CatchHandler @ 0053dd5b */
  this_01 = (Store *)operator_new(8);
                    /* try { // try from 0053d943 to 0053d947 has its CatchHandler @ 0053dd8b */
  KisMetaData::Store::Store(this_01,*(Store **)(*(long *)(param_1 + 0x30) + 8));
  this_00 = *(QByteArray **)(this + 0x30);
  pQVar4 = *(QByteArray **)(param_1 + 0x30);
  *(Store **)(this_00 + 8) = this_01;
  QByteArray::operator=(this_00,pQVar4);
                    /* try { // try from 0053d963 to 0053d967 has its CatchHandler @ 0053dd5b */
  QObject::objectName();
                    /* try { // try from 0053d96e to 0053d97f has its CatchHandler @ 0053dd73 */
  QObject::setObjectName((QString *)this);
  (**(code **)(*(long *)this + 0x148))(this);
  if (*(int *)local_48 == 0) {
LAB_0053d999:
    QArrayData::deallocate(local_48,2,8);
  }
  else if (*(int *)local_48 != -1) {
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 == 0) goto LAB_0053d999;
  }
                    /* try { // try from 0053d9ad to 0053d9b1 has its CatchHandler @ 0053dd5b */
  pvVar11 = operator_new(0x10);
                    /* try { // try from 0053d9bb to 0053d9bf has its CatchHandler @ 0053dd7f */
  FUN_00545e10(pvVar11,this);
                    /* try { // try from 0053d9c5 to 0053da1b has its CatchHandler @ 0053dd5b */
  puVar12 = (undefined4 *)operator_new(0x18);
  *(void **)(puVar12 + 4) = pvVar11;
  *(code **)(puVar12 + 2) = FUN_00542500;
  puVar12[1] = 1;
  *puVar12 = 1;
  lVar5 = *(long *)(this + 0x30);
  piVar14 = *(int **)(lVar5 + 0x40);
  *(void **)(lVar5 + 0x38) = pvVar11;
  *(undefined4 **)(lVar5 + 0x40) = puVar12;
  if (piVar14 != (int *)0x0) {
    LOCK();
    piVar7 = piVar14 + 1;
    *piVar7 = *piVar7 + -1;
    UNLOCK();
    if (*piVar7 == 0) {
      (**(code **)(piVar14 + 2))(piVar14);
    }
    LOCK();
    *piVar14 = *piVar14 + -1;
    UNLOCK();
    if (*piVar14 == 0) {
      operator_delete(piVar14,0x10);
    }
  }
  this_02 = (KisSafeNodeProjectionStore *)operator_new(0x28);
                    /* try { // try from 0053da2b to 0053da2f has its CatchHandler @ 0053dd97 */
  KisSafeNodeProjectionStore::KisSafeNodeProjectionStore
            (this_02,*(KisSafeNodeProjectionStore **)(*(long *)(param_1 + 0x30) + 0x48));
  lVar5 = *(long *)(this + 0x30);
  this_04 = *(KisSafeNodeProjectionStore **)(lVar5 + 0x48);
  if (this_02 != this_04) {
    LOCK();
    *(int *)(this_02 + 0x10) = *(int *)(this_02 + 0x10) + 1;
    UNLOCK();
    plVar6 = *(long **)(lVar5 + 0x48);
    *(KisSafeNodeProjectionStore **)(lVar5 + 0x48) = this_02;
    if (plVar6 != (long *)0x0) {
      LOCK();
      plVar1 = plVar6 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar6 + 0x20))();
      }
    }
    this_04 = *(KisSafeNodeProjectionStore **)(*(long *)(this + 0x30) + 0x48);
  }
                    /* try { // try from 0053da68 to 0053da6c has its CatchHandler @ 0053dd5b */
  KisBaseNode::image();
                    /* try { // try from 0053da73 to 0053da77 has its CatchHandler @ 0053dd67 */
  KisSafeNodeProjectionStoreBase::setImage
            ((KisSafeNodeProjectionStoreBase *)this_04,(KisWeakSharedPtr)&local_48);
  local_48 = (QArrayData *)0x0;
  if (piStack_40 != (int *)0x0) {
    LOCK();
    iVar2 = *piStack_40;
    *piStack_40 = *piStack_40 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piStack_40 != (int *)0x0)) {
      operator_delete(piStack_40,4);
    }
  }
  if (*(long **)(*(long *)(param_1 + 0x30) + 0x18) != (long *)0x0) {
                    /* try { // try from 0053dab4 to 0053dab6 has its CatchHandler @ 0053dd5b */
    (**(code **)(**(long **)(*(long *)(param_1 + 0x30) + 0x18) + 0x10))(&local_48);
    pQVar13 = local_48;
    piVar14 = (int *)0x0;
    if ((local_48 != (QArrayData *)0x0) &&
       (pQVar13 = (QArrayData *)
                  __dynamic_cast(local_48,PTR_typeinfo_008370b8,PTR_typeinfo_00837c80,0),
       pQVar13 != (QArrayData *)0x0)) {
      if (piStack_40 != (int *)0x0) {
        piVar7 = piStack_40 + 1;
        iVar2 = piStack_40[1];
        while (0 < iVar2) {
          LOCK();
          iVar3 = *piVar7;
          if (iVar2 == iVar3) {
            *piVar7 = iVar2 + 1;
          }
          UNLOCK();
          if (iVar2 == iVar3) {
            LOCK();
            *piStack_40 = *piStack_40 + 1;
            UNLOCK();
            piVar14 = piStack_40;
            if (*piVar7 != 0) goto LAB_0053db20;
            goto LAB_0053dce3;
          }
          iVar2 = *piVar7;
        }
      }
      piVar14 = (int *)0x0;
LAB_0053dce3:
      pQVar13 = (QArrayData *)0x0;
    }
LAB_0053db20:
    lVar5 = *(long *)(this + 0x30);
    piVar7 = *(int **)(lVar5 + 0x20);
    *(QArrayData **)(lVar5 + 0x18) = pQVar13;
    *(int **)(lVar5 + 0x20) = piVar14;
    if (piVar7 != (int *)0x0) {
      LOCK();
      piVar14 = piVar7 + 1;
      *piVar14 = *piVar14 + -1;
      UNLOCK();
      if (*piVar14 == 0) {
        (**(code **)(piVar7 + 2))(piVar7);
      }
      LOCK();
      *piVar7 = *piVar7 + -1;
      UNLOCK();
      if (*piVar7 == 0) {
        operator_delete(piVar7,0x10);
      }
    }
    piVar14 = piStack_40;
    if (piStack_40 != (int *)0x0) {
      LOCK();
      piVar7 = piStack_40 + 1;
      *piVar7 = *piVar7 + -1;
      UNLOCK();
      if (*piVar7 == 0) {
        (**(code **)(piStack_40 + 2))(piStack_40);
      }
      LOCK();
      *piVar14 = *piVar14 + -1;
      UNLOCK();
      if (*piVar14 == 0) {
        operator_delete(piVar14,0x10);
      }
    }
    if (*(long *)(*(long *)(param_1 + 0x30) + 0x28) != 0) {
                    /* try { // try from 0053db82 to 0053db86 has its CatchHandler @ 0053dd5b */
      this_03 = (KisLayerStyleProjectionPlane *)operator_new(0x10);
      local_48 = *(QArrayData **)(*(long *)(this + 0x30) + 0x18);
      piStack_40 = *(int **)(*(long *)(this + 0x30) + 0x20);
      if (piStack_40 != (int *)0x0) {
        LOCK();
        *piStack_40 = *piStack_40 + 1;
        UNLOCK();
        LOCK();
        piStack_40[1] = piStack_40[1] + 1;
        UNLOCK();
      }
                    /* try { // try from 0053dbc1 to 0053dbc5 has its CatchHandler @ 0053dd37 */
      KisLayerStyleProjectionPlane::KisLayerStyleProjectionPlane
                (this_03,*(KisLayerStyleProjectionPlane **)(*(long *)(param_1 + 0x30) + 0x28),this,
                 (KisWeakSharedPtr)&local_48);
                    /* try { // try from 0053dbcb to 0053dbcf has its CatchHandler @ 0053dd43 */
      puVar12 = (undefined4 *)operator_new(0x18);
      *(KisLayerStyleProjectionPlane **)(puVar12 + 4) = this_03;
      *(code **)(puVar12 + 2) = FUN_005424e0;
      puVar12[1] = 1;
      *puVar12 = 1;
      lVar5 = *(long *)(this + 0x30);
      piVar14 = *(int **)(lVar5 + 0x30);
      *(KisLayerStyleProjectionPlane **)(lVar5 + 0x28) = this_03;
      *(undefined4 **)(lVar5 + 0x30) = puVar12;
      if (piVar14 != (int *)0x0) {
        LOCK();
        piVar7 = piVar14 + 1;
        *piVar7 = *piVar7 + -1;
        UNLOCK();
        if (*piVar7 == 0) {
          (**(code **)(piVar14 + 2))(piVar14);
        }
        LOCK();
        *piVar14 = *piVar14 + -1;
        UNLOCK();
        if (*piVar14 == 0) {
          operator_delete(piVar14,0x10);
        }
      }
      piVar14 = piStack_40;
      if (piStack_40 != (int *)0x0) {
        LOCK();
        piVar7 = piStack_40 + 1;
        *piVar7 = *piVar7 + -1;
        UNLOCK();
        if (*piVar7 == 0) {
          (**(code **)(piStack_40 + 2))(piStack_40);
        }
        LOCK();
        *piVar14 = *piVar14 + -1;
        UNLOCK();
        if (*piVar14 == 0) {
          operator_delete(piVar14,0x10);
        }
      }
    }
  }
LAB_0053dc40:
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



