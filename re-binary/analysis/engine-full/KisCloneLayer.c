/* Class KisCloneLayer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCloneLayer @ 00206250 ======

void __thiscall KisCloneLayer::KisCloneLayer(KisCloneLayer *this,KisCloneLayer *param_1)

{
  (*(code *)PTR_KisCloneLayer_0083abf8)();
  return;
}



// ====== KisCloneLayer @ 0046fbf0 ======

/* KisCloneLayer::KisCloneLayer(KisSharedPtr<KisLayer>, KisWeakSharedPtr<KisImage>, QString const&,
   unsigned char) */

void __thiscall
KisCloneLayer::KisCloneLayer
          (KisCloneLayer *this,KisSharedPtr param_1,KisWeakSharedPtr param_2,QString *param_3,
          uchar param_4)

{
  KisDefaultBounds *pKVar1;
  KisImage *pKVar2;
  int iVar3;
  KisImage *this_00;
  long *plVar4;
  long *plVar5;
  KisLayer *pKVar6;
  undefined auVar7 [8];
  undefined auVar8 [16];
  undefined8 *puVar9;
  KisDefaultBounds *pKVar10;
  KisPaintDevice *this_01;
  KoColorSpace *pKVar11;
  long lVar12;
  int *piVar13;
  undefined4 in_register_00000014;
  ulong *puVar14;
  KisWeakSharedPtr KVar15;
  undefined4 in_register_00000034;
  KisLayer *this_02;
  long in_FS_OFFSET;
  KisDefaultBounds *local_78;
  QArrayData *local_70;
  KisCloneLayer *local_68;
  int *local_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  puVar14 = (ulong *)CONCAT44(in_register_00000014,param_2);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*puVar14 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar7 = local_58;
LAB_004700f1:
    local_58 = auVar7;
    piStack_50 = (int *)0x0;
  }
  else if (((uint *)puVar14[1] == (uint *)0x0) || ((*(uint *)puVar14[1] & 1) == 0)) {
    local_58 = (undefined  [8])0x0;
    piStack_50 = (int *)0x0;
  }
  else {
    auVar7 = (undefined  [8])*puVar14;
    piStack_50 = (int *)local_58;
    local_58 = auVar7;
    if (auVar7 == (undefined  [8])0x0) goto LAB_004700f1;
    piVar13 = *(int **)((long)auVar7 + 0x58);
    if (piVar13 == (int *)0x0) {
      piVar13 = (int *)operator_new(4);
      *piVar13 = 0;
      *(int **)((long)auVar7 + 0x58) = piVar13;
      LOCK();
      *piVar13 = *piVar13 + 1;
      UNLOCK();
      piVar13 = *(int **)((long)auVar7 + 0x58);
      auVar7 = local_58;
    }
    local_58 = auVar7;
    LOCK();
    *piVar13 = *piVar13 + 2;
    UNLOCK();
    piStack_50 = piVar13;
  }
  KVar15 = (KisWeakSharedPtr)local_58;
                    /* try { // try from 0046fc55 to 0046fc59 has its CatchHandler @ 004702c7 */
  KisLayer::KisLayer((KisLayer *)this,KVar15,param_3,param_4);
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
  *(undefined **)this = PTR_vtable_008373f0 + 0x10;
                    /* try { // try from 0046fc93 to 0046fc97 has its CatchHandler @ 00470231 */
  puVar9 = (undefined8 *)operator_new(0x48);
                    /* try { // try from 0046fca0 to 0046fca4 has its CatchHandler @ 00470219 */
  pKVar10 = (KisDefaultBounds *)operator_new(0x20);
  if (*puVar14 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar7 = local_58;
LAB_00470109:
    local_58 = auVar7;
    piStack_50 = (int *)0x0;
  }
  else if (((uint *)puVar14[1] == (uint *)0x0) || ((*(uint *)puVar14[1] & 1) == 0)) {
    _local_58 = (undefined  [16])0x0;
  }
  else {
    auVar7 = (undefined  [8])*puVar14;
    local_58 = auVar7;
    if (auVar7 == (undefined  [8])0x0) goto LAB_00470109;
    piVar13 = *(int **)((long)auVar7 + 0x58);
    if (piVar13 == (int *)0x0) {
                    /* try { // try from 004701fd to 00470201 has its CatchHandler @ 00470249 */
      piVar13 = (int *)operator_new(4);
      *piVar13 = 0;
      *(int **)((long)auVar7 + 0x58) = piVar13;
      LOCK();
      *piVar13 = *piVar13 + 1;
      UNLOCK();
      piVar13 = *(int **)((long)auVar7 + 0x58);
      auVar7 = local_58;
    }
    local_58 = auVar7;
    piStack_50 = piVar13;
    LOCK();
    *piVar13 = *piVar13 + 2;
    UNLOCK();
  }
                    /* try { // try from 0046fcd5 to 0046fcd9 has its CatchHandler @ 00470225 */
  KisDefaultBounds::KisDefaultBounds(pKVar10,KVar15);
  pKVar1 = pKVar10 + 8;
  LOCK();
  *(int *)(pKVar10 + 8) = *(int *)(pKVar10 + 8) + 1;
  UNLOCK();
  *puVar9 = 0;
  LOCK();
  *(int *)(pKVar10 + 8) = *(int *)(pKVar10 + 8) + 1;
  UNLOCK();
  puVar9[1] = pKVar10;
  LOCK();
  *(int *)(pKVar10 + 8) = *(int *)(pKVar10 + 8) + 1;
  UNLOCK();
  *(undefined (*) [16])(puVar9 + 2) = (undefined  [16])0x0;
  LOCK();
  *(int *)pKVar1 = *(int *)pKVar1 + -1;
  UNLOCK();
  if (*(int *)pKVar1 == 0) {
    (**(code **)(*(long *)pKVar10 + 8))(pKVar10);
  }
  puVar9[4] = 0;
                    /* try { // try from 0046fd1d to 0046fd21 has its CatchHandler @ 004702bb */
  KisNodeUuidInfo::KisNodeUuidInfo((KisNodeUuidInfo *)(puVar9 + 5));
  *(undefined4 *)(puVar9 + 8) = 0;
  *(undefined8 **)(this + 0x38) = puVar9;
  LOCK();
  *(int *)pKVar1 = *(int *)pKVar1 + -1;
  UNLOCK();
  if (*(int *)pKVar1 == 0) {
    (**(code **)(*(long *)pKVar10 + 8))(pKVar10);
  }
  local_58 = (undefined  [8])0x0;
  auVar7 = local_58;
  local_58 = (undefined  [8])0x0;
  if (piStack_50 != (int *)0x0) {
    LOCK();
    iVar3 = *piStack_50;
    *piStack_50 = *piStack_50 + -2;
    UNLOCK();
    if ((iVar3 < 3) && (piStack_50 != (int *)0x0)) {
      local_58 = auVar7;
      operator_delete(piStack_50,4);
    }
  }
  this_00 = (KisImage *)*puVar14;
  if ((((uint *)puVar14[1] != (uint *)0x0) && (this_00 != (KisImage *)0x0)) &&
     ((*(uint *)puVar14[1] & 1) != 0)) {
    pKVar2 = this_00 + 0x50;
    LOCK();
    *(int *)(this_00 + 0x50) = *(int *)(this_00 + 0x50) + 1;
    UNLOCK();
                    /* try { // try from 0046fd96 to 0046fd9a has its CatchHandler @ 00470299 */
    this_01 = (KisPaintDevice *)operator_new(0x28);
    local_70 = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 0046fdaf to 0046fdb3 has its CatchHandler @ 004702a5 */
    pKVar10 = (KisDefaultBounds *)operator_new(0x20);
    piVar13 = *(int **)(this_00 + 0x58);
    local_58 = (undefined  [8])this_00;
    auVar7 = (undefined  [8])this_00;
    if (piVar13 == (int *)0x0) {
                    /* try { // try from 0047016d to 00470171 has its CatchHandler @ 00470261 */
      piVar13 = (int *)operator_new(4);
      *piVar13 = 0;
      *(int **)(this_00 + 0x58) = piVar13;
      LOCK();
      *piVar13 = *piVar13 + 1;
      UNLOCK();
      piVar13 = *(int **)(this_00 + 0x58);
      auVar7 = local_58;
    }
    local_58 = auVar7;
    piStack_50 = piVar13;
    LOCK();
    *piVar13 = *piVar13 + 2;
    UNLOCK();
                    /* try { // try from 0046fdd8 to 0046fddc has its CatchHandler @ 0047026d */
    KisDefaultBounds::KisDefaultBounds(pKVar10,KVar15);
    LOCK();
    *(int *)(pKVar10 + 8) = *(int *)(pKVar10 + 8) + 1;
    UNLOCK();
    local_78 = pKVar10;
                    /* try { // try from 0046fdeb to 0046fdef has its CatchHandler @ 00470283 */
    pKVar11 = (KoColorSpace *)KisImage::colorSpace(this_00);
    local_60 = *(int **)(this + 0x18);
    local_68 = this;
    if (local_60 == (int *)0x0) {
                    /* try { // try from 00470145 to 00470149 has its CatchHandler @ 00470283 */
      piVar13 = (int *)operator_new(4);
      *piVar13 = 0;
      *(int **)(this + 0x18) = piVar13;
      LOCK();
      *piVar13 = *piVar13 + 1;
      UNLOCK();
      local_60 = *(int **)(this + 0x18);
    }
    LOCK();
    *local_60 = *local_60 + 2;
    UNLOCK();
                    /* try { // try from 0046fe34 to 0046fe38 has its CatchHandler @ 0047023d */
    KisPaintDevice::KisPaintDevice
              (this_01,(KisWeakSharedPtr)&local_68,pKVar11,(KisSharedPtr)&local_78,
               (QString *)&local_70);
    plVar4 = *(long **)(this + 0x38);
    if (this_01 != (KisPaintDevice *)*plVar4) {
      LOCK();
      *(int *)(this_01 + 0x10) = *(int *)(this_01 + 0x10) + 1;
      UNLOCK();
      plVar5 = (long *)*plVar4;
      *plVar4 = (long)this_01;
      if (plVar5 != (long *)0x0) {
        LOCK();
        plVar4 = plVar5 + 2;
        *(int *)plVar4 = *(int *)plVar4 + -1;
        UNLOCK();
        if (*(int *)plVar4 == 0) {
          (**(code **)(*plVar5 + 0x20))();
        }
      }
    }
    local_68 = (KisCloneLayer *)0x0;
    if (local_60 != (int *)0x0) {
      LOCK();
      iVar3 = *local_60;
      *local_60 = *local_60 + -2;
      UNLOCK();
      if ((iVar3 < 3) && (local_60 != (int *)0x0)) {
        operator_delete(local_60,4);
      }
    }
    if (local_78 != (KisDefaultBounds *)0x0) {
      LOCK();
      pKVar10 = local_78 + 8;
      *(int *)pKVar10 = *(int *)pKVar10 + -1;
      UNLOCK();
      if (*(int *)pKVar10 == 0) {
        (**(code **)(*(long *)local_78 + 8))();
      }
    }
    local_58 = (undefined  [8])0x0;
    if (piStack_50 != (int *)0x0) {
      LOCK();
      iVar3 = *piStack_50;
      *piStack_50 = *piStack_50 + -2;
      UNLOCK();
      if ((iVar3 < 3) && (piStack_50 != (int *)0x0)) {
        operator_delete(piStack_50,4);
      }
    }
    if (*(int *)local_70 == 0) {
LAB_004700d0:
      QArrayData::deallocate(local_70,2,8);
    }
    else if (*(int *)local_70 != -1) {
      LOCK();
      *(int *)local_70 = *(int *)local_70 + -1;
      UNLOCK();
      if (*(int *)local_70 == 0) goto LAB_004700d0;
    }
    lVar12 = *(long *)(this + 0x38);
    this_02 = *(KisLayer **)(lVar12 + 0x20);
    pKVar6 = *(KisLayer **)CONCAT44(in_register_00000034,param_1);
    if (pKVar6 != this_02) {
      if (pKVar6 != (KisLayer *)0x0) {
        LOCK();
        *(int *)(pKVar6 + 0x10) = *(int *)(pKVar6 + 0x10) + 1;
        UNLOCK();
        this_02 = *(KisLayer **)(lVar12 + 0x20);
      }
      *(KisLayer **)(lVar12 + 0x20) = pKVar6;
      if (this_02 != (KisLayer *)0x0) {
        LOCK();
        pKVar6 = this_02 + 0x10;
        *(int *)pKVar6 = *(int *)pKVar6 + -1;
        UNLOCK();
        if (*(int *)pKVar6 == 0) {
          (**(code **)(*(long *)this_02 + 0x20))(this_02);
        }
      }
      lVar12 = *(long *)(this + 0x38);
      this_02 = *(KisLayer **)(lVar12 + 0x20);
    }
    *(undefined4 *)(lVar12 + 0x40) = 0;
    if (this_02 != (KisLayer *)0x0) {
      piVar13 = *(int **)(this + 0x18);
      local_58 = (undefined  [8])this;
      auVar7 = (undefined  [8])this;
      if (piVar13 == (int *)0x0) {
                    /* try { // try from 00470195 to 00470199 has its CatchHandler @ 00470299 */
        piVar13 = (int *)operator_new(4);
        *piVar13 = 0;
        *(int **)(this + 0x18) = piVar13;
        LOCK();
        *piVar13 = *piVar13 + 1;
        UNLOCK();
        piVar13 = *(int **)(this + 0x18);
        auVar7 = local_58;
      }
      local_58 = auVar7;
      LOCK();
      *piVar13 = *piVar13 + 2;
      UNLOCK();
      piStack_50 = piVar13;
                    /* try { // try from 0046ff95 to 0046ff99 has its CatchHandler @ 00470255 */
      KisLayer::registerClone(this_02,KVar15);
      local_58 = (undefined  [8])0x0;
      if (piStack_50 != (int *)0x0) {
        LOCK();
        iVar3 = *piStack_50;
        *piStack_50 = *piStack_50 + -2;
        UNLOCK();
        if ((iVar3 < 3) && (piStack_50 != (int *)0x0)) {
          operator_delete(piStack_50,4);
        }
      }
    }
    LOCK();
    *(int *)pKVar2 = *(int *)pKVar2 + -1;
    UNLOCK();
    if (*(int *)pKVar2 == 0) {
      if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x00470003. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        (**(code **)(*(long *)this_00 + 0x20))(this_00);
        return;
      }
      goto LAB_004701b1;
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_004701b1:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCloneLayer @ 004702e0 ======

/* KisCloneLayer::KisCloneLayer(KisCloneLayer const&) */

void __thiscall KisCloneLayer::KisCloneLayer(KisCloneLayer *this,KisCloneLayer *param_1)

{
  KisCloneLayer *pKVar1;
  int iVar2;
  long *plVar3;
  long lVar4;
  long lVar5;
  KisLayer *this_00;
  undefined4 uVar6;
  undefined8 *puVar7;
  KisDefaultBounds *this_01;
  KisPaintDevice *this_02;
  KisDefaultBounds *pKVar8;
  KoColorSpace *pKVar9;
  int *piVar10;
  long lVar11;
  KisWeakSharedPtr KVar12;
  KisCloneLayer *pKVar13;
  KisCloneLayer *pKVar14;
  long *plVar15;
  long in_FS_OFFSET;
  KisDefaultBounds *local_78;
  QArrayData *local_70;
  KisCloneLayer *local_68;
  int *local_60;
  KisCloneLayer *local_58;
  int *local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisLayer::KisLayer((KisLayer *)this,(KisLayer *)param_1);
  *(undefined **)this = PTR_vtable_008373f0 + 0x10;
                    /* try { // try from 00470320 to 00470324 has its CatchHandler @ 004707d6 */
  puVar7 = (undefined8 *)operator_new(0x48);
                    /* try { // try from 0047032d to 00470331 has its CatchHandler @ 004707ca */
  this_01 = (KisDefaultBounds *)operator_new(0x20);
                    /* try { // try from 00470340 to 00470344 has its CatchHandler @ 004707be */
  KisBaseNode::image();
  KVar12 = (KisWeakSharedPtr)&local_58;
                    /* try { // try from 0047034b to 0047034f has its CatchHandler @ 004707b2 */
  KisDefaultBounds::KisDefaultBounds(this_01,KVar12);
  pKVar8 = this_01 + 8;
  LOCK();
  *(int *)(this_01 + 8) = *(int *)(this_01 + 8) + 1;
  UNLOCK();
  *puVar7 = 0;
  LOCK();
  *(int *)(this_01 + 8) = *(int *)(this_01 + 8) + 1;
  UNLOCK();
  puVar7[1] = this_01;
  LOCK();
  *(int *)(this_01 + 8) = *(int *)(this_01 + 8) + 1;
  UNLOCK();
  *(undefined (*) [16])(puVar7 + 2) = (undefined  [16])0x0;
  LOCK();
  *(int *)pKVar8 = *(int *)pKVar8 + -1;
  UNLOCK();
  if (*(int *)pKVar8 == 0) {
    (**(code **)(*(long *)this_01 + 8))(this_01);
  }
  puVar7[4] = 0;
                    /* try { // try from 00470396 to 0047039a has its CatchHandler @ 0047080b */
  KisNodeUuidInfo::KisNodeUuidInfo((KisNodeUuidInfo *)(puVar7 + 5));
  *(undefined4 *)(puVar7 + 8) = 0;
  *(undefined8 **)(this + 0x38) = puVar7;
  LOCK();
  *(int *)pKVar8 = *(int *)pKVar8 + -1;
  UNLOCK();
  if (*(int *)pKVar8 == 0) {
    (**(code **)(*(long *)this_01 + 8))(this_01);
  }
  local_58 = (KisCloneLayer *)0x0;
  if (local_50 != (int *)0x0) {
    LOCK();
    iVar2 = *local_50;
    *local_50 = *local_50 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (local_50 != (int *)0x0)) {
      operator_delete(local_50,4);
    }
  }
                    /* try { // try from 004703dc to 004703e0 has its CatchHandler @ 004707d6 */
  this_02 = (KisPaintDevice *)operator_new(0x28);
  local_70 = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 004703f5 to 004703f9 has its CatchHandler @ 00470823 */
  pKVar8 = (KisDefaultBounds *)operator_new(0x20);
                    /* try { // try from 00470403 to 00470407 has its CatchHandler @ 004707ff */
  KisBaseNode::image();
                    /* try { // try from 0047040e to 00470412 has its CatchHandler @ 004707f3 */
  KisDefaultBounds::KisDefaultBounds(pKVar8,KVar12);
  LOCK();
  *(int *)(pKVar8 + 8) = *(int *)(pKVar8 + 8) + 1;
  UNLOCK();
  local_78 = pKVar8;
                    /* try { // try from 00470425 to 00470429 has its CatchHandler @ 004707e2 */
  pKVar9 = (KoColorSpace *)
           KisPaintDevice::colorSpace((KisPaintDevice *)**(undefined8 **)(param_1 + 0x38));
  local_60 = *(int **)(this + 0x18);
  local_68 = this;
  if (local_60 == (int *)0x0) {
                    /* try { // try from 00470785 to 00470789 has its CatchHandler @ 004707e2 */
    piVar10 = (int *)operator_new(4);
    *piVar10 = 0;
    *(int **)(this + 0x18) = piVar10;
    LOCK();
    *piVar10 = *piVar10 + 1;
    UNLOCK();
    local_60 = *(int **)(this + 0x18);
  }
  LOCK();
  *local_60 = *local_60 + 2;
  UNLOCK();
                    /* try { // try from 0047046a to 0047046e has its CatchHandler @ 004707a6 */
  KisPaintDevice::KisPaintDevice
            (this_02,(KisWeakSharedPtr)&local_68,pKVar9,(KisSharedPtr)&local_78,(QString *)&local_70
            );
  plVar3 = *(long **)(this + 0x38);
  if (this_02 != (KisPaintDevice *)*plVar3) {
    LOCK();
    *(int *)(this_02 + 0x10) = *(int *)(this_02 + 0x10) + 1;
    UNLOCK();
    plVar15 = (long *)*plVar3;
    *plVar3 = (long)this_02;
    if (plVar15 != (long *)0x0) {
      LOCK();
      plVar3 = plVar15 + 2;
      *(int *)plVar3 = *(int *)plVar3 + -1;
      UNLOCK();
      if (*(int *)plVar3 == 0) {
        (**(code **)(*plVar15 + 0x20))();
      }
    }
  }
  local_68 = (KisCloneLayer *)0x0;
  if (local_60 != (int *)0x0) {
    LOCK();
    iVar2 = *local_60;
    *local_60 = *local_60 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (local_60 != (int *)0x0)) {
      operator_delete(local_60,4);
    }
  }
  if (local_78 != (KisDefaultBounds *)0x0) {
    LOCK();
    pKVar8 = local_78 + 8;
    *(int *)pKVar8 = *(int *)pKVar8 + -1;
    UNLOCK();
    if (*(int *)pKVar8 == 0) {
      (**(code **)(*(long *)local_78 + 8))();
    }
  }
  local_58 = (KisCloneLayer *)0x0;
  if (local_50 == (int *)0x0) {
LAB_004704f4:
    iVar2 = *(int *)local_70;
  }
  else {
    LOCK();
    iVar2 = *local_50;
    *local_50 = *local_50 + -2;
    UNLOCK();
    if ((2 < iVar2) || (local_50 == (int *)0x0)) goto LAB_004704f4;
    operator_delete(local_50,4);
    iVar2 = *(int *)local_70;
  }
  if (iVar2 != 0) {
    if (iVar2 == -1) goto LAB_00470517;
    LOCK();
    *(int *)local_70 = *(int *)local_70 + -1;
    UNLOCK();
    if (*(int *)local_70 != 0) goto LAB_00470517;
  }
  QArrayData::deallocate(local_70,2,8);
