/* Class KisRasterKeyframeChannel - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisRasterKeyframeChannel @ 00207a90 ======

void __thiscall
KisRasterKeyframeChannel::KisRasterKeyframeChannel
          (KisRasterKeyframeChannel *this,KisRasterKeyframeChannel *param_1,KisWeakSharedPtr param_2
          )

{
  (*(code *)PTR_KisRasterKeyframeChannel_0083b818)();
  return;
}



// ====== KisRasterKeyframeChannel @ 00208260 ======

void __thiscall
KisRasterKeyframeChannel::KisRasterKeyframeChannel
          (KisRasterKeyframeChannel *this,KoID *param_1,KisWeakSharedPtr param_2,
          KisSharedPtr param_3)

{
  (*(code *)PTR_KisRasterKeyframeChannel_0083bc00)();
  return;
}



// ====== KisRasterKeyframeChannel @ 00659d60 ======

/* KisRasterKeyframeChannel::KisRasterKeyframeChannel(KoID const&, KisWeakSharedPtr<KisPaintDevice>,
   KisSharedPtr<KisDefaultBoundsBase>) */

void __thiscall
KisRasterKeyframeChannel::KisRasterKeyframeChannel
          (KisRasterKeyframeChannel *this,KoID *param_1,KisWeakSharedPtr param_2,
          KisSharedPtr param_3)

{
  long *plVar1;
  uint uVar2;
  long lVar3;
  undefined auVar4 [8];
  undefined auVar5 [16];
  undefined *puVar6;
  undefined *puVar7;
  undefined *puVar8;
  undefined (*pauVar9) [16];
  uint *puVar10;
  int *piVar11;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000014;
  long *plVar12;
  long in_FS_OFFSET;
  undefined local_48 [8];
  uint *puStack_40;
  long local_30;
  
  plVar12 = (long *)CONCAT44(in_register_00000014,param_2);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  lVar3 = *(long *)CONCAT44(in_register_0000000c,param_3);
  puStack_40 = (uint *)local_48;
  local_48 = (undefined  [8])lVar3;
  if (lVar3 != 0) {
    LOCK();
    *(int *)(lVar3 + 8) = *(int *)(lVar3 + 8) + 1;
    UNLOCK();
  }
                    /* try { // try from 00659da1 to 00659da5 has its CatchHandler @ 0065a000 */
  KisKeyframeChannel::KisKeyframeChannel((KisKeyframeChannel *)this,param_1,(KisSharedPtr)local_48);
  if (local_48 != (undefined  [8])0x0) {
    LOCK();
    plVar1 = (long *)((long)local_48 + 8);
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*(long *)local_48 + 8))();
    }
  }
  *(undefined **)this = PTR_vtable_008373a8 + 0x10;
                    /* try { // try from 00659dcf to 00659dd3 has its CatchHandler @ 0065a00c */
  pauVar9 = (undefined (*) [16])operator_new(0x30);
  puVar8 = PTR_shared_null_008377d0;
  if (*plVar12 == 0) {
LAB_00659f81:
    puStack_40 = (uint *)0x0;
LAB_00659e79:
    *(undefined8 *)*pauVar9 = 0;
LAB_00659e80:
    *(undefined8 *)(*pauVar9 + 8) = 0;
  }
  else {
    if (((uint *)plVar12[1] == (uint *)0x0) || ((*(uint *)plVar12[1] & 1) == 0)) {
      puStack_40 = (uint *)0x0;
      goto LAB_00659e79;
    }
    auVar4 = (undefined  [8])*plVar12;
    local_48 = auVar4;
    if (auVar4 == (undefined  [8])0x0) goto LAB_00659f81;
    puVar10 = *(uint **)((long)auVar4 + 0x18);
    if (puVar10 == (uint *)0x0) {
                    /* try { // try from 00659fdd to 00659fe1 has its CatchHandler @ 0065a024 */
      piVar11 = (int *)operator_new(4);
      *piVar11 = 0;
      *(int **)((long)auVar4 + 0x18) = piVar11;
      LOCK();
      *piVar11 = *piVar11 + 1;
      UNLOCK();
      puVar10 = *(uint **)((long)auVar4 + 0x18);
      auVar4 = local_48;
    }
    local_48 = auVar4;
    auVar4 = local_48;
    LOCK();
    *puVar10 = *puVar10 + 2;
    UNLOCK();
    puStack_40 = puVar10;
    if (local_48 == (undefined  [8])0x0) goto LAB_00659e79;
    if ((puVar10 != (uint *)0x0) && ((*puVar10 & 1) != 0)) {
      *(undefined (*) [8])*pauVar9 = local_48;
      if (local_48 != (undefined  [8])0x0) {
        piVar11 = *(int **)((long)local_48 + 0x18);
        if (piVar11 == (int *)0x0) {
                    /* try { // try from 00659fb5 to 00659fb9 has its CatchHandler @ 0065a018 */
          piVar11 = (int *)operator_new(4);
          *piVar11 = 0;
          *(int **)((long)auVar4 + 0x18) = piVar11;
          LOCK();
          *piVar11 = *piVar11 + 1;
          UNLOCK();
          piVar11 = *(int **)((long)auVar4 + 0x18);
        }
        *(int **)(*pauVar9 + 8) = piVar11;
        LOCK();
        *piVar11 = *piVar11 + 2;
        UNLOCK();
        goto LAB_00659e88;
      }
      goto LAB_00659e80;
    }
    *pauVar9 = (undefined  [16])0x0;
  }
