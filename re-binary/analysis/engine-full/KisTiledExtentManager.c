/* Class KisTiledExtentManager - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTiledExtentManager @ 00205af0 ======

void __thiscall KisTiledExtentManager::KisTiledExtentManager(KisTiledExtentManager *this)

{
  (*(code *)PTR_KisTiledExtentManager_0083a848)();
  return;
}



// ====== KisTiledExtentManager @ 002f4ed0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTiledExtentManager::KisTiledExtentManager() */

void __thiscall KisTiledExtentManager::KisTiledExtentManager(KisTiledExtentManager *this)

{
  undefined8 uVar1;
  
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)this,0);
  uVar1 = DAT_00721778;
  *(undefined8 *)(this + 8) = _DAT_00721770;
  *(undefined8 *)(this + 0x10) = uVar1;
                    /* try { // try from 002f4efb to 002f4eff has its CatchHandler @ 002f4f62 */
  Data::Data((Data *)(this + 0x18));
                    /* try { // try from 002f4f07 to 002f4f0b has its CatchHandler @ 002f4f56 */
  Data::Data((Data *)(this + 0x48));
  if (((ulong)this & 1) == 0) {
                    /* try { // try from 002f4f43 to 002f4f47 has its CatchHandler @ 002f4f4a */
    QReadWriteLock::lockForWrite();
  }
  uVar1 = DAT_00721778;
  *(undefined8 *)(this + 8) = _DAT_00721770;
  *(undefined8 *)(this + 0x10) = uVar1;
  QReadWriteLock::unlock();
  return;
}



