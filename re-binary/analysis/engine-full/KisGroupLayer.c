/* Class KisGroupLayer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGroupLayer @ 00203650 ======

void __thiscall
KisGroupLayer::KisGroupLayer
          (KisGroupLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3,
          KoColorSpace *param_4)

{
  (*(code *)PTR_KisGroupLayer_008395f8)();
  return;
}



// ====== KisGroupLayer @ 0020afc0 ======

void __thiscall KisGroupLayer::KisGroupLayer(KisGroupLayer *this,KisGroupLayer *param_1)

{
  (*(code *)PTR_KisGroupLayer_0083d2b0)();
  return;
}



// ====== KisGroupLayer @ 00505680 ======

/* KisGroupLayer::KisGroupLayer(KisWeakSharedPtr<KisImage>, QString const&, unsigned char,
   KoColorSpace const*) */

void __thiscall
KisGroupLayer::KisGroupLayer
          (KisGroupLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3,
          KoColorSpace *param_4)

{
  int iVar1;
  undefined auVar2 [8];
  undefined auVar3 [16];
  undefined8 *puVar4;
  int *piVar5;
  undefined4 in_register_00000034;
  long *plVar6;
  long in_FS_OFFSET;
  undefined local_38 [8];
  int *piStack_30;
  long local_20;
  
  plVar6 = (long *)CONCAT44(in_register_00000034,param_1);
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar6 == 0) {
    local_38 = (undefined  [8])0x0;
    auVar2 = local_38;
  }
  else {
    if (((uint *)plVar6[1] == (uint *)0x0) || ((*(uint *)plVar6[1] & 1) == 0)) {
      local_38 = (undefined  [8])0x0;
      piStack_30 = (int *)0x0;
      goto LAB_005056cb;
    }
    auVar2 = (undefined  [8])*plVar6;
    piStack_30 = (int *)local_38;
    local_38 = auVar2;
    if (auVar2 != (undefined  [8])0x0) {
      piVar5 = *(int **)((long)auVar2 + 0x58);
      if (piVar5 == (int *)0x0) {
        piVar5 = (int *)operator_new(4);
        *piVar5 = 0;
        *(int **)((long)auVar2 + 0x58) = piVar5;
        LOCK();
        *piVar5 = *piVar5 + 1;
        UNLOCK();
        piVar5 = *(int **)((long)auVar2 + 0x58);
        auVar2 = local_38;
      }
      local_38 = auVar2;
      LOCK();
      *piVar5 = *piVar5 + 2;
      UNLOCK();
      piStack_30 = piVar5;
      goto LAB_005056cb;
    }
  }
  local_38 = auVar2;
  piStack_30 = (int *)0x0;
LAB_005056cb:
                    /* try { // try from 005056d6 to 005056da has its CatchHandler @ 005057e7 */
  KisLayer::KisLayer((KisLayer *)this,(KisWeakSharedPtr)local_38,param_2,param_3);
  piVar5 = piStack_30;
  auVar3._8_8_ = 0;
  auVar3._0_8_ = piStack_30;
  _local_38 = auVar3 << 0x40;
  if (piVar5 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar5;
    *piVar5 = *piVar5 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar5 != (int *)0x0)) {
      operator_delete(piVar5,4);
    }
  }
  *(undefined **)this = PTR_vtable_008371d0 + 0x10;
                    /* try { // try from 0050570f to 00505735 has its CatchHandler @ 005057f3 */
  puVar4 = (undefined8 *)operator_new(0x18);
  *puVar4 = 0;
  puVar4[1] = 0;
  *(undefined *)(puVar4 + 2) = 0;
  *(undefined8 **)(this + 0x38) = puVar4;
  resetCache(this,param_4);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisGroupLayer @ 005059e0 ======

/* KisGroupLayer::KisGroupLayer(KisGroupLayer const&) */

void __thiscall KisGroupLayer::KisGroupLayer(KisGroupLayer *this,KisGroupLayer *param_1)

