/* Class KisTileData - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTileData @ 00206780 ======

void __thiscall KisTileData::KisTileData(KisTileData *this,KisTileData *param_1,bool param_2)

{
  (*(code *)PTR_KisTileData_0083ae90)();
  return;
}



// ====== KisTileData @ 002089c0 ======

void __thiscall
KisTileData::KisTileData
          (KisTileData *this,int param_1,uchar *param_2,KisTileDataStore *param_3,bool param_4)

{
  (*(code *)PTR_KisTileData_0083bfb0)();
  return;
}



// ====== KisTileData @ 002e0800 ======

/* KisTileData::KisTileData(int, unsigned char const*, KisTileDataStore*, bool) */

void __thiscall
KisTileData::KisTileData
          (KisTileData *this,int param_1,uchar *param_2,KisTileDataStore *param_3,bool param_4)

{
  undefined8 uVar1;
  
  *(undefined8 *)(this + 0x10) = 0;
  *(undefined8 *)(this + 0x18) = 0xffffffff00000000;
  *(undefined8 *)(this + 0x20) = 0;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined (*) [16])this = (undefined  [16])0x0;
                    /* try { // try from 002e0856 to 002e085a has its CatchHandler @ 002e08a1 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(this + 0x30),0);
  *(undefined8 *)(this + 0x40) = 0;
  *(int *)(this + 0x48) = param_1;
  *(KisTileDataStore **)(this + 0x50) = param_3;
  if (param_4) {
                    /* try { // try from 002e0874 to 002e0891 has its CatchHandler @ 002e08ad */
    KisTileDataSwapper::checkFreeMemory((KisTileDataSwapper *)(param_3 + 0x40));
    param_1 = *(int *)(this + 0x48);
  }
  uVar1 = allocateData(param_1);
  *(undefined8 *)(this + 0x38) = uVar1;
  fillWithPixel(this,param_2);
  return;
}



// ====== KisTileData @ 002e08c0 ======

/* KisTileData::KisTileData(KisTileData const&, bool) */

void __thiscall KisTileData::KisTileData(KisTileData *this,KisTileData *param_1,bool param_2)

{
  long lVar1;
  void *__src;
  void *__dest;
  int iVar2;
  
  *(undefined8 *)(this + 0x10) = 0;
  *(undefined8 *)(this + 0x18) = 0xffffffff00000000;
  *(undefined8 *)(this + 0x20) = 0;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined (*) [16])this = (undefined  [16])0x0;
                    /* try { // try from 002e090d to 002e0911 has its CatchHandler @ 002e0961 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(this + 0x30),0);
  *(undefined8 *)(this + 0x40) = 0;
  lVar1 = *(long *)(param_1 + 0x50);
  iVar2 = *(int *)(param_1 + 0x48);
  *(long *)(this + 0x50) = lVar1;
  *(int *)(this + 0x48) = iVar2;
  if (param_2) {
                    /* try { // try from 002e0931 to 002e093d has its CatchHandler @ 002e096d */
    KisTileDataSwapper::checkFreeMemory((KisTileDataSwapper *)(lVar1 + 0x40));
    iVar2 = *(int *)(this + 0x48);
  }
  __dest = (void *)allocateData(iVar2);
  __src = *(void **)(param_1 + 0x38);
  *(void **)(this + 0x38) = __dest;
  memcpy(__dest,__src,(long)(*(int *)(this + 0x48) << 0xc));
  return;
}



