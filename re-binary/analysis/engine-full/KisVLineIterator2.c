/* Class KisVLineIterator2 - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisVLineIterator2 @ 0020c580 ======

void __thiscall
KisVLineIterator2::KisVLineIterator2
          (KisVLineIterator2 *this,KisDataManager *param_1,int param_2,int param_3,int param_4,
          int param_5,int param_6,bool param_7,KisIteratorCompleteListener *param_8)

{
  (*(code *)PTR_KisVLineIterator2_0083dd90)();
  return;
}



// ====== KisVLineIterator2 @ 00303270 ======

/* KisVLineIterator2::KisVLineIterator2(KisDataManager*, int, int, int, int, int, bool,
   KisIteratorCompleteListener*) */

void __thiscall
KisVLineIterator2::KisVLineIterator2
          (KisVLineIterator2 *this,KisDataManager *param_1,int param_2,int param_3,int param_4,
          int param_5,int param_6,bool param_7,KisIteratorCompleteListener *param_8)

{
  int iVar1;
  int iVar2;
  undefined8 uVar3;
  undefined *puVar4;
  uint uVar5;
  uint uVar6;
  uint *puVar7;
  undefined4 in_register_00000014;
  ulong uVar8;
  long lVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  undefined3 in_stack_00000011;
  undefined8 in_stack_00000020;
  
  lVar9 = *(long *)(param_1 + 0x10);
  uVar3 = *(undefined8 *)(param_1 + 0x18);
  *(long *)this = lVar9;
  uVar13 = param_4 - _param_7;
  uVar12 = param_3 - param_6;
  *(undefined8 *)(this + *(long *)(lVar9 + -0x18)) = uVar3;
  *(undefined **)(this + 8) = PTR_vtable_00836e80 + 0x10;
  lVar9 = *(long *)(param_1 + 8);
  uVar3 = *(undefined8 *)(param_1 + 0x20);
  *(long *)this = lVar9;
  *(undefined8 *)(this + *(long *)(lVar9 + -0x18)) = uVar3;
  puVar4 = PTR_vtable_00837670;
  *(long *)(this + 0x10) = CONCAT44(in_register_00000014,param_2);
  iVar10 = *(int *)(CONCAT44(in_register_00000014,param_2) + 0x30);
  *(undefined **)(this + 8) = puVar4 + 0x50;
  *(int *)(this + 0x18) = iVar10;
  this[0x1c] = SUB81(param_8,0);
  *(undefined8 *)(this + 0x20) = in_stack_00000020;
  lVar9 = *(long *)param_1;
  uVar3 = *(undefined8 *)(param_1 + 0x28);
  *(long *)this = lVar9;
  *(undefined8 *)(this + *(long *)(lVar9 + -0x18)) = uVar3;
  puVar4 = PTR_vtable_008376e0;
  *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x70) = (undefined  [16])0x0;
  *(undefined **)(this + 8) = puVar4 + 0x90;
  puVar4 = PTR_shared_null_008377d0;
  *(undefined8 *)(this + 0x38) = 0;
  *(undefined **)(this + 0x88) = puVar4;
  puVar4 = PTR_WIDTH_00837418;
  *(undefined4 *)(this + 0x40) = 0;
  iVar1 = *(int *)puVar4;
  *(undefined8 *)(this + 0x58) = 0;
  *(undefined4 *)(this + 0x90) = 0;
  *(uint *)(this + 100) = uVar13;
  *(uint *)(this + 0x6c) = uVar12;
  this[0x60] = (KisVLineIterator2)0x1;
  *(int *)(this + 0x80) = iVar10 * iVar1;
  if (param_5 < 1) {
    param_5 = 1;
  }
  *(ulong *)(this + 0x28) = CONCAT44(_param_7,param_6);
  *(ulong *)(this + 0x30) = CONCAT44(uVar13,uVar12);
  iVar10 = param_5 + uVar13 + -1;
  *(int *)(this + 0x68) = iVar10;
  if (iVar10 < (int)uVar13) {
    this[0x60] = (KisVLineIterator2)0x0;
    return;
  }
  iVar2 = *(int *)PTR_HEIGHT_00837120;
  if ((int)uVar13 < 0) {
    uVar5 = ~((int)~uVar13 / iVar2);
    *(uint *)(this + 0x70) = uVar5;
    if (iVar10 < 0) {
      uVar11 = ~((int)-(param_5 + uVar13) / iVar2);
      *(uint *)(this + 0x74) = uVar11;
      goto joined_r0x003034e8;
    }
  }
  else {
    uVar5 = (int)uVar13 / iVar2;
    *(uint *)(this + 0x70) = uVar5;
  }
  uVar11 = iVar10 / iVar2;
  *(uint *)(this + 0x74) = uVar11;