LAB_00470517:
                    /* try { // try from 0047051d to 00470576 has its CatchHandler @ 004707d6 */
  copyFrom();
  lVar4 = *(long *)(this + 0x38);
  pKVar13 = *(KisCloneLayer **)(lVar4 + 0x20);
  pKVar14 = pKVar13;
  if (local_58 != pKVar13) {
    if (local_58 != (KisCloneLayer *)0x0) {
      LOCK();
      *(int *)(local_58 + 0x10) = *(int *)(local_58 + 0x10) + 1;
      UNLOCK();
      pKVar13 = *(KisCloneLayer **)(lVar4 + 0x20);
    }
    *(KisCloneLayer **)(lVar4 + 0x20) = local_58;
    pKVar14 = local_58;
    if (pKVar13 != (KisCloneLayer *)0x0) {
      LOCK();
      pKVar1 = pKVar13 + 0x10;
      *(int *)pKVar1 = *(int *)pKVar1 + -1;
      UNLOCK();
      if (*(int *)pKVar1 == 0) {
        (**(code **)(*(long *)pKVar13 + 0x20))();
        pKVar14 = local_58;
      }
    }
  }
  if (pKVar14 != (KisCloneLayer *)0x0) {
    LOCK();
    pKVar13 = pKVar14 + 0x10;
    *(int *)pKVar13 = *(int *)pKVar13 + -1;
    UNLOCK();
    if (*(int *)pKVar13 == 0) {
      (**(code **)(*(long *)pKVar14 + 0x20))();
    }
  }
  lVar4 = *(long *)(this + 0x38);
  uVar6 = copyType(param_1);
  *(undefined4 *)(lVar4 + 0x40) = uVar6;
  lVar4 = *(long *)(param_1 + 0x38);
  lVar5 = *(long *)(this + 0x38);
  lVar11 = lVar5;
  if (lVar4 != lVar5) {
    plVar3 = *(long **)(lVar4 + 8);
    plVar15 = *(long **)(lVar5 + 8);
    if (plVar3 != plVar15) {
      if (plVar3 != (long *)0x0) {
        LOCK();
        *(int *)(plVar3 + 1) = *(int *)(plVar3 + 1) + 1;
        UNLOCK();
        plVar15 = *(long **)(lVar5 + 8);
      }
      *(long **)(lVar5 + 8) = plVar3;
      if (plVar15 != (long *)0x0) {
        LOCK();
        plVar3 = plVar15 + 1;
        *(int *)plVar3 = *(int *)plVar3 + -1;
        UNLOCK();
        if (*(int *)plVar3 == 0) {
          (**(code **)(*plVar15 + 8))();
        }
      }
      lVar11 = *(long *)(this + 0x38);
    }
    *(undefined8 *)(lVar5 + 0x10) = *(undefined8 *)(lVar4 + 0x10);
    *(undefined8 *)(lVar5 + 0x18) = *(undefined8 *)(lVar4 + 0x18);
  }
  this_00 = *(KisLayer **)(lVar11 + 0x20);
  if (this_00 != (KisLayer *)0x0) {
    local_50 = *(int **)(this + 0x18);
    local_58 = this;
    if (local_50 == (int *)0x0) {
                    /* try { // try from 0047072d to 00470731 has its CatchHandler @ 004707d6 */
      piVar10 = (int *)operator_new(4);
      *piVar10 = 0;
      *(int **)(this + 0x18) = piVar10;
      LOCK();
      *piVar10 = *piVar10 + 1;
      UNLOCK();
      local_50 = *(int **)(this + 0x18);
    }
    LOCK();
    *local_50 = *local_50 + 2;
    UNLOCK();
                    /* try { // try from 004705ff to 00470603 has its CatchHandler @ 00470817 */
    KisLayer::registerClone(this_00,KVar12);
    local_58 = (KisCloneLayer *)0x0;
    if (local_50 != (int *)0x0) {
      LOCK();
      iVar2 = *local_50;
      *local_50 = *local_50 + -2;
      UNLOCK();
      if ((iVar2 < 3) && (local_50 != (int *)0x0)) {
        operator_delete(local_50,4);
      }
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



