/* Class KisLayerStyleProjectionPlane - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLayerStyleProjectionPlane @ 002070f0 ======

void __thiscall
KisLayerStyleProjectionPlane::KisLayerStyleProjectionPlane
          (KisLayerStyleProjectionPlane *this,KisLayer *param_1)

{
  (*(code *)PTR_KisLayerStyleProjectionPlane_0083b348)();
  return;
}



// ====== KisLayerStyleProjectionPlane @ 00208720 ======

void __thiscall
KisLayerStyleProjectionPlane::KisLayerStyleProjectionPlane
          (KisLayerStyleProjectionPlane *this,KisLayerStyleProjectionPlane *param_1,
          KisLayer *param_2,QSharedPointer param_3)

{
  (*(code *)PTR_KisLayerStyleProjectionPlane_0083be60)();
  return;
}



// ====== KisLayerStyleProjectionPlane @ 00674070 ======

/* KisLayerStyleProjectionPlane::KisLayerStyleProjectionPlane(KisLayerStyleProjectionPlane const&,
   KisLayer*, QSharedPointer<KisPSDLayerStyle>) */

void __thiscall
KisLayerStyleProjectionPlane::KisLayerStyleProjectionPlane
          (KisLayerStyleProjectionPlane *this,KisLayerStyleProjectionPlane *param_1,
          KisLayer *param_2,QSharedPointer param_3)

