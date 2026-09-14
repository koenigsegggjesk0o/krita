/* Class KisLockedPropertiesServer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLockedPropertiesServer @ 00205170 ======

void __thiscall
KisLockedPropertiesServer::KisLockedPropertiesServer(KisLockedPropertiesServer *this)

{
  (*(code *)PTR_KisLockedPropertiesServer_0083a388)();
  return;
}



// ====== KisLockedPropertiesServer @ 00352720 ======

/* KisLockedPropertiesServer::KisLockedPropertiesServer() */

void __thiscall
KisLockedPropertiesServer::KisLockedPropertiesServer(KisLockedPropertiesServer *this)

{
  int *piVar1;
  undefined *puVar2;
  int *piVar3;
  
  QObject::QObject((QObject *)this,(QObject *)0x0);
  puVar2 = PTR_vtable_00837a60;
  *(undefined8 *)(this + 0x10) = 0;
  *(undefined **)this = puVar2 + 0x10;
                    /* try { // try from 0035274d to 00352751 has its CatchHandler @ 003527a7 */
  piVar3 = (int *)operator_new(0x18);
                    /* try { // try from 00352758 to 0035275c has its CatchHandler @ 003527b3 */
  FUN_0034eea0(piVar3);
  if (piVar3 != *(int **)(this + 0x10)) {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
    piVar1 = *(int **)(this + 0x10);
    *(int **)(this + 0x10) = piVar3;
    if (piVar1 != (int *)0x0) {
      LOCK();
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (*piVar1 == 0) {
        FUN_0034ef40(piVar1);
        operator_delete(piVar1,0x18);
      }
    }
  }
  this[0x18] = (KisLockedPropertiesServer)0x0;
  return;
}



