/* Class KisHLineIterator2 - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisHLineIterator2 @ 00206710 ======

void __thiscall
KisHLineIterator2::KisHLineIterator2
          (KisHLineIterator2 *this,KisDataManager *param_1,int param_2,int param_3,int param_4,
          int param_5,int param_6,bool param_7,KisIteratorCompleteListener *param_8)

{
  (*(code *)PTR_KisHLineIterator2_0083ae58)();
  return;
}



// ====== KisHLineIterator2 @ 003015f0 ======

/* KisHLineIterator2::KisHLineIterator2(KisDataManager*, int, int, int, int, int, bool,
   KisIteratorCompleteListener*) */

void __thiscall
KisHLineIterator2::KisHLineIterator2
          (KisHLineIterator2 *this,KisDataManager *param_1,int param_2,int param_3,int param_4,
          int param_5,int param_6,bool param_7,KisIteratorCompleteListener *param_8)

{
  undefined4 uVar1;
  int iVar2;
  long lVar3;
  undefined8 uVar4;
  ulong uVar5;
  undefined *puVar6;
  uint uVar7;
  uint uVar8;
  uint *puVar9;
  uint uVar10;
  undefined4 in_register_00000014;
  int iVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  undefined3 in_stack_00000011;
  undefined8 in_stack_00000020;
  
  lVar3 = *(long *)(param_1 + 0x10);
  uVar4 = *(undefined8 *)(param_1 + 0x18);
  *(long *)this = lVar3;
  uVar10 = param_3 - param_6;
  uVar14 = param_4 - _param_7;
  *(undefined8 *)(this + *(long *)(lVar3 + -0x18)) = uVar4;
  *(undefined **)(this + 8) = PTR_vtable_00836e80 + 0x10;
  lVar3 = *(long *)(param_1 + 8);
  uVar4 = *(undefined8 *)(param_1 + 0x20);
  *(long *)this = lVar3;
  *(undefined8 *)(this + *(long *)(lVar3 + -0x18)) = uVar4;
  puVar6 = PTR_vtable_00837108;
  *(long *)(this + 0x10) = CONCAT44(in_register_00000014,param_2);
  uVar1 = *(undefined4 *)(CONCAT44(in_register_00000014,param_2) + 0x30);
  *(undefined **)(this + 8) = puVar6 + 0x50;
  *(undefined4 *)(this + 0x18) = uVar1;
  this[0x1c] = SUB81(param_8,0);
  *(undefined8 *)(this + 0x20) = in_stack_00000020;
  lVar3 = *(long *)param_1;
  uVar4 = *(undefined8 *)(param_1 + 0x28);
  *(long *)this = lVar3;
  *(undefined8 *)(this + *(long *)(lVar3 + -0x18)) = uVar4;
  puVar6 = PTR_vtable_00836be0;
  *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x68) = (undefined  [16])0x0;
  *(undefined **)(this + 8) = puVar6 + 0x90;
  puVar6 = PTR_shared_null_008377d0;
  *(undefined4 *)(this + 0x38) = 0;
  *(undefined **)(this + 0x80) = puVar6;
  if (param_5 < 1) {
    param_5 = 1;
  }
  *(uint *)(this + 0x60) = uVar10;
  *(undefined8 *)(this + 0x3c) = 0;
  *(undefined4 *)(this + 0x78) = 0;
  iVar11 = param_5 + uVar10 + -1;
  *(uint *)(this + 100) = uVar14;
  *(undefined4 *)(this + 0x88) = 0;
  *(int *)(this + 0x5c) = iVar11;
  this[0x58] = (KisHLineIterator2)0x1;
  *(ulong *)(this + 0x28) = CONCAT44(_param_7,param_6);
  *(ulong *)(this + 0x30) = CONCAT44(uVar14,uVar10);
  if (iVar11 < (int)uVar10) {
    this[0x58] = (KisHLineIterator2)0x0;
    return;
  }
  iVar2 = *(int *)PTR_WIDTH_00837418;
  if ((int)uVar10 < 0) {
    uVar7 = ~((int)~uVar10 / iVar2);
    *(uint *)(this + 0x68) = uVar7;
    if (iVar11 < 0) {
      uVar12 = ~((int)-(param_5 + uVar10) / iVar2);
      goto LAB_00301746;
    }
  }
  else {
    uVar7 = (int)uVar10 / iVar2;
    *(uint *)(this + 0x68) = uVar7;
  }
  uVar12 = iVar11 / iVar2;