{
  int *piVar1;
  uint uVar2;
  int *piVar3;
  undefined8 uVar4;
  long lVar5;
  long lVar6;
  undefined *puVar7;
  undefined8 uVar8;
  undefined uVar9;
  undefined (*pauVar10) [16];
  long lVar11;
  undefined8 *puVar12;
  void *pvVar13;
  undefined4 *puVar14;
  int *piVar15;
  KisPSDLayerStyle *this_00;
  int iVar16;
  undefined4 in_register_0000000c;
  undefined8 *puVar17;
  undefined8 *puVar18;
  QArrayData *pQVar19;
  QArrayData *pQVar20;
  QArrayData *pQVar21;
  long in_FS_OFFSET;
  void *local_88;
  int *local_80;
  QArrayData *local_78;
  int *piStack_70;
  undefined local_68 [16];
  undefined8 *local_58;
  undefined4 local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisAbstractProjectionPlane::KisAbstractProjectionPlane((KisAbstractProjectionPlane *)this);
  *(undefined **)this = PTR_vtable_00837ca8 + 0x10;
                    /* try { // try from 006740b9 to 006740bd has its CatchHandler @ 00675077 */
  pauVar10 = (undefined (*) [16])operator_new(0x88);
  puVar7 = PTR_shared_null_008377d0;
  *(undefined8 *)(pauVar10[4] + 8) = 0;
  *(undefined8 *)pauVar10[6] = 0;
  *(undefined8 *)(pauVar10[7] + 8) = 0;
  *pauVar10 = (undefined  [16])0x0;
  *(undefined **)pauVar10[1] = puVar7;
  *(undefined **)(pauVar10[1] + 8) = puVar7;
  *(undefined **)pauVar10[2] = puVar7;
  *(undefined (*) [16])(pauVar10[2] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar10[3] + 8) = (undefined  [16])0x0;
  pauVar10[5] = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar10[6] + 8) = (undefined  [16])0x0;
  *(undefined2 *)pauVar10[8] = 0;
  *(undefined (**) [16])(this + 8) = pauVar10;
  if (param_2 == (KisLayer *)0x0) {
                    /* try { // try from 00675023 to 00675027 has its CatchHandler @ 006750d7 */
    kis_safe_assert_recoverable
              ("sourceLayer",
               "/builds/graphics/krita/libs/image/layerstyles/kis_layer_style_projection_plane.cpp",
               0x2e);
  }
  else {
                    /* try { // try from 0067412d to 00674132 has its CatchHandler @ 006750d7 */
    (**(code **)(*(long *)param_2 + 0x1f0))(local_68,param_2);
    uVar4 = local_68._8_8_;
    piVar15 = *(int **)*pauVar10;
    if ((int *)local_68._8_8_ != piVar15) {
      uVar8 = local_68._0_8_;
      if ((int *)local_68._8_8_ != (int *)0x0) {
        LOCK();
        *(int *)local_68._8_8_ = *(int *)local_68._8_8_ + 1;
        UNLOCK();
        piVar15 = *(int **)*pauVar10;
      }
      if (piVar15 != (int *)0x0) {
        LOCK();
        *piVar15 = *piVar15 + -1;
        UNLOCK();
        if ((*piVar15 == 0) && (*(void **)*pauVar10 != (void *)0x0)) {
          operator_delete(*(void **)*pauVar10,0x10);
        }
      }
      *(undefined8 *)*pauVar10 = uVar4;
      *(undefined8 *)(*pauVar10 + 8) = uVar8;
      piVar15 = (int *)local_68._8_8_;
    }
    if (piVar15 != (int *)0x0) {
      LOCK();
      piVar3 = piVar15 + 1;
      *piVar3 = *piVar3 + -1;
      UNLOCK();
      if (*piVar3 == 0) {
        (**(code **)(piVar15 + 2))(piVar15);
      }
      LOCK();
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 == 0) {
        operator_delete(piVar15,0x10);
      }
    }
                    /* try { // try from 00674196 to 0067419b has its CatchHandler @ 006750d7 */
    (**(code **)(*(long *)param_2 + 0x1c0))(local_68,param_2);
                    /* try { // try from 006741a1 to 006741a5 has its CatchHandler @ 006750a7 */
    uVar9 = KisProjectionLeaf::canHaveChildLayers((KisProjectionLeaf *)local_68._0_8_);
    uVar4 = local_68._8_8_;
    pauVar10[8][0] = uVar9;
    if ((int *)local_68._8_8_ != (int *)0x0) {
      LOCK();
      piVar15 = (int *)(local_68._8_8_ + 4);
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 == 0) {
        (**(code **)(local_68._8_8_ + 8))(local_68._8_8_);
      }
      LOCK();
      *(int *)uVar4 = *(int *)uVar4 + -1;
      UNLOCK();
      if (*(int *)uVar4 == 0) {
        operator_delete((void *)uVar4,0x10);
      }
    }
                    /* try { // try from 006741d9 to 006741de has its CatchHandler @ 006750d7 */
    (**(code **)(*(long *)param_2 + 0x1c0))(local_68,param_2);
                    /* try { // try from 006741e4 to 006741e8 has its CatchHandler @ 0067506b */
    uVar9 = KisProjectionLeaf::dependsOnLowerNodes((KisProjectionLeaf *)local_68._0_8_);
    uVar4 = local_68._8_8_;
    pauVar10[8][1] = uVar9;
    if ((int *)local_68._8_8_ != (int *)0x0) {
      LOCK();
      piVar15 = (int *)(local_68._8_8_ + 4);
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 == 0) {
        (**(code **)(local_68._8_8_ + 8))(local_68._8_8_);
      }
      LOCK();
      *(int *)uVar4 = *(int *)uVar4 + -1;
      UNLOCK();
      if (*(int *)uVar4 == 0) {
        operator_delete((void *)uVar4,0x10);
      }
    }
    *(KisLayer **)(pauVar10[6] + 8) = param_2;
  }
  uVar4 = *(undefined8 *)CONCAT44(in_register_0000000c,param_3);
  piVar15 = (int *)((undefined8 *)CONCAT44(in_register_0000000c,param_3))[1];
  lVar11 = *(long *)(this + 8);
  if (piVar15 != (int *)0x0) {
    LOCK();
    *piVar15 = *piVar15 + 1;
    UNLOCK();
    LOCK();
    piVar15[1] = piVar15[1] + 1;
    UNLOCK();
  }
  piVar3 = *(int **)(lVar11 + 0x78);
  *(undefined8 *)(lVar11 + 0x70) = uVar4;
  *(int **)(lVar11 + 0x78) = piVar15;
  if (piVar3 == (int *)0x0) {
LAB_0067425c:
    lVar11 = *(long *)(*(long *)(this + 8) + 0x70);
  }
  else {
    LOCK();
    piVar15 = piVar3 + 1;
    *piVar15 = *piVar15 + -1;
    UNLOCK();
    if (*piVar15 == 0) {
      (**(code **)(piVar3 + 2))(piVar3);
    }
    LOCK();
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 != 0) goto LAB_0067425c;
    operator_delete(piVar3,0x10);
    lVar11 = *(long *)(*(long *)(this + 8) + 0x70);
  }
  if (lVar11 == 0) {
                    /* try { // try from 00674aaf to 00674abd has its CatchHandler @ 006750d7 */
    kis_safe_assert_recoverable
              ("m_d->style",
               "/builds/graphics/krita/libs/image/layerstyles/kis_layer_style_projection_plane.cpp",
               0x6c);
    this_00 = (KisPSDLayerStyle *)operator_new(0x18);
    local_68 = (undefined  [16])0x0;
                    /* try { // try from 00674ad3 to 00674ad7 has its CatchHandler @ 00675053 */
    local_78 = (QArrayData *)QString::fromAscii_helper("",0);
                    /* try { // try from 00674aed to 00674af1 has its CatchHandler @ 0067505f */
    KisPSDLayerStyle::KisPSDLayerStyle(this_00,(QString *)&local_78,(QSharedPointer)local_68);
                    /* try { // try from 00674af7 to 00674afb has its CatchHandler @ 00675083 */
    puVar14 = (undefined4 *)operator_new(0x18);
    *(KisPSDLayerStyle **)(puVar14 + 4) = this_00;
    *(code **)(puVar14 + 2) = FUN_0067ab30;
    puVar14[1] = 1;
    *puVar14 = 1;
    lVar11 = *(long *)(this + 8);
    piVar15 = *(int **)(lVar11 + 0x78);
    *(KisPSDLayerStyle **)(lVar11 + 0x70) = this_00;
    *(undefined4 **)(lVar11 + 0x78) = puVar14;
    if (piVar15 != (int *)0x0) {
      LOCK();
      piVar3 = piVar15 + 1;
      *piVar3 = *piVar3 + -1;
      UNLOCK();
      if (*piVar3 == 0) {
        (**(code **)(piVar15 + 2))(piVar15);
      }
      LOCK();
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 == 0) {
        operator_delete(piVar15,0x10);
      }
    }
    if (*(int *)local_78 == 0) {
LAB_00674fe8:
      QArrayData::deallocate(local_78,2,8);
    }
    else if (*(int *)local_78 != -1) {
      LOCK();
      *(int *)local_78 = *(int *)local_78 + -1;
      UNLOCK();
      if (*(int *)local_78 == 0) goto LAB_00674fe8;
    }
    uVar4 = local_68._8_8_;
    if ((int *)local_68._8_8_ != (int *)0x0) {
      LOCK();
      piVar15 = (int *)(local_68._8_8_ + 4);
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 == 0) {
        (**(code **)(local_68._8_8_ + 8))(local_68._8_8_);
      }
      LOCK();
      *(int *)uVar4 = *(int *)uVar4 + -1;
      UNLOCK();
      if (*(int *)uVar4 == 0) {
        operator_delete((void *)uVar4,0x10);
      }
    }
  }
  lVar11 = *(long *)(param_1 + 8);
  piVar15 = *(int **)(lVar11 + 0x10);
  if (*piVar15 == 0) {
    if (*(char *)((long)piVar15 + 0xb) < '\0') {
      piVar15 = (int *)QArrayData::allocate(0x10,8,(ulong)(piVar15[2] & 0x7fffffff),0);
      local_68._0_8_ = piVar15;
      piVar3 = piVar15;
      if (piVar15 == (int *)0x0) {
        qBadAlloc();
        piVar3 = (int *)local_68._0_8_;
      }
      local_68._0_8_ = piVar3;
      *(byte *)((long)piVar15 + 0xb) = *(byte *)((long)piVar15 + 0xb) | 0x80;
      lVar5 = *(long *)(piVar15 + 4);
      uVar2 = piVar15[2];
    }
    else {
      piVar15 = (int *)QArrayData::allocate(0x10,8,(long)piVar15[1],0);
      local_68._0_8_ = piVar15;
      piVar3 = piVar15;
      if (piVar15 == (int *)0x0) {
        qBadAlloc();
        piVar3 = (int *)local_68._0_8_;
      }
      local_68._0_8_ = piVar3;
      lVar5 = *(long *)(piVar15 + 4);
      uVar2 = piVar15[2];
    }
    if ((uVar2 & 0x7fffffff) == 0) {
      puVar18 = (undefined8 *)(lVar5 + (long)piVar15);
      lVar11 = (long)piVar15[1] << 4;
    }
    else {
      lVar6 = *(long *)(lVar11 + 0x10);
      puVar18 = (undefined8 *)(lVar5 + (long)piVar15);
      iVar16 = *(int *)(lVar6 + 4);
      puVar12 = (undefined8 *)(*(long *)(lVar6 + 0x10) + lVar6);
      puVar17 = puVar12 + (long)iVar16 * 2;
      if (puVar12 == puVar17) {
        lVar11 = 0;
      }
      else {
        do {
          uVar4 = puVar12[1];
          piVar15 = (int *)puVar12[1];
          *puVar18 = *puVar12;
          puVar18[1] = uVar4;
          if (piVar15 != (int *)0x0) {
            LOCK();
            *piVar15 = *piVar15 + 1;
            UNLOCK();
            LOCK();
            *(int *)(puVar18[1] + 4) = *(int *)(puVar18[1] + 4) + 1;
            UNLOCK();
          }
          puVar12 = puVar12 + 2;
          puVar18 = puVar18 + 2;
        } while (puVar12 != puVar17);
        iVar16 = *(int *)(*(long *)(lVar11 + 0x10) + 4);
        puVar18 = (undefined8 *)(*(long *)(local_68._0_8_ + 0x10) + local_68._0_8_);
        lVar11 = (long)iVar16 << 4;
        piVar15 = (int *)local_68._0_8_;
      }
      piVar15[1] = iVar16;
    }
  }
  else {
    if (*piVar15 != -1) {
      LOCK();
      *piVar15 = *piVar15 + 1;
      UNLOCK();
      piVar15 = *(int **)(lVar11 + 0x10);
    }
    lVar11 = (long)piVar15[1] << 4;
    puVar18 = (undefined8 *)(*(long *)(piVar15 + 4) + (long)piVar15);
    local_68._0_8_ = piVar15;
  }
  puVar12 = (undefined8 *)(lVar11 + (long)puVar18);
  local_68._8_8_ = puVar18;
  local_50 = 1;
  local_58 = puVar12;
  if (puVar12 != puVar18) {
    do {
      while( true ) {
        piVar15 = (int *)puVar18[1];
        uVar4 = *puVar18;
        if (piVar15 != (int *)0x0) {
          LOCK();
          *piVar15 = *piVar15 + 1;
          UNLOCK();
          LOCK();
          piVar15[1] = piVar15[1] + 1;
          UNLOCK();
        }
        lVar11 = *(long *)(this + 8);
                    /* try { // try from 00674345 to 00674349 has its CatchHandler @ 006750e0 */
        pvVar13 = operator_new(0x10);
        local_78 = *(QArrayData **)(*(long *)(this + 8) + 0x70);
        piStack_70 = *(int **)(*(long *)(this + 8) + 0x78);
        if (piStack_70 != (int *)0x0) {
          LOCK();
          *piStack_70 = *piStack_70 + 1;
          UNLOCK();
          LOCK();
          piStack_70[1] = piStack_70[1] + 1;
          UNLOCK();
        }
                    /* try { // try from 00674381 to 00674385 has its CatchHandler @ 0067509b */
        FUN_00673a30(pvVar13,uVar4,param_2);
        local_88 = pvVar13;
                    /* try { // try from 00674390 to 00674394 has its CatchHandler @ 0067508f */
        local_80 = (int *)operator_new(0x18);
        *(void **)(local_80 + 4) = pvVar13;
        *(code **)(local_80 + 2) = FUN_0067ab50;
        local_80[1] = 1;
        *local_80 = 1;
                    /* try { // try from 006743bf to 006743c3 has its CatchHandler @ 006750ec */
        FUN_0067b950(lVar11 + 0x10,&local_88);
        piVar3 = local_80;
        if (local_80 != (int *)0x0) {
          LOCK();
          piVar1 = local_80 + 1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (*piVar1 == 0) {
            (**(code **)(local_80 + 2))(local_80);
          }
          LOCK();
          *piVar3 = *piVar3 + -1;
          UNLOCK();
          if (*piVar3 == 0) {
            operator_delete(piVar3,0x10);
          }
        }
        piVar3 = piStack_70;
        if (piStack_70 != (int *)0x0) {
          LOCK();
          piVar1 = piStack_70 + 1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (*piVar1 == 0) {
            (**(code **)(piStack_70 + 2))(piStack_70);
          }
          LOCK();
          *piVar3 = *piVar3 + -1;
          UNLOCK();
          if (*piVar3 == 0) {
            operator_delete(piVar3,0x10);
          }
        }
        if (piVar15 != (int *)0x0) break;
LAB_00674309:
        puVar18 = puVar18 + 2;
        local_68._8_8_ = puVar18;
        if (puVar12 == puVar18) goto LAB_00674421;
      }
      LOCK();
      piVar3 = piVar15 + 1;
      *piVar3 = *piVar3 + -1;
      UNLOCK();
      if (*piVar3 == 0) {
        (**(code **)(piVar15 + 2))(piVar15);
      }
      LOCK();
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 != 0) goto LAB_00674309;
      puVar18 = puVar18 + 2;
      operator_delete(piVar15,0x10);
      local_68._8_8_ = puVar18;
    } while (puVar12 != puVar18);
