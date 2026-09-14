/* Class KisMergeWalker - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMergeWalker @ 00201420 ======

void __thiscall KisMergeWalker::KisMergeWalker(void)

{
  (*(code *)PTR_KisMergeWalker_008384e0)();
  return;
}



// ====== KisMergeWalker @ 0020c670 ======

void __thiscall KisMergeWalker::KisMergeWalker(void)

{
  (*(code *)PTR_KisMergeWalker_0083de08)();
  return;
}



// ====== KisMergeWalker @ 004e42c0 ======

/* KisMergeWalker::KisMergeWalker(QRect, QFlags<KisMergeWalker::Flag>) */

void __thiscall
KisMergeWalker::KisMergeWalker
          (KisMergeWalker *this,long *param_2,long param_3,long param_4,uint param_5)

{
  long lVar1;
  KisMergeWalker *pKVar2;
  
  lVar1 = *(long *)(*param_2 + -0x18);
  *(long *)this = *param_2;
  pKVar2 = this + lVar1;
  *(long *)pKVar2 = param_2[1];
  *(uint *)(this + 8) = param_5;
  *(long *)(pKVar2 + 0x90) = param_3;
  *(long *)(pKVar2 + 0x98) = param_4;
  this[*(long *)(*(long *)this + -0x18) + 0xc4] = (KisMergeWalker)((byte)(param_5 >> 1) & 1);
  return;
}



// ====== KisMergeWalker @ 004e4310 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisMergeWalker::KisMergeWalker(QRect, QFlags<KisMergeWalker::Flag>) */

void __thiscall
KisMergeWalker::KisMergeWalker
          (KisMergeWalker *this,undefined8 param_2,undefined8 param_3,uint param_4)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined *puVar3;
  undefined *puVar4;
  
  KisShared::KisShared((KisShared *)(this + 0x18));
  *(uint *)(this + 8) = param_4;
  *(undefined2 *)(this + 0x68) = 0;
  puVar4 = PTR_shared_null_008377d0;
  puVar3 = PTR_vtable_00836f58;
  uVar2 = DAT_00721778;
  uVar1 = _DAT_00721770;
  *(undefined8 *)(this + 0x80) = 0;
  *(undefined8 *)(this + 0x98) = 0;
  *(undefined **)this = puVar3 + 0x18;
  *(undefined4 *)(this + 0xd0) = 0;
  *(undefined **)(this + 0x10) = puVar3 + 0x78;
  this[0xd4] = (KisMergeWalker)((byte)(param_4 >> 1) & 1);
  *(undefined8 *)(this + 0x28) = uVar1;
  *(undefined8 *)(this + 0x30) = uVar2;
  *(undefined8 *)(this + 0x38) = uVar1;
  *(undefined8 *)(this + 0x40) = uVar2;
  *(undefined8 *)(this + 0x48) = uVar1;
  *(undefined8 *)(this + 0x50) = uVar2;
  *(undefined8 *)(this + 0x58) = uVar1;
  *(undefined8 *)(this + 0x60) = uVar2;
  *(undefined **)(this + 0x70) = puVar4;
  *(undefined **)(this + 0x78) = puVar4;
  *(undefined8 *)(this + 0x88) = uVar1;
  *(undefined8 *)(this + 0x90) = uVar2;
  *(undefined8 *)(this + 0xb0) = uVar1;
  *(undefined8 *)(this + 0xb8) = uVar2;
  *(undefined8 *)(this + 0xc0) = uVar1;
  *(undefined8 *)(this + 200) = uVar2;
  *(undefined8 *)(this + 0xa0) = param_2;
  *(undefined8 *)(this + 0xa8) = param_3;
  return;
}



