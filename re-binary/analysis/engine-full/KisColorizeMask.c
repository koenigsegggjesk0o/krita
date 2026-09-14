/* Class KisColorizeMask - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisColorizeMask @ 00209770 ======

void __thiscall KisColorizeMask::KisColorizeMask(KisColorizeMask *this,KisColorizeMask *param_1)

{
  (*(code *)PTR_KisColorizeMask_0083c688)();
  return;
}



// ====== KisColorizeMask @ 00447660 ======

/* KisColorizeMask::KisColorizeMask(KisWeakSharedPtr<KisImage>, QString const&) */

void __thiscall
KisColorizeMask::KisColorizeMask(KisColorizeMask *this,KisWeakSharedPtr param_1,QString *param_2)

{
  int iVar1;
  long lVar2;
  undefined auVar3 [8];
  undefined auVar4 [16];
  undefined *puVar5;
  void *pvVar6;
  int *piVar7;
  undefined4 in_register_00000034;
  ulong *puVar8;
  long in_FS_OFFSET;
  undefined local_48 [8];
  int *piStack_40;
  long local_30;
  
  puVar8 = (ulong *)CONCAT44(in_register_00000034,param_1);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  if (*puVar8 == 0) {
    local_48 = (undefined  [8])0x0;
    auVar3 = local_48;
LAB_004478c1:
    local_48 = auVar3;
    piStack_40 = (int *)0x0;
  }
  else if (((uint *)puVar8[1] == (uint *)0x0) || ((*(uint *)puVar8[1] & 1) == 0)) {
    local_48 = (undefined  [8])0x0;
    piStack_40 = (int *)0x0;
  }
  else {
    auVar3 = (undefined  [8])*puVar8;
    piStack_40 = (int *)local_48;
    local_48 = auVar3;
    if (auVar3 == (undefined  [8])0x0) goto LAB_004478c1;
    piVar7 = *(int **)((long)auVar3 + 0x58);
    if (piVar7 == (int *)0x0) {
      piVar7 = (int *)operator_new(4);
      *piVar7 = 0;
      *(int **)((long)auVar3 + 0x58) = piVar7;
      LOCK();
      *piVar7 = *piVar7 + 1;
      UNLOCK();
      piVar7 = *(int **)((long)auVar3 + 0x58);
      auVar3 = local_48;
    }
    local_48 = auVar3;
    LOCK();
    *piVar7 = *piVar7 + 2;
    UNLOCK();
    piStack_40 = piVar7;
  }
                    /* try { // try from 004476b3 to 004476b7 has its CatchHandler @ 00447954 */
  KisEffectMask::KisEffectMask((KisEffectMask *)this,(KisWeakSharedPtr)(QObject *)local_48,param_2);
  piVar7 = piStack_40;
  auVar4._8_8_ = 0;
  auVar4._0_8_ = piStack_40;
  _local_48 = auVar4 << 0x40;
  if (piVar7 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar7;
    *piVar7 = *piVar7 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar7 != (int *)0x0)) {
      operator_delete(piVar7,4);
    }
  }
  puVar5 = PTR_vtable_008377b8 + 600;
  *(undefined **)this = PTR_vtable_008377b8 + 0x10;
  *(undefined **)(this + 0x30) = puVar5;
                    /* try { // try from 004476fa to 004476fe has its CatchHandler @ 00447960 */
  pvVar6 = operator_new(0x130);
  if (*puVar8 == 0) {
    local_48 = (undefined  [8])0x0;
    auVar3 = local_48;
  }
  else {
    if (((uint *)puVar8[1] == (uint *)0x0) || ((*(uint *)puVar8[1] & 1) == 0)) {
      _local_48 = (undefined  [16])0x0;
      goto LAB_0044772a;
    }
    auVar3 = (undefined  [8])*puVar8;
    local_48 = auVar3;
    if (auVar3 != (undefined  [8])0x0) {
      piVar7 = *(int **)((long)auVar3 + 0x58);
      if (piVar7 == (int *)0x0) {
                    /* try { // try from 00447925 to 00447929 has its CatchHandler @ 00447948 */
        piVar7 = (int *)operator_new(4);
        *piVar7 = 0;
        *(int **)((long)auVar3 + 0x58) = piVar7;
        LOCK();
        *piVar7 = *piVar7 + 1;
        UNLOCK();
        piVar7 = *(int **)((long)auVar3 + 0x58);
        auVar3 = local_48;
      }
      local_48 = auVar3;
      piStack_40 = piVar7;
      LOCK();
      *piVar7 = *piVar7 + 2;
      UNLOCK();
      goto LAB_0044772a;
    }
  }
  local_48 = auVar3;
  piStack_40 = (int *)0x0;