LAB_00674421:
    piVar15 = (int *)local_68._0_8_;
  }
  if (*piVar15 == 0) {
LAB_00674f40:
    uVar4 = local_68._0_8_;
    pQVar20 = (QArrayData *)(local_68._0_8_ + *(long *)(local_68._0_8_ + 0x10));
    pQVar19 = pQVar20 + (long)*(int *)(local_68._0_8_ + 4) * 0x10;
joined_r0x00674f5a:
    pQVar21 = pQVar20;
    if (pQVar20 != pQVar19) {
      do {
        pQVar20 = pQVar21 + 0x10;
        piVar15 = *(int **)(pQVar21 + 8);
        if (piVar15 != (int *)0x0) {
          LOCK();
          piVar3 = piVar15 + 1;
          *piVar3 = *piVar3 + -1;
          UNLOCK();
          if (*piVar3 == 0) {
            (**(code **)(piVar15 + 2))(piVar15);
          }
          LOCK();
          *piVar15 = *piVar15 + -1;
          UNLOCK();
          if (*piVar15 == 0) goto code_r0x00674f88;
        }
        pQVar21 = pQVar20;
        if (pQVar19 == pQVar20) break;
      } while( true );
    }
    QArrayData::deallocate((QArrayData *)uVar4,0x10,8);
    goto LAB_0067443f;
  }
  if (*piVar15 != -1) {
    LOCK();
    *piVar15 = *piVar15 + -1;
    UNLOCK();
    if (*piVar15 == 0) goto LAB_00674f40;
  }
