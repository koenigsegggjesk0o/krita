/* Class KisPaintOpPresetUpdateProxy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintOpPresetUpdateProxy @ 0020b3b0 ======

void __thiscall
KisPaintOpPresetUpdateProxy::KisPaintOpPresetUpdateProxy(KisPaintOpPresetUpdateProxy *this)

{
  (*(code *)PTR_KisPaintOpPresetUpdateProxy_0083d4a8)();
  return;
}



// ====== KisPaintOpPresetUpdateProxy @ 0034d080 ======

/* KisPaintOpPresetUpdateProxy::KisPaintOpPresetUpdateProxy() */

void __thiscall
KisPaintOpPresetUpdateProxy::KisPaintOpPresetUpdateProxy(KisPaintOpPresetUpdateProxy *this)

{
  KisSignalCompressor *this_00;
  long in_FS_OFFSET;
  QObject aQStack_28 [8];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837b78 + 0x10;
                    /* try { // try from 0034d0b9 to 0034d0bd has its CatchHandler @ 0034d141 */
  this_00 = (KisSignalCompressor *)operator_new(0x70);
                    /* try { // try from 0034d0d0 to 0034d0d4 has its CatchHandler @ 0034d135 */
  KisSignalCompressor::KisSignalCompressor(this_00,100,2,(QObject *)0x0);
  *(KisSignalCompressor **)(this + 0x10) = this_00;
  *(undefined8 *)(this_00 + 0x68) = 0;
                    /* try { // try from 0034d0fe to 0034d102 has its CatchHandler @ 0034d129 */
  QObject::connect(aQStack_28,(char *)this_00,(QObject *)"2timeout()",(char *)this,0x723830);
  QMetaObject::Connection::~Connection((Connection *)aQStack_28);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



