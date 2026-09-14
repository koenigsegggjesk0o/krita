/* Class KisImageConfigNotifier - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageConfigNotifier @ 0020d290 ======

void __thiscall KisImageConfigNotifier::KisImageConfigNotifier(KisImageConfigNotifier *this)

{
  (*(code *)PTR_KisImageConfigNotifier_0083e418)();
  return;
}



// ====== KisImageConfigNotifier @ 00504340 ======

/* KisImageConfigNotifier::KisImageConfigNotifier() */

void __thiscall KisImageConfigNotifier::KisImageConfigNotifier(KisImageConfigNotifier *this)

{
  KisSignalCompressor *this_00;
  long in_FS_OFFSET;
  QObject aQStack_38 [8];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_008377e0 + 0x10;
                    /* try { // try from 0050437b to 0050437f has its CatchHandler @ 0050446d */
  this_00 = (KisSignalCompressor *)operator_new(0xd0);
                    /* try { // try from 00504392 to 00504396 has its CatchHandler @ 00504461 */
  KisSignalCompressor::KisSignalCompressor(this_00,300,2,(QObject *)0x0);
                    /* try { // try from 005043a7 to 005043ab has its CatchHandler @ 00504455 */
  KisSignalCompressor::KisSignalCompressor(this_00 + 0x68,300,2,(QObject *)0x0);
  *(KisSignalCompressor **)(this + 0x10) = this_00;
                    /* try { // try from 005043d0 to 00504420 has its CatchHandler @ 00504449 */
  QObject::connect(aQStack_38,(char *)this_00,(QObject *)"2timeout()",(char *)this,0x731729);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QObject::connect(aQStack_38,*(char **)(this + 0x10),(QObject *)"2timeout()",(char *)this,0x72d898)
  ;
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QObject::connect(aQStack_38,(char *)(*(long *)(this + 0x10) + 0x68),(QObject *)"2timeout()",
                   (char *)this,0x72d898);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