LAB_0067443f:
  lVar11 = *(long *)(param_1 + 8);
  piVar15 = *(int **)(lVar11 + 0x18);
  if (*piVar15 == 0) {
    if (*(char *)((long)piVar15 + 0xb) < '\0') {
      piVar15 = (int *)QArrayData::allocate(0x10,8,(ulong)(piVar15[2] & 0x7fffffff),0);
      local_68._0_8_ = piVar15;
      piVar3 = piVar15;
      if (piVar15 == (int *)0x0) {
        qBadAlloc();
        piVar3 = (int *)local_68._0_8_;
      }
      local_68._0_8_ = piVar3;
      *(byte *)((long)piVar15 + 0xb) = *(byte *)((long)piVar15 + 0xb) | 0x80;
      lVar5 = *(long *)(piVar15 + 4);
      uVar2 = piVar15[2];
    }
    else {
      piVar15 = (int *)QArrayData::allocate(0x10,8,(long)piVar15[1],0);
      local_68._0_8_ = piVar15;
      piVar3 = piVar15;
      if (piVar15 == (int *)0x0) {
        qBadAlloc();
        piVar3 = (int *)local_68._0_8_;
      }
      local_68._0_8_ = piVar3;
      lVar5 = *(long *)(piVar15 + 4);
      uVar2 = piVar15[2];
    }
    if ((uVar2 & 0x7fffffff) == 0) {
      puVar18 = (undefined8 *)(lVar5 + (long)piVar15);
      lVar11 = (long)piVar15[1] << 4;
    }
    else {
      lVar6 = *(long *)(lVar11 + 0x18);
      puVar18 = (undefined8 *)(lVar5 + (long)piVar15);
      iVar16 = *(int *)(lVar6 + 4);
      puVar12 = (undefined8 *)(*(long *)(lVar6 + 0x10) + lVar6);
      puVar17 = puVar12 + (long)iVar16 * 2;
      if (puVar12 == puVar17) {
        lVar11 = 0;
      }
      else {
        do {
          uVar4 = puVar12[1];
          piVar15 = (int *)puVar12[1];
          *puVar18 = *puVar12;
          puVar18[1] = uVar4;
          if (piVar15 != (int *)0x0) {
            LOCK();
            *piVar15 = *piVar15 + 1;
            UNLOCK();
            LOCK();
            *(int *)(puVar18[1] + 4) = *(int *)(puVar18[1] + 4) + 1;
            UNLOCK();
          }
          puVar12 = puVar12 + 2;
          puVar18 = puVar18 + 2;
        } while (puVar12 != puVar17);
        iVar16 = *(int *)(*(long *)(lVar11 + 0x18) + 4);
        puVar18 = (undefined8 *)(*(long *)(local_68._0_8_ + 0x10) + local_68._0_8_);
        lVar11 = (long)iVar16 << 4;
        piVar15 = (int *)local_68._0_8_;
      }
      piVar15[1] = iVar16;
    }
  }
  else {
    if (*piVar15 != -1) {
      LOCK();
      *piVar15 = *piVar15 + 1;
      UNLOCK();
      piVar15 = *(int **)(lVar11 + 0x18);
    }
    lVar11 = (long)piVar15[1] << 4;
    puVar18 = (undefined8 *)(*(long *)(piVar15 + 4) + (long)piVar15);
    local_68._0_8_ = piVar15;
  }
  puVar12 = (undefined8 *)(lVar11 + (long)puVar18);
  local_68._8_8_ = puVar18;
  local_50 = 1;
  local_58 = puVar12;
  if (puVar18 != puVar12) {
    do {
      while( true ) {
        piVar15 = (int *)puVar18[1];
        uVar4 = *puVar18;
        if (piVar15 != (int *)0x0) {
          LOCK();
          *piVar15 = *piVar15 + 1;
          UNLOCK();
          LOCK();
          piVar15[1] = piVar15[1] + 1;
          UNLOCK();
        }
        lVar11 = *(long *)(this + 8);
                    /* try { // try from 0067451d to 00674521 has its CatchHandler @ 006750bf */
        pvVar13 = operator_new(0x10);
        local_78 = *(QArrayData **)(*(long *)(this + 8) + 0x70);
        piStack_70 = *(int **)(*(long *)(this + 8) + 0x78);
        if (piStack_70 != (int *)0x0) {
          LOCK();
          *piStack_70 = *piStack_70 + 1;
          UNLOCK();
          LOCK();
          piStack_70[1] = piStack_70[1] + 1;
          UNLOCK();
        }
                    /* try { // try from 00674559 to 0067455d has its CatchHandler @ 00675110 */
        FUN_00673a30(pvVar13,uVar4,param_2);
        local_88 = pvVar13;
                    /* try { // try from 00674568 to 0067456c has its CatchHandler @ 00675104 */
        local_80 = (int *)operator_new(0x18);
        *(void **)(local_80 + 4) = pvVar13;
        *(code **)(local_80 + 2) = FUN_0067ab50;
        local_80[1] = 1;
        *local_80 = 1;
                    /* try { // try from 00674597 to 0067459b has its CatchHandler @ 006750f8 */
        FUN_0067b950(lVar11 + 0x18,&local_88);
        piVar3 = local_80;
        if (local_80 != (int *)0x0) {
          LOCK();
          piVar1 = local_80 + 1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (*piVar1 == 0) {
            (**(code **)(local_80 + 2))(local_80);
          }
          LOCK();
          *piVar3 = *piVar3 + -1;
          UNLOCK();
          if (*piVar3 == 0) {
            operator_delete(piVar3,0x10);
          }
        }
        piVar3 = piStack_70;
        if (piStack_70 != (int *)0x0) {
          LOCK();
          piVar1 = piStack_70 + 1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (*piVar1 == 0) {
            (**(code **)(piStack_70 + 2))(piStack_70);
          }
          LOCK();
          *piVar3 = *piVar3 + -1;
          UNLOCK();
          if (*piVar3 == 0) {
            operator_delete(piVar3,0x10);
          }
        }
        if (piVar15 != (int *)0x0) break;
LAB_006744e1:
        puVar18 = puVar18 + 2;
        local_68._8_8_ = puVar18;
        if (puVar12 == puVar18) goto LAB_00674619;
      }
      LOCK();
      piVar3 = piVar15 + 1;
      *piVar3 = *piVar3 + -1;
      UNLOCK();
      if (*piVar3 == 0) {
        (**(code **)(piVar15 + 2))(piVar15);
      }
      LOCK();
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 != 0) goto LAB_006744e1;
      puVar18 = puVar18 + 2;
      operator_delete(piVar15,0x10);
      local_68._8_8_ = puVar18;
    } while (puVar12 != puVar18);
