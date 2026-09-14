/* Class KisTransactionData - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTransactionData @ 002040e0 ======

void __thiscall
KisTransactionData::KisTransactionData
          (KisTransactionData *this,KUndo2MagicString *param_1,KisSharedPtr param_2,bool param_3,
          KisTransactionWrapperFactory *param_4,KUndo2Command *param_5,bool param_6)

{
  (*(code *)PTR_KisTransactionData_00839b40)();
  return;
}



// ====== KisTransactionData @ 00600650 ======

/* KisTransactionData::KisTransactionData(KUndo2MagicString const&, KisSharedPtr<KisPaintDevice>,
   bool, KisTransactionWrapperFactory*, KUndo2Command*, bool) */

void __thiscall
KisTransactionData::KisTransactionData
          (KisTransactionData *this,KUndo2MagicString *param_1,KisSharedPtr param_2,bool param_3,
          KisTransactionWrapperFactory *param_4,KUndo2Command *param_5,bool param_6)

{
  long *plVar1;
  int *piVar2;
  undefined (*pauVar3) [16];
  KisInterstrokeDataTransactionWrapperFactory *pKVar4;
  bool bVar5;
  undefined8 *puVar6;
  undefined (*pauVar7) [16];
  long lVar8;
  undefined4 in_register_00000014;
  undefined8 *puVar9;
  long in_FS_OFFSET;
  byte bVar10;
  long *local_58;
  int *local_50;
  long local_40;
  
  bVar10 = 0;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KUndo2Command::KUndo2Command((KUndo2Command *)this,param_1,param_5);
  *(undefined **)this = PTR_vtable_00837730 + 0x10;
                    /* try { // try from 0060069f to 006006a3 has its CatchHandler @ 006008e0 */
  puVar6 = (undefined8 *)operator_new(0xa8);
  puVar9 = puVar6;
  for (lVar8 = 0x15; lVar8 != 0; lVar8 = lVar8 + -1) {
    *puVar9 = 0;
    puVar9 = puVar9 + (ulong)bVar10 * -2 + 1;
  }
                    /* try { // try from 006006b8 to 006006bc has its CatchHandler @ 006008ec */
  KoColor::KoColor((KoColor *)(puVar6 + 5));
  *(undefined *)(puVar6 + 0xd) = 0;
  QPainterPath::QPainterPath((QPainterPath *)(puVar6 + 0xe));
  *(undefined8 **)(this + 0x28) = puVar6;
  puVar6[0xf] = 0;
  *(bool *)(puVar6 + 0x10) = param_3;
  *(bool *)(puVar6 + 0x14) = param_6;
  *(undefined (*) [16])(puVar6 + 0x12) = (undefined  [16])0x0;
                    /* try { // try from 006006f7 to 00600712 has its CatchHandler @ 006008e0 */
  KUndo2Command::setTimedID((int)this);
  if (param_4 == (KisTransactionWrapperFactory *)0x0) {
    KisPaintDevice::interstrokeData();
    if (local_58 == (long *)0x0) {
      bVar5 = false;
      if (local_50 == (int *)0x0) goto LAB_0060079a;
LAB_00600866:
      LOCK();
      piVar2 = local_50 + 1;
      *piVar2 = *piVar2 + -1;
      UNLOCK();
      if (*piVar2 == 0) {
        (**(code **)(local_50 + 2))(local_50);
      }
      LOCK();
      *local_50 = *local_50 + -1;
      UNLOCK();
      if (*local_50 == 0) {
        operator_delete(local_50,0x10);
      }
      if (!bVar5) goto LAB_0060079a;
    }
    else {
      bVar5 = true;
      if (local_50 != (int *)0x0) goto LAB_00600866;
    }
                    /* try { // try from 0060088d to 00600891 has its CatchHandler @ 006008e0 */
    param_4 = (KisTransactionWrapperFactory *)operator_new(0x10);
                    /* try { // try from 0060089f to 006008a3 has its CatchHandler @ 006008d4 */
    KisInterstrokeDataTransactionWrapperFactory::KisInterstrokeDataTransactionWrapperFactory
              ((KisInterstrokeDataTransactionWrapperFactory *)param_4,
               (KisInterstrokeDataFactory *)0x0,true);
  }
  lVar8 = *(long *)(this + 0x28);
  pauVar7 = (undefined (*) [16])operator_new(0x18);
  *(undefined8 *)pauVar7[1] = 0;
  *pauVar7 = (undefined  [16])0x0;
  pauVar3 = *(undefined (**) [16])(lVar8 + 0x98);
  if ((pauVar7 != pauVar3) &&
     (*(undefined (**) [16])(lVar8 + 0x98) = pauVar7, pauVar3 != (undefined (*) [16])0x0)) {
    if (*(long **)pauVar3[1] != (long *)0x0) {
      (**(code **)(**(long **)pauVar3[1] + 8))();
    }
    if (*(long **)(*pauVar3 + 8) != (long *)0x0) {
      (**(code **)(**(long **)(*pauVar3 + 8) + 8))();
    }
    if (*(long **)*pauVar3 != (long *)0x0) {
      (**(code **)(**(long **)*pauVar3 + 8))();
    }
    operator_delete(pauVar3,0x18);
  }
  pKVar4 = (KisInterstrokeDataTransactionWrapperFactory *)**(long **)(*(long *)(this + 0x28) + 0x98)
  ;
  if ((param_4 != (KisTransactionWrapperFactory *)pKVar4) &&
     (**(long **)(*(long *)(this + 0x28) + 0x98) = (long)param_4,
     pKVar4 != (KisInterstrokeDataTransactionWrapperFactory *)0x0)) {
    (**(code **)(*(long *)pKVar4 + 8))();
  }
LAB_0060079a:
  local_58 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_58 != (long *)0x0) {
    LOCK();
    *(int *)(local_58 + 2) = *(int *)(local_58 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 006007b2 to 006007b6 has its CatchHandler @ 00600904 */
  possiblyFlattenSelection(this,(KisSharedPtr)&local_58);
  if (local_58 != (long *)0x0) {
    LOCK();
    plVar1 = local_58 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_58 + 0x20))();
    }
  }
  local_58 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_58 != (long *)0x0) {
    LOCK();
    *(int *)(local_58 + 2) = *(int *)(local_58 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 006007df to 006007e3 has its CatchHandler @ 006008f8 */
  init(this,(KisSharedPtr)&local_58);
  if (local_58 != (long *)0x0) {
    LOCK();
    plVar1 = local_58 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_58 + 0x20))();
    }
  }
                    /* try { // try from 006007f7 to 0060084e has its CatchHandler @ 006008e0 */
  saveSelectionOutlineCache(this);
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