LAB_00301746:
  puVar6 = PTR_HEIGHT_00837120;
  *(uint *)(this + 0x6c) = uVar12;
  iVar11 = *(int *)puVar6;
  if ((int)uVar14 < 0) {
    uVar5 = (long)(int)~uVar14 % (long)iVar11;
    uVar8 = ~((int)~uVar14 / iVar11);
  }
  else {
    uVar8 = (int)uVar14 / iVar11;
    uVar5 = (long)(int)uVar14 % (long)iVar11;
  }
  *(uint *)(this + 0x38) = uVar8;
  iVar13 = (uVar12 - uVar7) + 1;
  *(int *)(this + 0x88) = iVar13;
  *(uint *)(this + 0x74) = uVar10 - iVar2 * uVar7;
  *(uint *)(this + 0x78) = uVar14 - uVar8 * iVar11;
                    /* try { // try from 0030178d to 00301871 has its CatchHandler @ 003018ac */
  FUN_00301e90(this + 0x80,iVar13,uVar5 & 0xffffffff);
  *(int *)(this + 0x40) = iVar11 * *(int *)(this + 0x18);
  uVar10 = 0;
  if (*(int *)(this + 0x88) != 0) {
    do {
      puVar9 = *(uint **)(this + 0x80);
      iVar11 = *(int *)(this + 0x68);
      iVar2 = *(int *)(this + 0x38);
      if (1 < *puVar9) {
        if ((puVar9[2] & 0x7fffffff) == 0) {
          puVar9 = (uint *)QArrayData::allocate(0x20,8,0,2);
          *(uint **)(this + 0x80) = puVar9;
        }
        else {
          FUN_00301ca0(this + 0x80,puVar9[2] & 0x7fffffff,0);
          puVar9 = *(uint **)(this + 0x80);
        }
      }
      fetchTileDataForCache
                (this,(KisTileInfo *)
                      ((long)puVar9 + (long)(int)uVar10 * 0x20 + *(long *)(puVar9 + 4)),
                 iVar11 + uVar10,iVar2);
      uVar10 = uVar10 + 1;
    } while (uVar10 < *(uint *)(this + 0x88));
  }
  *(undefined4 *)(this + 0x3c) = 0;
  switchToTile(this,*(int *)(this + 0x74));
  return;
}



// ====== KisHLineIterator2 @ 003018c0 ======

/* KisHLineIterator2::KisHLineIterator2(KisDataManager*, int, int, int, int, int, bool,
   KisIteratorCompleteListener*) */

