/* Class KisAdjustmentLayer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisAdjustmentLayer @ 00206490 ======

void __thiscall
KisAdjustmentLayer::KisAdjustmentLayer(KisAdjustmentLayer *this,KisAdjustmentLayer *param_1)

{
  (*(code *)PTR_KisAdjustmentLayer_0083ad18)();
  return;
}



// ====== KisAdjustmentLayer @ 00458150 ======

/* KisAdjustmentLayer::KisAdjustmentLayer(KisWeakSharedPtr<KisImage>, QString const&,
   KisPinnedSharedPtr<KisFilterConfiguration>, KisSharedPtr<KisSelection>) */

void __thiscall
KisAdjustmentLayer::KisAdjustmentLayer
          (KisAdjustmentLayer *this,KisWeakSharedPtr param_1,QString *param_2,
          KisPinnedSharedPtr param_3,KisSharedPtr param_4)

{
  int iVar1;
  undefined auVar2 [16];
  undefined auVar3 [16];
  undefined *puVar4;
  long lVar5;
  int *piVar6;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000034;
  long *plVar7;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  long *local_78;
  long *local_70;
  QArrayData *local_68;
  QTextStream *local_60;
  long local_58;
  undefined local_50 [16];
  undefined8 local_40;
  long local_30;
  
  plVar7 = (long *)CONCAT44(in_register_00000034,param_1);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  local_70 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (local_70 != (long *)0x0) {
    LOCK();
    *(int *)(local_70 + 1) = *(int *)(local_70 + 1) + 1;
    UNLOCK();
  }
  local_78 = *(long **)CONCAT44(in_register_00000084,param_4);
  if (local_78 != (long *)0x0) {
    LOCK();
    *(int *)(local_78 + 1) = *(int *)(local_78 + 1) + 1;
    UNLOCK();
  }
  if (*plVar7 == 0) {
    local_58 = 0;
    auVar2._8_8_ = 0;
    auVar2._0_8_ = local_50._8_8_;
    local_50 = auVar2 << 0x40;
    goto LAB_004581e4;
  }
                    /* try { // try from 004582b0 to 004582f2 has its CatchHandler @ 0045845c */
  if ((((uint *)plVar7[1] == (uint *)0x0) || ((*(uint *)plVar7[1] & 1) == 0)) &&
     (lVar5 = _41000(), *(char *)(lVar5 + 0x11) != '\0')) {
    lVar5 = _41000();
    local_40 = *(undefined8 *)(lVar5 + 8);
    local_50 = (undefined  [16])0x0;
    local_58 = 2;
    QMessageLogger::warning();
    if (1 < *(int *)(local_60 + 0x28)) {
      *(uint *)(local_60 + 0x48) = *(uint *)(local_60 + 0x48) | 1;
    }
                    /* try { // try from 0045830a to 0045830e has its CatchHandler @ 00458450 */
    kisBacktrace();
                    /* try { // try from 0045831e to 00458322 has its CatchHandler @ 00458438 */
    QDebug::putString((QChar *)&local_60,(ulong)(local_68 + *(long *)(local_68 + 0x10)));
    if (local_60[0x20] != (QTextStream)0x0) {
                    /* try { // try from 0045841d to 00458421 has its CatchHandler @ 00458438 */
      QTextStream::operator<<(local_60,' ');
    }
    if (*(int *)local_68 == 0) {
LAB_00458350:
      QArrayData::deallocate(local_68,2,8);
    }
    else if (*(int *)local_68 != -1) {
      LOCK();
      *(int *)local_68 = *(int *)local_68 + -1;
      UNLOCK();
      if (*(int *)local_68 == 0) goto LAB_00458350;
    }
    QDebug::~QDebug((QDebug *)&local_60);
    lVar5 = *plVar7;
  }
  else {
    lVar5 = *plVar7;
  }
  local_58 = lVar5;
  if (lVar5 == 0) {
    auVar3._8_8_ = 0;
    auVar3._0_8_ = local_50._8_8_;
    local_50 = auVar3 << 0x40;
  }
  else {
    piVar6 = *(int **)(lVar5 + 0x58);
    if (piVar6 == (int *)0x0) {
                    /* try { // try from 004583f5 to 004583f9 has its CatchHandler @ 0045845c */
      piVar6 = (int *)operator_new(4);
      *piVar6 = 0;
      *(int **)(lVar5 + 0x58) = piVar6;
      LOCK();
      *piVar6 = *piVar6 + 1;
      UNLOCK();
      piVar6 = *(int **)(lVar5 + 0x58);
    }
    local_50._0_8_ = piVar6;
    LOCK();
    *piVar6 = *piVar6 + 2;
    UNLOCK();
  }
LAB_004581e4:
                    /* try { // try from 004581f7 to 004581fb has its CatchHandler @ 0045842c */
  KisSelectionBasedLayer::KisSelectionBasedLayer
            ((KisSelectionBasedLayer *)this,(KisWeakSharedPtr)&local_58,param_2,
             (KisSharedPtr)&local_78,(KisPinnedSharedPtr)&local_70);
  local_58 = 0;
  if ((int *)local_50._0_8_ != (int *)0x0) {
    LOCK();
    iVar1 = *(int *)local_50._0_8_;
    *(int *)local_50._0_8_ = *(int *)local_50._0_8_ + -2;
    UNLOCK();
    if ((iVar1 < 3) && ((int *)local_50._0_8_ != (int *)0x0)) {
      operator_delete((void *)local_50._0_8_,4);
    }
  }
  if (local_78 != (long *)0x0) {
    LOCK();
    plVar7 = local_78 + 1;
    *(int *)plVar7 = *(int *)plVar7 + -1;
    UNLOCK();
    if (*(int *)plVar7 == 0) {
      (**(code **)(*local_78 + 8))();
    }
  }
  if (local_70 != (long *)0x0) {
    LOCK();
    plVar7 = local_70 + 1;
    *(int *)plVar7 = *(int *)plVar7 + -1;
    UNLOCK();
    if (*(int *)plVar7 == 0) {
      (**(code **)(*local_70 + 8))();
    }
  }
  puVar4 = PTR_vtable_00837960;
  *(undefined **)this = PTR_vtable_00837960 + 0x10;
  *(undefined **)(this + 0x38) = puVar4 + 0x280;
  *(undefined **)(this + 0x48) = puVar4 + 0x2c0;
                    /* try { // try from 00458277 to 00458285 has its CatchHandler @ 00458444 */
  KisBaseNode::setCompositeOpId((KisBaseNode *)this,(QString *)&DAT_00849178);
  KisSelectionBasedLayer::setUseSelectionInProjection((KisSelectionBasedLayer *)this,false);
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisAdjustmentLayer @ 00458470 ======

/* KisAdjustmentLayer::KisAdjustmentLayer(KisAdjustmentLayer const&) */

void __thiscall
KisAdjustmentLayer::KisAdjustmentLayer(KisAdjustmentLayer *this,KisAdjustmentLayer *param_1)

{
  undefined *puVar1;
  
  KisSelectionBasedLayer::KisSelectionBasedLayer
            ((KisSelectionBasedLayer *)this,(KisSelectionBasedLayer *)param_1);
  puVar1 = PTR_vtable_00837960;
  *(undefined **)this = PTR_vtable_00837960 + 0x10;
  *(undefined **)(this + 0x38) = puVar1 + 0x280;
  *(undefined **)(this + 0x48) = puVar1 + 0x2c0;
  return;
}



