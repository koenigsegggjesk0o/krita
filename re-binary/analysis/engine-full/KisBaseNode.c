/* Class KisBaseNode - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisBaseNode @ 00200c30 ======

void __thiscall KisBaseNode::KisBaseNode(KisBaseNode *this,KisBaseNode *param_1)

{
  (*(code *)PTR_KisBaseNode_008380e8)();
  return;
}



// ====== KisBaseNode @ 0020bc10 ======

void __thiscall KisBaseNode::KisBaseNode(KisBaseNode *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisBaseNode_0083d8d8)();
  return;
}



// ====== KisBaseNode @ 00462500 ======

/* KisBaseNode::KisBaseNode(KisWeakSharedPtr<KisImage>) */

void __thiscall KisBaseNode::KisBaseNode(KisBaseNode *this,KisWeakSharedPtr param_1)

{
  int iVar1;
  uint uVar2;
  long lVar3;
  undefined *puVar4;
  undefined8 *puVar5;
  KisDefaultBounds *pKVar6;
  uint *puVar7;
  int *piVar8;
  undefined4 in_register_00000034;
  long *plVar9;
  long in_FS_OFFSET;
  undefined auVar10 [16];
  KisDefaultBounds *local_70;
  long local_68;
  uint *puStack_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  piStack_50 = (int *)local_58;
  puStack_60 = (uint *)local_68;
  plVar9 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
                    /* try { // try from 0046253b to 0046253f has its CatchHandler @ 00462996 */
  KisShared::KisShared((KisShared *)(this + 0x10));
  *(undefined **)this = PTR_vtable_00837648 + 0x10;
                    /* try { // try from 00462554 to 00462558 has its CatchHandler @ 004629a2 */
  puVar5 = (undefined8 *)operator_new(0xb8);
  if (*plVar9 == 0) {
    local_68 = 0;
    lVar3 = local_68;
LAB_004628a9:
    local_68 = lVar3;
    puStack_60 = (uint *)0x0;
  }
  else if (((uint *)plVar9[1] == (uint *)0x0) || ((*(uint *)plVar9[1] & 1) == 0)) {
    local_68 = 0;
    puStack_60 = (uint *)0x0;
  }
  else {
    lVar3 = *plVar9;
    local_68 = lVar3;
    if (lVar3 == 0) goto LAB_004628a9;
    puVar7 = *(uint **)(lVar3 + 0x58);
    if (puVar7 == (uint *)0x0) {
                    /* try { // try from 00462935 to 00462939 has its CatchHandler @ 004629c6 */
      piVar8 = (int *)operator_new(4);
      *piVar8 = 0;
      *(int **)(lVar3 + 0x58) = piVar8;
      LOCK();
      *piVar8 = *piVar8 + 1;
      UNLOCK();
      puVar7 = *(uint **)(lVar3 + 0x58);
      lVar3 = local_68;
    }
    local_68 = lVar3;
    LOCK();
    *puVar7 = *puVar7 + 2;
    UNLOCK();
    puStack_60 = puVar7;
  }
  puVar4 = PTR_shared_null_008377d0;
  *puVar5 = PTR_shared_null_008377d0;
                    /* try { // try from 004625a3 to 004625a7 has its CatchHandler @ 0046298a */
  KoProperties::KoProperties((KoProperties *)(puVar5 + 1));
  *(undefined *)(puVar5 + 4) = 0;
  puVar5[2] = puVar4;
  puVar5[3] = puVar4;
  QIcon::QIcon((QIcon *)(puVar5 + 5));
  QIcon::QIcon((QIcon *)(puVar5 + 6));
  *(undefined4 *)(puVar5 + 8) = 0x80000000;
  puVar5[7] = 0;
  *(undefined2 *)(puVar5 + 9) = 0;
  *(undefined *)((long)puVar5 + 0x4a) = 0;
                    /* try { // try from 004625e0 to 004625e4 has its CatchHandler @ 004629f6 */
  auVar10 = QUuid::createUuid();
  puVar5[10] = auVar10._0_8_;
  puVar4 = PTR_shared_null_008372c0;
  puVar5[0xb] = auVar10._8_8_;
  puVar5[0xc] = puVar4;
                    /* try { // try from 00462601 to 00462605 has its CatchHandler @ 004629ea */
  pKVar6 = (KisDefaultBounds *)operator_new(0x20);
  lVar3 = local_68;
  if (local_68 == 0) {
    local_58 = (undefined  [8])0x0;
LAB_004628c9:
    piStack_50 = (int *)0x0;
  }
  else if ((puStack_60 == (uint *)0x0) || ((*puStack_60 & 1) == 0)) {
    local_58 = (undefined  [8])0x0;
    piStack_50 = (int *)0x0;
  }
  else {
    local_58 = (undefined  [8])local_68;
    auVar10 = _local_58;
    if (local_68 == 0) goto LAB_004628c9;
    piStack_50 = *(int **)(local_68 + 0x58);
    if (piStack_50 == (int *)0x0) {
      _local_58 = auVar10;
                    /* try { // try from 0046290d to 00462911 has its CatchHandler @ 004629ba */
      piVar8 = (int *)operator_new(4);
      *piVar8 = 0;
      *(int **)(lVar3 + 0x58) = piVar8;
      LOCK();
      *piVar8 = *piVar8 + 1;
      UNLOCK();
      piStack_50 = *(int **)(lVar3 + 0x58);
    }
    LOCK();
    *piStack_50 = *piStack_50 + 2;
    UNLOCK();
  }
                    /* try { // try from 0046263d to 00462641 has its CatchHandler @ 004629de */
  KisDefaultBounds::KisDefaultBounds(pKVar6,(KisWeakSharedPtr)(QObject *)local_58);
  LOCK();
  *(int *)(pKVar6 + 8) = *(int *)(pKVar6 + 8) + 1;
  UNLOCK();
  local_70 = pKVar6;
                    /* try { // try from 00462661 to 00462665 has its CatchHandler @ 004629d2 */
  KisAnimatedOpacityProperty::KisAnimatedOpacityProperty
            ((KisAnimatedOpacityProperty *)(puVar5 + 0xd),(KisSharedPtr)&local_70,
             (KoProperties *)(puVar5 + 1),0xff,(QObject *)0x0);
  if (local_70 != (KisDefaultBounds *)0x0) {
    LOCK();
    pKVar6 = local_70 + 8;
    *(int *)pKVar6 = *(int *)pKVar6 + -1;
    UNLOCK();
    if (*(int *)pKVar6 == 0) {
      (**(code **)(*(long *)local_70 + 8))();
    }
  }
  piVar8 = piStack_50;
  auVar10._8_8_ = 0;
  auVar10._0_8_ = piStack_50;
  _local_58 = auVar10 << 0x40;
  if (piVar8 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar8;
    *piVar8 = *piVar8 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar8 != (int *)0x0)) {
      operator_delete(piVar8,4);
    }
  }
  lVar3 = local_68;
  *(undefined4 *)(puVar5 + 0x13) = 0;
  if (local_68 == 0) {
    puVar5[0x14] = 0;
LAB_0046288b:
    puVar5[0x15] = 0;
  }
  else {
    if (puStack_60 == (uint *)0x0) {
      *(undefined4 *)(puVar5 + 0x16) = 1;
      *(undefined8 **)(this + 0x20) = puVar5;
      *(undefined (*) [16])(puVar5 + 0x14) = (undefined  [16])0x0;
      goto LAB_00462761;
    }
    if ((*puStack_60 & 1) == 0) {
      *(undefined (*) [16])(puVar5 + 0x14) = (undefined  [16])0x0;
    }
    else {
      puVar5[0x14] = local_68;
      if (local_68 == 0) goto LAB_0046288b;
      piVar8 = *(int **)(local_68 + 0x58);
      if (piVar8 == (int *)0x0) {
                    /* try { // try from 0046295d to 00462961 has its CatchHandler @ 004629ae */
        piVar8 = (int *)operator_new(4);
        *piVar8 = 0;
        *(int **)(lVar3 + 0x58) = piVar8;
        LOCK();
        *piVar8 = *piVar8 + 1;
        UNLOCK();
        piVar8 = *(int **)(lVar3 + 0x58);
      }
      puVar5[0x15] = piVar8;
      LOCK();
      *piVar8 = *piVar8 + 2;
      UNLOCK();
    }
  }
  *(undefined4 *)(puVar5 + 0x16) = 1;
  *(undefined8 **)(this + 0x20) = puVar5;
  local_68 = 0;
  if (puStack_60 != (uint *)0x0) {
    LOCK();
    uVar2 = *puStack_60;
    *puStack_60 = *puStack_60 - 2;
    UNLOCK();
    if (((int)uVar2 < 3) && (puStack_60 != (uint *)0x0)) {
      operator_delete(puStack_60,4);
    }
  }
