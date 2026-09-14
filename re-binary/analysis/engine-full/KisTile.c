/* Class KisTile - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTile @ 002035d0 ======

void __thiscall
KisTile::KisTile(KisTile *this,int param_1,int param_2,KisTileData *param_3,
                KisMementoManager *param_4)

{
  (*(code *)PTR_KisTile_008395b8)();
  return;
}



// ====== KisTile @ 00209d30 ======

void __thiscall KisTile::KisTile(KisTile *this,KisTile *param_1,KisMementoManager *param_2)

{
  (*(code *)PTR_KisTile_0083c968)();
  return;
}



// ====== KisTile @ 002ded60 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTile::KisTile(int, int, KisTileData*, KisMementoManager*) */

void __thiscall
KisTile::KisTile(KisTile *this,int param_1,int param_2,KisTileData *param_3,
                KisMementoManager *param_4)

{
  undefined8 uVar1;
  undefined8 uVar2;
  
  KisShared::KisShared((KisShared *)this);
  uVar2 = DAT_00721778;
  uVar1 = _DAT_00721770;
  *(undefined **)(this + 0x18) = PTR_shared_null_008377d0;
  *(undefined8 *)(this + 0x2c) = uVar1;
  *(undefined8 *)(this + 0x34) = uVar2;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x50) = (undefined  [16])0x0;
                    /* try { // try from 002dedb0 to 002dedb4 has its CatchHandler @ 002dedbe */
  init(this,param_1,param_2,param_3,param_4);
  return;
}



// ====== KisTile @ 002dedd0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTile::KisTile(KisTile const&, int, int, KisMementoManager*) */

void __thiscall
KisTile::KisTile(KisTile *this,KisTile *param_1,int param_2,int param_3,KisMementoManager *param_4)

{
  undefined8 uVar1;
  undefined *puVar2;
  
  KisShared::KisShared((KisShared *)this);
  puVar2 = PTR_shared_null_008377d0;
  uVar1 = DAT_00721778;
  *(undefined8 *)(this + 0x2c) = _DAT_00721770;
  *(undefined8 *)(this + 0x34) = uVar1;
  *(undefined **)(this + 0x18) = puVar2;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x50) = (undefined  [16])0x0;
                    /* try { // try from 002dee21 to 002dee25 has its CatchHandler @ 002dee2f */
  init(this,param_2,param_3,*(KisTileData **)(param_1 + 0x10),param_4);
  return;
}



// ====== KisTile @ 002dee40 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTile::KisTile(KisTile const&, KisMementoManager*) */

void __thiscall KisTile::KisTile(KisTile *this,KisTile *param_1,KisMementoManager *param_2)

{
  undefined8 uVar1;
  undefined *puVar2;
  
  KisShared::KisShared((KisShared *)this);
  puVar2 = PTR_shared_null_008377d0;
  uVar1 = DAT_00721778;
  *(undefined8 *)(this + 0x2c) = _DAT_00721770;
  *(undefined8 *)(this + 0x34) = uVar1;
  *(undefined **)(this + 0x18) = puVar2;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x50) = (undefined  [16])0x0;
                    /* try { // try from 002dee89 to 002dee8d has its CatchHandler @ 002dee93 */
  init(this,*(int *)(param_1 + 0x24),*(int *)(param_1 + 0x28),*(KisTileData **)(param_1 + 0x10),
       param_2);
  return;
}



// ====== KisTile @ 002deea0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTile::KisTile(KisTile const&) */

void __thiscall KisTile::KisTile(KisTile *this,KisTile *param_1)

{
  undefined8 uVar1;
  undefined8 uVar2;
  
  KisShared::KisShared((KisShared *)this);
  uVar2 = DAT_00721778;
  uVar1 = _DAT_00721770;
  *(undefined **)(this + 0x18) = PTR_shared_null_008377d0;
  *(undefined8 *)(this + 0x2c) = uVar1;
  *(undefined8 *)(this + 0x34) = uVar2;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x50) = (undefined  [16])0x0;
                    /* try { // try from 002deee9 to 002deeed has its CatchHandler @ 002deef5 */
  init(this,*(int *)(param_1 + 0x24),*(int *)(param_1 + 0x28),*(KisTileData **)(param_1 + 0x10),
       *(KisMementoManager **)(param_1 + 0x48));
  return;
}



