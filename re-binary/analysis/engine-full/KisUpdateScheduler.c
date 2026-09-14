/* Class KisUpdateScheduler - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisUpdateScheduler @ 00205090 ======

void __thiscall KisUpdateScheduler::KisUpdateScheduler(KisUpdateScheduler *this)

{
  (*(code *)PTR_KisUpdateScheduler_0083a318)();
  return;
}



// ====== KisUpdateScheduler @ 002080d0 ======

void __thiscall
KisUpdateScheduler::KisUpdateScheduler
          (KisUpdateScheduler *this,KisProjectionUpdateListener *param_1,QObject *param_2)

{
  (*(code *)PTR_KisUpdateScheduler_0083bb38)();
  return;
}



// ====== KisUpdateScheduler @ 004fcdd0 ======

/* KisUpdateScheduler::KisUpdateScheduler(KisProjectionUpdateListener*, QObject*) */

void __thiscall
KisUpdateScheduler::KisUpdateScheduler
          (KisUpdateScheduler *this,KisProjectionUpdateListener *param_1,QObject *param_2)

{
  KisUpdateScheduler *pKVar1;
  undefined8 uVar2;
  int iVar3;
  undefined *puVar4;
  undefined8 *puVar5;
  long in_FS_OFFSET;
  KisImageConfig local_58 [24];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,param_2);
  puVar4 = PTR_vtable_00837af0 + 0xa0;
  *(undefined **)this = PTR_vtable_00837af0 + 0x10;
  *(undefined **)(this + 0x10) = puVar4;
                    /* try { // try from 004fce1e to 004fce22 has its CatchHandler @ 004fcf70 */
  puVar5 = (undefined8 *)operator_new(0x100);
  *puVar5 = this;
                    /* try { // try from 004fce35 to 004fce39 has its CatchHandler @ 004fcf64 */
  KisSimpleUpdateQueue::KisSimpleUpdateQueue((KisSimpleUpdateQueue *)(puVar5 + 1));
                    /* try { // try from 004fce41 to 004fce45 has its CatchHandler @ 004fcf58 */
  KisStrokesQueue::KisStrokesQueue((KisStrokesQueue *)(puVar5 + 10));
  pKVar1 = (KisUpdateScheduler *)*puVar5;
                    /* try { // try from 004fce5e to 004fce62 has its CatchHandler @ 004fcf4c */
  KisImageConfig::KisImageConfig(local_58,true);
                    /* try { // try from 004fce68 to 004fce7a has its CatchHandler @ 004fcf40 */
  iVar3 = KisImageConfig::maxNumberOfThreads(local_58,false);
  KisUpdaterContext::KisUpdaterContext((KisUpdaterContext *)(puVar5 + 0xc),iVar3,pKVar1);
  KisImageConfig::~KisImageConfig(local_58);
  uVar2 = DAT_007227c0;
  *(undefined *)(puVar5 + 0x17) = 0;
  puVar5[0x19] = param_1;
  puVar5[0x18] = uVar2;
  puVar5[0x1a] = 0;
  *(undefined4 *)(puVar5 + 0x1b) = 0;
                    /* try { // try from 004fcec0 to 004fcec4 has its CatchHandler @ 004fcf34 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(puVar5 + 0x1c),0);
  puVar5[0x1d] = 0;
                    /* try { // try from 004fced7 to 004fcedb has its CatchHandler @ 004fcf28 */
  QWaitCondition::QWaitCondition((QWaitCondition *)(puVar5 + 0x1e));
  *(undefined4 *)(puVar5 + 0x1f) = 0;
  *(undefined4 *)((long)puVar5 + 0xfc) = 0;
  *(undefined8 **)(this + 0x18) = puVar5;
                    /* try { // try from 004fcef7 to 004fcf03 has its CatchHandler @ 004fcf70 */
  updateSettings(this);
  connectSignals(this);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisUpdateScheduler @ 004fd090 ======

/* KisUpdateScheduler::KisUpdateScheduler() */

void __thiscall KisUpdateScheduler::KisUpdateScheduler(KisUpdateScheduler *this)

{
  KisUpdateScheduler *pKVar1;
  undefined8 uVar2;
  int iVar3;
  undefined *puVar4;
  undefined8 *puVar5;
  long in_FS_OFFSET;
  KisImageConfig local_58 [24];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  puVar4 = PTR_vtable_00837af0 + 0xa0;
  *(undefined **)this = PTR_vtable_00837af0 + 0x10;
  *(undefined **)(this + 0x10) = puVar4;
                    /* try { // try from 004fd0da to 004fd0de has its CatchHandler @ 004fd214 */
  puVar5 = (undefined8 *)operator_new(0x100);
  *puVar5 = this;
                    /* try { // try from 004fd0ec to 004fd0f0 has its CatchHandler @ 004fd208 */
  KisSimpleUpdateQueue::KisSimpleUpdateQueue((KisSimpleUpdateQueue *)(puVar5 + 1));
                    /* try { // try from 004fd0f8 to 004fd0fc has its CatchHandler @ 004fd1fc */
  KisStrokesQueue::KisStrokesQueue((KisStrokesQueue *)(puVar5 + 10));
  pKVar1 = (KisUpdateScheduler *)*puVar5;
                    /* try { // try from 004fd116 to 004fd11a has its CatchHandler @ 004fd1f0 */
  KisImageConfig::KisImageConfig(local_58,true);
                    /* try { // try from 004fd120 to 004fd133 has its CatchHandler @ 004fd1e4 */
  iVar3 = KisImageConfig::maxNumberOfThreads(local_58,false);
  KisUpdaterContext::KisUpdaterContext((KisUpdaterContext *)(puVar5 + 0xc),iVar3,pKVar1);
  KisImageConfig::~KisImageConfig(local_58);
  uVar2 = DAT_007227c0;
  *(undefined *)(puVar5 + 0x17) = 0;
  *(undefined4 *)(puVar5 + 0x1b) = 0;
  puVar5[0x18] = uVar2;
  *(undefined (*) [16])(puVar5 + 0x19) = (undefined  [16])0x0;
                    /* try { // try from 004fd172 to 004fd176 has its CatchHandler @ 004fd1d8 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(puVar5 + 0x1c),0);
  puVar5[0x1d] = 0;
                    /* try { // try from 004fd189 to 004fd18d has its CatchHandler @ 004fd1ca */
  QWaitCondition::QWaitCondition((QWaitCondition *)(puVar5 + 0x1e));
  *(undefined4 *)(puVar5 + 0x1f) = 0;
  *(undefined4 *)((long)puVar5 + 0xfc) = 0;
  *(undefined8 **)(this + 0x18) = puVar5;
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