LAB_00674619:
    piVar15 = (int *)local_68._0_8_;
  }
  if (*piVar15 == 0) {
LAB_00674e40:
    uVar4 = local_68._0_8_;
    pQVar20 = (QArrayData *)(local_68._0_8_ + *(long *)(local_68._0_8_ + 0x10));
    pQVar19 = pQVar20 + (long)*(int *)(local_68._0_8_ + 4) * 0x10;
joined_r0x00674e5a:
    pQVar21 = pQVar20;
    if (pQVar20 != pQVar19) {
      do {
        pQVar20 = pQVar21 + 0x10;
        piVar15 = *(int **)(pQVar21 + 8);
        if (piVar15 != (int *)0x0) {
          LOCK();
          piVar3 = piVar15 + 1;
          *piVar3 = *piVar3 + -1;
          UNLOCK();
          if (*piVar3 == 0) {
            (**(code **)(piVar15 + 2))(piVar15);
          }
          LOCK();
          *piVar15 = *piVar15 + -1;
          UNLOCK();
          if (*piVar15 == 0) goto code_r0x00674e88;
        }
        pQVar21 = pQVar20;
        if (pQVar19 == pQVar20) break;
      } while( true );
    }
    QArrayData::deallocate((QArrayData *)uVar4,0x10,8);
    goto LAB_00674637;
  }
  if (*piVar15 != -1) {
    LOCK();
    *piVar15 = *piVar15 + -1;
    UNLOCK();
    if (*piVar15 == 0) goto LAB_00674e40;
  }
