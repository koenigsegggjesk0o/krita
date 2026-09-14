/* Class KisMultiwayCut - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMultiwayCut @ 0042ed80 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisMultiwayCut::KisMultiwayCut(KisSharedPtr<KisPaintDevice>, KisSharedPtr<KisPaintDevice>, QRect
   const&) */

void __thiscall
KisMultiwayCut::KisMultiwayCut
          (KisMultiwayCut *this,KisSharedPtr param_1,KisSharedPtr param_2,QRect *param_3)

{
  long *plVar1;
  long lVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined *puVar5;
  undefined (*pauVar6) [16];
  KisPaintDevice *this_00;
  KoColorSpace *pKVar7;
  undefined4 in_register_00000014;
  long *plVar8;
  undefined4 in_register_00000034;
  long *plVar9;
  long in_FS_OFFSET;
  QArrayData *local_38;
  long local_30;
  
  plVar8 = (long *)CONCAT44(in_register_00000014,param_2);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  pauVar6 = (undefined (*) [16])operator_new(0x30);
  puVar5 = PTR_shared_null_008377d0;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *pauVar6 = (undefined  [16])0x0;
  uVar4 = DAT_00721778;
  uVar3 = _DAT_00721770;
  *(undefined8 *)pauVar6[1] = 0;
  *(undefined **)(pauVar6[2] + 8) = puVar5;
  *(undefined (**) [16])this = pauVar6;
  *(undefined8 *)(pauVar6[1] + 8) = uVar3;
  *(undefined8 *)pauVar6[2] = uVar4;
  if (lVar2 == 0) {
    plVar8 = (long *)*plVar8;
    if (plVar8 != (long *)0x0) goto LAB_0042eefc;
  }
  else {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
    plVar9 = *(long **)*pauVar6;
    *(long *)*pauVar6 = lVar2;
    if (plVar9 == (long *)0x0) {
LAB_0042ee07:
      pauVar6 = *(undefined (**) [16])this;
      plVar8 = (long *)*plVar8;
      plVar9 = *(long **)(*pauVar6 + 8);
      if (plVar8 == plVar9) goto LAB_0042ee34;
    }
    else {
      LOCK();
      plVar1 = plVar9 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 != 0) goto LAB_0042ee07;
      (**(code **)(*plVar9 + 0x20))();
      pauVar6 = *(undefined (**) [16])this;
      plVar8 = (long *)*plVar8;
      plVar9 = *(long **)(*pauVar6 + 8);
      if (plVar8 == plVar9) goto LAB_0042ee34;
    }
    if (plVar8 != (long *)0x0) {
LAB_0042eefc:
      LOCK();
      *(int *)(plVar8 + 2) = *(int *)(plVar8 + 2) + 1;
      UNLOCK();
      plVar9 = *(long **)(*pauVar6 + 8);
    }
    *(long **)(*pauVar6 + 8) = plVar8;
    if (plVar9 != (long *)0x0) {
      LOCK();
      plVar8 = plVar9 + 2;
      *(int *)plVar8 = *(int *)plVar8 + -1;
      UNLOCK();
      if (*(int *)plVar8 == 0) {
        (**(code **)(*plVar9 + 0x20))();
      }
    }
  }
LAB_0042ee34:
                    /* try { // try from 0042ee39 to 0042ee3d has its CatchHandler @ 0042ef5c */
  this_00 = (KisPaintDevice *)operator_new(0x28);
  local_38 = (QArrayData *)puVar5;
                    /* try { // try from 0042ee48 to 0042ee62 has its CatchHandler @ 0042ef50 */
  KoColorSpaceRegistry::instance();
  pKVar7 = (KoColorSpace *)KoColorSpaceRegistry::alpha8();
  KisPaintDevice::KisPaintDevice(this_00,pKVar7,(QString *)&local_38);
  lVar2 = *(long *)this;
  if (this_00 != *(KisPaintDevice **)(lVar2 + 0x10)) {
    LOCK();
    *(int *)(this_00 + 0x10) = *(int *)(this_00 + 0x10) + 1;
    UNLOCK();
    plVar8 = *(long **)(lVar2 + 0x10);
    *(KisPaintDevice **)(lVar2 + 0x10) = this_00;
    if (plVar8 != (long *)0x0) {
      LOCK();
      plVar9 = plVar8 + 2;
      *(int *)plVar9 = *(int *)plVar9 + -1;
      UNLOCK();
      if (*(int *)plVar9 == 0) {
        (**(code **)(*plVar8 + 0x20))();
      }
    }
  }
  if (*(int *)local_38 != 0) {
    if (*(int *)local_38 == -1) goto LAB_0042eea3;
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 != 0) goto LAB_0042eea3;
  }
  QArrayData::deallocate(local_38,2,8);
LAB_0042eea3:
  lVar2 = *(long *)this;
  uVar3 = *(undefined8 *)(param_3 + 8);
  *(undefined8 *)(lVar2 + 0x18) = *(undefined8 *)param_3;
  *(undefined8 *)(lVar2 + 0x20) = uVar3;
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



