/* Class KisTileDataPooler - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTileDataPooler @ 00207020 ======

void __thiscall
KisTileDataPooler::KisTileDataPooler(KisTileDataPooler *this,KisTileDataStore *param_1,int param_2)

{
  (*(code *)PTR_KisTileDataPooler_0083b2e0)();
  return;
}



// ====== KisTileDataPooler @ 002e58a0 ======

/* KisTileDataPooler::KisTileDataPooler(KisTileDataStore*, int) */

void __thiscall
KisTileDataPooler::KisTileDataPooler(KisTileDataPooler *this,KisTileDataStore *param_1,int param_2)

{
  undefined auVar1 [16];
  int iVar2;
  long in_FS_OFFSET;
  undefined4 local_48 [6];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QThread::QThread((QThread *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837b98 + 0x10;
                    /* try { // try from 002e58e4 to 002e58e8 has its CatchHandler @ 002e598f */
  QSemaphore::QSemaphore((QSemaphore *)(this + 0x10),0);
  *(undefined4 *)(this + 0x18) = 0;
  local_48[0] = 0;
  *(undefined4 *)(this + 0x18) = 0;
  *(KisTileDataStore **)(this + 0x20) = param_1;
  *(undefined4 *)(this + 0x28) = 100;
  this[0x2c] = (KisTileDataPooler)0x0;
  *(undefined8 *)(this + 0x34) = 0;
  *(undefined4 *)(this + 0x3c) = 0;
  if (param_2 < 0) {
                    /* try { // try from 002e594b to 002e594f has its CatchHandler @ 002e599b */
    KisImageConfig::KisImageConfig((KisImageConfig *)local_48,true);
                    /* try { // try from 002e5953 to 002e5957 has its CatchHandler @ 002e59a7 */
    iVar2 = KisImageConfig::poolLimit((KisImageConfig *)local_48);
    auVar1._8_8_ = 0;
    auVar1._0_8_ = (long)(*(int *)PTR_WIDTH_00837418 * *(int *)PTR_HEIGHT_00837120);
    *(int *)(this + 0x30) = iVar2 * SUB164((ZEXT816(0) << 0x40 | ZEXT816(0x100000)) / auVar1,0);
    KisImageConfig::~KisImageConfig((KisImageConfig *)local_48);
  }
  else {
    *(int *)(this + 0x30) = param_2;
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



