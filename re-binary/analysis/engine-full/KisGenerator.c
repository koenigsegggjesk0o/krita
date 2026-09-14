/* Class KisGenerator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGenerator @ 00384d30 ======

/* KisGenerator::KisGenerator(KoID const&, KoID const&, QString const&) */

void __thiscall
KisGenerator::KisGenerator(KisGenerator *this,KoID *param_1,KoID *param_2,QString *param_3)

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
  *(undefined **)this = PTR_vtable_008370d0 + 0x10;
                    /* try { // try from 00384d73 to 00384d77 has its CatchHandler @ 00384e92 */
  KisBaseProcessor::id((KisBaseProcessor *)&local_50);
  iVar1 = *(int *)(local_50 + 4);
                    /* try { // try from 00384d90 to 00384d94 has its CatchHandler @ 00384eaa */
  QString::QString((QString *)&local_58,iVar1 + 0x14,0);
  iVar2 = *(int *)(local_50 + 4);
  __dest = local_58 + *(long *)(local_58 + 0x10);
  local_48 = __dest;
  memcpy(__dest,local_50 + *(long *)(local_50 + 0x10),(long)iVar2 * 2);
  local_48 = local_48 + (long)iVar2 * 2;
  QAbstractConcatenable::convertFromAscii("_generator_bookmarks",0x14,(QChar **)&local_48);
  if ((long)(iVar1 + 0x14) != (long)local_48 - (long)__dest >> 1) {
                    /* try { // try from 00384e83 to 00384e87 has its CatchHandler @ 00384eb6 */
    QString::resize((int)&local_58);
  }
                    /* try { // try from 00384df5 to 00384df9 has its CatchHandler @ 00384e9e */
  KisBaseProcessor::init((KisBaseProcessor *)this,(QString *)&local_58);
  if (*(int *)local_58 == 0) {
LAB_00384e50:
    QArrayData::deallocate(local_58,2,8);
    iVar1 = *(int *)local_50;
  }
  else {
    if (*(int *)local_58 != -1) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + -1;
      UNLOCK();
      if (*(int *)local_58 == 0) goto LAB_00384e50;
    }
    iVar1 = *(int *)local_50;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_00384e2e;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_00384e2e;
  }
  QArrayData::deallocate(local_50,2,8);
LAB_00384e2e:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