void __thiscall
KisHLineIterator2::KisHLineIterator2
          (KisHLineIterator2 *this,KisDataManager *param_1,int param_2,int param_3,int param_4,
          int param_5,int param_6,bool param_7,KisIteratorCompleteListener *param_8)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  ulong uVar6;
  undefined *puVar7;
  uint uVar8;
  uint uVar9;
  uint *puVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  
  puVar7 = PTR_vtable_00836be0;
  uVar14 = param_2 - param_5;
  puVar2 = PTR_vtable_00836be0 + 0x90;
  puVar1 = PTR_vtable_00836be0 + 0x18;
  KisShared::KisShared((KisShared *)(this + 0x98));
  uVar13 = param_3 - param_6;
  *(KisDataManager **)(this + 0x10) = param_1;
  uVar3 = *(undefined4 *)(param_1 + 0x30);
  *(undefined **)this = puVar1;
  *(undefined **)(this + 8) = puVar2;
  *(undefined4 *)(this + 0x18) = uVar3;
  *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
  *(KisIteratorCompleteListener **)(this + 0x20) = param_8;
  *(undefined **)(this + 0x90) = puVar7 + 0xf8;
  puVar1 = PTR_shared_null_008377d0;
  *(undefined (*) [16])(this + 0x68) = (undefined  [16])0x0;
  *(undefined **)(this + 0x80) = puVar1;
  if (param_4 < 1) {
    param_4 = 1;
  }
  this[0x1c] = (KisHLineIterator2)param_7;
  *(undefined4 *)(this + 0x38) = 0;
  *(uint *)(this + 0x60) = uVar14;
  iVar5 = param_4 + uVar14 + -1;
  *(uint *)(this + 100) = uVar13;
  *(undefined8 *)(this + 0x3c) = 0;
  *(undefined4 *)(this + 0x78) = 0;
  *(undefined4 *)(this + 0x88) = 0;
  *(int *)(this + 0x5c) = iVar5;
  this[0x58] = (KisHLineIterator2)0x1;
  *(ulong *)(this + 0x28) = CONCAT44(param_6,param_5);
  *(ulong *)(this + 0x30) = CONCAT44(uVar13,uVar14);
  if (iVar5 < (int)uVar14) {
    this[0x58] = (KisHLineIterator2)0x0;
    return;
  }
  iVar4 = *(int *)PTR_WIDTH_00837418;
  if ((int)uVar14 < 0) {
    uVar8 = ~((int)~uVar14 / iVar4);
    *(uint *)(this + 0x68) = uVar8;
    if (iVar5 < 0) {
      uVar11 = ~((int)-(param_4 + uVar14) / iVar4);
      goto LAB_00301a0e;
    }
  }
  else {
    uVar8 = (int)uVar14 / iVar4;
    *(uint *)(this + 0x68) = uVar8;
  }
  uVar11 = iVar5 / iVar4;
LAB_00301a0e:
  puVar1 = PTR_HEIGHT_00837120;
  *(uint *)(this + 0x6c) = uVar11;
  iVar5 = *(int *)puVar1;
  if ((int)uVar13 < 0) {
    uVar6 = (long)(int)~uVar13 % (long)iVar5;
    uVar9 = ~((int)~uVar13 / iVar5);
  }
  else {
    uVar9 = (int)uVar13 / iVar5;
    uVar6 = (long)(int)uVar13 % (long)iVar5;
  }
  *(uint *)(this + 0x38) = uVar9;
  iVar12 = (uVar11 - uVar8) + 1;
  *(int *)(this + 0x88) = iVar12;
  *(uint *)(this + 0x74) = uVar14 - iVar4 * uVar8;
  *(uint *)(this + 0x78) = uVar13 - uVar9 * iVar5;
                    /* try { // try from 00301a5a to 00301b41 has its CatchHandler @ 00301b7c */
  FUN_00301e90(this + 0x80,iVar12,uVar6 & 0xffffffff);
  *(int *)(this + 0x40) = iVar5 * *(int *)(this + 0x18);
  uVar13 = 0;
  if (*(int *)(this + 0x88) != 0) {
    do {
      puVar10 = *(uint **)(this + 0x80);
      iVar5 = *(int *)(this + 0x68);
      iVar4 = *(int *)(this + 0x38);
      if (1 < *puVar10) {
        if ((puVar10[2] & 0x7fffffff) == 0) {
          puVar10 = (uint *)QArrayData::allocate(0x20,8,0,2);
          *(uint **)(this + 0x80) = puVar10;
        }
        else {
          FUN_00301ca0(this + 0x80,puVar10[2] & 0x7fffffff,0);
          puVar10 = *(uint **)(this + 0x80);
        }
      }
      fetchTileDataForCache
                (this,(KisTileInfo *)
                      ((long)puVar10 + (long)(int)uVar13 * 0x20 + *(long *)(puVar10 + 4)),
                 iVar5 + uVar13,iVar4);
      uVar13 = uVar13 + 1;
    } while (uVar13 < *(uint *)(this + 0x88));
  }
  *(undefined4 *)(this + 0x3c) = 0;
  switchToTile(this,*(int *)(this + 0x74));
  return;
}