LAB_00674637:
  lVar11 = *(long *)(param_1 + 8);
  piVar15 = *(int **)(lVar11 + 0x20);
  if (*piVar15 == 0) {
    if (*(char *)((long)piVar15 + 0xb) < '\0') {
      piVar15 = (int *)QArrayData::allocate(0x10,8,(ulong)(piVar15[2] & 0x7fffffff),0);
      local_68._0_8_ = piVar15;
      piVar3 = piVar15;
      if (piVar15 == (int *)0x0) {
        qBadAlloc();
        piVar3 = (int *)local_68._0_8_;
      }
      local_68._0_8_ = piVar3;
      *(byte *)((long)piVar15 + 0xb) = *(byte *)((long)piVar15 + 0xb) | 0x80;
      lVar5 = *(long *)(piVar15 + 4);
      uVar2 = piVar15[2];
    }
    else {
      piVar15 = (int *)QArrayData::allocate(0x10,8,(long)piVar15[1],0);
      local_68._0_8_ = piVar15;
      piVar3 = piVar15;
      if (piVar15 == (int *)0x0) {
        qBadAlloc();
        piVar3 = (int *)local_68._0_8_;
      }
      local_68._0_8_ = piVar3;
      lVar5 = *(long *)(piVar15 + 4);
      uVar2 = piVar15[2];
    }
    if ((uVar2 & 0x7fffffff) == 0) {
      puVar12 = (undefined8 *)(lVar5 + (long)piVar15);
      lVar11 = (long)piVar15[1] << 4;
    }
    else {
      lVar6 = *(long *)(lVar11 + 0x20);
      puVar12 = (undefined8 *)(lVar5 + (long)piVar15);
      iVar16 = *(int *)(lVar6 + 4);
      puVar18 = (undefined8 *)(*(long *)(lVar6 + 0x10) + lVar6);
      if (puVar18 == puVar18 + (long)iVar16 * 2) {
        lVar11 = 0;
      }
      else {
        puVar17 = (undefined8 *)
                  (((long)(puVar18 + (long)iVar16 * 2) - (long)puVar18) + (long)puVar12);
        do {
          uVar4 = puVar18[1];
          piVar15 = (int *)puVar18[1];
          *puVar12 = *puVar18;
          puVar12[1] = uVar4;
          if (piVar15 != (int *)0x0) {
            LOCK();
            *piVar15 = *piVar15 + 1;
            UNLOCK();
            LOCK();
            *(int *)(puVar12[1] + 4) = *(int *)(puVar12[1] + 4) + 1;
            UNLOCK();
          }
          puVar12 = puVar12 + 2;
          puVar18 = puVar18 + 2;
        } while (puVar12 != puVar17);
        iVar16 = *(int *)(*(long *)(lVar11 + 0x20) + 4);
        puVar12 = (undefined8 *)(*(long *)(local_68._0_8_ + 0x10) + local_68._0_8_);
        lVar11 = (long)iVar16 << 4;
        piVar15 = (int *)local_68._0_8_;
      }
      piVar15[1] = iVar16;
    }
  }
  else {
    if (*piVar15 != -1) {
      LOCK();
      *piVar15 = *piVar15 + 1;
      UNLOCK();
      piVar15 = *(int **)(lVar11 + 0x20);
    }
    lVar11 = (long)piVar15[1] << 4;
    puVar12 = (undefined8 *)(*(long *)(piVar15 + 4) + (long)piVar15);
    local_68._0_8_ = piVar15;
  }
  puVar18 = (undefined8 *)(lVar11 + (long)puVar12);
  local_68._8_8_ = puVar12;
  local_50 = 1;
  local_58 = puVar18;
  if (puVar12 != puVar18) {
    do {
      while( true ) {
        piVar15 = (int *)puVar12[1];
        uVar4 = *puVar12;
        if (piVar15 != (int *)0x0) {
          LOCK();
          *piVar15 = *piVar15 + 1;
          UNLOCK();
          LOCK();
          piVar15[1] = piVar15[1] + 1;
          UNLOCK();
        }
        lVar11 = *(long *)(this + 8);
                    /* try { // try from 006746e8 to 006746ec has its CatchHandler @ 006750b3 */
        pvVar13 = operator_new(0x10);
        local_78 = *(QArrayData **)(*(long *)(this + 8) + 0x70);
        piStack_70 = *(int **)(*(long *)(this + 8) + 0x78);
        if (piStack_70 != (int *)0x0) {
          LOCK();
          *piStack_70 = *piStack_70 + 1;
          UNLOCK();
          LOCK();
          piStack_70[1] = piStack_70[1] + 1;
          UNLOCK();
        }
                    /* try { // try from 00674724 to 00674728 has its CatchHandler @ 00675134 */
        FUN_00673a30(pvVar13,uVar4,param_2,&local_78);
        local_88 = pvVar13;
                    /* try { // try from 00674733 to 00674737 has its CatchHandler @ 00675128 */
        local_80 = (int *)operator_new(0x18);
        *(void **)(local_80 + 4) = pvVar13;
        *(code **)(local_80 + 2) = FUN_0067ab50;
        local_80[1] = 1;
        *local_80 = 1;
                    /* try { // try from 00674762 to 00674766 has its CatchHandler @ 0067511c */
        FUN_0067b950(lVar11 + 0x20,&local_88);
        piVar3 = local_80;
        if (local_80 != (int *)0x0) {
          LOCK();
          piVar1 = local_80 + 1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (*piVar1 == 0) {
            (**(code **)(local_80 + 2))(local_80);
          }
          LOCK();
          *piVar3 = *piVar3 + -1;
          UNLOCK();
          if (*piVar3 == 0) {
            operator_delete(piVar3,0x10);
          }
        }
        piVar3 = piStack_70;
        if (piStack_70 != (int *)0x0) {
          LOCK();
          piVar1 = piStack_70 + 1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (*piVar1 == 0) {
            (**(code **)(piStack_70 + 2))(piStack_70);
          }
          LOCK();
          *piVar3 = *piVar3 + -1;
          UNLOCK();
          if (*piVar3 == 0) {
            operator_delete(piVar3,0x10);
          }
        }
        if (piVar15 != (int *)0x0) break;
LAB_006746ac:
        puVar12 = puVar12 + 2;
        local_68._8_8_ = puVar12;
        if (puVar18 == puVar12) goto LAB_00674809;
      }
      LOCK();
      piVar3 = piVar15 + 1;
      *piVar3 = *piVar3 + -1;
      UNLOCK();
      if (*piVar3 == 0) {
        (**(code **)(piVar15 + 2))(piVar15);
      }
      LOCK();
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 != 0) goto LAB_006746ac;
      puVar12 = puVar12 + 2;
      operator_delete(piVar15,0x10);
      local_68._8_8_ = puVar12;
    } while (puVar18 != puVar12);
