/* Class KisOnionSkinCompositor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisOnionSkinCompositor @ 00207fe0 ======

void __thiscall KisOnionSkinCompositor::KisOnionSkinCompositor(KisOnionSkinCompositor *this)

{
  (*(code *)PTR_KisOnionSkinCompositor_0083bac0)();
  return;
}



// ====== KisOnionSkinCompositor @ 006637c0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisOnionSkinCompositor::KisOnionSkinCompositor() */

void __thiscall KisOnionSkinCompositor::KisOnionSkinCompositor(KisOnionSkinCompositor *this)

{
  long lVar1;
  undefined8 uVar2;
  undefined *puVar3;
  undefined *puVar4;
  byte bVar5;
  byte bVar6;
  int iVar7;
  uint uVar8;
  int *piVar9;
  uint *puVar10;
  long lVar11;
  int iVar12;
  long in_FS_OFFSET;
  double dVar13;
  KisImageConfig local_78 [32];
  int local_58;
  undefined8 local_54;
  undefined2 local_4c;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837bd0 + 0x10;
                    /* try { // try from 00663804 to 00663808 has its CatchHandler @ 00663ae0 */
  piVar9 = (int *)operator_new(0x48);
  piVar9[0] = 0;
  piVar9[1] = 0;
  piVar9[2] = 0;
  puVar4 = PTR_shared_null_008377d0;
  uVar2 = DAT_007338a8;
  *(undefined2 *)(piVar9 + 5) = 0;
  *(undefined8 *)(piVar9 + 3) = uVar2;
  *(undefined8 *)(piVar9 + 7) = uVar2;
  puVar3 = PTR_shared_null_00836c40;
  piVar9[6] = 0;
  *(undefined2 *)(piVar9 + 9) = 0;
  piVar9[0xe] = 0;
  *(undefined **)(piVar9 + 0x10) = puVar3;
  *(int **)(this + 0x10) = piVar9;
  *(undefined **)(piVar9 + 10) = puVar4;
  *(undefined **)(piVar9 + 0xc) = puVar4;
                    /* try { // try from 00663878 to 0066387c has its CatchHandler @ 00663ad4 */
  KisImageConfig::KisImageConfig(local_78,true);
                    /* try { // try from 00663880 to 00663a8b has its CatchHandler @ 00663ac8 */
  iVar7 = KisImageConfig::numberOfOnionSkins(local_78);
  *piVar9 = iVar7;
  iVar7 = KisImageConfig::onionSkinTintFactor(local_78);
  piVar9[1] = iVar7;
  KisImageConfig::onionSkinTintColorBackward();
  piVar9[2] = local_58;
  *(undefined8 *)(piVar9 + 3) = local_54;
  *(undefined2 *)(piVar9 + 5) = local_4c;
  KisImageConfig::onionSkinTintColorForward();
  piVar9[6] = local_58;
  *(undefined8 *)(piVar9 + 7) = local_54;
  *(undefined2 *)(piVar9 + 9) = local_4c;
  FUN_006193b0(piVar9 + 10,*piVar9);
  FUN_006193b0(piVar9 + 0xc);
  bVar5 = KisImageConfig::onionSkinState(local_78,0);
  uVar8 = KisImageConfig::onionSkinOpacity(local_78,0,false);
  lVar11 = 0;
  dVar13 = (double)(int)(-(uint)bVar5 & uVar8) / _DAT_00722cb8;
  if (0 < *piVar9) {
    do {
      uVar8 = ~(uint)lVar11;
      bVar5 = KisImageConfig::onionSkinState(local_78,uVar8);
      iVar12 = (uint)lVar11 + 1;
      bVar6 = KisImageConfig::onionSkinState(local_78,iVar12);
      iVar7 = KisImageConfig::onionSkinOpacity(local_78,uVar8,false);
      puVar10 = *(uint **)(piVar9 + 10);
      if (1 < *puVar10) {
        if ((puVar10[2] & 0x7fffffff) == 0) {
          puVar10 = (uint *)QArrayData::allocate(4,8,0,2);
          *(uint **)(piVar9 + 10) = puVar10;
        }
        else {
          FUN_002edfc0(piVar9 + 10,puVar10[2] & 0x7fffffff,0);
          puVar10 = *(uint **)(piVar9 + 10);
        }
      }
      lVar1 = lVar11 * 4;
      *(int *)((long)puVar10 + *(long *)(puVar10 + 4) + lVar1) =
           (int)((double)iVar7 * (double)bVar5 * dVar13);
      iVar7 = KisImageConfig::onionSkinOpacity(local_78,iVar12,false);
      puVar10 = *(uint **)(piVar9 + 0xc);
      if (1 < *puVar10) {
        if ((puVar10[2] & 0x7fffffff) == 0) {
          puVar10 = (uint *)QArrayData::allocate(4,8,0,2);
          *(uint **)(piVar9 + 0xc) = puVar10;
        }
        else {
          FUN_002edfc0(piVar9 + 0xc,puVar10[2] & 0x7fffffff,0);
          puVar10 = *(uint **)(piVar9 + 0xc);
        }
      }
      lVar11 = lVar11 + 1;
      *(int *)((long)puVar10 + *(long *)(puVar10 + 4) + lVar1) =
           (int)((double)iVar7 * (double)bVar6 * dVar13);
    } while ((int)lVar11 < *piVar9);
  }
  piVar9[0xe] = piVar9[0xe] + 1;
  KisImageConfig::~KisImageConfig(local_78);
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