joined_r0x003034e8:
  if ((int)uVar12 < 0) {
    uVar8 = (long)(int)~uVar12 % (long)iVar1 & 0xffffffff;
    uVar6 = ~((int)~uVar12 / iVar1);
  }
  else {
    uVar6 = (int)uVar12 / iVar1;
    uVar8 = (ulong)(uint)((int)uVar12 % iVar1);
  }
  *(uint *)(this + 0x38) = uVar6;
  iVar10 = (uVar11 - uVar5) + 1;
  *(int *)(this + 0x90) = iVar10;
  *(uint *)(this + 0x7c) = uVar12 - uVar6 * iVar1;
  *(uint *)(this + 0x78) = uVar13 - iVar1 * uVar5;
                    /* try { // try from 00303419 to 00303511 has its CatchHandler @ 0030353b */
  FUN_00303b00(this + 0x88,iVar10,uVar8);
  *(int *)(this + 0x40) = iVar2 * *(int *)(this + 0x80);
  lVar9 = 0;
  if (0 < *(int *)(this + 0x90)) {
    do {
      puVar7 = *(uint **)(this + 0x88);
      iVar10 = *(int *)(this + 0x70);
      iVar1 = *(int *)(this + 0x38);
      if (1 < *puVar7) {
        if ((puVar7[2] & 0x7fffffff) == 0) {
          puVar7 = (uint *)QArrayData::allocate(0x20,8,0,2);
          *(uint **)(this + 0x88) = puVar7;
        }
        else {
          FUN_00303910(this + 0x88,puVar7[2] & 0x7fffffff,0);
          puVar7 = *(uint **)(this + 0x88);
        }
      }
      fetchTileDataForCache
                (this,(KisTileInfo *)((long)puVar7 + lVar9 * 0x20 + *(long *)(puVar7 + 4)),iVar1,
                 iVar10 + (int)lVar9);
      lVar9 = lVar9 + 1;
    } while ((int)lVar9 < *(int *)(this + 0x90));
  }
  *(undefined4 *)(this + 0x3c) = 0;
  switchToTile(this,*(int *)(this + 0x78));
  return;
}



// ====== KisVLineIterator2 @ 00303550 ======

/* KisVLineIterator2::KisVLineIterator2(KisDataManager*, int, int, int, int, int, bool,
   KisIteratorCompleteListener*) */

void __thiscall
KisVLineIterator2::KisVLineIterator2
          (KisVLineIterator2 *this,KisDataManager *param_1,int param_2,int param_3,int param_4,
          int param_5,int param_6,bool param_7,KisIteratorCompleteListener *param_8)

