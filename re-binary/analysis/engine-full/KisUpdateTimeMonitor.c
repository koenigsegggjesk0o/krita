/* Class KisUpdateTimeMonitor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisUpdateTimeMonitor @ 002030b0 ======

void __thiscall KisUpdateTimeMonitor::KisUpdateTimeMonitor(KisUpdateTimeMonitor *this)

{
  (*(code *)PTR_KisUpdateTimeMonitor_00839328)();
  return;
}



// ====== KisUpdateTimeMonitor @ 00502700 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisUpdateTimeMonitor::KisUpdateTimeMonitor() */

void __thiscall KisUpdateTimeMonitor::KisUpdateTimeMonitor(KisUpdateTimeMonitor *this)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined *puVar3;
  undefined uVar4;
  char cVar5;
  undefined8 *puVar6;
  long in_FS_OFFSET;
  QDir local_50 [8];
  QArrayData *local_48 [3];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  puVar6 = (undefined8 *)operator_new(0x70);
  puVar3 = PTR_shared_null_00836c40;
  *(undefined (*) [16])(puVar6 + 6) = (undefined  [16])0x0;
  uVar2 = _UNK_0072d018;
  uVar1 = _DAT_0072d010;
  puVar6[4] = 0;
  puVar6[5] = 0;
  puVar6[8] = 0;
  *(undefined *)(puVar6 + 0xd) = 0;
  *puVar6 = puVar3;
  puVar6[1] = puVar3;
  *(undefined (*) [16])(puVar6 + 2) = (undefined  [16])0x0;
  puVar6[9] = uVar1;
  puVar6[10] = uVar2;
  *(undefined (*) [16])(puVar6 + 0xb) = (undefined  [16])0x0;
                    /* try { // try from 00502786 to 0050278a has its CatchHandler @ 00502971 */
  KisImageConfig::KisImageConfig((KisImageConfig *)local_48,true);
                    /* try { // try from 00502790 to 00502794 has its CatchHandler @ 00502965 */
  uVar4 = KisImageConfig::enablePerfLog((KisImageConfig *)local_48,false);
  *(undefined *)(puVar6 + 0xd) = uVar4;
  KisImageConfig::~KisImageConfig((KisImageConfig *)local_48);
  cVar5 = *(char *)(puVar6 + 0xd);
  *(undefined8 **)this = puVar6;
  if (cVar5 == '\0') goto LAB_005027aa;
  local_48[0] = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 005027e7 to 005027eb has its CatchHandler @ 0050297d */
  QDir::QDir(local_50,(QString *)local_48);
  if (*(int *)local_48[0] == 0) {
LAB_005028e8:
    QArrayData::deallocate(local_48[0],2,8);
  }
  else if (*(int *)local_48[0] != -1) {
    LOCK();
    *(int *)local_48[0] = *(int *)local_48[0] + -1;
    UNLOCK();
    if (*(int *)local_48[0] == 0) goto LAB_005028e8;
  }
                    /* try { // try from 0050281e to 00502822 has its CatchHandler @ 00502989 */
  local_48[0] = (QArrayData *)QString::fromAscii_helper("log",3);
                    /* try { // try from 0050282e to 00502832 has its CatchHandler @ 00502995 */
  cVar5 = QDir::exists((QString *)local_50);
  if (*(int *)local_48[0] == 0) {
LAB_00502920:
    QArrayData::deallocate(local_48[0],2,8);
  }
  else if (*(int *)local_48[0] != -1) {
    LOCK();
    *(int *)local_48[0] = *(int *)local_48[0] + -1;
    UNLOCK();
    if (*(int *)local_48[0] == 0) goto LAB_00502920;
  }
  if (cVar5 != '\0') {
                    /* try { // try from 00502865 to 00502869 has its CatchHandler @ 00502989 */
    local_48[0] = (QArrayData *)QString::fromAscii_helper("log",3);
                    /* try { // try from 00502875 to 00502879 has its CatchHandler @ 00502959 */
    QDir::remove((QString *)local_50);
    if (*(int *)local_48[0] == 0) {
LAB_00502940:
      QArrayData::deallocate(local_48[0],2,8);
    }
    else if (*(int *)local_48[0] != -1) {
      LOCK();
      *(int *)local_48[0] = *(int *)local_48[0] + -1;
      UNLOCK();
      if (*(int *)local_48[0] == 0) goto LAB_00502940;
    }
  }
                    /* try { // try from 005028a8 to 005028ac has its CatchHandler @ 00502989 */
  local_48[0] = (QArrayData *)QString::fromAscii_helper("log",3);
                    /* try { // try from 005028b8 to 005028bc has its CatchHandler @ 005029a1 */
  QDir::mkdir((QString *)local_50);
  if (*(int *)local_48[0] == 0) {
LAB_00502900:
    QArrayData::deallocate(local_48[0],2,8);
    QDir::~QDir(local_50);
  }
  else {
    if (*(int *)local_48[0] != -1) {
      LOCK();
      *(int *)local_48[0] = *(int *)local_48[0] + -1;
      UNLOCK();
      if (*(int *)local_48[0] == 0) goto LAB_00502900;
    }
    QDir::~QDir(local_50);
  }
LAB_005027aa:
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



