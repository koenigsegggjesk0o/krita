/* Class KisGeneratorLayer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGeneratorLayer @ 00204040 ======

void __thiscall
KisGeneratorLayer::KisGeneratorLayer(KisGeneratorLayer *this,KisGeneratorLayer *param_1)

{
  (*(code *)PTR_KisGeneratorLayer_00839af0)();
  return;
}



// ====== KisGeneratorLayer @ 003853e0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisGeneratorLayer::KisGeneratorLayer(KisGeneratorLayer const&) */

void __thiscall
KisGeneratorLayer::KisGeneratorLayer(KisGeneratorLayer *this,KisGeneratorLayer *param_1)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined *puVar3;
  KisThreadSafeSignalCompressor *this_00;
  long in_FS_OFFSET;
  QObject aQStack_28 [8];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisSelectionBasedLayer::KisSelectionBasedLayer
            ((KisSelectionBasedLayer *)this,(KisSelectionBasedLayer *)param_1);
  puVar3 = PTR_vtable_00837368;
  *(undefined **)this = PTR_vtable_00837368 + 0x10;
  *(undefined **)(this + 0x38) = puVar3 + 0x290;
  *(undefined **)(this + 0x48) = puVar3 + 0x2d0;
  *(undefined **)(this + 0x60) = puVar3 + 0x308;
                    /* try { // try from 00385437 to 0038543b has its CatchHandler @ 003854d1 */
  this_00 = (KisThreadSafeSignalCompressor *)operator_new(0x58);
                    /* try { // try from 0038544c to 00385450 has its CatchHandler @ 003854c5 */
  KisThreadSafeSignalCompressor::KisThreadSafeSignalCompressor(this_00,100,3);
  *(KisThreadSafeSignalCompressor **)(this + 0x68) = this_00;
  uVar2 = DAT_00721778;
  uVar1 = _DAT_00721770;
  *(undefined8 *)(this_00 + 0x18) = _DAT_00721770;
  *(undefined8 *)(this_00 + 0x20) = uVar2;
  *(undefined8 *)(this_00 + 0x28) = uVar1;
  *(undefined8 *)(this_00 + 0x30) = uVar2;
  *(undefined (*) [16])(this_00 + 0x38) = (undefined  [16])0x0;
  *(undefined (*) [16])(this_00 + 0x48) = (undefined  [16])0x0;
                    /* try { // try from 0038548e to 00385492 has its CatchHandler @ 003854b9 */
  QObject::connect(aQStack_28,(char *)this_00,(QObject *)"2timeout()",(char *)this,0x7268d1);
  QMetaObject::Connection::~Connection((Connection *)aQStack_28);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisGeneratorLayer @ 003854e0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisGeneratorLayer::KisGeneratorLayer(KisWeakSharedPtr<KisImage>, QString const&,
   KisPinnedSharedPtr<KisFilterConfiguration>, KisSharedPtr<KisSelection>) */

void __thiscall
KisGeneratorLayer::KisGeneratorLayer
          (KisGeneratorLayer *this,KisWeakSharedPtr param_1,QString *param_2,
          KisPinnedSharedPtr param_3,KisSharedPtr param_4)