LAB_00674809:
    piVar15 = (int *)local_68._0_8_;
  }
  if (*piVar15 == 0) {
LAB_00674ec0:
    uVar4 = local_68._0_8_;
    pQVar20 = (QArrayData *)(local_68._0_8_ + *(long *)(local_68._0_8_ + 0x10));
    pQVar19 = pQVar20 + (long)*(int *)(local_68._0_8_ + 4) * 0x10;
joined_r0x00674eda:
    pQVar21 = pQVar20;
    if (pQVar20 != pQVar19) {
      do {
        pQVar20 = pQVar21 + 0x10;
        piVar15 = *(int **)(pQVar21 + 8);
        if (piVar15 != (int *)0x0) {
          LOCK();
          piVar3 = piVar15 + 1;
          *piVar3 = *piVar3 + -1;
          UNLOCK();
          if (*piVar3 == 0) {
            (**(code **)(piVar15 + 2))(piVar15);
          }
          LOCK();
          *piVar15 = *piVar15 + -1;
          UNLOCK();
          if (*piVar15 == 0) goto code_r0x00674f08;
        }
        pQVar21 = pQVar20;
        if (pQVar19 == pQVar20) break;
      } while( true );
    }
    QArrayData::deallocate((QArrayData *)uVar4,0x10,8);
    goto LAB_00674827;
  }
  if (*piVar15 != -1) {
    LOCK();
    *piVar15 = *piVar15 + -1;
    UNLOCK();
    if (*piVar15 == 0) goto LAB_00674ec0;
  }
LAB_00674827:
  lVar11 = *(long *)(this + 8);
                    /* try { // try from 00674830 to 00674834 has its CatchHandler @ 006750d7 */
  pvVar13 = operator_new(0x10);
  piVar15 = *(int **)(*(long *)(this + 8) + 0x78);
  local_68 = *(undefined (*) [16])(*(long *)(this + 8) + 0x70);
  if (piVar15 != (int *)0x0) {
    LOCK();
    *piVar15 = *piVar15 + 1;
    UNLOCK();
    LOCK();
    piVar15[1] = piVar15[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 00674876 to 0067487a has its CatchHandler @ 006750cb */
  FUN_0067bbb0(pvVar13,*(undefined8 *)(*(long *)(param_1 + 8) + 0x28),param_2,local_68);
                    /* try { // try from 00674880 to 00674884 has its CatchHandler @ 00675047 */
  puVar14 = (undefined4 *)operator_new(0x18);
  *(void **)(puVar14 + 4) = pvVar13;
  *(code **)(puVar14 + 2) = FUN_0067ab70;
  puVar14[1] = 1;
  *puVar14 = 1;
  *(void **)(lVar11 + 0x28) = pvVar13;
  piVar15 = *(int **)(lVar11 + 0x30);
  *(undefined4 **)(lVar11 + 0x30) = puVar14;
  if (piVar15 != (int *)0x0) {
    LOCK();
    piVar3 = piVar15 + 1;
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      (**(code **)(piVar15 + 2))(piVar15);
    }
    LOCK();
    *piVar15 = *piVar15 + -1;
    UNLOCK();
    if (*piVar15 == 0) {
      operator_delete(piVar15,0x10);
    }
  }
  uVar4 = local_68._8_8_;
  if ((int *)local_68._8_8_ != (int *)0x0) {
    LOCK();
    piVar15 = (int *)(local_68._8_8_ + 4);
    *piVar15 = *piVar15 + -1;
    UNLOCK();
    if (*piVar15 == 0) {
      (**(code **)(local_68._8_8_ + 8))(local_68._8_8_);
    }
    LOCK();
    *(int *)uVar4 = *(int *)uVar4 + -1;
    UNLOCK();
    if (*(int *)uVar4 == 0) {
      operator_delete((void *)uVar4,0x10);
    }
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
code_r0x00674f88:
  operator_delete(piVar15,0x10);
  goto joined_r0x00674f5a;
code_r0x00674e88:
  operator_delete(piVar15,0x10);
  goto joined_r0x00674e5a;
code_r0x00674f08:
  operator_delete(piVar15,0x10);
  goto joined_r0x00674eda;
}



// ====== KisLayerStyleProjectionPlane @ 006760a0 ======

/* KisLayerStyleProjectionPlane::KisLayerStyleProjectionPlane(KisLayer*) */

void __thiscall
KisLayerStyleProjectionPlane::KisLayerStyleProjectionPlane
          (KisLayerStyleProjectionPlane *this,KisLayer *param_1)

{
  int *piVar1;
  int *piVar2;
  undefined *puVar3;
  undefined8 uVar4;
  undefined (*pauVar5) [16];
  KisPSDLayerStyle *this_00;
  int *piVar6;
  long in_FS_OFFSET;
  QArrayData *local_60;
  KisPSDLayerStyle *local_58;
  int *local_50;
  undefined local_48 [24];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisAbstractProjectionPlane::KisAbstractProjectionPlane((KisAbstractProjectionPlane *)this);
  *(undefined **)this = PTR_vtable_00837ca8 + 0x10;
                    /* try { // try from 006760de to 006760e2 has its CatchHandler @ 00676350 */
  pauVar5 = (undefined (*) [16])operator_new(0x88);
  *(undefined (**) [16])(this + 8) = pauVar5;
  puVar3 = PTR_shared_null_008377d0;
  *(undefined8 *)(pauVar5[4] + 8) = 0;
  *(undefined8 *)pauVar5[6] = 0;
  *(undefined8 *)(pauVar5[7] + 8) = 0;
  *(undefined2 *)pauVar5[8] = 0;
  *pauVar5 = (undefined  [16])0x0;
  *(undefined **)pauVar5[1] = puVar3;
  *(undefined **)(pauVar5[1] + 8) = puVar3;
  *(undefined **)pauVar5[2] = puVar3;
  *(undefined (*) [16])(pauVar5[2] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar5[3] + 8) = (undefined  [16])0x0;
  pauVar5[5] = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar5[6] + 8) = (undefined  [16])0x0;
                    /* try { // try from 00676140 to 00676144 has its CatchHandler @ 0067638c */
  KisLayer::layerStyle();
  if (local_58 != (KisPSDLayerStyle *)0x0) goto LAB_00676158;
                    /* try { // try from 00676223 to 00676231 has its CatchHandler @ 00676380 */
  kis_assert_recoverable
            ("style",
             "/builds/graphics/krita/libs/image/layerstyles/kis_layer_style_projection_plane.cpp",
             0x5f);
  this_00 = (KisPSDLayerStyle *)operator_new(0x18);
  local_48._0_16_ = (undefined  [16])0x0;
                    /* try { // try from 00676247 to 0067624b has its CatchHandler @ 00676374 */
  local_60 = (QArrayData *)QString::fromAscii_helper("",0);
                    /* try { // try from 0067625f to 00676263 has its CatchHandler @ 00676368 */
  KisPSDLayerStyle::KisPSDLayerStyle(this_00,(QString *)&local_60,(QSharedPointer)local_48);
                    /* try { // try from 00676269 to 0067626d has its CatchHandler @ 00676344 */
  piVar6 = (int *)operator_new(0x18);
  piVar1 = local_50;
  *(KisPSDLayerStyle **)(piVar6 + 4) = this_00;
  *(code **)(piVar6 + 2) = FUN_0067ab30;
  piVar6[1] = 1;
  *piVar6 = 1;
  local_58 = this_00;
  if (local_50 != (int *)0x0) {
    LOCK();
    piVar2 = local_50 + 1;
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (*piVar2 == 0) {
      piVar2 = local_50 + 2;
      local_50 = piVar6;
      (**(code **)piVar2)(piVar1);
      piVar6 = local_50;
    }
    local_50 = piVar6;
    LOCK();
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    piVar6 = local_50;
    if (*piVar1 == 0) {
      operator_delete(piVar1,0x10);
      piVar6 = local_50;
    }
  }
  local_50 = piVar6;
  if (*(int *)local_60 == 0) {
LAB_00676300:
    QArrayData::deallocate(local_60,2,8);
  }
  else if (*(int *)local_60 != -1) {
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 == 0) goto LAB_00676300;
  }
  uVar4 = local_48._8_8_;
  if ((int *)local_48._8_8_ != (int *)0x0) {
    LOCK();
    piVar1 = (int *)(local_48._8_8_ + 4);
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_48._8_8_ + 8))(local_48._8_8_);
    }
    LOCK();
    *(int *)uVar4 = *(int *)uVar4 + -1;
    UNLOCK();
    if (*(int *)uVar4 == 0) {
      operator_delete((void *)uVar4,0x10);
    }
  }
