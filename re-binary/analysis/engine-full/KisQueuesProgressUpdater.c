/* Class KisQueuesProgressUpdater - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisQueuesProgressUpdater @ 0020b670 ======

void __thiscall
KisQueuesProgressUpdater::KisQueuesProgressUpdater
          (KisQueuesProgressUpdater *this,KoProgressProxy *param_1,QObject *param_2)

{
  (*(code *)PTR_KisQueuesProgressUpdater_0083d608)();
  return;
}



// ====== KisQueuesProgressUpdater @ 004fd7b0 ======

/* KisQueuesProgressUpdater::KisQueuesProgressUpdater(KoProgressProxy*, QObject*) */

void __thiscall
KisQueuesProgressUpdater::KisQueuesProgressUpdater
          (KisQueuesProgressUpdater *this,KoProgressProxy *param_1,QObject *param_2)

{
  long lVar1;
  undefined *puVar2;
  undefined8 *puVar3;
  long in_FS_OFFSET;
  QObject aQStack_38 [8];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,param_2);
  *(undefined **)this = PTR_vtable_00837818 + 0x10;
                    /* try { // try from 004fd7ef to 004fd7f3 has its CatchHandler @ 004fd972 */
  puVar3 = (undefined8 *)operator_new(0x68);
  *puVar3 = 0;
                    /* try { // try from 004fd808 to 004fd80c has its CatchHandler @ 004fd966 */
  QTimer::QTimer((QTimer *)(puVar3 + 1),(QObject *)this);
                    /* try { // try from 004fd814 to 004fd818 has its CatchHandler @ 004fd95a */
  QTimer::QTimer((QTimer *)(puVar3 + 5),(QObject *)this);
  puVar2 = PTR_shared_null_008377d0;
  *(undefined *)(puVar3 + 0xc) = 0;
  puVar3[9] = 0;
  puVar3[10] = puVar2;
  *(undefined8 **)(this + 0x10) = puVar3;
  puVar3[0xb] = param_1;
                    /* try { // try from 004fd840 to 004fd931 has its CatchHandler @ 004fd972 */
  QTimer::setInterval((int)(QTimer *)(puVar3 + 1));
  *(byte *)(*(long *)(this + 0x10) + 0x24) = *(byte *)(*(long *)(this + 0x10) + 0x24) & 0xfe;
  QObject::connect(aQStack_38,(char *)this,(QObject *)"2sigStartTicking()",(char *)this,0x7317c1);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QObject::connect(aQStack_38,(char *)this,(QObject *)"2sigStopTicking()",(char *)this,0x7317e4);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QObject::connect(aQStack_38,(char *)(*(long *)(this + 0x10) + 8),(QObject *)"2timeout()",
                   (char *)this,0x731805);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QTimer::setInterval((int)*(undefined8 *)(this + 0x10) + 0x28);
  lVar1 = *(long *)(this + 0x10);
  *(byte *)(lVar1 + 0x44) = *(byte *)(lVar1 + 0x44) | 1;
  QObject::connect(aQStack_38,(char *)(lVar1 + 0x28),(QObject *)"2timeout()",(char *)(lVar1 + 8),
                   0x72d02d);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QObject::connect(aQStack_38,(char *)(*(long *)(this + 0x10) + 0x28),(QObject *)"2timeout()",
                   (char *)this,0x731805);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



