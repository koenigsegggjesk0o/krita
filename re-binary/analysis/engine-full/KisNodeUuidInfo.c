/* Class KisNodeUuidInfo - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeUuidInfo @ 00204ba0 ======

void __thiscall KisNodeUuidInfo::KisNodeUuidInfo(KisNodeUuidInfo *this)

{
  (*(code *)PTR_KisNodeUuidInfo_0083a0a0)();
  return;
}



// ====== KisNodeUuidInfo @ 0020bad0 ======

void __thiscall KisNodeUuidInfo::KisNodeUuidInfo(KisNodeUuidInfo *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisNodeUuidInfo_0083d838)();
  return;
}



// ====== KisNodeUuidInfo @ 0046e630 ======

/* KisNodeUuidInfo::KisNodeUuidInfo() */

void __thiscall KisNodeUuidInfo::KisNodeUuidInfo(KisNodeUuidInfo *this)

{
  undefined *puVar1;
  
  puVar1 = PTR_shared_null_008377d0;
  *(undefined8 *)this = 0;
  *(undefined8 *)(this + 8) = 0;
  *(undefined **)(this + 0x10) = puVar1;
  return;
}



// ====== KisNodeUuidInfo @ 0046e650 ======

/* KisNodeUuidInfo::KisNodeUuidInfo(QUuid const&) */

void __thiscall KisNodeUuidInfo::KisNodeUuidInfo(KisNodeUuidInfo *this,QUuid *param_1)

{
  undefined8 uVar1;
  undefined *puVar2;
  
  puVar2 = PTR_shared_null_008377d0;
  *(undefined8 *)this = 0;
  *(undefined8 *)(this + 8) = 0;
  *(undefined **)(this + 0x10) = puVar2;
  uVar1 = *(undefined8 *)(param_1 + 8);
  *(undefined8 *)this = *(undefined8 *)param_1;
  *(undefined8 *)(this + 8) = uVar1;
  return;
}



// ====== KisNodeUuidInfo @ 0046e680 ======

/* KisNodeUuidInfo::KisNodeUuidInfo(QString const&) */

void __thiscall KisNodeUuidInfo::KisNodeUuidInfo(KisNodeUuidInfo *this,QString *param_1)

{
  undefined *puVar1;
  
  puVar1 = PTR_shared_null_008377d0;
  *(undefined8 *)this = 0;
  *(undefined8 *)(this + 8) = 0;
  *(undefined **)(this + 0x10) = puVar1;
  QString::operator=((QString *)(this + 0x10),(QString *)param_1);
  return;
}



// ====== KisNodeUuidInfo @ 0046e6b0 ======

/* KisNodeUuidInfo::KisNodeUuidInfo(KisSharedPtr<KisNode>) */

void __thiscall KisNodeUuidInfo::KisNodeUuidInfo(KisNodeUuidInfo *this,KisSharedPtr param_1)

{
  long lVar1;
  QArrayData *pQVar2;
  undefined *puVar3;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  undefined auVar4 [16];
  undefined8 local_28;
  
  puVar3 = PTR_shared_null_008377d0;
  lVar1 = *(long *)(in_FS_OFFSET + 0x28);
  *(undefined8 *)this = 0;
  *(undefined8 *)(this + 8) = 0;
  *(undefined **)(this + 0x10) = puVar3;
                    /* try { // try from 0046e6ed to 0046e704 has its CatchHandler @ 0046e75a */
  auVar4 = KisBaseNode::uuid(*(KisBaseNode **)CONCAT44(in_register_00000034,param_1));
  *(undefined (*) [16])this = auVar4;
  QObject::objectName();
  pQVar2 = *(QArrayData **)(this + 0x10);
  *(undefined8 *)(this + 0x10) = local_28;
  if (*(int *)pQVar2 != 0) {
    if (*(int *)pQVar2 == -1) goto LAB_0046e726;
    LOCK();
    *(int *)pQVar2 = *(int *)pQVar2 + -1;
    UNLOCK();
    if (*(int *)pQVar2 != 0) goto LAB_0046e726;
  }
  QArrayData::deallocate(pQVar2,2,8);
LAB_0046e726:
  if (lVar1 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



