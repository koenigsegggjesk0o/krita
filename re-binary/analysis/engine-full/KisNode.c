/* Class KisNode - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNode @ 00202480 ======

void __thiscall KisNode::KisNode(KisNode *this,KisNode *param_1)

{
  (*(code *)PTR_KisNode_00838d10)();
  return;
}



// ====== KisNode @ 002065b0 ======

void __thiscall KisNode::KisNode(KisNode *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisNode_0083ada8)();
  return;
}



// ====== KisNode @ 005b56b0 ======

/* KisNode::KisNode(KisWeakSharedPtr<KisImage>) */

void __thiscall KisNode::KisNode(KisNode *this,KisWeakSharedPtr param_1)

{
  int iVar1;
  undefined auVar2 [8];
  undefined auVar3 [16];
  undefined *puVar4;
  undefined (*pauVar5) [16];
  KisProjectionLeaf *this_00;
  undefined4 *puVar6;
  undefined (*pauVar7) [16];
  int *piVar8;
  undefined4 in_register_00000034;
  long *plVar9;
  long in_FS_OFFSET;
  undefined local_48 [8];
  int *piStack_40;
  long local_30;
  
  plVar9 = (long *)CONCAT44(in_register_00000034,param_1);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar9 == 0) {
    local_48 = (undefined  [8])0x0;
    auVar2 = local_48;
  }
  else {
    if (((uint *)plVar9[1] == (uint *)0x0) || ((*(uint *)plVar9[1] & 1) == 0)) {
      local_48 = (undefined  [8])0x0;
      piStack_40 = (int *)0x0;
      goto LAB_005b56f6;
    }
    auVar2 = (undefined  [8])*plVar9;
    piStack_40 = (int *)local_48;
    local_48 = auVar2;
    if (auVar2 != (undefined  [8])0x0) {
      piVar8 = *(int **)((long)auVar2 + 0x58);
      if (piVar8 == (int *)0x0) {
        piVar8 = (int *)operator_new(4);
        *piVar8 = 0;
        *(int **)((long)auVar2 + 0x58) = piVar8;
        LOCK();
        *piVar8 = *piVar8 + 1;
        UNLOCK();
        piVar8 = *(int **)((long)auVar2 + 0x58);
        auVar2 = local_48;
      }
      local_48 = auVar2;
      LOCK();
      *piVar8 = *piVar8 + 2;
      UNLOCK();
      piStack_40 = piVar8;
      goto LAB_005b56f6;
    }
  }
  local_48 = auVar2;
  piStack_40 = (int *)0x0;
LAB_005b56f6:
                    /* try { // try from 005b56ff to 005b5703 has its CatchHandler @ 005b58ff */
  KisBaseNode::KisBaseNode((KisBaseNode *)this,(KisWeakSharedPtr)local_48);
  piVar8 = piStack_40;
  auVar3._8_8_ = 0;
  auVar3._0_8_ = piStack_40;
  _local_48 = auVar3 << 0x40;
  if (piVar8 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar8;
    *piVar8 = *piVar8 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar8 != (int *)0x0)) {
      operator_delete(piVar8,4);
    }
  }
  *(undefined **)this = PTR_vtable_008370f0 + 0x10;
                    /* try { // try from 005b573c to 005b5740 has its CatchHandler @ 005b590b */
  pauVar5 = (undefined (*) [16])operator_new(0x78);
  *(undefined8 *)pauVar5[1] = 0;
  *pauVar5 = (undefined  [16])0x0;
  puVar4 = PTR_shared_null_00837830;
  *(undefined8 *)pauVar5[2] = 0;
  *(undefined **)(pauVar5[1] + 8) = puVar4;
  *(undefined8 *)(pauVar5[2] + 8) = 0;
                    /* try { // try from 005b5777 to 005b577b has its CatchHandler @ 005b5917 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(pauVar5 + 3),0);
                    /* try { // try from 005b5781 to 005b5785 has its CatchHandler @ 005b58f3 */
  this_00 = (KisProjectionLeaf *)operator_new(0x10);
                    /* try { // try from 005b578f to 005b5793 has its CatchHandler @ 005b5923 */
  KisProjectionLeaf::KisProjectionLeaf(this_00,this);
  *(KisProjectionLeaf **)(pauVar5[3] + 8) = this_00;
                    /* try { // try from 005b579d to 005b57a1 has its CatchHandler @ 005b58f3 */
  puVar6 = (undefined4 *)operator_new(0x18);
  *(KisProjectionLeaf **)(puVar6 + 4) = this_00;
  *(code **)(puVar6 + 2) = FUN_005b7af0;
  puVar6[1] = 1;
  *puVar6 = 1;
  *(undefined4 **)pauVar5[4] = puVar6;
  pauVar5[7][0] = 0;
  piVar8 = *(int **)(*pauVar5 + 8);
  *(undefined (**) [16])(this + 0x28) = pauVar5;
  *(undefined8 *)*pauVar5 = 0;
  pauVar7 = pauVar5;
  if (piVar8 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar8;
    *piVar8 = *piVar8 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (*(void **)(*pauVar5 + 8) != (void *)0x0)) {
      operator_delete(*(void **)(*pauVar5 + 8),4);
    }
    pauVar7 = *(undefined (**) [16])(this + 0x28);
  }
  *pauVar5 = (undefined  [16])0x0;
  *(undefined8 *)pauVar7[1] = 0;
                    /* try { // try from 005b5809 to 005b5818 has its CatchHandler @ 005b590b */
  QObject::thread();
  QObject::moveToThread((QThread *)this);
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisNode @ 005b6740 ======

