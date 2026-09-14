/* Class KisTileDataStore - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTileDataStore @ 00205150 ======

void __thiscall KisTileDataStore::KisTileDataStore(KisTileDataStore *this)

{
  (*(code *)PTR_KisTileDataStore_0083a378)();
  return;
}



// ====== KisTileDataStore @ 002e2cd0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTileDataStore::KisTileDataStore() */

void __thiscall KisTileDataStore::KisTileDataStore(KisTileDataStore *this)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  undefined8 *puVar5;
  undefined8 *puVar6;
  
  KisTileDataPooler::KisTileDataPooler((KisTileDataPooler *)this,this,-1);
                    /* try { // try from 002e2cf6 to 002e2cfa has its CatchHandler @ 002e2e01 */
  KisTileDataSwapper::KisTileDataSwapper((KisTileDataSwapper *)(this + 0x40),this);
                    /* try { // try from 002e2d02 to 002e2d06 has its CatchHandler @ 002e2e31 */
  KisSwappedDataStore::KisSwappedDataStore((KisSwappedDataStore *)(this + 0x58));
  uVar1 = _UNK_00721bf8;
  *(undefined8 *)(this + 0x88) = _DAT_00721bf0;
  *(undefined8 *)(this + 0x90) = uVar1;
  puVar2 = (undefined8 *)malloc(0xb8);
  *puVar2 = 7;
  puVar2[1] = 0;
  puVar2[2] = 0;
  puVar2[3] = 0;
                    /* try { // try from 002e2d46 to 002e2d4a has its CatchHandler @ 002e2e19 */
  QWaitCondition::QWaitCondition((QWaitCondition *)(puVar2 + 4));
  puVar5 = puVar2 + 0xe;
  do {
    puVar3 = puVar5 + -8;
    puVar6 = puVar5 + -9;
    do {
      *(undefined *)puVar6 = 0;
      puVar4 = puVar3 + 2;
      *(undefined *)((long)puVar6 + 4) = 0;
      *(undefined4 *)puVar3 = 0;
      puVar3[1] = 0;
      puVar3 = puVar4;
      puVar6 = (undefined8 *)((long)puVar6 + 1);
    } while (puVar5 != puVar4);
    puVar5 = puVar5 + 9;
  } while (puVar5 != puVar2 + 0x20);
  *(undefined8 **)(this + 0x98) = puVar2;
  *(undefined4 *)(this + 0xa0) = 0;
  *(undefined8 *)(this + 0xb8) = 0;
  *(undefined8 *)(this + 0xd0) = 0;
  *(undefined (*) [16])(this + 0xa8) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0xc0) = (undefined  [16])0x0;
                    /* try { // try from 002e2dd9 to 002e2ddd has its CatchHandler @ 002e2e25 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(this + 0xd8),0);
                    /* try { // try from 002e2de6 to 002e2df7 has its CatchHandler @ 002e2e0d */
  QThread::start((Priority)this);
  QThread::start((Priority)(KisTileDataSwapper *)(this + 0x40));
  return;
}



