/* Class KisImageSignalRouter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageSignalRouter @ 00203b80 ======

void __thiscall
KisImageSignalRouter::KisImageSignalRouter(KisImageSignalRouter *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisImageSignalRouter_00839890)();
  return;
}



// ====== KisImageSignalRouter @ 005280b0 ======

/* KisImageSignalRouter::KisImageSignalRouter(KisWeakSharedPtr<KisImage>) */

void __thiscall
KisImageSignalRouter::KisImageSignalRouter(KisImageSignalRouter *this,KisWeakSharedPtr param_1)

{
  long lVar1;
  int *piVar2;
  char *pcVar3;
  undefined4 in_register_00000034;
  long *plVar4;
  QObject *pQVar5;
  long in_FS_OFFSET;
  QArrayData *local_58;
  QTextStream *local_50;
  undefined8 local_48;
  undefined local_40 [16];
  undefined8 local_30;
  long local_20;
  
  plVar4 = (long *)CONCAT44(in_register_00000034,param_1);
  pQVar5 = (QObject *)*plVar4;
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (pQVar5 != (QObject *)0x0) {
    if (((uint *)plVar4[1] == (uint *)0x0) || ((*(uint *)plVar4[1] & 1) == 0)) {
      lVar1 = _41000();
      if (*(char *)(lVar1 + 0x11) != '\0') {
        lVar1 = _41000();
        local_30 = *(undefined8 *)(lVar1 + 8);
        local_40 = (undefined  [16])0x0;
        local_48 = 2;
        QMessageLogger::warning();
        if (1 < *(int *)(local_50 + 0x28)) {
          *(uint *)(local_50 + 0x48) = *(uint *)(local_50 + 0x48) | 1;
        }
                    /* try { // try from 0052849d to 005284a1 has its CatchHandler @ 00528634 */
        kisBacktrace();
                    /* try { // try from 005284b0 to 005284b4 has its CatchHandler @ 0052864c */
        QDebug::putString((QChar *)&local_50,(ulong)(local_58 + *(long *)(local_58 + 0x10)));
        if (local_50[0x20] != (QTextStream)0x0) {
                    /* try { // try from 00528625 to 00528629 has its CatchHandler @ 0052864c */
          QTextStream::operator<<(local_50,' ');
        }
        if (*(int *)local_58 == 0) {
LAB_005284e0:
          QArrayData::deallocate(local_58,2,8);
        }
        else if (*(int *)local_58 != -1) {
          LOCK();
          *(int *)local_58 = *(int *)local_58 + -1;
          UNLOCK();
          if (*(int *)local_58 == 0) goto LAB_005284e0;
        }
        QDebug::~QDebug((QDebug *)&local_50);
      }
    }
    pQVar5 = (QObject *)*plVar4;
  }
  QObject::QObject((QObject *)this,pQVar5);
  lVar1 = *plVar4;
  *(undefined **)this = PTR_vtable_00837388 + 0x10;
  if (lVar1 == 0) {
    *(undefined8 *)(this + 0x10) = 0;
  }
  else {
    if (((uint *)plVar4[1] == (uint *)0x0) || ((*(uint *)plVar4[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x10) = (undefined  [16])0x0;
      goto LAB_00528131;
    }
    lVar1 = *plVar4;
    *(long *)(this + 0x10) = lVar1;
    if (lVar1 != 0) {
      piVar2 = *(int **)(lVar1 + 0x58);
      if (piVar2 == (int *)0x0) {
                    /* try { // try from 005285fd to 00528601 has its CatchHandler @ 00528658 */
        piVar2 = (int *)operator_new(4);
        *piVar2 = 0;
        *(int **)(lVar1 + 0x58) = piVar2;
        LOCK();
        *piVar2 = *piVar2 + 1;
        UNLOCK();
        piVar2 = *(int **)(lVar1 + 0x58);
      }
      *(int **)(this + 0x18) = piVar2;
      LOCK();
      *piVar2 = *piVar2 + 2;
      UNLOCK();
      goto LAB_00528131;
    }
  }
  *(undefined8 *)(this + 0x18) = 0;
LAB_00528131:
                    /* try { // try from 00528150 to 0052841f has its CatchHandler @ 00528640 */
  QObject::connect((QObject *)&local_48,(char *)this,
                   (QObject *)"2sigNotification(KisImageSignalType)",(char *)this,0x732f98);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,(QObject *)"2sigImageModified()",pcVar3,
                   0x732917);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,(QObject *)"2sigImageModifiedWithoutUndo()",
                   pcVar3,0x732fe8);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,
                   (QObject *)"2sigSizeChanged(const QPointF&, const QPointF&)",pcVar3,0x733008);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,
                   (QObject *)"2sigResolutionChanged(double, double)",pcVar3,0x733038);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,
                   (QObject *)"2sigRequestNodeReselection(KisNodeSP, const KisNodeList&)",pcVar3,
                   0x733060);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,(QObject *)"2sigNodeChanged(KisNodeSP)",pcVar3,
                   0x733153);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,
                   (QObject *)"2sigNodeAddedAsync(KisNodeSP, KisNodeAdditionFlags)",pcVar3,0x7330a0)
  ;
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,(QObject *)"2sigRemoveNodeAsync(KisNodeSP)",
                   pcVar3,0x7330d8);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,(QObject *)"2sigLayersChangedAsync()",pcVar3,
                   0x73316e);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,
                   (QObject *)"2sigProfileChanged(const KoColorProfile*)",pcVar3,0x7330f8);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  pcVar3 = *(char **)(this + 0x10);
  if (pcVar3 != (char *)0x0) {
    if ((*(uint **)(this + 0x18) == (uint *)0x0) || ((**(uint **)(this + 0x18) & 1) == 0)) {
      pcVar3 = (char *)0x0;
    }
    else {
      pcVar3 = *(char **)(this + 0x10);
    }
  }
  QObject::connect((QObject *)&local_48,(char *)this,
                   (QObject *)"2sigColorSpaceChanged(const KoColorSpace*)",pcVar3,0x733128);
  QMetaObject::Connection::~Connection((Connection *)&local_48);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