{
  long *plVar1;
  long lVar2;
  long lVar3;
  undefined uVar4;
  undefined8 *puVar5;
  KisPaintDevice *this_00;
  long *plVar6;
  QArrayData *pQVar7;
  KisPaintDevice *this_01;
  long in_FS_OFFSET;
  KoColor aKStack_78 [56];
  QMapNodeBase *local_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisLayer::KisLayer((KisLayer *)this,(KisLayer *)param_1);
  *(undefined **)this = PTR_vtable_008371d0 + 0x10;
                    /* try { // try from 00505a1e to 00505a43 has its CatchHandler @ 00505c49 */
  puVar5 = (undefined8 *)operator_new(0x18);
  *puVar5 = 0;
  puVar5[1] = 0;
  *(undefined *)(puVar5 + 2) = 0;
  *(undefined8 **)(this + 0x38) = puVar5;
  this_00 = (KisPaintDevice *)operator_new(0x28);
                    /* try { // try from 00505a56 to 00505a5a has its CatchHandler @ 00505c55 */
  KisPaintDevice::KisPaintDevice
            (this_00,(KisPaintDevice *)**(undefined8 **)(param_1 + 0x38),0,(KisNode *)0x0);
  plVar6 = *(long **)(this + 0x38);
  this_01 = (KisPaintDevice *)*plVar6;
  if (this_00 != this_01) {
    LOCK();
    *(int *)(this_00 + 0x10) = *(int *)(this_00 + 0x10) + 1;
    UNLOCK();
    plVar1 = (long *)*plVar6;
    *plVar6 = (long)this_00;
    if (plVar1 != (long *)0x0) {
      LOCK();
      plVar6 = plVar1 + 2;
      *(int *)plVar6 = *(int *)plVar6 + -1;
      UNLOCK();
      if (*(int *)plVar6 == 0) {
        (**(code **)(*plVar1 + 0x20))();
      }
    }
    plVar6 = *(long **)(this + 0x38);
    this_01 = (KisPaintDevice *)*plVar6;
  }
  plVar6[1] = *(long *)(*(long *)(param_1 + 0x38) + 8);
                    /* try { // try from 00505a9f to 00505aa3 has its CatchHandler @ 00505c49 */
  KisPaintDevice::defaultPixel();
                    /* try { // try from 00505aaa to 00505aae has its CatchHandler @ 00505c61 */
  KisPaintDevice::setDefaultPixel(this_01,aKStack_78);
  if (*(int *)local_40 != 0) {
    if (*(int *)local_40 == -1) goto LAB_00505acc;
    LOCK();
    *(int *)local_40 = *(int *)local_40 + -1;
    UNLOCK();
    if (*(int *)local_40 != 0) goto LAB_00505acc;
  }
  lVar2 = *(long *)(local_40 + 0x10);
  if (lVar2 != 0) {
    pQVar7 = *(QArrayData **)(lVar2 + 0x18);
    if (*(int *)pQVar7 == 0) {
LAB_00505c30:
      QArrayData::deallocate(pQVar7,2,8);
    }
    else if (*(int *)pQVar7 != -1) {
      LOCK();
      *(int *)pQVar7 = *(int *)pQVar7 + -1;
      UNLOCK();
      if (*(int *)pQVar7 == 0) {
        pQVar7 = *(QArrayData **)(lVar2 + 0x18);
        goto LAB_00505c30;
      }
    }
    QVariant::~QVariant((QVariant *)(lVar2 + 0x20));
    lVar3 = *(long *)(lVar2 + 8);
    if (lVar3 != 0) {
      pQVar7 = *(QArrayData **)(lVar3 + 0x18);
      if (*(int *)pQVar7 == 0) {
LAB_00505b68:
        QArrayData::deallocate(pQVar7,2,8);
      }
      else if (*(int *)pQVar7 != -1) {
        LOCK();
        *(int *)pQVar7 = *(int *)pQVar7 + -1;
        UNLOCK();
        if (*(int *)pQVar7 == 0) {
          pQVar7 = *(QArrayData **)(lVar3 + 0x18);
          goto LAB_00505b68;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar3 + 0x20));
      if (*(long *)(lVar3 + 8) != 0) {
        FUN_00323e20();
      }
      if (*(long *)(lVar3 + 0x10) != 0) {
        FUN_00323e20();
      }
    }
    lVar2 = *(long *)(lVar2 + 0x10);
    if (lVar2 != 0) {
      pQVar7 = *(QArrayData **)(lVar2 + 0x18);
      if (*(int *)pQVar7 == 0) {
LAB_00505bc0:
        QArrayData::deallocate(pQVar7,2,8);
      }
      else if (*(int *)pQVar7 != -1) {
        LOCK();
        *(int *)pQVar7 = *(int *)pQVar7 + -1;
        UNLOCK();
        if (*(int *)pQVar7 == 0) {
          pQVar7 = *(QArrayData **)(lVar2 + 0x18);
          goto LAB_00505bc0;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar2 + 0x20));
      if (*(long *)(lVar2 + 8) != 0) {
        FUN_00323e20();
      }
      if (*(long *)(lVar2 + 0x10) != 0) {
        FUN_00323e20();
      }
    }
    QMapDataBase::freeTree(local_40,(int)*(undefined8 *)(local_40 + 0x10));
  }
  QMapDataBase::freeData((QMapDataBase *)local_40);
LAB_00505acc:
                    /* try { // try from 00505ad8 to 00505ae8 has its CatchHandler @ 00505c49 */
  KisPaintDevice::setProjectionDevice((KisPaintDevice *)**(undefined8 **)(this + 0x38),true);
  lVar2 = *(long *)(this + 0x38);
  uVar4 = passThroughMode(param_1);
  *(undefined *)(lVar2 + 0x10) = uVar4;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