{
  int iVar1;
  undefined auVar2 [8];
  undefined auVar3 [16];
  undefined8 uVar4;
  undefined8 uVar5;
  undefined *puVar6;
  KisThreadSafeSignalCompressor *this_00;
  int *piVar7;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000034;
  long *plVar8;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  long *local_48;
  long *local_40;
  undefined local_38 [8];
  int *piStack_30;
  long local_20;
  
  plVar8 = (long *)CONCAT44(in_register_00000034,param_1);
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_40 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (local_40 != (long *)0x0) {
    LOCK();
    *(int *)(local_40 + 1) = *(int *)(local_40 + 1) + 1;
    UNLOCK();
  }
  local_48 = *(long **)CONCAT44(in_register_00000084,param_4);
  if (local_48 != (long *)0x0) {
    LOCK();
    *(int *)(local_48 + 1) = *(int *)(local_48 + 1) + 1;
    UNLOCK();
  }
  if (*plVar8 == 0) {
    local_38 = (undefined  [8])0x0;
    auVar2 = local_38;
  }
  else {
    if (((uint *)plVar8[1] == (uint *)0x0) || ((*(uint *)plVar8[1] & 1) == 0)) {
      local_38 = (undefined  [8])0x0;
      piStack_30 = (int *)0x0;
      goto LAB_00385549;
    }
    auVar2 = (undefined  [8])*plVar8;
    piStack_30 = (int *)local_38;
    local_38 = auVar2;
    if (auVar2 != (undefined  [8])0x0) {
      piVar7 = *(int **)((long)auVar2 + 0x58);
      if (piVar7 == (int *)0x0) {
                    /* try { // try from 003856f5 to 003856f9 has its CatchHandler @ 00385745 */
        piVar7 = (int *)operator_new(4);
        *piVar7 = 0;
        *(int **)((long)auVar2 + 0x58) = piVar7;
        LOCK();
        *piVar7 = *piVar7 + 1;
        UNLOCK();
        piVar7 = *(int **)((long)auVar2 + 0x58);
        auVar2 = local_38;
      }
      local_38 = auVar2;
      LOCK();
      *piVar7 = *piVar7 + 2;
      UNLOCK();
      piStack_30 = piVar7;
      goto LAB_00385549;
    }
  }
  local_38 = auVar2;
  piStack_30 = (int *)0x0;
LAB_00385549:
                    /* try { // try from 0038555f to 00385563 has its CatchHandler @ 00385739 */
  KisSelectionBasedLayer::KisSelectionBasedLayer
            ((KisSelectionBasedLayer *)this,(KisWeakSharedPtr)(QObject *)local_38,param_2,
             (KisSharedPtr)&local_48,(KisPinnedSharedPtr)&local_40);
  piVar7 = piStack_30;
  auVar3._8_8_ = 0;
  auVar3._0_8_ = piStack_30;
  _local_38 = auVar3 << 0x40;
  if (piVar7 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar7;
    *piVar7 = *piVar7 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar7 != (int *)0x0)) {
      operator_delete(piVar7,4);
    }
  }
  if (local_48 != (long *)0x0) {
    LOCK();
    plVar8 = local_48 + 1;
    *(int *)plVar8 = *(int *)plVar8 + -1;
    UNLOCK();
    if (*(int *)plVar8 == 0) {
      (**(code **)(*local_48 + 8))();
    }
  }
  if (local_40 != (long *)0x0) {
    LOCK();
    plVar8 = local_40 + 1;
    *(int *)plVar8 = *(int *)plVar8 + -1;
    UNLOCK();
    if (*(int *)plVar8 == 0) {
      (**(code **)(*local_40 + 8))();
    }
  }
  puVar6 = PTR_vtable_00837368;
  *(undefined **)this = PTR_vtable_00837368 + 0x10;
  *(undefined **)(this + 0x38) = puVar6 + 0x290;
  *(undefined **)(this + 0x48) = puVar6 + 0x2d0;
  *(undefined **)(this + 0x60) = puVar6 + 0x308;
                    /* try { // try from 003855e5 to 003855e9 has its CatchHandler @ 00385721 */
  this_00 = (KisThreadSafeSignalCompressor *)operator_new(0x58);
                    /* try { // try from 003855fa to 003855fe has its CatchHandler @ 00385715 */
  KisThreadSafeSignalCompressor::KisThreadSafeSignalCompressor(this_00,100,3);
  *(KisThreadSafeSignalCompressor **)(this + 0x68) = this_00;
  uVar5 = DAT_00721778;
  uVar4 = _DAT_00721770;
  *(undefined8 *)(this_00 + 0x18) = _DAT_00721770;
  *(undefined8 *)(this_00 + 0x20) = uVar5;
  *(undefined8 *)(this_00 + 0x28) = uVar4;
  *(undefined8 *)(this_00 + 0x30) = uVar5;
  *(undefined (*) [16])(this_00 + 0x38) = (undefined  [16])0x0;
  *(undefined (*) [16])(this_00 + 0x48) = (undefined  [16])0x0;
                    /* try { // try from 00385639 to 0038563d has its CatchHandler @ 0038572d */
  QObject::connect((QObject *)local_38,(char *)this_00,(QObject *)"2timeout()",(char *)this,0x7268d1
                  );
  QMetaObject::Connection::~Connection((Connection *)local_38);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