LAB_00676158:
  local_48._0_8_ = local_58;
  local_48._8_8_ = local_50;
  if (local_50 != (int *)0x0) {
    LOCK();
    *local_50 = *local_50 + 1;
    UNLOCK();
    LOCK();
    local_50[1] = local_50[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 0067618c to 00676190 has its CatchHandler @ 0067635c */
  init(this,param_1,(QSharedPointer)local_48);
  uVar4 = local_48._8_8_;
  if ((int *)local_48._8_8_ != (int *)0x0) {
    LOCK();
    piVar1 = (int *)(local_48._8_8_ + 4);
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_48._8_8_ + 8))(local_48._8_8_);
    }
    LOCK();
    *(int *)uVar4 = *(int *)uVar4 + -1;
    UNLOCK();
    if (*(int *)uVar4 == 0) {
      operator_delete((void *)uVar4,0x10);
    }
  }
  piVar1 = local_50;
  if (local_50 != (int *)0x0) {
    LOCK();
    piVar6 = local_50 + 1;
    *piVar6 = *piVar6 + -1;
    UNLOCK();
    if (*piVar6 == 0) {
      (**(code **)(local_50 + 2))(local_50);
    }
    LOCK();
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      operator_delete(piVar1,0x10);
    }
  }
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisLayerStyleProjectionPlane @ 00676410 ======

/* KisLayerStyleProjectionPlane::KisLayerStyleProjectionPlane(KisLayer*,
   QSharedPointer<KisPSDLayerStyle>) */

void __thiscall
KisLayerStyleProjectionPlane::KisLayerStyleProjectionPlane
          (KisLayerStyleProjectionPlane *this,KisLayer *param_1,QSharedPointer param_2)

{
  int *piVar1;
  undefined *puVar2;
  int *piVar3;
  undefined (*pauVar4) [16];
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  undefined8 local_38;
  int *piStack_30;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisAbstractProjectionPlane::KisAbstractProjectionPlane((KisAbstractProjectionPlane *)this);
  *(undefined **)this = PTR_vtable_00837ca8 + 0x10;
                    /* try { // try from 0067644d to 00676451 has its CatchHandler @ 00676530 */
  pauVar4 = (undefined (*) [16])operator_new(0x88);
  puVar2 = PTR_shared_null_008377d0;
  *(undefined (**) [16])(this + 8) = pauVar4;
  local_38 = *(undefined8 *)CONCAT44(in_register_00000014,param_2);
  piStack_30 = (int *)((undefined8 *)CONCAT44(in_register_00000014,param_2))[1];
  *(undefined8 *)(pauVar4[4] + 8) = 0;
  *(undefined8 *)pauVar4[6] = 0;
  *(undefined8 *)(pauVar4[7] + 8) = 0;
  *(undefined2 *)pauVar4[8] = 0;
  *pauVar4 = (undefined  [16])0x0;
  *(undefined **)pauVar4[1] = puVar2;
  *(undefined **)(pauVar4[1] + 8) = puVar2;
  *(undefined **)pauVar4[2] = puVar2;
  *(undefined (*) [16])(pauVar4[2] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar4[3] + 8) = (undefined  [16])0x0;
  pauVar4[5] = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar4[6] + 8) = (undefined  [16])0x0;
  if (piStack_30 != (int *)0x0) {
    LOCK();
    *piStack_30 = *piStack_30 + 1;
    UNLOCK();
    LOCK();
    piStack_30[1] = piStack_30[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 006764d2 to 006764d6 has its CatchHandler @ 00676524 */
  init(this,param_1,(QSharedPointer)&local_38);
  piVar3 = piStack_30;
  if (piStack_30 != (int *)0x0) {
    LOCK();
    piVar1 = piStack_30 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piStack_30 + 2))(piStack_30);
    }
    LOCK();
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      operator_delete(piVar3,0x10);
    }
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



