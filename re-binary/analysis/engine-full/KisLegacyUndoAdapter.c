/* Class KisLegacyUndoAdapter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLegacyUndoAdapter @ 00205fc0 ======

void __thiscall
KisLegacyUndoAdapter::KisLegacyUndoAdapter
          (KisLegacyUndoAdapter *this,KisUndoStore *param_1,KisWeakSharedPtr param_2)

{
  (*(code *)PTR_KisLegacyUndoAdapter_0083aab0)();
  return;
}



// ====== KisLegacyUndoAdapter @ 0062b740 ======

/* KisLegacyUndoAdapter::KisLegacyUndoAdapter(KisUndoStore*, KisWeakSharedPtr<KisImage>) */

void __thiscall
KisLegacyUndoAdapter::KisLegacyUndoAdapter
          (KisLegacyUndoAdapter *this,KisUndoStore *param_1,KisWeakSharedPtr param_2)

{
  long lVar1;
  int *piVar2;
  undefined4 in_register_00000014;
  long *plVar3;
  QObject *pQVar4;
  long in_FS_OFFSET;
  QArrayData *local_68;
  QTextStream *local_60;
  undefined8 local_58;
  undefined local_50 [16];
  undefined8 local_40;
  long local_30;
  
  plVar3 = (long *)CONCAT44(in_register_00000014,param_2);
  pQVar4 = (QObject *)*plVar3;
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  if (pQVar4 != (QObject *)0x0) {
    if (((uint *)plVar3[1] == (uint *)0x0) || ((*(uint *)plVar3[1] & 1) == 0)) {
      lVar1 = _41000();
      if (*(char *)(lVar1 + 0x11) != '\0') {
        lVar1 = _41000();
        local_58 = 2;
        local_40 = *(undefined8 *)(lVar1 + 8);
        local_50 = (undefined  [16])0x0;
        QMessageLogger::warning();
        if (1 < *(int *)(local_60 + 0x28)) {
          *(uint *)(local_60 + 0x48) = *(uint *)(local_60 + 0x48) | 1;
        }
                    /* try { // try from 0062b844 to 0062b848 has its CatchHandler @ 0062b944 */
        kisBacktrace();
                    /* try { // try from 0062b857 to 0062b85b has its CatchHandler @ 0062b938 */
        QDebug::putString((QChar *)&local_60,(ulong)(local_68 + *(long *)(local_68 + 0x10)));
        if (local_60[0x20] != (QTextStream)0x0) {
                    /* try { // try from 0062b91d to 0062b921 has its CatchHandler @ 0062b938 */
          QTextStream::operator<<(local_60,' ');
        }
        if (*(int *)local_68 == 0) {
LAB_0062b888:
          QArrayData::deallocate(local_68,2,8);
        }
        else if (*(int *)local_68 != -1) {
          LOCK();
          *(int *)local_68 = *(int *)local_68 + -1;
          UNLOCK();
          if (*(int *)local_68 == 0) goto LAB_0062b888;
        }
        QDebug::~QDebug((QDebug *)&local_60);
      }
    }
    pQVar4 = (QObject *)*plVar3;
  }
  KisUndoAdapter::KisUndoAdapter((KisUndoAdapter *)this,param_1,pQVar4);
  lVar1 = *plVar3;
  *(undefined **)this = PTR_vtable_00837fa0 + 0x10;
  if (lVar1 == 0) {
    *(undefined8 *)(this + 0x18) = 0;
  }
  else {
    if (((uint *)plVar3[1] == (uint *)0x0) || ((*(uint *)plVar3[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x18) = (undefined  [16])0x0;
      goto LAB_0062b8b0;
    }
    lVar1 = *plVar3;
    *(long *)(this + 0x18) = lVar1;
    if (lVar1 != 0) {
      piVar2 = *(int **)(lVar1 + 0x58);
      if (piVar2 == (int *)0x0) {
                    /* try { // try from 0062b8f5 to 0062b8f9 has its CatchHandler @ 0062b92c */
        piVar2 = (int *)operator_new(4);
        *piVar2 = 0;
        *(int **)(lVar1 + 0x58) = piVar2;
        LOCK();
        *piVar2 = *piVar2 + 1;
        UNLOCK();
        piVar2 = *(int **)(lVar1 + 0x58);
      }
      *(int **)(this + 0x20) = piVar2;
      LOCK();
      *piVar2 = *piVar2 + 2;
      UNLOCK();
      goto LAB_0062b8b0;
    }
  }
  *(undefined8 *)(this + 0x20) = 0;
LAB_0062b8b0:
  *(undefined4 *)(this + 0x28) = 0;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