/* KisNode::KisNode(KisNode const&) */

void __thiscall KisNode::KisNode(KisNode *this,KisNode *param_1)

{
  KisNode *pKVar1;
  int iVar2;
  long lVar3;
  KisCloneLayer *pKVar4;
  undefined *puVar5;
  undefined (*pauVar6) [16];
  KisProjectionLeaf *this_00;
  undefined4 *puVar7;
  undefined (*pauVar8) [16];
  long lVar9;
  KisCloneLayer *this_01;
  int *piVar10;
  undefined8 *puVar11;
  long in_FS_OFFSET;
  KisNode *local_60;
  KisNode *local_58;
  int *local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisBaseNode::KisBaseNode((KisBaseNode *)this,(KisBaseNode *)param_1);
  *(undefined **)this = PTR_vtable_008370f0 + 0x10;
                    /* try { // try from 005b6781 to 005b6785 has its CatchHandler @ 005b6a59 */
  pauVar6 = (undefined (*) [16])operator_new(0x78);
  *(undefined8 *)pauVar6[1] = 0;
  *pauVar6 = (undefined  [16])0x0;
  puVar5 = PTR_shared_null_00837830;
  *(undefined8 *)pauVar6[2] = 0;
  *(undefined **)(pauVar6[1] + 8) = puVar5;
  *(undefined8 *)(pauVar6[2] + 8) = 0;
                    /* try { // try from 005b67bc to 005b67c0 has its CatchHandler @ 005b6a4d */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(pauVar6 + 3),0);
                    /* try { // try from 005b67c6 to 005b67ca has its CatchHandler @ 005b6a41 */
  this_00 = (KisProjectionLeaf *)operator_new(0x10);
                    /* try { // try from 005b67d4 to 005b67d8 has its CatchHandler @ 005b6a35 */
  KisProjectionLeaf::KisProjectionLeaf(this_00,this);
  *(KisProjectionLeaf **)(pauVar6[3] + 8) = this_00;
                    /* try { // try from 005b67e2 to 005b67e6 has its CatchHandler @ 005b6a41 */
  puVar7 = (undefined4 *)operator_new(0x18);
  *(KisProjectionLeaf **)(puVar7 + 4) = this_00;
  *(code **)(puVar7 + 2) = FUN_005b7af0;
  puVar7[1] = 1;
  *puVar7 = 1;
  *(undefined4 **)pauVar6[4] = puVar7;
  pauVar6[7][0] = 0;
  piVar10 = *(int **)(*pauVar6 + 8);
  *(undefined (**) [16])(this + 0x28) = pauVar6;
  *(undefined8 *)*pauVar6 = 0;
  pauVar8 = pauVar6;
  if (piVar10 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar10;
    *piVar10 = *piVar10 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (*(void **)(*pauVar6 + 8) != (void *)0x0)) {
      operator_delete(*(void **)(*pauVar6 + 8),4);
    }
    pauVar8 = *(undefined (**) [16])(this + 0x28);
  }
  *pauVar6 = (undefined  [16])0x0;
  *(undefined8 *)pauVar8[1] = 0;
                    /* try { // try from 005b6852 to 005b68a9 has its CatchHandler @ 005b6a59 */
  QObject::thread();
  QObject::moveToThread((QThread *)this);
  lVar3 = *(long *)(*(long *)(param_1 + 0x28) + 0x18);
  lVar9 = (long)*(int *)(lVar3 + 8) * 8;
  puVar11 = (undefined8 *)(lVar3 + 0x10 + lVar9);
  if ((long)*(int *)(lVar3 + 0xc) * 8 != lVar9) {
    do {
      pKVar4 = *(KisCloneLayer **)*puVar11;
      if (*(code **)(*(long *)pKVar4 + 0x178) == FUN_002dd780) {
        this_01 = (KisCloneLayer *)operator_new(0x40);
                    /* try { // try from 005b68b3 to 005b68b7 has its CatchHandler @ 005b6a71 */
        KisCloneLayer::KisCloneLayer(this_01,pKVar4);
        LOCK();
        *(int *)(this_01 + 0x10) = *(int *)(this_01 + 0x10) + 1;
        UNLOCK();
        local_60 = (KisNode *)this_01;
      }
      else {
                    /* try { // try from 005b699d to 005b699e has its CatchHandler @ 005b6a59 */
        (**(code **)(*(long *)pKVar4 + 0x178))(&local_60,pKVar4);
      }
                    /* try { // try from 005b68c7 to 005b68dd has its CatchHandler @ 005b6a65 */
      createNodeProgressProxy(local_60);
      FUN_00374e90(*(long *)(this + 0x28) + 0x18,&local_60);
      pKVar1 = local_60;
      piVar10 = *(int **)(this + 0x18);
      local_58 = this;
      if (piVar10 == (int *)0x0) {
                    /* try { // try from 005b69ad to 005b69b1 has its CatchHandler @ 005b6a65 */
        piVar10 = (int *)operator_new(4);
        *piVar10 = 0;
        *(int **)(this + 0x18) = piVar10;
        LOCK();
        *piVar10 = *piVar10 + 1;
        UNLOCK();
        piVar10 = *(int **)(this + 0x18);
      }
      LOCK();
      *piVar10 = *piVar10 + 2;
      UNLOCK();
      local_50 = piVar10;
                    /* try { // try from 005b6909 to 005b690d has its CatchHandler @ 005b6a7d */
      setParent(pKVar1,(KisWeakSharedPtr)&local_58);
      local_58 = (KisNode *)0x0;
      if (local_50 != (int *)0x0) {
        LOCK();
        iVar2 = *local_50;
        *local_50 = *local_50 + -2;
        UNLOCK();
        if ((iVar2 < 3) && (local_50 != (int *)0x0)) {
          operator_delete(local_50,4);
        }
      }
      if (local_60 != (KisNode *)0x0) {
        LOCK();
        pKVar1 = local_60 + 0x10;
        *(int *)pKVar1 = *(int *)pKVar1 + -1;
        UNLOCK();
        if (*(int *)pKVar1 == 0) {
          (**(code **)(*(long *)local_60 + 0x20))();
        }
      }
      puVar11 = puVar11 + 1;
    } while ((undefined8 *)
             (*(long *)(*(long *)(param_1 + 0x28) + 0x18) + 0x10 +
             (long)*(int *)(*(long *)(*(long *)(param_1 + 0x28) + 0x18) + 0xc) * 8) != puVar11);
  }
                    /* try { // try from 005b69dd to 005b69e1 has its CatchHandler @ 005b6a59 */
  FUN_005b3e90(*(undefined8 *)(this + 0x28),param_1,this,this);
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