LAB_0044772a:
                    /* try { // try from 00447733 to 00447737 has its CatchHandler @ 00447978 */
  FUN_0044eaf0(pvVar6,this,(QObject *)local_48);
  *(void **)(this + 0x48) = pvVar6;
  FUN_0035b390((QObject *)local_48);
                    /* try { // try from 00447769 to 0044781f has its CatchHandler @ 0044796c */
  QObject::connect((QObject *)local_48,(char *)(*(long *)(this + 0x48) + 0xa8),
                   (QObject *)"2timeout()",(char *)this,0x72d100);
  QMetaObject::Connection::~Connection((Connection *)local_48);
  QObject::connect((QObject *)local_48,(char *)this,(QObject *)"2sigUpdateOnDirtyParent()",
                   (char *)(*(long *)(this + 0x48) + 0xc0),0x72d02d);
  QMetaObject::Connection::~Connection((Connection *)local_48);
  QObject::connect((QObject *)local_48,(char *)(*(long *)(this + 0x48) + 0xc0),
                   (QObject *)"2timeout()",(char *)this,0x72d050);
  QMetaObject::Connection::~Connection((Connection *)local_48);
  QObject::connect((QObject *)local_48,(char *)(*(long *)(this + 0x48) + 0xd8),
                   (QObject *)"2timeout()",(char *)this,0x72d120);
  QMetaObject::Connection::~Connection((Connection *)local_48);
  lVar2 = *(long *)(this + 0x48);
  QObject::thread();
  QObject::moveToThread((QThread *)(lVar2 + 0xa8));
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisColorizeMask @ 00449920 ======

/* KisColorizeMask::KisColorizeMask(KisColorizeMask const&) */

void __thiscall KisColorizeMask::KisColorizeMask(KisColorizeMask *this,KisColorizeMask *param_1)

{
  long lVar1;
  undefined *puVar2;
  void *pvVar3;
  long in_FS_OFFSET;
  QObject aQStack_38 [8];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisEffectMask::KisEffectMask((KisEffectMask *)this,(KisEffectMask *)param_1);
  puVar2 = PTR_vtable_008377b8 + 600;
  *(undefined **)this = PTR_vtable_008377b8 + 0x10;
  *(undefined **)(this + 0x30) = puVar2;
                    /* try { // try from 00449966 to 0044996a has its CatchHandler @ 00449a6a */
  pvVar3 = operator_new(0x130);
                    /* try { // try from 00449979 to 0044997d has its CatchHandler @ 00449a5e */
  FUN_004505a0(pvVar3,*(undefined8 *)(param_1 + 0x48),this);
  *(void **)(this + 0x48) = pvVar3;
                    /* try { // try from 004499a6 to 00449a31 has its CatchHandler @ 00449a52 */
  QObject::connect(aQStack_38,(char *)((long)pvVar3 + 0xa8),(QObject *)"2timeout()",(char *)this,
                   0x72d100);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QObject::connect(aQStack_38,(char *)this,(QObject *)"2sigUpdateOnDirtyParent()",
                   (char *)(*(long *)(this + 0x48) + 0xc0),0x72d02d);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QObject::connect(aQStack_38,(char *)(*(long *)(this + 0x48) + 0xc0),(QObject *)"2timeout()",
                   (char *)this,0x72d050);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  lVar1 = *(long *)(this + 0x48);
  QObject::thread();
  QObject::moveToThread((QThread *)(lVar1 + 0xa8));
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



