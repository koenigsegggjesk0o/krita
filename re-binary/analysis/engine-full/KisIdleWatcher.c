/* Class KisIdleWatcher - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisIdleWatcher @ 00668b30 ======

/* KisIdleWatcher::KisIdleWatcher(int, QObject*) */

void __thiscall KisIdleWatcher::KisIdleWatcher(KisIdleWatcher *this,int param_1,QObject *param_2)

{
  undefined *puVar1;
  undefined8 *puVar2;
  long in_FS_OFFSET;
  QObject aQStack_38 [8];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,param_2);
  *(undefined **)this = PTR_vtable_00837d88 + 0x10;
                    /* try { // try from 00668b72 to 00668b76 has its CatchHandler @ 00668c72 */
  puVar2 = (undefined8 *)operator_new(0xa0);
  puVar1 = PTR_shared_null_008377d0;
  *puVar2 = PTR_shared_null_008377d0;
  puVar2[1] = puVar1;
                    /* try { // try from 00668b98 to 00668b9c has its CatchHandler @ 00668c5a */
  KisSignalCompressor::KisSignalCompressor
            ((KisSignalCompressor *)(puVar2 + 2),param_1,0,(QObject *)this);
                    /* try { // try from 00668ba6 to 00668baa has its CatchHandler @ 00668c66 */
  QTimer::QTimer((QTimer *)(puVar2 + 0xf),(QObject *)0x0);
  *(byte *)((long)puVar2 + 0x94) = *(byte *)((long)puVar2 + 0x94) | 1;
  *(undefined4 *)(puVar2 + 0x13) = 0;
                    /* try { // try from 00668bc2 to 00668bc6 has its CatchHandler @ 00668c4e */
  QTimer::setInterval((int)(QTimer *)(puVar2 + 0xf));
  *(undefined8 **)(this + 0x10) = puVar2;
                    /* try { // try from 00668beb to 00668c17 has its CatchHandler @ 00668c42 */
  QObject::connect(aQStack_38,(char *)(puVar2 + 2),(QObject *)"2timeout()",(char *)this,0x74b3c7);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QObject::connect(aQStack_38,(char *)(*(long *)(this + 0x10) + 0x78),(QObject *)"2timeout()",
                   (char *)this,0x74b3d9);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