{
  undefined *puVar1;
  undefined *puVar2;
  int iVar3;
  int iVar4;
  undefined *puVar5;
  uint uVar6;
  uint uVar7;
  uint *puVar8;
  ulong uVar9;
  long lVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  
  puVar5 = PTR_vtable_008376e0;
  uVar14 = param_2 - param_5;
  puVar2 = PTR_vtable_008376e0 + 0x90;
  puVar1 = PTR_vtable_008376e0 + 0x18;
  KisShared::KisShared((KisShared *)(this + 0xa0));
  uVar13 = param_3 - param_6;
  *(KisDataManager **)(this + 0x10) = param_1;
  iVar12 = *(int *)(param_1 + 0x30);
  *(undefined **)this = puVar1;
  *(undefined **)(this + 8) = puVar2;
  *(undefined **)(this + 0x98) = puVar5 + 0xf8;
  puVar1 = PTR_shared_null_008377d0;
  *(int *)(this + 0x18) = iVar12;
  *(undefined **)(this + 0x88) = puVar1;
  puVar1 = PTR_WIDTH_00837418;
  *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
  iVar3 = *(int *)puVar1;
  *(undefined (*) [16])(this + 0x70) = (undefined  [16])0x0;
  this[0x1c] = (KisVLineIterator2)param_7;
  *(undefined8 *)(this + 0x38) = 0;
  *(KisIteratorCompleteListener **)(this + 0x20) = param_8;
  *(undefined4 *)(this + 0x40) = 0;
  *(int *)(this + 0x80) = iVar12 * iVar3;
  if (param_4 < 1) {
    param_4 = 1;
  }
  *(undefined8 *)(this + 0x58) = 0;
  *(undefined4 *)(this + 0x90) = 0;
  *(uint *)(this + 100) = uVar13;
  iVar12 = param_4 + uVar13 + -1;
  *(uint *)(this + 0x6c) = uVar14;
  *(int *)(this + 0x68) = iVar12;
  this[0x60] = (KisVLineIterator2)0x1;
  *(ulong *)(this + 0x28) = CONCAT44(param_6,param_5);
  *(ulong *)(this + 0x30) = CONCAT44(uVar13,uVar14);
  if (iVar12 < (int)uVar13) {
    this[0x60] = (KisVLineIterator2)0x0;
    return;
  }
  iVar4 = *(int *)PTR_HEIGHT_00837120;
  if ((int)uVar13 < 0) {
    uVar6 = ~((int)~uVar13 / iVar4);
    *(uint *)(this + 0x70) = uVar6;
    if (iVar12 < 0) {
      uVar11 = ~((int)-(param_4 + uVar13) / iVar4);
      *(uint *)(this + 0x74) = uVar11;
      goto joined_r0x003037cb;
    }
  }
  else {
    uVar6 = (int)uVar13 / iVar4;
    *(uint *)(this + 0x70) = uVar6;
  }
  uVar11 = iVar12 / iVar4;
  *(uint *)(this + 0x74) = uVar11;
joined_r0x003037cb:
  if ((int)uVar14 < 0) {
    uVar9 = (long)(int)~uVar14 % (long)iVar3 & 0xffffffff;
    uVar7 = ~((int)~uVar14 / iVar3);
  }
  else {
    uVar7 = (int)uVar14 / iVar3;
    uVar9 = (ulong)(uint)((int)uVar14 % iVar3);
  }
  *(uint *)(this + 0x38) = uVar7;
  iVar12 = (uVar11 - uVar6) + 1;
  *(int *)(this + 0x90) = iVar12;
  *(uint *)(this + 0x7c) = uVar14 - uVar7 * iVar3;
  *(uint *)(this + 0x78) = uVar13 - iVar3 * uVar6;
                    /* try { // try from 003036f8 to 003037f1 has its CatchHandler @ 0030381b */
  FUN_00303b00(this + 0x88,iVar12,uVar9);
  lVar10 = 0;
  *(int *)(this + 0x40) = iVar4 * *(int *)(this + 0x80);
  if (0 < *(int *)(this + 0x90)) {
    do {
      puVar8 = *(uint **)(this + 0x88);
      iVar12 = *(int *)(this + 0x70);
      iVar3 = *(int *)(this + 0x38);
      if (1 < *puVar8) {
        if ((puVar8[2] & 0x7fffffff) == 0) {
          puVar8 = (uint *)QArrayData::allocate(0x20,8,0,2);
          *(uint **)(this + 0x88) = puVar8;
        }
        else {
          FUN_00303910(this + 0x88,puVar8[2] & 0x7fffffff,0);
          puVar8 = *(uint **)(this + 0x88);
        }
      }
      fetchTileDataForCache
                (this,(KisTileInfo *)((long)puVar8 + lVar10 * 0x20 + *(long *)(puVar8 + 4)),iVar3,
                 iVar12 + (int)lVar10);
      lVar10 = lVar10 + 1;
    } while ((int)lVar10 < *(int *)(this + 0x90));
  }
  *(undefined4 *)(this + 0x3c) = 0;
  switchToTile(this,*(int *)(this + 0x78));
  return;
}



