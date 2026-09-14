/* Class KisFilterMask - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisFilterMask @ 0020a100 ======

void __thiscall KisFilterMask::KisFilterMask(KisFilterMask *this,KisFilterMask *param_1)

{
  (*(code *)PTR_KisFilterMask_0083cb50)();
  return;
}



// ====== KisFilterMask @ 004be3a0 ======

/* KisFilterMask::KisFilterMask(KisWeakSharedPtr<KisImage>, QString const&) */

void __thiscall
KisFilterMask::KisFilterMask(KisFilterMask *this,KisWeakSharedPtr param_1,QString *param_2)

{
  int iVar1;
  undefined auVar2 [8];
  undefined auVar3 [16];
  undefined *puVar4;
  undefined (*pauVar5) [16];
  int *piVar6;
  undefined4 in_register_00000034;
  long *plVar7;
  long in_FS_OFFSET;
  undefined local_48 [8];
  int *piStack_40;
  long local_30;
  
  plVar7 = (long *)CONCAT44(in_register_00000034,param_1);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar7 == 0) {
    local_48 = (undefined  [8])0x0;
    auVar2 = local_48;
  }
  else {
    if (((uint *)plVar7[1] == (uint *)0x0) || ((*(uint *)plVar7[1] & 1) == 0)) {
      local_48 = (undefined  [8])0x0;
      piStack_40 = (int *)0x0;
      goto LAB_004be3e5;
    }
    auVar2 = (undefined  [8])*plVar7;
    piStack_40 = (int *)local_48;
    local_48 = auVar2;
    if (auVar2 != (undefined  [8])0x0) {
      piVar6 = *(int **)((long)auVar2 + 0x58);
      if (piVar6 == (int *)0x0) {
        piVar6 = (int *)operator_new(4);
        *piVar6 = 0;
        *(int **)((long)auVar2 + 0x58) = piVar6;
        LOCK();
        *piVar6 = *piVar6 + 1;
        UNLOCK();
        piVar6 = *(int **)((long)auVar2 + 0x58);
        auVar2 = local_48;
      }
      local_48 = auVar2;
      LOCK();
      *piVar6 = *piVar6 + 2;
      UNLOCK();
      piStack_40 = piVar6;
      goto LAB_004be3e5;
    }
  }
  local_48 = auVar2;
  piStack_40 = (int *)0x0;
LAB_004be3e5:
                    /* try { // try from 004be3f0 to 004be3f4 has its CatchHandler @ 004be581 */
  KisEffectMask::KisEffectMask((KisEffectMask *)this,(KisWeakSharedPtr)local_48,param_2);
  local_48 = (undefined  [8])0x0;
  if (piStack_40 != (int *)0x0) {
    LOCK();
    iVar1 = *piStack_40;
    *piStack_40 = *piStack_40 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piStack_40 != (int *)0x0)) {
      operator_delete(piStack_40,4);
    }
  }
  auVar3._8_8_ = 0;
  auVar3._0_8_ = piStack_40;
  _local_48 = auVar3 << 0x40;
                    /* try { // try from 004be42d to 004be431 has its CatchHandler @ 004be55d */
  KisNodeFilterInterface::KisNodeFilterInterface
            ((KisNodeFilterInterface *)(this + 0x48),(KisWeakSharedPtr)local_48);
  if (local_48 != (undefined  [8])0x0) {
    LOCK();
    plVar7 = (long *)((long)local_48 + 8);
    *(int *)plVar7 = *(int *)plVar7 + -1;
    UNLOCK();
    if (*(int *)plVar7 == 0) {
      (**(code **)(*(long *)local_48 + 8))();
    }
  }
  puVar4 = PTR_vtable_00837060;
  *(undefined **)this = PTR_vtable_00837060 + 0x10;
  *(undefined **)(this + 0x30) = puVar4 + 0x240;
  *(undefined **)(this + 0x48) = puVar4 + 0x280;
                    /* try { // try from 004be46f to 004be473 has its CatchHandler @ 004be569 */
  pauVar5 = (undefined (*) [16])operator_new(0x20);
  *(undefined (**) [16])(this + 0x58) = pauVar5;
  *pauVar5 = (undefined  [16])0x0;
  pauVar5[1] = (undefined  [16])0x0;
                    /* try { // try from 004be48d to 004be491 has its CatchHandler @ 004be575 */
  KisBaseNode::setCompositeOpId((KisBaseNode *)this,(QString *)&DAT_0084b318);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisFilterMask @ 004be590 ======

/* KisFilterMask::KisFilterMask(KisFilterMask const&) */

void __thiscall KisFilterMask::KisFilterMask(KisFilterMask *this,KisFilterMask *param_1)

{
  undefined *puVar1;
  undefined (*pauVar2) [16];
  
  KisEffectMask::KisEffectMask((KisEffectMask *)this,(KisEffectMask *)param_1);
                    /* try { // try from 004be5ae to 004be5b2 has its CatchHandler @ 004be5f4 */
  KisNodeFilterInterface::KisNodeFilterInterface
            ((KisNodeFilterInterface *)(this + 0x48),(KisNodeFilterInterface *)(param_1 + 0x48));
  puVar1 = PTR_vtable_00837060;
  *(undefined **)this = PTR_vtable_00837060 + 0x10;
  *(undefined **)(this + 0x30) = puVar1 + 0x240;
  *(undefined **)(this + 0x48) = puVar1 + 0x280;
                    /* try { // try from 004be5db to 004be5df has its CatchHandler @ 004be600 */
  pauVar2 = (undefined (*) [16])operator_new(0x20);
  *(undefined (**) [16])(this + 0x58) = pauVar2;
  *pauVar2 = (undefined  [16])0x0;
  pauVar2[1] = (undefined  [16])0x0;
  return;
}



