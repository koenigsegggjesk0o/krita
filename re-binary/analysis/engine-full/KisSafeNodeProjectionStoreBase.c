/* Class KisSafeNodeProjectionStoreBase - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSafeNodeProjectionStoreBase @ 002014b0 ======

void __thiscall
KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase
          (KisSafeNodeProjectionStoreBase *this,KisSafeNodeProjectionStoreBase *param_1)

{
  (*(code *)PTR_KisSafeNodeProjectionStoreBase_00838528)();
  return;
}



// ====== KisSafeNodeProjectionStoreBase @ 00208640 ======

void __thiscall
KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase
          (KisSafeNodeProjectionStoreBase *this,StoreImplementationInterface *param_1)

{
  (*(code *)PTR_KisSafeNodeProjectionStoreBase_0083bdf0)();
  return;
}



// ====== KisSafeNodeProjectionStoreBase @ 00570570 ======

/* KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase(StoreImplementationInterface*) */

void __thiscall
KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase
          (KisSafeNodeProjectionStoreBase *this,StoreImplementationInterface *param_1)

{
  undefined (*pauVar1) [16];
  long in_FS_OFFSET;
  QObject aQStack_28 [8];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
                    /* try { // try from 005705a0 to 005705a4 has its CatchHandler @ 0057064e */
  KisShared::KisShared((KisShared *)(this + 0x10));
  *(undefined **)this = PTR_vtable_00836cf8 + 0x10;
                    /* try { // try from 005705b8 to 005705bc has its CatchHandler @ 00570642 */
  pauVar1 = (undefined (*) [16])operator_new(0x20);
  *(undefined8 *)pauVar1[1] = 0;
  *pauVar1 = (undefined  [16])0x0;
  *(undefined (**) [16])(this + 0x20) = pauVar1;
  *(StoreImplementationInterface **)(pauVar1[1] + 8) = param_1;
                    /* try { // try from 005705de to 0057060f has its CatchHandler @ 00570636 */
  QObject::thread();
  QObject::moveToThread((QThread *)this);
  QObject::connect(aQStack_28,(char *)this,(QObject *)"2internalInitiateProjectionsCleanup()",
                   (char *)this,0x735958);
  QMetaObject::Connection::~Connection((Connection *)aQStack_28);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSafeNodeProjectionStoreBase @ 00570700 ======

/* KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase(KisSafeNodeProjectionStoreBase
   const&) */

void __thiscall
KisSafeNodeProjectionStoreBase::KisSafeNodeProjectionStoreBase
          (KisSafeNodeProjectionStoreBase *this,KisSafeNodeProjectionStoreBase *param_1)

{
  int iVar1;
  long *plVar2;
  bool bVar3;
  undefined (*pauVar4) [16];
  long *plVar5;
  int *piVar6;
  long lVar7;
  long in_FS_OFFSET;
  QObject local_48 [8];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
                    /* try { // try from 00570736 to 0057073a has its CatchHandler @ 00570947 */
  KisShared::KisShared((KisShared *)(this + 0x10));
  *(undefined **)this = PTR_vtable_00836cf8 + 0x10;
                    /* try { // try from 0057074e to 00570752 has its CatchHandler @ 00570953 */
  pauVar4 = (undefined (*) [16])operator_new(0x20);
  *(undefined (**) [16])(this + 0x20) = pauVar4;
  lVar7 = *(long *)(param_1 + 0x20);
  *pauVar4 = (undefined  [16])0x0;
  pauVar4[1] = (undefined  [16])0x0;
  if (lVar7 == 0) {
    *(undefined8 *)(*pauVar4 + 8) = 0;
    bVar3 = false;
    lVar7 = 0;
  }
  else {
    QMutex::lock();
    pauVar4 = *(undefined (**) [16])(this + 0x20);
    lVar7 = *(long *)(param_1 + 0x20);
    piVar6 = *(int **)pauVar4[1];
    *(undefined8 *)(*pauVar4 + 8) = 0;
    if (piVar6 != (int *)0x0) {
      LOCK();
      iVar1 = *piVar6;
      *piVar6 = *piVar6 + -2;
      UNLOCK();
      if (iVar1 < 3) {
        if (*(void **)pauVar4[1] != (void *)0x0) {
          operator_delete(*(void **)pauVar4[1],4);
        }
        *(undefined8 *)pauVar4[1] = 0;
      }
    }
    bVar3 = true;
  }
  if (*(long *)(lVar7 + 8) == 0) {
    *(undefined8 *)(*pauVar4 + 8) = 0;
  }
  else {
    if ((*(uint **)(lVar7 + 0x10) == (uint *)0x0) || ((**(uint **)(lVar7 + 0x10) & 1) == 0)) {
      *(undefined (*) [16])(*pauVar4 + 8) = (undefined  [16])0x0;
      goto LAB_005707dc;
    }
    lVar7 = *(long *)(lVar7 + 8);
    *(long *)(*pauVar4 + 8) = lVar7;
    if (lVar7 != 0) {
      piVar6 = *(int **)(lVar7 + 0x58);
      if (piVar6 == (int *)0x0) {
                    /* try { // try from 0057091d to 00570921 has its CatchHandler @ 0057095f */
        piVar6 = (int *)operator_new(4);
        *piVar6 = 0;
        *(int **)(lVar7 + 0x58) = piVar6;
        LOCK();
        *piVar6 = *piVar6 + 1;
        UNLOCK();
        piVar6 = *(int **)(lVar7 + 0x58);
      }
      *(int **)pauVar4[1] = piVar6;
      LOCK();
      *piVar6 = *piVar6 + 2;
      UNLOCK();
      goto LAB_005707dc;
    }
  }
  *(undefined8 *)pauVar4[1] = 0;
LAB_005707dc:
  lVar7 = *(long *)(this + 0x20);
                    /* try { // try from 005707ec to 005707ee has its CatchHandler @ 0057095f */
  plVar5 = (long *)(**(code **)(**(long **)(*(long *)(param_1 + 0x20) + 0x18) + 0x10))();
  plVar2 = *(long **)(lVar7 + 0x18);
  if ((plVar5 != plVar2) && (*(long **)(lVar7 + 0x18) = plVar5, plVar2 != (long *)0x0)) {
    (**(code **)(*plVar2 + 8))();
  }
  if (bVar3) {
    QMutex::unlock();
  }
                    /* try { // try from 0057081a to 0057084d has its CatchHandler @ 0057093b */
  QObject::thread();
  QObject::moveToThread((QThread *)this);
  QObject::connect(local_48,(char *)this,(QObject *)"2internalInitiateProjectionsCleanup()",
                   (char *)this,0x735958);
  QMetaObject::Connection::~Connection((Connection *)local_48);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



