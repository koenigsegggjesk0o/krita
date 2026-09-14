/* Class KisLayerComposition - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLayerComposition @ 00209ed0 ======

void __thiscall
KisLayerComposition::KisLayerComposition
          (KisLayerComposition *this,KisLayerComposition *param_1,KisWeakSharedPtr param_2)

{
  (*(code *)PTR_KisLayerComposition_0083ca38)();
  return;
}



// ====== KisLayerComposition @ 0063e310 ======

/* KisLayerComposition::KisLayerComposition(KisWeakSharedPtr<KisImage>, QString const&) */

void __thiscall
KisLayerComposition::KisLayerComposition
          (KisLayerComposition *this,KisWeakSharedPtr param_1,QString *param_2)

{
  uint *puVar1;
  long lVar2;
  undefined *puVar3;
  int *piVar4;
  undefined4 in_register_00000034;
  
  puVar1 = *(uint **)(CONCAT44(in_register_00000034,param_1) + 8);
  if (*(long *)CONCAT44(in_register_00000034,param_1) == 0) {
    *(undefined8 *)this = 0;
  }
  else {
    if ((puVar1 == (uint *)0x0) || ((*puVar1 & 1) == 0)) {
      *(undefined (*) [16])this = (undefined  [16])0x0;
      goto LAB_0063e35f;
    }
    lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
    *(long *)this = lVar2;
    if (lVar2 != 0) {
      piVar4 = *(int **)(lVar2 + 0x58);
      if (piVar4 == (int *)0x0) {
        piVar4 = (int *)operator_new(4);
        *piVar4 = 0;
        *(int **)(lVar2 + 0x58) = piVar4;
        LOCK();
        *piVar4 = *piVar4 + 1;
        UNLOCK();
        piVar4 = *(int **)(lVar2 + 0x58);
      }
      *(int **)(this + 8) = piVar4;
      LOCK();
      *piVar4 = *piVar4 + 2;
      UNLOCK();
      goto LAB_0063e35f;
    }
  }
  *(undefined8 *)(this + 8) = 0;
LAB_0063e35f:
  piVar4 = *(int **)param_2;
  *(int **)(this + 0x10) = piVar4;
  if (1 < *piVar4 + 1U) {
    LOCK();
    *piVar4 = *piVar4 + 1;
    UNLOCK();
  }
  puVar3 = PTR_shared_null_008372c0;
  this[0x28] = (KisLayerComposition)0x1;
  *(undefined **)(this + 0x18) = puVar3;
  *(undefined **)(this + 0x20) = puVar3;
  return;
}



// ====== KisLayerComposition @ 0063e8f0 ======

/* KisLayerComposition::KisLayerComposition(KisLayerComposition const&, KisWeakSharedPtr<KisImage>)
    */

void __thiscall
KisLayerComposition::KisLayerComposition
          (KisLayerComposition *this,KisLayerComposition *param_1,KisWeakSharedPtr param_2)