LAB_00462761:
                    /* try { // try from 0046276e to 004627c7 has its CatchHandler @ 004629a2 */
  setVisible(this,true,true);
  setUserLocked(this,false);
  setCollapsed(this,false);
  setSupportsLodMoves(this,true);
  QString::operator=(*(QString **)(this + 0x20),(QString *)&DAT_00849e90);
  QObject::connect((QObject *)local_58,(char *)(*(long *)(this + 0x20) + 0x68),
                   (QObject *)"2changed(quint8)",(char *)this,0x72ddd8);
  QMetaObject::Connection::~Connection((Connection *)local_58);
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisBaseNode @ 00462a10 ======

/* KisBaseNode::KisBaseNode(KisBaseNode const&) */

void __thiscall KisBaseNode::KisBaseNode(KisBaseNode *this,KisBaseNode *param_1)

{
  KoProperties *this_00;
  int iVar1;
  undefined8 *puVar2;
  ulong uVar3;
  undefined auVar4 [8];
  long lVar5;
  long lVar6;
  undefined *puVar7;
  char cVar8;
  undefined8 *puVar9;
  KisDefaultBounds *pKVar10;
  undefined8 uVar11;
  long lVar12;
  long lVar13;
  ulong *puVar14;
  int *piVar15;
  uint *puVar16;
  uint *puVar17;
  QArrayData *pQVar18;
  uint *puVar19;
  long in_FS_OFFSET;
  undefined auVar20 [16];
  KisAnimatedOpacityProperty *local_90;
  KisDefaultBounds *local_60;
  undefined local_58 [8];
  int *piStack_50;
  QMapNodeBase *pQStack_48;
  long local_40;
  
  piStack_50 = (int *)local_58;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
                    /* try { // try from 00462a50 to 00462a54 has its CatchHandler @ 004636c4 */
  KisShared::KisShared((KisShared *)(this + 0x10));
  *(undefined **)this = PTR_vtable_00837648 + 0x10;
                    /* try { // try from 00462a68 to 00462a6c has its CatchHandler @ 0046370c */
  puVar9 = (undefined8 *)operator_new(0xb8);
  puVar2 = *(undefined8 **)(param_1 + 0x20);
  piVar15 = (int *)*puVar2;
  *puVar9 = piVar15;
  if (1 < *piVar15 + 1U) {
    LOCK();
    *piVar15 = *piVar15 + 1;
    UNLOCK();
  }
  this_00 = (KoProperties *)(puVar9 + 1);
                    /* try { // try from 00462a90 to 00462a94 has its CatchHandler @ 004636f4 */
  KoProperties::KoProperties(this_00);
  puVar7 = PTR_shared_null_008377d0;
  *(undefined *)(puVar9 + 4) = 0;
  puVar9[2] = puVar7;
  puVar9[3] = puVar7;
  QIcon::QIcon((QIcon *)(puVar9 + 5));
  QIcon::QIcon((QIcon *)(puVar9 + 6));
  *(undefined4 *)(puVar9 + 8) = 0x80000000;
  puVar9[7] = 0;
  *(undefined2 *)(puVar9 + 9) = 0;
  *(undefined *)((long)puVar9 + 0x4a) = 0;
                    /* try { // try from 00462ad4 to 00462ad8 has its CatchHandler @ 004636e8 */
  auVar20 = QUuid::createUuid();
  puVar9[10] = auVar20._0_8_;
  puVar7 = PTR_shared_null_008372c0;
  puVar9[0xb] = auVar20._8_8_;
  puVar9[0xc] = puVar7;
  local_90 = (KisAnimatedOpacityProperty *)(puVar9 + 0xd);
                    /* try { // try from 00462afa to 00462afe has its CatchHandler @ 004636dc */
  pKVar10 = (KisDefaultBounds *)operator_new(0x20);
  if (puVar2[0x14] == 0) {
    local_58 = (undefined  [8])0x0;
    auVar4 = local_58;
LAB_00463461:
    local_58 = auVar4;
    piStack_50 = (int *)0x0;
  }
  else if (((uint *)puVar2[0x15] == (uint *)0x0) || ((*(uint *)puVar2[0x15] & 1) == 0)) {
    local_58 = (undefined  [8])0x0;
    piStack_50 = (int *)0x0;
  }
  else {
    auVar4 = (undefined  [8])puVar2[0x14];
    local_58 = auVar4;
    if (auVar4 == (undefined  [8])0x0) goto LAB_00463461;
    piVar15 = *(int **)((long)auVar4 + 0x58);
    if (piVar15 == (int *)0x0) {
                    /* try { // try from 004634ff to 00463503 has its CatchHandler @ 00463724 */
      piVar15 = (int *)operator_new(4);
      *piVar15 = 0;
      *(int **)((long)auVar4 + 0x58) = piVar15;
      LOCK();
      *piVar15 = *piVar15 + 1;
      UNLOCK();
      piVar15 = *(int **)((long)auVar4 + 0x58);
      auVar4 = local_58;
    }
    local_58 = auVar4;
    LOCK();
    *piVar15 = *piVar15 + 2;
    UNLOCK();
    piStack_50 = piVar15;
  }
                    /* try { // try from 00462b3a to 00462b3e has its CatchHandler @ 00463718 */
  KisDefaultBounds::KisDefaultBounds(pKVar10,(KisWeakSharedPtr)(KisKeyframeChannel *)local_58);
  LOCK();
  *(int *)(pKVar10 + 8) = *(int *)(pKVar10 + 8) + 1;
  UNLOCK();
  local_60 = pKVar10;
                    /* try { // try from 00462b5f to 00462b63 has its CatchHandler @ 00463700 */
  KisAnimatedOpacityProperty::KisAnimatedOpacityProperty
            (local_90,(KisSharedPtr)&local_60,this_00,0xff,(QObject *)0x0);
  if (local_60 != (KisDefaultBounds *)0x0) {
    LOCK();
    pKVar10 = local_60 + 8;
    *(int *)pKVar10 = *(int *)pKVar10 + -1;
    UNLOCK();
    if (*(int *)pKVar10 == 0) {
      (**(code **)(*(long *)local_60 + 8))();
    }
  }
  piVar15 = piStack_50;
  auVar20._8_8_ = 0;
  auVar20._0_8_ = piStack_50;
  _local_58 = auVar20 << 0x40;
  if (piVar15 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar15;
    *piVar15 = *piVar15 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar15 != (int *)0x0)) {
      operator_delete(piVar15,4);
    }
  }
  lVar12 = puVar2[0x14];
  *(undefined4 *)(puVar9 + 0x13) = *(undefined4 *)(puVar2 + 0x13);
  if (lVar12 == 0) {
    puVar9[0x14] = 0;
LAB_00463443:
    puVar9[0x15] = 0;
  }
  else if (((uint *)puVar2[0x15] == (uint *)0x0) || ((*(uint *)puVar2[0x15] & 1) == 0)) {
    *(undefined (*) [16])(puVar9 + 0x14) = (undefined  [16])0x0;
  }
  else {
    lVar12 = puVar2[0x14];
    puVar9[0x14] = lVar12;
    if (lVar12 == 0) goto LAB_00463443;
    piVar15 = *(int **)(lVar12 + 0x58);
    if (piVar15 == (int *)0x0) {
                    /* try { // try from 004634de to 004634e2 has its CatchHandler @ 004636a0 */
      piVar15 = (int *)operator_new(4);
      *piVar15 = 0;
      *(int **)(lVar12 + 0x58) = piVar15;
      LOCK();
      *piVar15 = *piVar15 + 1;
      UNLOCK();
      piVar15 = *(int **)(lVar12 + 0x58);
    }
    puVar9[0x15] = piVar15;
    LOCK();
    *piVar15 = *piVar15 + 2;
    UNLOCK();
  }
  *(undefined4 *)(puVar9 + 0x16) = 1;
                    /* try { // try from 00462beb to 00462bef has its CatchHandler @ 004636ac */
  KoProperties::propertyIterator();
  while( true ) {
    auVar4 = local_58;
    piVar15 = piStack_50;
    if ((QMapNodeBase *)piStack_50 == (QMapNodeBase *)((long)local_58 + 8)) break;
                    /* try { // try from 00462bfb to 00462c22 has its CatchHandler @ 004636d0 */
    uVar11 = QMapNodeBase::nextNode();
    piStack_50 = (int *)uVar11;
    pQStack_48 = (QMapNodeBase *)piVar15;
    KoProperties::setProperty((QString *)this_00,(QVariant *)((long)piVar15 + 0x18));
  }
  if (*(int *)local_58 == 0) {
LAB_00462ec0:
    lVar12 = *(long *)((long)local_58 + 0x10);
    if (lVar12 != 0) {
      pQVar18 = *(QArrayData **)(lVar12 + 0x18);
      if (*(int *)pQVar18 == 0) {
LAB_0046348f:
        QArrayData::deallocate(pQVar18,2,8);
      }
      else if (*(int *)pQVar18 != -1) {
        LOCK();
        *(int *)pQVar18 = *(int *)pQVar18 + -1;
        UNLOCK();
        if (*(int *)pQVar18 == 0) {
          pQVar18 = *(QArrayData **)(lVar12 + 0x18);
          goto LAB_0046348f;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar12 + 0x20));
      lVar13 = *(long *)(lVar12 + 8);
      if (lVar13 != 0) {
        pQVar18 = *(QArrayData **)(lVar13 + 0x18);
        if (*(int *)pQVar18 == 0) {
LAB_00462f18:
          QArrayData::deallocate(pQVar18,2,8);
        }
        else if (*(int *)pQVar18 != -1) {
          LOCK();
          *(int *)pQVar18 = *(int *)pQVar18 + -1;
          UNLOCK();
          if (*(int *)pQVar18 == 0) {
            pQVar18 = *(QArrayData **)(lVar13 + 0x18);
            goto LAB_00462f18;
          }
        }
        QVariant::~QVariant((QVariant *)(lVar13 + 0x20));
        if (*(long *)(lVar13 + 8) != 0) {
          FUN_00323e20();
        }
        if (*(long *)(lVar13 + 0x10) != 0) {
          FUN_00323e20();
        }
      }
      lVar12 = *(long *)(lVar12 + 0x10);
      if (lVar12 != 0) {
        pQVar18 = *(QArrayData **)(lVar12 + 0x18);
        if (*(int *)pQVar18 == 0) {
LAB_00462f75:
          QArrayData::deallocate(pQVar18,2,8);
        }
        else if (*(int *)pQVar18 != -1) {
          LOCK();
          *(int *)pQVar18 = *(int *)pQVar18 + -1;
          UNLOCK();
          if (*(int *)pQVar18 == 0) {
            pQVar18 = *(QArrayData **)(lVar12 + 0x18);
            goto LAB_00462f75;
          }
        }
        QVariant::~QVariant((QVariant *)(lVar12 + 0x20));
        if (*(long *)(lVar12 + 8) != 0) {
          FUN_00323e20();
        }
        lVar12 = *(long *)(lVar12 + 0x10);
        if (lVar12 != 0) {
          pQVar18 = *(QArrayData **)(lVar12 + 0x18);
          if (*(int *)pQVar18 == 0) {
LAB_004634c5:
            QArrayData::deallocate(pQVar18,2,8);
          }
          else if (*(int *)pQVar18 != -1) {
            LOCK();
            *(int *)pQVar18 = *(int *)pQVar18 + -1;
            UNLOCK();
            if (*(int *)pQVar18 == 0) {
              pQVar18 = *(QArrayData **)(lVar12 + 0x18);
              goto LAB_004634c5;
            }
          }
          QVariant::~QVariant((QVariant *)(lVar12 + 0x20));
          lVar13 = *(long *)(lVar12 + 8);
          if (lVar13 != 0) {
            pQVar18 = *(QArrayData **)(lVar13 + 0x18);
            if (*(int *)pQVar18 == 0) {
LAB_00463538:
              QArrayData::deallocate(pQVar18,2,8);
            }
            else if (*(int *)pQVar18 != -1) {
              LOCK();
              *(int *)pQVar18 = *(int *)pQVar18 + -1;
              UNLOCK();
              if (*(int *)pQVar18 == 0) {
                pQVar18 = *(QArrayData **)(lVar13 + 0x18);
                goto LAB_00463538;
              }
            }
            QVariant::~QVariant((QVariant *)(lVar13 + 0x20));
            lVar5 = *(long *)(lVar13 + 8);
            if (lVar5 != 0) {
              pQVar18 = *(QArrayData **)(lVar5 + 0x18);
              if (*(int *)pQVar18 == 0) {
LAB_0046356d:
                QArrayData::deallocate(pQVar18,2,8);
              }
              else if (*(int *)pQVar18 != -1) {
                LOCK();
                *(int *)pQVar18 = *(int *)pQVar18 + -1;
                UNLOCK();
                if (*(int *)pQVar18 == 0) {
                  pQVar18 = *(QArrayData **)(lVar5 + 0x18);
                  goto LAB_0046356d;
                }
              }
              QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
              lVar6 = *(long *)(lVar5 + 8);
              if (lVar6 != 0) {
                pQVar18 = *(QArrayData **)(lVar6 + 0x18);
                if (*(int *)pQVar18 == 0) {
LAB_00463621:
                  QArrayData::deallocate(pQVar18,2,8);
                }
                else if (*(int *)pQVar18 != -1) {
                  LOCK();
                  *(int *)pQVar18 = *(int *)pQVar18 + -1;
                  UNLOCK();
                  if (*(int *)pQVar18 == 0) {
                    pQVar18 = *(QArrayData **)(lVar6 + 0x18);
                    goto LAB_00463621;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                if (*(long *)(lVar6 + 8) != 0) {
                  FUN_00323e20();
                }
                if (*(long *)(lVar6 + 0x10) != 0) {
                  FUN_00323e20();
                }
              }
              lVar5 = *(long *)(lVar5 + 0x10);
              if (lVar5 != 0) {
                pQVar18 = *(QArrayData **)(lVar5 + 0x18);
                if (*(int *)pQVar18 == 0) {
LAB_0046367d:
                  QArrayData::deallocate(pQVar18,2,8);
                }
                else if (*(int *)pQVar18 != -1) {
                  LOCK();
                  *(int *)pQVar18 = *(int *)pQVar18 + -1;
                  UNLOCK();
                  if (*(int *)pQVar18 == 0) {
                    pQVar18 = *(QArrayData **)(lVar5 + 0x18);
                    goto LAB_0046367d;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                if (*(long *)(lVar5 + 8) != 0) {
                  FUN_00323e20();
                }
                if (*(long *)(lVar5 + 0x10) != 0) {
                  FUN_00323e20();
                }
              }
            }
            lVar13 = *(long *)(lVar13 + 0x10);
            if (lVar13 != 0) {
              pQVar18 = *(QArrayData **)(lVar13 + 0x18);
              if (*(int *)pQVar18 == 0) {
LAB_00463585:
                QArrayData::deallocate(pQVar18,2,8);
              }
              else if (*(int *)pQVar18 != -1) {
                LOCK();
                *(int *)pQVar18 = *(int *)pQVar18 + -1;
                UNLOCK();
                if (*(int *)pQVar18 == 0) {
                  pQVar18 = *(QArrayData **)(lVar13 + 0x18);
                  goto LAB_00463585;
                }
              }
              QVariant::~QVariant((QVariant *)(lVar13 + 0x20));
              lVar5 = *(long *)(lVar13 + 8);
              if (lVar5 != 0) {
                pQVar18 = *(QArrayData **)(lVar5 + 0x18);
                if (*(int *)pQVar18 == 0) {
LAB_0046365b:
                  QArrayData::deallocate(pQVar18,2,8);
                }
                else if (*(int *)pQVar18 != -1) {
                  LOCK();
                  *(int *)pQVar18 = *(int *)pQVar18 + -1;
                  UNLOCK();
                  if (*(int *)pQVar18 == 0) {
                    pQVar18 = *(QArrayData **)(lVar5 + 0x18);
                    goto LAB_0046365b;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                if (*(long *)(lVar5 + 8) != 0) {
                  FUN_00323e20();
                }
                if (*(long *)(lVar5 + 0x10) != 0) {
                  FUN_00323e20();
                }
              }
              lVar13 = *(long *)(lVar13 + 0x10);
              if (lVar13 != 0) {
                pQVar18 = *(QArrayData **)(lVar13 + 0x18);
                if (*(int *)pQVar18 == 0) {
LAB_00463643:
                  QArrayData::deallocate(pQVar18,2,8);
                }
                else if (*(int *)pQVar18 != -1) {
                  LOCK();
                  *(int *)pQVar18 = *(int *)pQVar18 + -1;
                  UNLOCK();
                  if (*(int *)pQVar18 == 0) {
                    pQVar18 = *(QArrayData **)(lVar13 + 0x18);
                    goto LAB_00463643;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar13 + 0x20));
                if (*(long *)(lVar13 + 8) != 0) {
                  FUN_00323e20();
                }
                if (*(long *)(lVar13 + 0x10) != 0) {
                  FUN_00323e20();
                }
              }
            }
          }
          lVar12 = *(long *)(lVar12 + 0x10);
          if (lVar12 != 0) {
            pQVar18 = *(QArrayData **)(lVar12 + 0x18);
            if (*(int *)pQVar18 == 0) {
LAB_00463520:
              QArrayData::deallocate(pQVar18,2,8);
            }
            else if (*(int *)pQVar18 != -1) {
              LOCK();
              *(int *)pQVar18 = *(int *)pQVar18 + -1;
              UNLOCK();
              if (*(int *)pQVar18 == 0) {
                pQVar18 = *(QArrayData **)(lVar12 + 0x18);
                goto LAB_00463520;
              }
            }
            QVariant::~QVariant((QVariant *)(lVar12 + 0x20));
            lVar13 = *(long *)(lVar12 + 8);
            if (lVar13 != 0) {
              pQVar18 = *(QArrayData **)(lVar13 + 0x18);
              if (*(int *)pQVar18 == 0) {
LAB_00463550:
                QArrayData::deallocate(pQVar18,2,8);
              }
              else if (*(int *)pQVar18 != -1) {
                LOCK();
                *(int *)pQVar18 = *(int *)pQVar18 + -1;
                UNLOCK();
                if (*(int *)pQVar18 == 0) {
                  pQVar18 = *(QArrayData **)(lVar13 + 0x18);
                  goto LAB_00463550;
                }
              }
              QVariant::~QVariant((QVariant *)(lVar13 + 0x20));
              lVar5 = *(long *)(lVar13 + 8);
              if (lVar5 != 0) {
                pQVar18 = *(QArrayData **)(lVar5 + 0x18);
                if (*(int *)pQVar18 == 0) {
LAB_004635ce:
                  QArrayData::deallocate(pQVar18,2,8);
                }
                else if (*(int *)pQVar18 != -1) {
                  LOCK();
                  *(int *)pQVar18 = *(int *)pQVar18 + -1;
                  UNLOCK();
                  if (*(int *)pQVar18 == 0) {
                    pQVar18 = *(QArrayData **)(lVar5 + 0x18);
                    goto LAB_004635ce;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                if (*(long *)(lVar5 + 8) != 0) {
                  FUN_00323e20();
                }
                if (*(long *)(lVar5 + 0x10) != 0) {
                  FUN_00323e20();
                }
              }
              lVar13 = *(long *)(lVar13 + 0x10);
              if (lVar13 != 0) {
                pQVar18 = *(QArrayData **)(lVar13 + 0x18);
                if (*(int *)pQVar18 == 0) {
LAB_004635b6:
                  QArrayData::deallocate(pQVar18,2,8);
                }
                else if (*(int *)pQVar18 != -1) {
                  LOCK();
                  *(int *)pQVar18 = *(int *)pQVar18 + -1;
                  UNLOCK();
                  if (*(int *)pQVar18 == 0) {
                    pQVar18 = *(QArrayData **)(lVar13 + 0x18);
                    goto LAB_004635b6;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar13 + 0x20));
                if (*(long *)(lVar13 + 8) != 0) {
                  FUN_00323e20();
                }
                if (*(long *)(lVar13 + 0x10) != 0) {
                  FUN_00323e20();
                }
              }
            }
            lVar12 = *(long *)(lVar12 + 0x10);
            if (lVar12 != 0) {
              pQVar18 = *(QArrayData **)(lVar12 + 0x18);
              if (*(int *)pQVar18 == 0) {
LAB_0046359e:
                QArrayData::deallocate(pQVar18,2,8);
              }
              else if (*(int *)pQVar18 != -1) {
                LOCK();
                *(int *)pQVar18 = *(int *)pQVar18 + -1;
                UNLOCK();
                if (*(int *)pQVar18 == 0) {
                  pQVar18 = *(QArrayData **)(lVar12 + 0x18);
                  goto LAB_0046359e;
                }
              }
              QVariant::~QVariant((QVariant *)(lVar12 + 0x20));
              lVar13 = *(long *)(lVar12 + 8);
              if (lVar13 != 0) {
                pQVar18 = *(QArrayData **)(lVar13 + 0x18);
                if (*(int *)pQVar18 == 0) {
LAB_004635f0:
                  QArrayData::deallocate(pQVar18,2,8);
                }
                else if (*(int *)pQVar18 != -1) {
                  LOCK();
                  *(int *)pQVar18 = *(int *)pQVar18 + -1;
                  UNLOCK();
                  if (*(int *)pQVar18 == 0) {
                    pQVar18 = *(QArrayData **)(lVar13 + 0x18);
                    goto LAB_004635f0;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar13 + 0x20));
                if (*(long *)(lVar13 + 8) != 0) {
                  FUN_00323e20();
                }
                if (*(long *)(lVar13 + 0x10) != 0) {
                  FUN_00323e20();
                }
              }
              lVar12 = *(long *)(lVar12 + 0x10);
              if (lVar12 != 0) {
                pQVar18 = *(QArrayData **)(lVar12 + 0x18);
                if (*(int *)pQVar18 == 0) {
LAB_00463609:
                  QArrayData::deallocate(pQVar18,2,8);
                }
                else if (*(int *)pQVar18 != -1) {
                  LOCK();
                  *(int *)pQVar18 = *(int *)pQVar18 + -1;
                  UNLOCK();
                  if (*(int *)pQVar18 == 0) {
                    pQVar18 = *(QArrayData **)(lVar12 + 0x18);
                    goto LAB_00463609;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar12 + 0x20));
                if (*(long *)(lVar12 + 8) != 0) {
                  FUN_00323e20();
                }
                if (*(long *)(lVar12 + 0x10) != 0) {
                  FUN_00323e20();
                }
              }
            }
          }
        }
      }
      QMapDataBase::freeTree((QMapNodeBase *)auVar4,(int)*(undefined8 *)((long)auVar4 + 0x10));
    }
    QMapDataBase::freeData((QMapDataBase *)auVar4);
  }
  else if (*(int *)local_58 != -1) {
    LOCK();
    *(int *)local_58 = *(int *)local_58 + -1;
    UNLOCK();
    if (*(int *)local_58 == 0) goto LAB_00462ec0;
  }
  *(undefined8 **)(this + 0x20) = puVar9;
  if (*(long *)(*(long *)(param_1 + 0x20) + 0x88) == 0) goto LAB_00462d76;
                    /* try { // try from 00462c7a to 00462c95 has its CatchHandler @ 0046370c */
  KisAnimatedOpacityProperty::transferKeyframeData
            (local_90,(KisAnimatedOpacityProperty *)(*(long *)(param_1 + 0x20) + 0x68));
  lVar12 = *(long *)(this + 0x20);
  uVar11 = *(undefined8 *)(lVar12 + 0x88);
  KisKeyframeChannel::id((KisKeyframeChannel *)local_58);
  puVar17 = *(uint **)(lVar12 + 0x60);
  if (1 < *puVar17) {
                    /* try { // try from 00462dc8 to 00462e27 has its CatchHandler @ 004636b8 */
    lVar13 = QMapDataBase::createData();
    piVar15 = *(int **)(lVar12 + 0x60);
    if (*(long *)(piVar15 + 4) != 0) {
      puVar14 = (ulong *)FUN_00463d70(*(long *)(piVar15 + 4),lVar13);
      uVar3 = *puVar14;
      piVar15 = *(int **)(lVar12 + 0x60);
      *(ulong **)(lVar13 + 0x10) = puVar14;
      *puVar14 = (ulong)((uint)uVar3 & 3) | lVar13 + 8U;
    }
    if (*piVar15 == 0) {
LAB_00463480:
                    /* try { // try from 00463480 to 00463484 has its CatchHandler @ 004636b8 */
      FUN_00464400(piVar15);
    }
    else if (*piVar15 != -1) {
      LOCK();
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 == 0) {
        piVar15 = *(int **)(lVar12 + 0x60);
        goto LAB_00463480;
      }
    }
    *(long *)(lVar12 + 0x60) = lVar13;
    QMapDataBase::recalcMostLeftNode();
    puVar17 = *(uint **)(lVar12 + 0x60);
  }
  if (*(uint **)(puVar17 + 4) == (uint *)0x0) {
    puVar19 = puVar17 + 2;
LAB_00462d19:
                    /* try { // try from 00462d26 to 00462d2a has its CatchHandler @ 004636b8 */
    lVar12 = QMapDataBase::createNode
                       ((int)puVar17,0x28,(QMapNodeBase *)&DAT_00000008,SUB81(puVar19,0));
    *(undefined (*) [8])(lVar12 + 0x18) = local_58;
    if (1 < *(int *)local_58 + 1U) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + 1;
      UNLOCK();
    }
    *(undefined8 *)(lVar12 + 0x20) = uVar11;
  }
  else {
    puVar16 = (uint *)0x0;
    puVar17 = *(uint **)(puVar17 + 4);
    do {
      while( true ) {
        puVar19 = puVar17;
        cVar8 = operator<((QString *)(puVar19 + 6),(QString *)local_58);
        if (cVar8 == '\0') break;
        puVar17 = *(uint **)(puVar19 + 4);
        if (*(uint **)(puVar19 + 4) == (uint *)0x0) {
          if (puVar16 == (uint *)0x0) {
            puVar17 = *(uint **)(lVar12 + 0x60);
            goto LAB_00462d19;
          }
          goto LAB_00462cf6;
        }
      }
      puVar16 = puVar19;
      puVar17 = *(uint **)(puVar19 + 2);
    } while (*(uint **)(puVar19 + 2) != (uint *)0x0);
LAB_00462cf6:
    cVar8 = operator<((QString *)local_58,(QString *)(puVar16 + 6));
    if (cVar8 != '\0') {
      puVar17 = *(uint **)(lVar12 + 0x60);
      goto LAB_00462d19;
    }
    *(undefined8 *)(puVar16 + 8) = uVar11;
  }
  if (*(int *)local_58 == 0) {
LAB_00463410:
    QArrayData::deallocate((QArrayData *)local_58,2,8);
  }
  else if (*(int *)local_58 != -1) {
    LOCK();
    *(int *)local_58 = *(int *)local_58 + -1;
    UNLOCK();
    if (*(int *)local_58 == 0) goto LAB_00463410;
  }
  local_90 = (KisAnimatedOpacityProperty *)(*(long *)(this + 0x20) + 0x68);
LAB_00462d76:
                    /* try { // try from 00462d92 to 00462d96 has its CatchHandler @ 0046370c */
  QObject::connect((QObject *)local_58,(char *)local_90,(QObject *)"2changed(quint8)",(char *)this,
                   0x72ddd8);
  QMetaObject::Connection::~Connection((Connection *)local_58);
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