LAB_00659e88:
  puVar10 = puStack_40;
  puVar6 = PTR_shared_null_00836c40;
  *(undefined **)pauVar9[2] = puVar8;
  puVar7 = PTR_shared_null_008372c0;
  *(undefined **)pauVar9[1] = puVar6;
  *(undefined **)(pauVar9[1] + 8) = puVar7;
  if (1 < *(int *)puVar8 + 1U) {
    LOCK();
    *(int *)puVar8 = *(int *)puVar8 + 1;
    UNLOCK();
  }
  pauVar9[2][8] = 0;
  *(undefined (**) [16])(this + 0x18) = pauVar9;
  auVar5._8_8_ = 0;
  auVar5._0_8_ = puStack_40;
  _local_48 = auVar5 << 0x40;
  if (puVar10 != (uint *)0x0) {
    LOCK();
    uVar2 = *puVar10;
    *puVar10 = *puVar10 - 2;
    UNLOCK();
    if (((int)uVar2 < 3) && (puVar10 != (uint *)0x0)) {
      operator_delete(puVar10,4);
    }
  }
  if (*(int *)puVar8 == 0) {
LAB_00659f30:
    if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
      QArrayData::deallocate((QArrayData *)puVar8,2,8);
      return;
    }
  }
  else {
    if (*(int *)puVar8 != -1) {
      LOCK();
      *(int *)puVar8 = *(int *)puVar8 + -1;
      UNLOCK();
      if (*(int *)puVar8 == 0) goto LAB_00659f30;
    }
    if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisRasterKeyframeChannel @ 0065e7c0 ======

/* KisRasterKeyframeChannel::KisRasterKeyframeChannel(KisRasterKeyframeChannel const&,
   KisWeakSharedPtr<KisPaintDevice>) */

void __thiscall
KisRasterKeyframeChannel::KisRasterKeyframeChannel
          (KisRasterKeyframeChannel *this,KisRasterKeyframeChannel *param_1,KisWeakSharedPtr param_2
          )

{
  Data *pDVar1;
  QObject *pQVar2;
  Data *pDVar3;
  int *piVar4;
  code *pcVar5;
  int iVar6;
  QArrayData *pQVar7;
  undefined8 *puVar8;
  undefined8 uVar9;
  undefined auVar10 [16];
  uint *puVar11;
  _func_void_Node_ptr_void_ptr *p_Var12;
  undefined *puVar13;
  undefined *puVar14;
  uint *puVar15;
  uint *puVar16;
  uint uVar17;
  undefined4 uVar18;
  undefined (*pauVar19) [16];
  uint *puVar20;
  long *plVar21;
  long lVar22;
  undefined auVar23 [8];
  KisRasterKeyframe *this_00;
  QObject *pQVar24;
  undefined8 *puVar25;
  long lVar26;
  _func_void_Node_ptr_void_ptr *p_Var27;
  long lVar28;
  ulong *puVar29;
  int *piVar30;
  uint *puVar31;
  ulong uVar32;
  undefined4 in_register_00000014;
  long *plVar33;
  Data *pDVar34;
  int iVar35;
  _func_void_Node_ptr *p_Var36;
  uint *puVar37;
  _func_void_Node_ptr_void_ptr *p_Var38;
  Data *pDVar39;
  uint uVar40;
  long in_FS_OFFSET;
  QObject *local_d8;
  int local_94;
  Data *local_90;
  QArrayData *local_88;
  Data *pDStack_80;
  Data *local_78;
  undefined4 local_70;
  undefined local_68 [8];
  uint *puStack_60;
  Data *local_58;
  undefined4 local_50;
  long local_40;
  
  puStack_60 = (uint *)local_68;
  plVar33 = (long *)CONCAT44(in_register_00000014,param_2);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisKeyframeChannel::KisKeyframeChannel((KisKeyframeChannel *)this,(KisKeyframeChannel *)param_1);
  *(undefined **)this = PTR_vtable_008373a8 + 0x10;
                    /* try { // try from 0065e815 to 0065e819 has its CatchHandler @ 0065f4a1 */
  pauVar19 = (undefined (*) [16])operator_new(0x30);
  pQVar7 = *(QArrayData **)(*(long *)(param_1 + 0x18) + 0x20);
  local_88 = pQVar7;
  if (*(int *)pQVar7 + 1U < 2) {
    puVar20 = (uint *)plVar33[1];
    if (*plVar33 == 0) goto LAB_0065f331;
LAB_0065e84c:
    if ((puVar20 == (uint *)0x0) || ((*puVar20 & 1) == 0)) {
      puStack_60 = (uint *)0x0;
      goto LAB_0065e8e3;
    }
    auVar23 = (undefined  [8])*plVar33;
    local_68 = auVar23;
    if (auVar23 == (undefined  [8])0x0) goto LAB_0065f331;
    puVar20 = *(uint **)((long)auVar23 + 0x18);
    if (puVar20 == (uint *)0x0) {
                    /* try { // try from 0065f412 to 0065f416 has its CatchHandler @ 0065f44d */
      piVar30 = (int *)operator_new(4);
      *piVar30 = 0;
      *(int **)((long)auVar23 + 0x18) = piVar30;
      LOCK();
      *piVar30 = *piVar30 + 1;
      UNLOCK();
      puVar20 = *(uint **)((long)auVar23 + 0x18);
      auVar23 = local_68;
    }
    local_68 = auVar23;
    auVar23 = local_68;
    LOCK();
    *puVar20 = *puVar20 + 2;
    UNLOCK();
    puStack_60 = puVar20;
    if (local_68 == (undefined  [8])0x0) goto LAB_0065e8e3;
    if ((puVar20 != (uint *)0x0) && ((*puVar20 & 1) != 0)) {
      *(undefined (*) [8])*pauVar19 = local_68;
      if (local_68 != (undefined  [8])0x0) {
        piVar30 = *(int **)((long)local_68 + 0x18);
        if (piVar30 == (int *)0x0) {
                    /* try { // try from 0065f3ef to 0065f3f3 has its CatchHandler @ 0065f435 */
          piVar30 = (int *)operator_new(4);
          *piVar30 = 0;
          *(int **)((long)auVar23 + 0x18) = piVar30;
          LOCK();
          *piVar30 = *piVar30 + 1;
          UNLOCK();
          piVar30 = *(int **)((long)auVar23 + 0x18);
        }
        *(int **)(*pauVar19 + 8) = piVar30;
        LOCK();
        *piVar30 = *piVar30 + 2;
        UNLOCK();
        goto LAB_0065e8f2;
      }
      goto LAB_0065e8ea;
    }
    *pauVar19 = (undefined  [16])0x0;
  }
  else {
    LOCK();
    *(int *)pQVar7 = *(int *)pQVar7 + 1;
    UNLOCK();
    puVar20 = (uint *)plVar33[1];
    if (*plVar33 != 0) goto LAB_0065e84c;
LAB_0065f331:
    puStack_60 = (uint *)0x0;
LAB_0065e8e3:
    *(undefined8 *)*pauVar19 = 0;
LAB_0065e8ea:
    *(undefined8 *)(*pauVar19 + 8) = 0;
  }
LAB_0065e8f2:
  puVar20 = puStack_60;
  puVar13 = PTR_shared_null_00836c40;
  *(QArrayData **)pauVar19[2] = pQVar7;
  puVar14 = PTR_shared_null_008372c0;
  *(undefined **)pauVar19[1] = puVar13;
  *(undefined **)(pauVar19[1] + 8) = puVar14;
  if (1 < *(int *)pQVar7 + 1U) {
    LOCK();
    *(int *)pQVar7 = *(int *)pQVar7 + 1;
    UNLOCK();
  }
  pauVar19[2][8] = 0;
  auVar10._8_8_ = 0;
  auVar10._0_8_ = puStack_60;
  _local_68 = auVar10 << 0x40;
  *(undefined (**) [16])(this + 0x18) = pauVar19;
  if (puVar20 != (uint *)0x0) {
    LOCK();
    uVar17 = *puVar20;
    *puVar20 = *puVar20 - 2;
    UNLOCK();
    if (((int)uVar17 < 3) && (puVar20 != (uint *)0x0)) {
      operator_delete(puVar20,4);
    }
  }
  if (*(int *)local_88 == 0) {
LAB_0065f2a2:
    QArrayData::deallocate(local_88,2,8);
  }
  else if (*(int *)local_88 != -1) {
    LOCK();
    *(int *)local_88 = *(int *)local_88 + -1;
    UNLOCK();
    if (*(int *)local_88 == 0) goto LAB_0065f2a2;
  }
  if (this == param_1) {
                    /* try { // try from 0065f2d4 to 0065f2d8 has its CatchHandler @ 0065f459 */
    kis_assert_recoverable
              ("&rhs != this","/builds/graphics/krita/libs/image/kis_raster_keyframe_channel.cpp",
               0x85);
  }
  lVar26 = *(long *)(param_1 + 0x18);
  lVar22 = *(long *)(this + 0x18);
  piVar30 = *(int **)(lVar26 + 0x18);
  if (piVar30 != *(int **)(lVar22 + 0x18)) {
    if (*piVar30 == 0) {
                    /* try { // try from 0065f385 to 0065f3c3 has its CatchHandler @ 0065f459 */
      lVar28 = QMapDataBase::createData();
      lVar26 = *(long *)(*(long *)(lVar26 + 0x18) + 0x10);
      if (lVar26 != 0) {
        puVar29 = (ulong *)FUN_00661ba0(lVar26,lVar28);
        uVar32 = *puVar29;
        *(ulong **)(lVar28 + 0x10) = puVar29;
        *puVar29 = (ulong)((uint)uVar32 & 3) | lVar28 + 8U;
        QMapDataBase::recalcMostLeftNode();
      }
    }
    else {
      if (*piVar30 != -1) {
        LOCK();
        *piVar30 = *piVar30 + 1;
        UNLOCK();
      }
      lVar28 = *(long *)(lVar26 + 0x18);
    }
    piVar30 = *(int **)(lVar22 + 0x18);
    *(long *)(lVar22 + 0x18) = lVar28;
    if (*piVar30 == 0) {
LAB_0065ee3f:
      FUN_006608b0();
    }
    else if (*piVar30 != -1) {
      LOCK();
      *piVar30 = *piVar30 + -1;
      UNLOCK();
      if (*piVar30 == 0) goto LAB_0065ee3f;
    }
    lVar26 = *(long *)(param_1 + 0x18);
    lVar22 = *(long *)(this + 0x18);
  }
  *(undefined *)(lVar22 + 0x28) = *(undefined *)(lVar26 + 0x28);
                    /* try { // try from 0065ea0d to 0065ea11 has its CatchHandler @ 0065f459 */
  plVar21 = (long *)KisKeyframeChannel::constKeys((KisKeyframeChannel *)param_1);
  local_68 = (undefined  [8])PTR_shared_null_00837830;
  lVar26 = *plVar21;
  if (*(int *)(PTR_shared_null_00837830 + 4) < *(int *)(lVar26 + 4)) {
    if (*(uint *)PTR_shared_null_00837830 < 2) {
      QListData::realloc((int)local_68);
    }
    else {
                    /* try { // try from 0065f2ff to 0065f376 has its CatchHandler @ 0065f465 */
      FUN_00661c90();
    }
    lVar26 = *plVar21;
  }
  if (*(long *)(lVar26 + 0x10) != 0) {
    lVar22 = *(long *)(lVar26 + 0x20);
    while (lVar22 != lVar26 + 8) {
                    /* try { // try from 0065ea5c to 0065ea68 has its CatchHandler @ 0065f465 */
      FUN_00661d10(local_68,lVar22 + 0x18);
      lVar22 = QMapNodeBase::nextNode();
      lVar26 = *plVar21;
    }
  }
  puVar13 = PTR_shared_null_00837830;
  local_88 = (QArrayData *)local_68;
  local_70 = 1;
  pDVar1 = (Data *)((long)local_68 + 8);
  pDVar39 = (Data *)((long)local_68 + 0xc);
  local_68 = (undefined  [8])PTR_shared_null_00837830;
  pDStack_80 = (Data *)((undefined  [8])local_88 + (long)*(int *)pDVar1 * 8 + 0x10);
  pDVar1 = (Data *)((undefined  [8])local_88 + (long)*(int *)pDVar39 * 8 + 0x10);
  local_78 = pDVar1;
  if (*(int *)PTR_shared_null_00837830 == 0) {
LAB_0065f298:
    QListData::dispose((Data *)puVar13);
  }
  else if (*(int *)PTR_shared_null_00837830 != -1) {
    LOCK();
    *(int *)PTR_shared_null_00837830 = *(int *)PTR_shared_null_00837830 + -1;
    UNLOCK();
    if (*(int *)puVar13 == 0) goto LAB_0065f298;
  }
  for (pDVar39 = pDStack_80; pDStack_80 = pDVar39, pDVar1 != pDVar39; pDVar39 = pDVar39 + 8) {
                    /* try { // try from 0065eb1e to 0065eb22 has its CatchHandler @ 0065f471 */
    KisKeyframeChannel::keyframeAt((KisWeakSharedPtr)local_68);
    puVar20 = puStack_60;
    local_d8 = (QObject *)puStack_60;
    auVar23 = local_68;
    if ((local_68 == (undefined  [8])0x0) ||
       (auVar23 = (undefined  [8])
                  __dynamic_cast(local_68,PTR_typeinfo_00837230,PTR_typeinfo_00836d68,0),
       auVar23 == (undefined  [8])0x0)) {
      local_d8 = (QObject *)0x0;
      pDVar34 = (Data *)puVar20;
LAB_0065eba8:
      if (pDVar34 != (Data *)0x0) {
        LOCK();
        pDVar3 = pDVar34 + 4;
        *(int *)pDVar3 = *(int *)pDVar3 + -1;
        UNLOCK();
        if (*(int *)pDVar3 == 0) {
          (**(code **)(pDVar34 + 8))(pDVar34);
        }
        LOCK();
        *(int *)pDVar34 = *(int *)pDVar34 + -1;
        UNLOCK();
        if (*(int *)pDVar34 == 0) {
          operator_delete(pDVar34,0x10);
        }
      }
    }
    else {
      if ((QObject *)puVar20 != (QObject *)0x0) {
        pQVar2 = (QObject *)((long)puVar20 + 4);
        iVar35 = *(int *)((long)puVar20 + 4);
        while (0 < iVar35) {
          LOCK();
          iVar6 = *(int *)pQVar2;
          if (iVar35 == iVar6) {
            *(int *)pQVar2 = iVar35 + 1;
          }
          UNLOCK();
          if (iVar35 == iVar6) {
            LOCK();
            *puVar20 = *puVar20 + 1;
            UNLOCK();
            pDVar34 = (Data *)puStack_60;
            if (*(int *)pQVar2 != 0) goto LAB_0065eba8;
            local_d8 = (QObject *)puVar20;
            goto LAB_0065eba0;
          }
          iVar35 = *(int *)pQVar2;
        }
                    /* try { // try from 0065f258 to 0065f25c has its CatchHandler @ 0065f4c5 */
        QtSharedPointer::ExternalRefCountData::checkQObjectShared((QObject *)puVar20);
        local_d8 = (QObject *)0x0;
LAB_0065eba0:
        auVar23 = (undefined  [8])0x0;
        pDVar34 = (Data *)puStack_60;
        goto LAB_0065eba8;
      }
      local_d8 = (QObject *)0x0;
      auVar23 = (undefined  [8])0x0;
    }
    lVar26 = *(long *)(this + 0x18);
                    /* try { // try from 0065ebd3 to 0065ebf2 has its CatchHandler @ 0065f4ad */
    uVar17 = KisRasterKeyframe::frameID((KisRasterKeyframe *)auVar23);
    puVar25 = *(undefined8 **)(lVar26 + 0x10);
    if (*(uint *)(puVar25 + 4) != 0) {
      uVar40 = *(uint *)((long)puVar25 + 0x24) ^ uVar17;
      for (puVar8 = *(undefined8 **)
                     (puVar25[1] + ((ulong)uVar40 % (ulong)*(uint *)(puVar25 + 4)) * 8);
          puVar25 != puVar8; puVar8 = (undefined8 *)*puVar8) {
        if ((uVar40 == *(uint *)(puVar8 + 1)) && (uVar17 == *(uint *)((long)puVar8 + 0xc))) {
          if (puVar8 != puVar25) {
            if (local_d8 != (QObject *)0x0) {
              LOCK();
              pQVar2 = local_d8 + 4;
              *(int *)pQVar2 = *(int *)pQVar2 + -1;
              UNLOCK();
              if (*(int *)pQVar2 == 0) {
                (**(code **)(local_d8 + 8))(local_d8);
              }
              LOCK();
              *(int *)local_d8 = *(int *)local_d8 + -1;
              UNLOCK();
              if (*(int *)local_d8 == 0) {
                operator_delete(local_d8,0x10);
              }
            }
            goto LAB_0065eec6;
          }
          break;
        }
      }
    }
    this_00 = (KisRasterKeyframe *)operator_new(0x30);
                    /* try { // try from 0065ebfb to 0065ec0b has its CatchHandler @ 0065f4b9 */
    uVar18 = KisKeyframe::colorLabel((KisKeyframe *)auVar23);
    local_90 = (Data *)CONCAT44(local_90._4_4_,uVar18);
    local_94 = KisRasterKeyframe::frameID((KisRasterKeyframe *)auVar23);
    if (*plVar33 == 0) {
      local_68 = (undefined  [8])0x0;
      auVar23 = local_68;
LAB_0065f358:
      local_68 = auVar23;
      puStack_60 = (uint *)0x0;
    }
    else if (((uint *)plVar33[1] == (uint *)0x0) || ((*(uint *)plVar33[1] & 1) == 0)) {
      local_68 = (undefined  [8])0x0;
      puStack_60 = (uint *)0x0;
    }
    else {
      auVar23 = (undefined  [8])*plVar33;
      local_68 = auVar23;
      if (auVar23 == (undefined  [8])0x0) goto LAB_0065f358;
      piVar30 = *(int **)((long)auVar23 + 0x18);
      if (piVar30 == (int *)0x0) {
                    /* try { // try from 0065f3ce to 0065f3d2 has its CatchHandler @ 0065f4b9 */
        piVar30 = (int *)operator_new(4);
        *piVar30 = 0;
        *(int **)((long)auVar23 + 0x18) = piVar30;
        LOCK();
        *piVar30 = *piVar30 + 1;
        UNLOCK();
        piVar30 = *(int **)((long)auVar23 + 0x18);
        auVar23 = local_68;
      }
      local_68 = auVar23;
      LOCK();
      *piVar30 = *piVar30 + 2;
      UNLOCK();
      puStack_60 = (uint *)piVar30;
    }
                    /* try { // try from 0065ec80 to 0065ec84 has its CatchHandler @ 0065f47d */
    KisRasterKeyframe::KisRasterKeyframe
              (this_00,(KisWeakSharedPtr)local_68,&local_94,(int *)&local_90);
                    /* try { // try from 0065ec8a to 0065ec8e has its CatchHandler @ 0065f489 */
    pQVar24 = (QObject *)operator_new(0x18);
    *(KisRasterKeyframe **)(pQVar24 + 0x10) = this_00;
    *(code **)(pQVar24 + 8) = FUN_0065fb20;
    pQVar2 = pQVar24 + 4;
    *(undefined4 *)(pQVar24 + 4) = 1;
    *(undefined4 *)pQVar24 = 1;
                    /* try { // try from 0065ecc4 to 0065ecc8 has its CatchHandler @ 0065f4d1 */
    QtSharedPointer::ExternalRefCountData::setQObjectShared(pQVar24,SUB81(this_00,0));
    local_68 = (undefined  [8])0x0;
    if (puStack_60 != (uint *)0x0) {
      LOCK();
      iVar35 = *puStack_60;
      *puStack_60 = *puStack_60 + -2;
      UNLOCK();
      if ((iVar35 < 3) && (puStack_60 != (uint *)0x0)) {
        operator_delete(puStack_60,4);
      }
    }
    lVar26 = *(long *)(param_1 + 0x18);
                    /* try { // try from 0065ed27 to 0065ed3d has its CatchHandler @ 0065f4dd */
    local_94 = KisRasterKeyframe::frameID(this_00);
    FUN_00661d70(&local_90,lVar26 + 0x10,&local_94);
    local_68 = (undefined  [8])local_90;
    puVar13 = PTR_shared_null_00837830;
    pDVar34 = local_90 + 8;
    local_90 = (Data *)PTR_shared_null_00837830;
    puStack_60 = (uint *)((long)local_68 + (long)*(int *)pDVar34 * 8 + 0x10);
    local_50 = 1;
    pDVar34 = (Data *)((long)local_68 + (long)*(int *)((long)local_68 + 0xc) * 8 + 0x10);
    local_58 = pDVar34;
    if (*(int *)PTR_shared_null_00837830 == 0) {
LAB_0065f284:
      QListData::dispose((Data *)puVar13);
      pDVar3 = (Data *)puStack_60;
    }
    else {
      pDVar3 = (Data *)puStack_60;
      if (*(int *)PTR_shared_null_00837830 != -1) {
        LOCK();
        *(int *)PTR_shared_null_00837830 = *(int *)PTR_shared_null_00837830 + -1;
        UNLOCK();
        if (*(int *)puVar13 == 0) goto LAB_0065f284;
      }
    }
    for (; puStack_60 = (uint *)pDVar3, pDVar34 != pDVar3; pDVar3 = pDVar3 + 8) {
                    /* try { // try from 0065edcd to 0065edd1 has its CatchHandler @ 0065f441 */
      puVar25 = (undefined8 *)KisKeyframeChannel::keys((KisKeyframeChannel *)this);
      LOCK();
      *(int *)pQVar24 = *(int *)pQVar24 + 1;
      UNLOCK();
      LOCK();
      *(int *)pQVar2 = *(int *)pQVar2 + 1;
      UNLOCK();
      puVar20 = (uint *)*puVar25;
      if (*puVar20 < 2) {
        puVar31 = *(uint **)(puVar20 + 4);
        if (puVar31 != (uint *)0x0) goto LAB_0065edfe;
LAB_0065f1b0:
        iVar35 = (int)puVar20;
        puVar20 = puVar20 + 2;
LAB_0065ef42:
                    /* try { // try from 0065ef4c to 0065ef50 has its CatchHandler @ 0065f495 */
        lVar26 = QMapDataBase::createNode
                           (iVar35,0x30,(QMapNodeBase *)&DAT_00000008,SUB81(puVar20,0));
        iVar35 = *(int *)pDVar3;
        *(QObject **)(lVar26 + 0x28) = pQVar24;
        *(int *)(lVar26 + 0x18) = iVar35;
        *(KisRasterKeyframe **)(lVar26 + 0x20) = this_00;
        LOCK();
        *(int *)pQVar24 = *(int *)pQVar24 + 1;
        UNLOCK();
        LOCK();
        piVar30 = (int *)(*(long *)(lVar26 + 0x28) + 4);
        *piVar30 = *piVar30 + 1;
        UNLOCK();
      }
      else {
                    /* try { // try from 0065f19b to 0065f19f has its CatchHandler @ 0065f495 */
        FUN_00662130(puVar25);
        puVar20 = (uint *)*puVar25;
        puVar31 = *(uint **)(puVar20 + 4);
        if (puVar31 == (uint *)0x0) goto LAB_0065f1b0;
LAB_0065edfe:
        iVar35 = (int)puVar20;
        puVar37 = (uint *)0x0;
        do {
          uVar17 = puVar31[6];
          puVar15 = *(uint **)(puVar31 + 2);
          puVar16 = *(uint **)(puVar31 + 4);
          puVar20 = puVar31;
          while (puVar11 = puVar16, puVar31 = puVar15, (int)uVar17 < *(int *)pDVar3) {
            if (puVar11 == (uint *)0x0) {
              if (puVar37 == (uint *)0x0) goto LAB_0065ef42;
              uVar17 = puVar37[6];
              goto joined_r0x0065ef3c;
            }
            puVar15 = *(uint **)(puVar11 + 2);
            puVar16 = *(uint **)(puVar11 + 4);
            puVar20 = puVar11;
            uVar17 = puVar11[6];
          }
          puVar37 = puVar20;
        } while (puVar31 != (uint *)0x0);
        uVar17 = puVar20[6];
joined_r0x0065ef3c:
        if (*(int *)pDVar3 < (int)uVar17) goto LAB_0065ef42;
        LOCK();
        *(int *)pQVar24 = *(int *)pQVar24 + 1;
        UNLOCK();
        LOCK();
        *(int *)pQVar2 = *(int *)pQVar2 + 1;
        UNLOCK();
        piVar30 = *(int **)(puVar37 + 10);
        *(QObject **)(puVar37 + 10) = pQVar24;
        *(KisRasterKeyframe **)(puVar37 + 8) = this_00;
        if (piVar30 != (int *)0x0) {
          LOCK();
          piVar4 = piVar30 + 1;
          *piVar4 = *piVar4 + -1;
          UNLOCK();
          if (*piVar4 == 0) {
            (**(code **)(piVar30 + 2))(piVar30);
          }
          LOCK();
          *piVar30 = *piVar30 + -1;
          UNLOCK();
          if (*piVar30 == 0) {
            operator_delete(piVar30,0x10);
          }
        }
      }
      LOCK();
      *(int *)pQVar2 = *(int *)pQVar2 + -1;
      UNLOCK();
      if (*(int *)pQVar2 == 0) {
        (**(code **)(pQVar24 + 8))(pQVar24);
      }
      LOCK();
      *(int *)pQVar24 = *(int *)pQVar24 + -1;
      UNLOCK();
      if (*(int *)pQVar24 == 0) {
        operator_delete(pQVar24,0x10);
      }
      lVar26 = *(long *)(this + 0x18);
                    /* try { // try from 0065efa2 to 0065f022 has its CatchHandler @ 0065f441 */
      uVar17 = KisRasterKeyframe::frameID(this_00);
      p_Var27 = *(_func_void_Node_ptr_void_ptr **)(lVar26 + 0x10);
      if (*(uint *)(p_Var27 + 0x10) < 2) {
        uVar32 = (ulong)*(uint *)(p_Var27 + 0x20);
        if ((int)*(uint *)(p_Var27 + 0x20) <= *(int *)(p_Var27 + 0x14)) {
LAB_0065f175:
          QHashData::rehash((int)p_Var27);
          p_Var27 = *(_func_void_Node_ptr_void_ptr **)(lVar26 + 0x10);
          uVar32 = (ulong)*(uint *)(p_Var27 + 0x20);
        }
      }
      else {
                    /* try { // try from 0065f131 to 0065f183 has its CatchHandler @ 0065f441 */
        p_Var27 = (_func_void_Node_ptr_void_ptr *)
                  QHashData::detach_helper(p_Var27,FUN_0065f9a0,0x65f990,0x18);
        p_Var36 = *(_func_void_Node_ptr **)(lVar26 + 0x10);
        pcVar5 = p_Var36 + 0x10;
        if (*(int *)(p_Var36 + 0x10) == 0) {
LAB_0065f210:
                    /* try { // try from 0065f217 to 0065f21b has its CatchHandler @ 0065f441 */
          QHashData::free_helper(p_Var36);
        }
        else if (*(int *)(p_Var36 + 0x10) != -1) {
          LOCK();
          *(int *)pcVar5 = *(int *)pcVar5 + -1;
          UNLOCK();
          if (*(int *)pcVar5 == 0) {
            p_Var36 = *(_func_void_Node_ptr **)(lVar26 + 0x10);
            goto LAB_0065f210;
          }
        }
        uVar40 = *(uint *)(p_Var27 + 0x20);
        uVar32 = (ulong)uVar40;
        *(_func_void_Node_ptr_void_ptr **)(lVar26 + 0x10) = p_Var27;
        if ((int)uVar40 <= *(int *)(p_Var27 + 0x14)) goto LAB_0065f175;
      }
      uVar40 = *(uint *)(p_Var27 + 0x24) ^ uVar17;
      p_Var38 = (_func_void_Node_ptr_void_ptr *)(lVar26 + 0x10);
      if ((int)uVar32 != 0) {
        p_Var38 = (_func_void_Node_ptr_void_ptr *)
                  (*(long *)(p_Var27 + 8) + ((ulong)uVar40 % uVar32) * 8);
        for (p_Var12 = *(_func_void_Node_ptr_void_ptr **)p_Var38;
            (p_Var27 != p_Var12 &&
            ((uVar40 != *(uint *)(p_Var12 + 8) || (uVar17 != *(uint *)(p_Var12 + 0xc)))));
            p_Var12 = *(_func_void_Node_ptr_void_ptr **)p_Var12) {
          p_Var38 = p_Var12;
        }
      }
      puVar25 = (undefined8 *)QHashData::allocateNode((int)p_Var27);
      uVar9 = *(undefined8 *)p_Var38;
      *(uint *)(puVar25 + 1) = uVar40;
      *puVar25 = uVar9;
      iVar35 = *(int *)pDVar3;
      *(uint *)((long)puVar25 + 0xc) = uVar17;
      *(int *)(puVar25 + 2) = iVar35;
      *(undefined8 **)p_Var38 = puVar25;
      piVar30 = (int *)(*(long *)(lVar26 + 0x10) + 0x14);
      *piVar30 = *piVar30 + 1;
    }
    if (*(int *)local_68 == 0) {
LAB_0065f28e:
      QListData::dispose((Data *)local_68);
    }
    else if (*(int *)local_68 != -1) {
      LOCK();
      *(int *)local_68 = *(int *)local_68 + -1;
      UNLOCK();
      if (*(int *)local_68 == 0) goto LAB_0065f28e;
    }
    FUN_00658760(pQVar24);
    if (local_d8 != (QObject *)0x0) {
      FUN_00658760(local_d8);
    }
LAB_0065eec6:
  }
  if (*(int *)local_88 != 0) {
    if (*(int *)local_88 == -1) goto LAB_0065eefd;
    LOCK();
    *(int *)local_88 = *(int *)local_88 + -1;
    UNLOCK();
    if (*(int *)local_88 != 0) goto LAB_0065eefd;
  }
  QListData::dispose((Data *)local_88);
LAB_0065eefd:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



