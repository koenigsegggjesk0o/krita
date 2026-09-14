/* Class KisTileDataSwapper - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTileDataSwapper @ 00201af0 ======

void __thiscall
KisTileDataSwapper::KisTileDataSwapper(KisTileDataSwapper *this,KisTileDataStore *param_1)

{
  (*(code *)PTR_KisTileDataSwapper_00838848)();
  return;
}



// ====== KisTileDataSwapper @ 00308ff0 ======

/* KisTileDataSwapper::KisTileDataSwapper(KisTileDataStore*) */

void __thiscall
KisTileDataSwapper::KisTileDataSwapper(KisTileDataSwapper *this,KisTileDataStore *param_1)

{
  undefined auVar1 [16];
  int iVar2;
  int iVar3;
  QSemaphore *this_00;
  int iVar4;
  long in_FS_OFFSET;
  undefined4 local_48 [6];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QThread::QThread((QThread *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837090 + 0x10;
                    /* try { // try from 00309031 to 00309035 has its CatchHandler @ 00309162 */
  this_00 = (QSemaphore *)operator_new(0x38);
  *(undefined8 *)(this_00 + 0x30) = 0;
  *(undefined (*) [16])this_00 = (undefined  [16])0x0;
  *(undefined (*) [16])(this_00 + 0x10) = (undefined  [16])0x0;
  *(undefined (*) [16])(this_00 + 0x20) = (undefined  [16])0x0;
                    /* try { // try from 00309055 to 00309059 has its CatchHandler @ 00309156 */
  QSemaphore::QSemaphore(this_00,0);
  *(undefined4 *)(this_00 + 8) = 0;
                    /* try { // try from 0030906c to 00309070 has its CatchHandler @ 0030914a */
  KisImageConfig::KisImageConfig((KisImageConfig *)local_48,true);
                    /* try { // try from 00309074 to 003090cc has its CatchHandler @ 0030913e */
  iVar2 = KisImageConfig::tilesHardLimit((KisImageConfig *)local_48);
  auVar1._8_8_ = 0;
  auVar1._0_8_ = (long)(*(int *)PTR_WIDTH_00837418 * *(int *)PTR_HEIGHT_00837120);
  iVar4 = SUB164((ZEXT816(0) << 0x40 | ZEXT816(0x100000)) / auVar1,0);
  iVar2 = iVar2 * iVar4;
  *(int *)(this_00 + 0x18) = iVar2;
  iVar3 = iVar2 + 7;
  if (-1 < iVar2) {
    iVar3 = iVar2;
  }
  iVar2 = iVar2 - (iVar3 >> 3);
  *(int *)(this_00 + 0x1c) = iVar2;
  iVar3 = iVar2 + 7;
  if (-1 < iVar2) {
    iVar3 = iVar2;
  }
  *(int *)(this_00 + 0x20) = iVar2 - (iVar3 >> 3);
  iVar3 = KisImageConfig::tilesSoftLimit((KisImageConfig *)local_48);
  iVar3 = iVar3 * iVar4;
  if (*(int *)(this_00 + 0x1c) < iVar3) {
    iVar3 = *(int *)(this_00 + 0x1c);
  }
  if (iVar3 < 0) {
    iVar3 = 0;
  }
  *(int *)(this_00 + 0x24) = iVar3;
  iVar2 = iVar3 + 7;
  if (-1 < iVar3) {
    iVar2 = iVar3;
  }
  *(int *)(this_00 + 0x28) = iVar3 - (iVar2 >> 3);
  KisImageConfig::~KisImageConfig((KisImageConfig *)local_48);
  *(QSemaphore **)(this + 0x10) = this_00;
  *(undefined8 *)(this_00 + 0x30) = 0;
  local_48[0] = 0;
  *(undefined4 *)(this_00 + 8) = 0;
  *(KisTileDataStore **)(*(long *)(this + 0x10) + 0x10) = param_1;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



