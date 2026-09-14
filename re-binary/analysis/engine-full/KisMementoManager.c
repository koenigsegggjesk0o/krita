/* Class KisMementoManager - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMementoManager @ 002092a0 ======

void __thiscall KisMementoManager::KisMementoManager(KisMementoManager *this)

{
  (*(code *)PTR_KisMementoManager_0083c420)();
  return;
}



// ====== KisMementoManager @ 002f7290 ======

/* KisMementoManager::KisMementoManager() */

void __thiscall KisMementoManager::KisMementoManager(KisMementoManager *this)

{
  undefined *puVar1;
  
  FUN_002fb880(this,0);
  puVar1 = PTR_shared_null_00837830;
  *(undefined **)(this + 0x68) = PTR_shared_null_00837830;
  *(undefined **)(this + 0x70) = puVar1;
                    /* try { // try from 002f72ba to 002f72be has its CatchHandler @ 002f72d8 */
  FUN_002fb880(this + 0x78,0);
  this[0xf0] = (KisMementoManager)0x0;
  *(undefined (*) [16])(this + 0xe0) = (undefined  [16])0x0;
  return;
}



// ====== KisMementoManager @ 002f72f0 ======

/* KisMementoManager::KisMementoManager(KisMementoManager const&) */

void __thiscall
KisMementoManager::KisMementoManager(KisMementoManager *this,KisMementoManager *param_1)

{
  KisMementoManager KVar1;
  int *piVar2;
  
  FUN_002fdfd0(this,param_1,0);
                    /* try { // try from 002f7316 to 002f731a has its CatchHandler @ 002f7374 */
  FUN_002fb640(this + 0x68,param_1 + 0x68);
                    /* try { // try from 002f7326 to 002f732a has its CatchHandler @ 002f738c */
  FUN_002fb640(this + 0x70,param_1 + 0x70);
                    /* try { // try from 002f7335 to 002f7339 has its CatchHandler @ 002f7380 */
  FUN_002fdfd0(this + 0x78,param_1 + 0x78,0);
  piVar2 = *(int **)(param_1 + 0xe0);
  *(int **)(this + 0xe0) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  KVar1 = param_1[0xf0];
  *(undefined8 *)(this + 0xe8) = 0;
  this[0xf0] = KVar1;
  return;
}



