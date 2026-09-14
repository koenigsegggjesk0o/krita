/* Class KisMemoryStatisticsServer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMemoryStatisticsServer @ 00209170 ======

void __thiscall
KisMemoryStatisticsServer::KisMemoryStatisticsServer(KisMemoryStatisticsServer *this)

{
  (*(code *)PTR_KisMemoryStatisticsServer_0083c388)();
  return;
}



// ====== KisMemoryStatisticsServer @ 005b2e70 ======

/* KisMemoryStatisticsServer::KisMemoryStatisticsServer() */

void __thiscall
KisMemoryStatisticsServer::KisMemoryStatisticsServer(KisMemoryStatisticsServer *this)

{
  KisSignalCompressor *this_00;
  long in_FS_OFFSET;
  QObject aQStack_28 [8];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837ab0 + 0x10;
                    /* try { // try from 005b2ea9 to 005b2ead has its CatchHandler @ 005b2f42 */
  this_00 = (KisSignalCompressor *)operator_new(0x68);
                    /* try { // try from 005b2ebe to 005b2ec2 has its CatchHandler @ 005b2f36 */
  KisSignalCompressor::KisSignalCompressor(this_00,1000,0,(QObject *)this);
  *(KisSignalCompressor **)(this + 0x10) = this_00;
                    /* try { // try from 005b2ed1 to 005b2f03 has its CatchHandler @ 005b2f2a */
  QObject::thread();
  QObject::moveToThread((QThread *)this);
  QObject::connect(aQStack_28,*(char **)(this + 0x10),(QObject *)"2timeout()",(char *)this,0x737e2c)
  ;
  QMetaObject::Connection::~Connection((Connection *)aQStack_28);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



