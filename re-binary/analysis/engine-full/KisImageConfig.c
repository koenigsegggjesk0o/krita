/* Class KisImageConfig - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageConfig @ 00204950 ======

void __thiscall KisImageConfig::KisImageConfig(KisImageConfig *this,bool param_1)

{
  (*(code *)PTR_KisImageConfig_00839f78)();
  return;
}



// ====== KisImageConfig @ 0052e4a0 ======

/* KisImageConfig::KisImageConfig(bool) */

void __thiscall KisImageConfig::KisImageConfig(KisImageConfig *this,bool param_1)

{
  long *plVar1;
  int iVar2;
  undefined *puVar3;
  long lVar4;
  long lVar5;
  long in_FS_OFFSET;
  long *local_48;
  QArrayData *local_40;
  QArrayData *local_38;
  long local_30;
  
  puVar3 = PTR_shared_null_008377d0;
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  local_40 = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 0052e4e9 to 0052e4ed has its CatchHandler @ 0052e61d */
  KSharedConfig::openConfig((QString *)&local_48,(QFlags)&local_40,3);
  local_38 = (QArrayData *)puVar3;
                    /* try { // try from 0052e502 to 0052e506 has its CatchHandler @ 0052e629 */
  KConfigBase::group((QString *)this);
  if (*(int *)local_38 == 0) {
LAB_0052e5f8:
    QArrayData::deallocate(local_38,2,8);
  }
  else if (*(int *)local_38 != -1) {
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 == 0) goto LAB_0052e5f8;
  }
  if (local_48 == (long *)0x0) {
LAB_0052e53a:
    iVar2 = *(int *)local_40;
  }
  else {
    LOCK();
    plVar1 = local_48 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if ((*(int *)plVar1 != 0) || (local_48 == (long *)0x0)) goto LAB_0052e53a;
    (**(code **)(*local_48 + 8))();
    iVar2 = *(int *)local_40;
  }
  if (iVar2 == 0) {
LAB_0052e5a0:
    QArrayData::deallocate(local_40,2,8);
    this[0x10] = (KisImageConfig)param_1;
  }
  else {
    if (iVar2 != -1) {
      LOCK();
      *(int *)local_40 = *(int *)local_40 + -1;
      UNLOCK();
      if (*(int *)local_40 == 0) goto LAB_0052e5a0;
    }
    this[0x10] = (KisImageConfig)param_1;
  }
  if (!param_1) {
                    /* try { // try from 0052e5c2 to 0052e5eb has its CatchHandler @ 0052e611 */
    lVar4 = QObject::thread();
    lVar5 = QThread::currentThread();
    if (lVar4 != lVar5) {
      kis_safe_assert_recoverable
                ("(static_cast<QApplication *>(QCoreApplication::instance()))->thread() == QThread::currentThread()"
                 ,"/builds/graphics/krita/libs/image/kis_image_config.cpp",0x27);
    }
  }
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



