/* Class KisFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisFilter @ 0020ab40 ======

void __thiscall KisFilter::KisFilter(KisFilter *this,KoID *param_1,KoID *param_2,QString *param_3)

{
  (*(code *)PTR_KisFilter_0083d070)();
  return;
}



// ====== KisFilter @ 0037f280 ======

/* KisFilter::KisFilter(KoID const&, KoID const&, QString const&) */

void __thiscall KisFilter::KisFilter(KisFilter *this,KoID *param_1,KoID *param_2,QString *param_3)

{
  int iVar1;
  int iVar2;
  QArrayData *__dest;
  long in_FS_OFFSET;
  QArrayData *local_58;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisBaseProcessor::KisBaseProcessor((KisBaseProcessor *)this,param_1,param_2,param_3);
  this[0x20] = (KisFilter)0x0;
  *(undefined **)this = PTR_vtable_00837fb8 + 0x10;
                    /* try { // try from 0037f2c7 to 0037f2cb has its CatchHandler @ 0037f3ea */
  KisBaseProcessor::id((KisBaseProcessor *)&local_50);
  iVar1 = *(int *)(local_50 + 4);
                    /* try { // try from 0037f2e4 to 0037f2e8 has its CatchHandler @ 0037f402 */
  QString::QString((QString *)&local_58,iVar1 + 0x11,0);
  iVar2 = *(int *)(local_50 + 4);
  __dest = local_58 + *(long *)(local_58 + 0x10);
  local_48 = __dest;
  memcpy(__dest,local_50 + *(long *)(local_50 + 0x10),(long)iVar2 * 2);
  local_48 = local_48 + (long)iVar2 * 2;
  QAbstractConcatenable::convertFromAscii("_filter_bookmarks",0x11,(QChar **)&local_48);
  if ((long)(iVar1 + 0x11) != (long)local_48 - (long)__dest >> 1) {
                    /* try { // try from 0037f3db to 0037f3df has its CatchHandler @ 0037f40e */
    QString::resize((int)&local_58);
  }
                    /* try { // try from 0037f349 to 0037f34d has its CatchHandler @ 0037f3f6 */
  KisBaseProcessor::init((KisBaseProcessor *)this,(QString *)&local_58);
  if (*(int *)local_58 == 0) {
LAB_0037f3a8:
    QArrayData::deallocate(local_58,2,8);
    iVar1 = *(int *)local_50;
  }
  else {
    if (*(int *)local_58 != -1) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + -1;
      UNLOCK();
      if (*(int *)local_58 == 0) goto LAB_0037f3a8;
    }
    iVar1 = *(int *)local_50;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_0037f382;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_0037f382;
  }
  QArrayData::deallocate(local_50,2,8);
LAB_0037f382:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