{
  QTextStream *pQVar1;
  KisBaseNode *pKVar2;
  long *plVar3;
  ulong uVar4;
  undefined *puVar5;
  char cVar6;
  uint *puVar7;
  long lVar8;
  long lVar9;
  ulong *puVar10;
  int *piVar11;
  undefined4 in_register_00000014;
  KisLayerComposition *pKVar12;
  int iVar13;
  QMapNodeBase *pQVar14;
  uint *puVar15;
  long in_FS_OFFSET;
  undefined auVar16 [16];
  uint *local_c0;
  long *local_a8;
  KisNodeQueryPath local_a0 [8];
  KisBaseNode *local_98;
  QTextStream *local_90;
  long *local_88;
  int *piStack_80;
  undefined8 uStack_78;
  undefined8 local_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined local_58 [16];
  long local_40;
  
  pKVar12 = (KisLayerComposition *)CONCAT44(in_register_00000014,param_2);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if ((((*(long *)pKVar12 == 0) || (*(uint **)(pKVar12 + 8) == (uint *)0x0)) ||
      ((**(uint **)(pKVar12 + 8) & 1) == 0)) || (*(long *)pKVar12 == 0)) {
    puVar7 = *(uint **)(param_1 + 8);
    pKVar12 = param_1;
    if (*(long *)param_1 == 0) goto LAB_0063f37b;
LAB_0063e946:
    if ((puVar7 == (uint *)0x0) || ((*puVar7 & 1) == 0)) {
      *(undefined (*) [16])this = (undefined  [16])0x0;
      goto LAB_0063e95e;
    }
    lVar8 = *(long *)pKVar12;
    *(long *)this = lVar8;
    if (lVar8 != 0) {
      piVar11 = *(int **)(lVar8 + 0x58);
      if (piVar11 == (int *)0x0) {
        piVar11 = (int *)operator_new(4);
        *piVar11 = 0;
        *(int **)(lVar8 + 0x58) = piVar11;
        LOCK();
        *piVar11 = *piVar11 + 1;
        UNLOCK();
        piVar11 = *(int **)(lVar8 + 0x58);
      }
      *(int **)(this + 8) = piVar11;
      LOCK();
      *piVar11 = *piVar11 + 2;
      UNLOCK();
      goto LAB_0063e95e;
    }
  }
  else {
    puVar7 = *(uint **)(pKVar12 + 8);
    if (*(long *)pKVar12 != 0) goto LAB_0063e946;
LAB_0063f37b:
    *(undefined8 *)this = 0;
  }
  *(undefined8 *)(this + 8) = 0;
LAB_0063e95e:
  auVar16._8_8_ = local_58._8_8_;
  auVar16._0_8_ = local_58._0_8_;
  piVar11 = *(int **)(param_1 + 0x10);
  *(int **)(this + 0x10) = piVar11;
  if (1 < *piVar11 + 1U) {
    LOCK();
    *piVar11 = *piVar11 + 1;
    UNLOCK();
  }
  puVar5 = PTR_shared_null_008372c0;
  this[0x28] = param_1[0x28];
  *(undefined **)(this + 0x18) = puVar5;
  *(undefined **)(this + 0x20) = puVar5;
  lVar8 = *(long *)(param_1 + 0x18);
  if (*(long *)(lVar8 + 0x10) != 0) {
    lVar9 = *(long *)(lVar8 + 0x20);
    local_58 = auVar16;
    if (lVar9 != lVar8 + 8) {
      do {
        local_68 = *(undefined8 *)(lVar9 + 0x18);
        uStack_60 = *(undefined8 *)(lVar9 + 0x20);
        local_58 = auVar16;
        if (((*(long *)param_1 == 0) || (*(uint **)(param_1 + 8) == (uint *)0x0)) ||
           ((**(uint **)(param_1 + 8) & 1) == 0)) {
                    /* try { // try from 0063e9e1 to 0063ea03 has its CatchHandler @ 0063f4eb */
          lVar8 = _41000();
          if (*(char *)(lVar8 + 0x11) != '\0') {
                    /* try { // try from 0063f268 to 0063f29c has its CatchHandler @ 0063f4eb */
            lVar8 = _41000();
            local_70 = *(undefined8 *)(lVar8 + 8);
            piStack_80 = (int *)0x0;
            uStack_78 = 0;
            local_88 = DAT_00721760;
            QMessageLogger::warning();
            if (1 < *(int *)(local_90 + 0x28)) {
              *(uint *)(local_90 + 0x48) = *(uint *)(local_90 + 0x48) | 1;
            }
                    /* try { // try from 0063f2b4 to 0063f2b8 has its CatchHandler @ 0063f4d3 */
            kisBacktrace();
                    /* try { // try from 0063f2c8 to 0063f2cc has its CatchHandler @ 0063f51b */
            QDebug::putString((QChar *)&local_90,(ulong)(local_98 + *(long *)(local_98 + 0x10)));
            if (local_90[0x20] != (QTextStream)0x0) {
                    /* try { // try from 0063f40d to 0063f411 has its CatchHandler @ 0063f51b */
              QTextStream::operator<<(local_90,' ');
            }
            if (*(int *)local_98 == 0) {
LAB_0063f3b8:
              QArrayData::deallocate((QArrayData *)local_98,2,8);
              QDebug::~QDebug((QDebug *)&local_90);
            }
            else {
              if (*(int *)local_98 != -1) {
                LOCK();
                *(int *)local_98 = *(int *)local_98 + -1;
                UNLOCK();
                if (*(int *)local_98 == 0) goto LAB_0063f3b8;
              }
              QDebug::~QDebug((QDebug *)&local_90);
            }
          }
        }
        KisNodeFacade::root();
                    /* try { // try from 0063ea13 to 0063ea17 has its CatchHandler @ 0063f4df */
        KisLayerUtils::findNodeByUuid
                  ((KisLayerUtils *)&local_a8,(KisSharedPtr)&local_88,(QUuid *)&local_68);
        if (local_88 != (long *)0x0) {
          LOCK();
          plVar3 = local_88 + 2;
          *(int *)plVar3 = *(int *)plVar3 + -1;
          UNLOCK();
          if (*(int *)plVar3 == 0) {
            (**(code **)(*local_88 + 0x20))();
          }
        }
        if (local_a8 != (long *)0x0) {
          local_88 = local_a8;
          LOCK();
          *(int *)(local_a8 + 2) = *(int *)(local_a8 + 2) + 1;
          UNLOCK();
                    /* try { // try from 0063ea55 to 0063ea59 has its CatchHandler @ 0063f4c7 */
          KisNodeQueryPath::absolutePath(local_a0,(KisSharedPtr)&local_88);
          if (local_88 != (long *)0x0) {
            LOCK();
            plVar3 = local_88 + 2;
            *(int *)plVar3 = *(int *)plVar3 + -1;
            UNLOCK();
            if (*(int *)plVar3 == 0) {
              (**(code **)(*local_88 + 0x20))();
            }
          }
          local_90 = (QTextStream *)0x0;
          if (*(long *)this == 0) {
            local_88 = (long *)0x0;
LAB_0063ef91:
            piStack_80 = (int *)0x0;
          }
          else if ((*(uint **)(this + 8) == (uint *)0x0) || ((**(uint **)(this + 8) & 1) == 0)) {
            local_88 = (long *)0x0;
            piStack_80 = (int *)0x0;
          }
          else {
            plVar3 = *(long **)this;
            local_88 = plVar3;
            if (plVar3 == (long *)0x0) goto LAB_0063ef91;
            piVar11 = (int *)plVar3[0xb];
            if (piVar11 == (int *)0x0) {
                    /* try { // try from 0063f41c to 0063f420 has its CatchHandler @ 0063f50f */
              piVar11 = (int *)operator_new(4);
              *piVar11 = 0;
              plVar3[0xb] = (long)piVar11;
              LOCK();
              *piVar11 = *piVar11 + 1;
              UNLOCK();
              piVar11 = (int *)plVar3[0xb];
            }
            LOCK();
            *piVar11 = *piVar11 + 2;
            UNLOCK();
            piStack_80 = piVar11;
          }
                    /* try { // try from 0063eade to 0063eae2 has its CatchHandler @ 0063f4bb */
          KisNodeQueryPath::queryUniqueNode((KisWeakSharedPtr)&local_98,(KisSharedPtr)local_a0);
          local_88 = (long *)0x0;
          if (piStack_80 != (int *)0x0) {
            LOCK();
            iVar13 = *piStack_80;
            *piStack_80 = *piStack_80 + -2;
            UNLOCK();
            if ((iVar13 < 3) && (piStack_80 != (int *)0x0)) {
              operator_delete(piStack_80,4);
            }
          }
          if (local_90 != (QTextStream *)0x0) {
            LOCK();
            pQVar1 = local_90 + 0x10;
            *(int *)pQVar1 = *(int *)pQVar1 + -1;
            UNLOCK();
            if (*(int *)pQVar1 == 0) {
              (**(code **)(*(long *)local_90 + 0x20))();
            }
          }
          if (local_98 == (KisBaseNode *)0x0) {
                    /* try { // try from 0063f1b3 to 0063f1b7 has its CatchHandler @ 0063f4af */
            kis_assert_recoverable
                      ("newNode","/builds/graphics/krita/libs/image/kis_layer_composition.cpp",0x74)
            ;
          }
          else {
                    /* try { // try from 0063eb4e to 0063ec12 has its CatchHandler @ 0063f4af */
            auVar16 = KisBaseNode::uuid(local_98);
            puVar7 = *(uint **)(this + 0x18);
            local_58 = auVar16;
            if (*puVar7 < 2) {
              puVar15 = *(uint **)(puVar7 + 4);
              if (puVar15 == (uint *)0x0) goto LAB_0063ef5f;
LAB_0063eb7a:
              local_c0 = (uint *)0x0;
              do {
                while( true ) {
                  puVar7 = puVar15;
                  cVar6 = QUuid::operator<((QUuid *)(puVar7 + 6),(QUuid *)local_58);
                  if (cVar6 == '\0') break;
                  puVar15 = *(uint **)(puVar7 + 4);
                  if (*(uint **)(puVar7 + 4) == (uint *)0x0) {
                    if (local_c0 == (uint *)0x0) {
                      iVar13 = (int)*(undefined8 *)(this + 0x18);
                      goto LAB_0063ebfe;
                    }
                    goto LAB_0063ebd6;
                  }
                }
                puVar15 = *(uint **)(puVar7 + 2);
                local_c0 = puVar7;
              } while (*(uint **)(puVar7 + 2) != (uint *)0x0);
LAB_0063ebd6:
              cVar6 = QUuid::operator<((QUuid *)local_58,(QUuid *)(local_c0 + 6));
              if (cVar6 == '\0') {
                *(undefined *)(local_c0 + 10) = *(undefined *)(lVar9 + 0x28);
                goto LAB_0063ec25;
              }
              iVar13 = (int)*(undefined8 *)(this + 0x18);
            }
            else {
                    /* try { // try from 0063eef0 to 0063ef4c has its CatchHandler @ 0063f4af */
              lVar8 = QMapDataBase::createData();
              pQVar14 = *(QMapNodeBase **)(this + 0x18);
              if (*(long *)(pQVar14 + 0x10) != 0) {
                puVar10 = (ulong *)FUN_00640c10(*(long *)(pQVar14 + 0x10),lVar8);
                uVar4 = *puVar10;
                pQVar14 = *(QMapNodeBase **)(this + 0x18);
                *(ulong **)(lVar8 + 0x10) = puVar10;
                *puVar10 = (ulong)((uint)uVar4 & 3) | lVar8 + 8U;
              }
              if (*(int *)pQVar14 == 0) {
LAB_0063f340:
                if (*(long *)(pQVar14 + 0x10) != 0) {
                    /* try { // try from 0063f351 to 0063f35d has its CatchHandler @ 0063f4af */
                  QMapDataBase::freeTree(pQVar14,(int)*(long *)(pQVar14 + 0x10));
                }
                QMapDataBase::freeData((QMapDataBase *)pQVar14);
              }
              else if (*(int *)pQVar14 != -1) {
                LOCK();
                *(int *)pQVar14 = *(int *)pQVar14 + -1;
                UNLOCK();
                if (*(int *)pQVar14 == 0) {
                  pQVar14 = *(QMapNodeBase **)(this + 0x18);
                  goto LAB_0063f340;
                }
              }
              *(long *)(this + 0x18) = lVar8;
              QMapDataBase::recalcMostLeftNode();
              puVar7 = *(uint **)(this + 0x18);
              puVar15 = *(uint **)(puVar7 + 4);
              if (puVar15 != (uint *)0x0) goto LAB_0063eb7a;
LAB_0063ef5f:
              iVar13 = (int)puVar7;
              puVar7 = puVar7 + 2;
            }
LAB_0063ebfe:
            lVar8 = QMapDataBase::createNode
                              (iVar13,0x30,(QMapNodeBase *)&DAT_00000008,SUB81(puVar7,0));
            *(undefined (*) [16])(lVar8 + 0x18) = local_58;
            *(undefined *)(lVar8 + 0x28) = *(undefined *)(lVar9 + 0x28);
          }
LAB_0063ec25:
          if (local_98 != (KisBaseNode *)0x0) {
            LOCK();
            pKVar2 = local_98 + 0x10;
            *(int *)pKVar2 = *(int *)pKVar2 + -1;
            UNLOCK();
            if (*(int *)pKVar2 == 0) {
              (**(code **)(*(long *)local_98 + 0x20))();
            }
          }
          KisNodeQueryPath::~KisNodeQueryPath(local_a0);
          if (local_a8 != (long *)0x0) {
            LOCK();
            plVar3 = local_a8 + 2;
            *(int *)plVar3 = *(int *)plVar3 + -1;
            UNLOCK();
            if (*(int *)plVar3 == 0) {
              (**(code **)(*local_a8 + 0x20))();
            }
          }
        }
                    /* try { // try from 0063ec60 to 0063eceb has its CatchHandler @ 0063f4eb */
        lVar9 = QMapNodeBase::nextNode();
        auVar16 = local_58;
      } while (lVar9 != *(long *)(param_1 + 0x18) + 8);
    }
  }
  lVar8 = *(long *)(param_1 + 0x20);
  if (*(long *)(lVar8 + 0x10) != 0) {
    lVar9 = *(long *)(lVar8 + 0x20);
    if (lVar9 != lVar8 + 8) {
      do {
        local_68 = *(undefined8 *)(lVar9 + 0x18);
        uStack_60 = *(undefined8 *)(lVar9 + 0x20);
        if (((*(long *)param_1 == 0) || (*(uint **)(param_1 + 8) == (uint *)0x0)) ||
           ((**(uint **)(param_1 + 8) & 1) == 0)) {
          lVar8 = _41000();
          if (*(char *)(lVar8 + 0x11) != '\0') {
                    /* try { // try from 0063f1c0 to 0063f1f4 has its CatchHandler @ 0063f4eb */
            lVar8 = _41000();
            local_70 = *(undefined8 *)(lVar8 + 8);
            piStack_80 = (int *)0x0;
            uStack_78 = 0;
            local_88 = DAT_00721760;
            QMessageLogger::warning();
            if (1 < *(int *)(local_90 + 0x28)) {
              *(uint *)(local_90 + 0x48) = *(uint *)(local_90 + 0x48) | 1;
            }
                    /* try { // try from 0063f20c to 0063f210 has its CatchHandler @ 0063f4a3 */
            kisBacktrace();
                    /* try { // try from 0063f220 to 0063f224 has its CatchHandler @ 0063f4f7 */
            QDebug::putString((QChar *)&local_90,(ulong)(local_98 + *(long *)(local_98 + 0x10)));
            if (local_90[0x20] != (QTextStream)0x0) {
                    /* try { // try from 0063f3fe to 0063f402 has its CatchHandler @ 0063f4f7 */
              QTextStream::operator<<(local_90,' ');
            }
            if (*(int *)local_98 == 0) {
LAB_0063f3d4:
              QArrayData::deallocate((QArrayData *)local_98,2,8);
              QDebug::~QDebug((QDebug *)&local_90);
            }
            else {
              if (*(int *)local_98 != -1) {
                LOCK();
                *(int *)local_98 = *(int *)local_98 + -1;
                UNLOCK();
                if (*(int *)local_98 == 0) goto LAB_0063f3d4;
              }
              QDebug::~QDebug((QDebug *)&local_90);
            }
          }
        }
        KisNodeFacade::root();
                    /* try { // try from 0063ecfb to 0063ecff has its CatchHandler @ 0063f503 */
        KisLayerUtils::findNodeByUuid
                  ((KisLayerUtils *)&local_a8,(KisSharedPtr)&local_88,(QUuid *)&local_68);
        if (local_88 != (long *)0x0) {
          LOCK();
          plVar3 = local_88 + 2;
          *(int *)plVar3 = *(int *)plVar3 + -1;
          UNLOCK();
          if (*(int *)plVar3 == 0) {
            (**(code **)(*local_88 + 0x20))();
          }
        }
        if (local_a8 != (long *)0x0) {
          local_88 = local_a8;
          LOCK();
          *(int *)(local_a8 + 2) = *(int *)(local_a8 + 2) + 1;
          UNLOCK();
                    /* try { // try from 0063ed45 to 0063ed49 has its CatchHandler @ 0063f48b */
          KisNodeQueryPath::absolutePath(local_a0,(KisSharedPtr)&local_88);
          if (local_88 != (long *)0x0) {
            LOCK();
            plVar3 = local_88 + 2;
            *(int *)plVar3 = *(int *)plVar3 + -1;
            UNLOCK();
            if (*(int *)plVar3 == 0) {
              (**(code **)(*local_88 + 0x20))();
            }
          }
          local_90 = (QTextStream *)0x0;
          if (*(long *)this == 0) {
            local_88 = (long *)0x0;
LAB_0063f171:
            piStack_80 = (int *)0x0;
          }
          else if ((*(uint **)(this + 8) == (uint *)0x0) || ((**(uint **)(this + 8) & 1) == 0)) {
            local_88 = (long *)0x0;
            piStack_80 = (int *)0x0;
          }
          else {
            plVar3 = *(long **)this;
            local_88 = plVar3;
            if (plVar3 == (long *)0x0) goto LAB_0063f171;
            piVar11 = (int *)plVar3[0xb];
            if (piVar11 == (int *)0x0) {
                    /* try { // try from 0063f43d to 0063f441 has its CatchHandler @ 0063f527 */
              piVar11 = (int *)operator_new(4);
              *piVar11 = 0;
              plVar3[0xb] = (long)piVar11;
              LOCK();
              *piVar11 = *piVar11 + 1;
              UNLOCK();
              piVar11 = (int *)plVar3[0xb];
            }
            LOCK();
            *piVar11 = *piVar11 + 2;
            UNLOCK();
            piStack_80 = piVar11;
          }
                    /* try { // try from 0063edce to 0063edd2 has its CatchHandler @ 0063f47f */
          KisNodeQueryPath::queryUniqueNode((KisWeakSharedPtr)&local_98,(KisSharedPtr)local_a0);
          local_88 = (long *)0x0;
          if (piStack_80 != (int *)0x0) {
            LOCK();
            iVar13 = *piStack_80;
            *piStack_80 = *piStack_80 + -2;
            UNLOCK();
            if ((iVar13 < 3) && (piStack_80 != (int *)0x0)) {
              operator_delete(piStack_80,4);
            }
          }
          if (local_90 != (QTextStream *)0x0) {
            LOCK();
            pQVar1 = local_90 + 0x10;
            *(int *)pQVar1 = *(int *)pQVar1 + -1;
            UNLOCK();
            if (*(int *)pQVar1 == 0) {
              (**(code **)(*(long *)local_90 + 0x20))();
            }
          }
          if (local_98 == (KisBaseNode *)0x0) {
            kis_assert_recoverable
                      ("newNode","/builds/graphics/krita/libs/image/kis_layer_composition.cpp",0x83)
            ;
          }
          else {
                    /* try { // try from 0063ee3e to 0063ee42 has its CatchHandler @ 0063f497 */
            auVar16 = KisBaseNode::uuid(local_98);
            puVar7 = *(uint **)(this + 0x20);
            local_58 = auVar16;
            if (*puVar7 < 2) {
              puVar15 = *(uint **)(puVar7 + 4);
              if (puVar15 == (uint *)0x0) goto LAB_0063f13f;
LAB_0063ee6a:
              local_c0 = (uint *)0x0;
              do {
                while( true ) {
                  puVar7 = puVar15;
                  cVar6 = QUuid::operator<((QUuid *)(puVar7 + 6),(QUuid *)local_58);
                  if (cVar6 == '\0') break;
                  puVar15 = *(uint **)(puVar7 + 4);
                  if (*(uint **)(puVar7 + 4) == (uint *)0x0) {
                    if (local_c0 == (uint *)0x0) {
                      iVar13 = (int)*(undefined8 *)(this + 0x20);
                      goto LAB_0063efe6;
                    }
                    goto LAB_0063efbe;
                  }
                }
                puVar15 = *(uint **)(puVar7 + 2);
                local_c0 = puVar7;
              } while (*(uint **)(puVar7 + 2) != (uint *)0x0);
LAB_0063efbe:
              cVar6 = QUuid::operator<((QUuid *)local_58,(QUuid *)(local_c0 + 6));
              if (cVar6 == '\0') {
                *(undefined *)(local_c0 + 10) = *(undefined *)(lVar9 + 0x28);
                goto LAB_0063f00d;
              }
              iVar13 = (int)*(undefined8 *)(this + 0x20);
            }
            else {
                    /* try { // try from 0063f0d0 to 0063f197 has its CatchHandler @ 0063f497 */
              lVar8 = QMapDataBase::createData();
              pQVar14 = *(QMapNodeBase **)(this + 0x20);
              if (*(long *)(pQVar14 + 0x10) != 0) {
                puVar10 = (ulong *)FUN_00640c10(*(long *)(pQVar14 + 0x10),lVar8);
                uVar4 = *puVar10;
                pQVar14 = *(QMapNodeBase **)(this + 0x20);
                *(ulong **)(lVar8 + 0x10) = puVar10;
                *puVar10 = (ulong)((uint)uVar4 & 3) | lVar8 + 8U;
              }
              if (*(int *)pQVar14 == 0) {
LAB_0063f318:
                if (*(long *)(pQVar14 + 0x10) != 0) {
                    /* try { // try from 0063f329 to 0063f335 has its CatchHandler @ 0063f497 */
                  QMapDataBase::freeTree(pQVar14,(int)*(long *)(pQVar14 + 0x10));
                }
                QMapDataBase::freeData((QMapDataBase *)pQVar14);
              }
              else if (*(int *)pQVar14 != -1) {
                LOCK();
                *(int *)pQVar14 = *(int *)pQVar14 + -1;
                UNLOCK();
                if (*(int *)pQVar14 == 0) {
                  pQVar14 = *(QMapNodeBase **)(this + 0x20);
                  goto LAB_0063f318;
                }
              }
              *(long *)(this + 0x20) = lVar8;
              QMapDataBase::recalcMostLeftNode();
              puVar7 = *(uint **)(this + 0x20);
              puVar15 = *(uint **)(puVar7 + 4);
              if (puVar15 != (uint *)0x0) goto LAB_0063ee6a;
LAB_0063f13f:
              iVar13 = (int)puVar7;
              puVar7 = puVar7 + 2;
            }
LAB_0063efe6:
                    /* try { // try from 0063eff6 to 0063effa has its CatchHandler @ 0063f497 */
            lVar8 = QMapDataBase::createNode
                              (iVar13,0x30,(QMapNodeBase *)&DAT_00000008,SUB81(puVar7,0));
            *(undefined (*) [16])(lVar8 + 0x18) = local_58;
            *(undefined *)(lVar8 + 0x28) = *(undefined *)(lVar9 + 0x28);
          }
LAB_0063f00d:
          if (local_98 != (KisBaseNode *)0x0) {
            LOCK();
            pKVar2 = local_98 + 0x10;
            *(int *)pKVar2 = *(int *)pKVar2 + -1;
            UNLOCK();
            if (*(int *)pKVar2 == 0) {
              (**(code **)(*(long *)local_98 + 0x20))();
            }
          }
          KisNodeQueryPath::~KisNodeQueryPath(local_a0);
          if (local_a8 != (long *)0x0) {
            LOCK();
            plVar3 = local_a8 + 2;
            *(int *)plVar3 = *(int *)plVar3 + -1;
            UNLOCK();
            if (*(int *)plVar3 == 0) {
              (**(code **)(*local_a8 + 0x20))();
            }
          }
        }
                    /* try { // try from 0063f044 to 0063f048 has its CatchHandler @ 0063f4eb */
        lVar9 = QMapNodeBase::nextNode();
      } while (lVar9 != *(long *)(param_1 + 0x20) + 8);
    }
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



