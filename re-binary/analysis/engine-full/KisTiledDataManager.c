/* Class KisTiledDataManager - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTiledDataManager @ 00206440 ======

void __thiscall
KisTiledDataManager::KisTiledDataManager(KisTiledDataManager *this,KisTiledDataManager *param_1)

{
  (*(code *)PTR_KisTiledDataManager_0083acf0)();
  return;
}



// ====== KisTiledDataManager @ 00208860 ======

void __thiscall
KisTiledDataManager::KisTiledDataManager(KisTiledDataManager *this,uint param_1,uchar *param_2)

{
  (*(code *)PTR_KisTiledDataManager_0083bf00)();
  return;
}



// ====== KisTiledDataManager @ 002e77f0 ======

/* KisTiledDataManager::KisTiledDataManager(unsigned int, unsigned char const*) */

void __thiscall
KisTiledDataManager::KisTiledDataManager(KisTiledDataManager *this,uint param_1,uchar *param_2)

{
  KisMementoManager *this_00;
  void *pvVar1;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00836ed0 + 0x10;
                    /* try { // try from 002e782d to 002e7831 has its CatchHandler @ 002e78a4 */
  KisTiledExtentManager::KisTiledExtentManager((KisTiledExtentManager *)(this + 0x38));
                    /* try { // try from 002e783e to 002e7842 has its CatchHandler @ 002e78d4 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(this + 0xb0),0);
                    /* try { // try from 002e7848 to 002e784c has its CatchHandler @ 002e78bc */
  this_00 = (KisMementoManager *)operator_new(0xf8);
                    /* try { // try from 002e7853 to 002e7857 has its CatchHandler @ 002e78c8 */
  KisMementoManager::KisMementoManager(this_00);
  *(KisMementoManager **)(this + 0x20) = this_00;
                    /* try { // try from 002e7861 to 002e7865 has its CatchHandler @ 002e78bc */
  pvVar1 = operator_new(0x68);
                    /* try { // try from 002e7870 to 002e7874 has its CatchHandler @ 002e78b0 */
  FUN_002ee470(pvVar1,*(undefined8 *)(this + 0x20));
  *(void **)(this + 0x18) = pvVar1;
  *(uint *)(this + 0x30) = param_1;
                    /* try { // try from 002e787f to 002e7894 has its CatchHandler @ 002e78bc */
  pvVar1 = operator_new__((long)(int)param_1);
  *(void **)(this + 0x28) = pvVar1;
  setDefaultPixel(this,param_2);
  return;
}



// ====== KisTiledDataManager @ 002ebe60 ======

/* KisTiledDataManager::KisTiledDataManager(KisTiledDataManager const&) */

void __thiscall
KisTiledDataManager::KisTiledDataManager(KisTiledDataManager *this,KisTiledDataManager *param_1)

{
  int *piVar1;
  KisTileData *pKVar2;
  int iVar3;
  ulong uVar4;
  KisMementoManager *this_00;
  void *pvVar5;
  void *__dest;
  KisTileData *pKVar6;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00836ed0 + 0x10;
                    /* try { // try from 002ebe99 to 002ebe9d has its CatchHandler @ 002ebfd5 */
  KisTiledExtentManager::KisTiledExtentManager((KisTiledExtentManager *)(this + 0x38));
                    /* try { // try from 002ebeaa to 002ebeae has its CatchHandler @ 002ebfbd */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(this + 0xb0),0);
                    /* try { // try from 002ebeb4 to 002ebeb8 has its CatchHandler @ 002ebfc9 */
  this_00 = (KisMementoManager *)operator_new(0xf8);
                    /* try { // try from 002ebebf to 002ebec3 has its CatchHandler @ 002ebfb1 */
  KisMementoManager::KisMementoManager(this_00);
  *(KisMementoManager **)(this + 0x20) = this_00;
  uVar4 = *(ulong *)(param_1 + 0x18);
  if ((uVar4 & 1) == 0) {
    QReadWriteLock::lockForRead();
    LOCK();
    piVar1 = (int *)(*(long *)(uVar4 + 0x58) + 0x44);
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    pKVar6 = *(KisTileData **)(uVar4 + 0x58);
  }
  else {
    LOCK();
    piVar1 = (int *)(*(long *)(uVar4 + 0x58) + 0x44);
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    pKVar6 = *(KisTileData **)(uVar4 + 0x58);
  }
  QReadWriteLock::unlock();
                    /* try { // try from 002ebf04 to 002ebf1b has its CatchHandler @ 002ebfc9 */
  KisMementoManager::setDefaultTileData(*(KisMementoManager **)(this + 0x20),pKVar6);
  LOCK();
  pKVar2 = pKVar6 + 0x44;
  *(int *)pKVar2 = *(int *)pKVar2 + -1;
  UNLOCK();
  if (*(int *)pKVar2 == 0) {
    KisTileDataStore::freeTileData(*(KisTileDataStore **)(pKVar6 + 0x50),pKVar6);
  }
  pvVar5 = operator_new(0x68);
                    /* try { // try from 002ebf2a to 002ebf2e has its CatchHandler @ 002ebfa5 */
  FUN_002f07a0(pvVar5,*(undefined8 *)(param_1 + 0x18),*(undefined8 *)(this + 0x20));
  iVar3 = *(int *)(param_1 + 0x30);
  *(void **)(this + 0x18) = pvVar5;
  *(int *)(this + 0x30) = iVar3;
                    /* try { // try from 002ebf3a to 002ebf90 has its CatchHandler @ 002ebfc9 */
  __dest = operator_new__((long)iVar3);
  pvVar5 = *(void **)(param_1 + 0x28);
  *(void **)(this + 0x28) = __dest;
  memcpy(__dest,pvVar5,(long)*(int *)(this + 0x30));
  recalculateExtent(this);
  return;
}



