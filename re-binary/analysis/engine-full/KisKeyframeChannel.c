/* Class KisKeyframeChannel - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisKeyframeChannel @ 00209820 ======

void __thiscall
KisKeyframeChannel::KisKeyframeChannel(KisKeyframeChannel *this,KoID *param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisKeyframeChannel_0083c6e0)();
  return;
}



// ====== KisKeyframeChannel @ 0020b8e0 ======

void __thiscall
KisKeyframeChannel::KisKeyframeChannel(KisKeyframeChannel *this,KisKeyframeChannel *param_1)

{
  (*(code *)PTR_KisKeyframeChannel_0083d740)();
  return;
}



// ====== KisKeyframeChannel @ 0020cdc0 ======

void __thiscall
KisKeyframeChannel::KisKeyframeChannel(KisKeyframeChannel *this,KoID *param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisKeyframeChannel_0083e1b0)();
  return;
}



// ====== KisKeyframeChannel @ 0064f310 ======

/* KisKeyframeChannel::KisKeyframeChannel(KoID const&, KisSharedPtr<KisDefaultBoundsBase>) */

void __thiscall
KisKeyframeChannel::KisKeyframeChannel(KisKeyframeChannel *this,KoID *param_1,KisSharedPtr param_2)

{
  long *plVar1;
  long *plVar2;
  long *plVar3;
  long *plVar4;
  undefined *puVar5;
  KoID *this_00;
  undefined4 *puVar6;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  undefined *local_78;
  undefined8 local_70;
  undefined *local_68;
  undefined8 local_60;
  undefined *local_58;
  undefined8 local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837518 + 0x10;
                    /* try { // try from 0064f357 to 0064f35b has its CatchHandler @ 0064f5a1 */
  this_00 = (KoID *)operator_new(0x38);
  plVar3 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (plVar3 == (long *)0x0) {
                    /* try { // try from 0064f55b to 0064f55f has its CatchHandler @ 0064f5ad */
    KoID::KoID(this_00);
    puVar5 = PTR_shared_null_008372c0;
    this_00[0x30] = (KoID)0x0;
    *(undefined8 *)(this_00 + 0x18) = 0;
    *(undefined **)(this_00 + 0x10) = puVar5;
    *(undefined8 *)(this_00 + 0x20) = 0;
    *(undefined8 *)(this_00 + 0x28) = 0;
                    /* try { // try from 0064f58e to 0064f592 has its CatchHandler @ 0064f5b9 */
    KoID::operator=(this_00,param_1);
    *(KoID **)(this + 0x10) = this_00;
  }
  else {
    plVar2 = plVar3 + 1;
    LOCK();
    *(int *)(plVar3 + 1) = *(int *)(plVar3 + 1) + 1;
    UNLOCK();
                    /* try { // try from 0064f37b to 0064f37f has its CatchHandler @ 0064f5ad */
    KoID::KoID(this_00);
    puVar5 = PTR_shared_null_008372c0;
    *(undefined8 *)(this_00 + 0x18) = 0;
    *(undefined8 *)(this_00 + 0x20) = 0;
    *(undefined **)(this_00 + 0x10) = puVar5;
    *(undefined8 *)(this_00 + 0x28) = 0;
    this_00[0x30] = (KoID)0x0;
    LOCK();
    *(int *)(plVar3 + 1) = *(int *)(plVar3 + 1) + 1;
    UNLOCK();
    plVar4 = *(long **)(this_00 + 0x18);
    *(long **)(this_00 + 0x18) = plVar3;
    if (plVar4 == (long *)0x0) {
                    /* try { // try from 0064f546 to 0064f54a has its CatchHandler @ 0064f5b9 */
      KoID::operator=(this_00,param_1);
      *(KoID **)(this + 0x10) = this_00;
    }
    else {
      LOCK();
      plVar1 = plVar4 + 1;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar4 + 8))();
      }
                    /* try { // try from 0064f3d3 to 0064f3d7 has its CatchHandler @ 0064f5b9 */
      KoID::operator=(this_00,param_1);
      *(KoID **)(this + 0x10) = this_00;
    }
    LOCK();
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*plVar3 + 8))(plVar3);
    }
  }
  local_70 = 0;
  local_78 = PTR_sigAddedKeyframe_00836f40;
                    /* try { // try from 0064f405 to 0064f50d has its CatchHandler @ 0064f5c5 */
  puVar6 = (undefined4 *)operator_new(0x18);
  *puVar6 = 1;
  *(KisKeyframeChannel **)(puVar6 + 4) = this;
  *(code **)(puVar6 + 2) = FUN_0064b820;
  QObject::connectImpl
            ((QObject *)&local_58,(void **)this,(QObject *)&local_78,(void **)this,
             (QSlotObjectBase *)0x0,(ConnectionType)puVar6,(int *)0x1,(QMetaObject *)0x0);
  QMetaObject::Connection::~Connection((Connection *)&local_58);
  local_60 = 0;
  local_68 = PTR_sigKeyframeHasBeenRemoved_00837050;
  puVar6 = (undefined4 *)operator_new(0x18);
  *puVar6 = 1;
  *(code **)(puVar6 + 2) = FUN_0064b870;
  *(KisKeyframeChannel **)(puVar6 + 4) = this;
  QObject::connectImpl
            ((QObject *)&local_58,(void **)this,(QObject *)&local_68,(void **)this,
             (QSlotObjectBase *)0x0,(ConnectionType)puVar6,(int *)0x1,(QMetaObject *)0x0);
  QMetaObject::Connection::~Connection((Connection *)&local_58);
  local_50 = 0;
  local_58 = PTR_sigKeyframeChanged_008375f0;
  puVar6 = (undefined4 *)operator_new(0x18);
  *puVar6 = 1;
  *(code **)(puVar6 + 2) = FUN_0064b8c0;
  *(KisKeyframeChannel **)(puVar6 + 4) = this;
  QObject::connectImpl
            ((QObject *)&local_68,(void **)this,(QObject *)&local_58,(void **)this,
             (QSlotObjectBase *)0x0,(ConnectionType)puVar6,(int *)0x1,(QMetaObject *)0x0);
  QMetaObject::Connection::~Connection((Connection *)&local_68);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisKeyframeChannel @ 0064f5e0 ======

/* KisKeyframeChannel::KisKeyframeChannel(KisKeyframeChannel const&) */

void __thiscall
KisKeyframeChannel::KisKeyframeChannel(KisKeyframeChannel *this,KisKeyframeChannel *param_1)

{
  long *plVar1;
  int *piVar2;
  int iVar3;
  KoID *pKVar4;
  KoID *pKVar5;
  int *piVar6;
  long *plVar7;
  long lVar8;
  long lVar9;
  long lVar10;
  long lVar11;
  undefined auVar12 [16];
  undefined *puVar13;
  undefined8 uVar14;
  KisDefaultBounds *pKVar15;
  KoID *this_00;
  QMapNodeBase *pQVar16;
  long in_FS_OFFSET;
  KisDefaultBounds *local_60;
  undefined local_58 [24];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  pKVar15 = (KisDefaultBounds *)operator_new(0x20);
  local_58._0_16_ = (undefined  [16])0x0;
                    /* try { // try from 0064f626 to 0064f62a has its CatchHandler @ 0064fcec */
  KisDefaultBounds::KisDefaultBounds(pKVar15,(KisWeakSharedPtr)local_58);
  LOCK();
  *(int *)(pKVar15 + 8) = *(int *)(pKVar15 + 8) + 1;
  UNLOCK();
  local_60 = pKVar15;
                    /* try { // try from 0064f642 to 0064f646 has its CatchHandler @ 0064fcf8 */
  KisKeyframeChannel(this,*(KoID **)(param_1 + 0x10),(KisSharedPtr)&local_60);
  if (local_60 != (KisDefaultBounds *)0x0) {
    LOCK();
    pKVar15 = local_60 + 8;
    *(int *)pKVar15 = *(int *)pKVar15 + -1;
    UNLOCK();
    if (*(int *)pKVar15 == 0) {
      (**(code **)(*(long *)local_60 + 8))();
    }
  }
  uVar14 = local_58._8_8_;
  auVar12._8_8_ = 0;
  auVar12._0_8_ = local_58._8_8_;
  local_58._0_16_ = auVar12 << 0x40;
  if ((int *)uVar14 != (int *)0x0) {
    LOCK();
    iVar3 = *(int *)uVar14;
    *(int *)uVar14 = *(int *)uVar14 + -2;
    UNLOCK();
    if ((iVar3 < 3) && ((int *)uVar14 != (int *)0x0)) {
      operator_delete((void *)uVar14,4);
    }
  }
                    /* try { // try from 0064f686 to 0064f68a has its CatchHandler @ 0064fd10 */
  this_00 = (KoID *)operator_new(0x38);
  pKVar4 = *(KoID **)(param_1 + 0x10);
                    /* try { // try from 0064f696 to 0064f69a has its CatchHandler @ 0064fd1c */
  KoID::KoID(this_00);
  puVar13 = PTR_shared_null_008372c0;
  this_00[0x30] = (KoID)0x0;
  *(undefined8 *)(this_00 + 0x18) = 0;
  *(undefined **)(this_00 + 0x10) = puVar13;
  *(undefined8 *)(this_00 + 0x20) = 0;
  *(undefined8 *)(this_00 + 0x28) = 0;
                    /* try { // try from 0064f6c8 to 0064f6cc has its CatchHandler @ 0064fd04 */
  KoID::operator=(this_00,pKVar4);
  pKVar5 = *(KoID **)(this + 0x10);
  this_00[0x30] = pKVar4[0x30];
  if ((this_00 == pKVar5) || (*(KoID **)(this + 0x10) = this_00, pKVar5 == (KoID *)0x0)) {
    if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
    goto LAB_0064fce7;
  }
  *(undefined8 *)(pKVar5 + 0x20) = 0;
  piVar6 = *(int **)(pKVar5 + 0x28);
  if (piVar6 != (int *)0x0) {
    LOCK();
    iVar3 = *piVar6;
    *piVar6 = *piVar6 + -2;
    UNLOCK();
    if ((iVar3 < 3) && (*(void **)(pKVar5 + 0x28) != (void *)0x0)) {
      operator_delete(*(void **)(pKVar5 + 0x28),4);
    }
  }
  plVar7 = *(long **)(pKVar5 + 0x18);
  if (plVar7 != (long *)0x0) {
    LOCK();
    plVar1 = plVar7 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*plVar7 + 8))();
    }
  }
  pQVar16 = *(QMapNodeBase **)(pKVar5 + 0x10);
  if (*(int *)pQVar16 == 0) {
LAB_0064f825:
    lVar8 = *(long *)(pQVar16 + 0x10);
    if (lVar8 != 0) {
      piVar6 = *(int **)(lVar8 + 0x28);
      if (piVar6 != (int *)0x0) {
        LOCK();
        piVar2 = piVar6 + 1;
        *piVar2 = *piVar2 + -1;
        UNLOCK();
        if (*piVar2 == 0) {
          (**(code **)(piVar6 + 2))(piVar6);
        }
        LOCK();
        *piVar6 = *piVar6 + -1;
        UNLOCK();
        if (*piVar6 == 0) {
          operator_delete(piVar6,0x10);
        }
      }
      lVar9 = *(long *)(lVar8 + 8);
      if (lVar9 != 0) {
        piVar6 = *(int **)(lVar9 + 0x28);
        if (piVar6 != (int *)0x0) {
          LOCK();
          piVar2 = piVar6 + 1;
          *piVar2 = *piVar2 + -1;
          UNLOCK();
          if (*piVar2 == 0) {
            (**(code **)(piVar6 + 2))(piVar6);
          }
          LOCK();
          *piVar6 = *piVar6 + -1;
          UNLOCK();
          if (*piVar6 == 0) {
            operator_delete(piVar6,0x10);
          }
        }
        lVar10 = *(long *)(lVar9 + 8);
        if (lVar10 != 0) {
          piVar6 = *(int **)(lVar10 + 0x28);
          if (piVar6 != (int *)0x0) {
            LOCK();
            piVar2 = piVar6 + 1;
            *piVar2 = *piVar2 + -1;
            UNLOCK();
            if (*piVar2 == 0) {
              (**(code **)(piVar6 + 2))(piVar6);
            }
            LOCK();
            *piVar6 = *piVar6 + -1;
            UNLOCK();
            if (*piVar6 == 0) {
              operator_delete(piVar6,0x10);
            }
          }
          lVar11 = *(long *)(lVar10 + 8);
          if (lVar11 != 0) {
            piVar6 = *(int **)(lVar11 + 0x28);
            if (piVar6 != (int *)0x0) {
              LOCK();
              piVar2 = piVar6 + 1;
              *piVar2 = *piVar2 + -1;
              UNLOCK();
              if (*piVar2 == 0) {
                (**(code **)(piVar6 + 2))();
              }
              LOCK();
              *piVar6 = *piVar6 + -1;
              UNLOCK();
              if (*piVar6 == 0) {
                operator_delete(piVar6,0x10);
              }
            }
            if (*(long *)(lVar11 + 8) != 0) {
              FUN_00650d50();
            }
            if (*(long *)(lVar11 + 0x10) != 0) {
              FUN_00650d50();
            }
          }
          lVar10 = *(long *)(lVar10 + 0x10);
          if (lVar10 != 0) {
            piVar6 = *(int **)(lVar10 + 0x28);
            if (piVar6 != (int *)0x0) {
              LOCK();
              piVar2 = piVar6 + 1;
              *piVar2 = *piVar2 + -1;
              UNLOCK();
              if (*piVar2 == 0) {
                (**(code **)(piVar6 + 2))(piVar6);
              }
              LOCK();
              *piVar6 = *piVar6 + -1;
              UNLOCK();
              if (*piVar6 == 0) {
                operator_delete(piVar6,0x10);
              }
            }
            if (*(long *)(lVar10 + 8) != 0) {
              FUN_00650d50();
            }
            if (*(long *)(lVar10 + 0x10) != 0) {
              FUN_00650d50();
            }
          }
        }
        lVar9 = *(long *)(lVar9 + 0x10);
        if (lVar9 != 0) {
          piVar6 = *(int **)(lVar9 + 0x28);
          if (piVar6 != (int *)0x0) {
            LOCK();
            piVar2 = piVar6 + 1;
            *piVar2 = *piVar2 + -1;
            UNLOCK();
            if (*piVar2 == 0) {
              (**(code **)(piVar6 + 2))(piVar6);
            }
            LOCK();
            *piVar6 = *piVar6 + -1;
            UNLOCK();
            if (*piVar6 == 0) {
              operator_delete(piVar6,0x10);
            }
          }
          lVar10 = *(long *)(lVar9 + 8);
          if (lVar10 != 0) {
            piVar6 = *(int **)(lVar10 + 0x28);
            if (piVar6 != (int *)0x0) {
              LOCK();
              piVar2 = piVar6 + 1;
              *piVar2 = *piVar2 + -1;
              UNLOCK();
              if (*piVar2 == 0) {
                (**(code **)(piVar6 + 2))(piVar6);
              }
              LOCK();
              *piVar6 = *piVar6 + -1;
              UNLOCK();
              if (*piVar6 == 0) {
                operator_delete(piVar6,0x10);
              }
            }
            if (*(long *)(lVar10 + 8) != 0) {
              FUN_00650d50();
            }
            if (*(long *)(lVar10 + 0x10) != 0) {
              FUN_00650d50();
            }
          }
          lVar9 = *(long *)(lVar9 + 0x10);
          if (lVar9 != 0) {
            piVar6 = *(int **)(lVar9 + 0x28);
            if (piVar6 != (int *)0x0) {
              LOCK();
              piVar2 = piVar6 + 1;
              *piVar2 = *piVar2 + -1;
              UNLOCK();
              if (*piVar2 == 0) {
                (**(code **)(piVar6 + 2))(piVar6);
              }
              LOCK();
              *piVar6 = *piVar6 + -1;
              UNLOCK();
              if (*piVar6 == 0) {
                operator_delete(piVar6,0x10);
              }
            }
            if (*(long *)(lVar9 + 8) != 0) {
              FUN_00650d50();
            }
            if (*(long *)(lVar9 + 0x10) != 0) {
              FUN_00650d50();
            }
          }
        }
      }
      lVar8 = *(long *)(lVar8 + 0x10);
      if (lVar8 != 0) {
        piVar6 = *(int **)(lVar8 + 0x28);
        if (piVar6 != (int *)0x0) {
          LOCK();
          piVar2 = piVar6 + 1;
          *piVar2 = *piVar2 + -1;
          UNLOCK();
          if (*piVar2 == 0) {
            (**(code **)(piVar6 + 2))(piVar6);
          }
          LOCK();
          *piVar6 = *piVar6 + -1;
          UNLOCK();
          if (*piVar6 == 0) {
            operator_delete(piVar6,0x10);
          }
        }
        lVar9 = *(long *)(lVar8 + 8);
        if (lVar9 != 0) {
          piVar6 = *(int **)(lVar9 + 0x28);
          if (piVar6 != (int *)0x0) {
            LOCK();
            piVar2 = piVar6 + 1;
            *piVar2 = *piVar2 + -1;
            UNLOCK();
            if (*piVar2 == 0) {
              (**(code **)(piVar6 + 2))(piVar6);
            }
            LOCK();
            *piVar6 = *piVar6 + -1;
            UNLOCK();
            if (*piVar6 == 0) {
              operator_delete(piVar6,0x10);
            }
          }
          lVar10 = *(long *)(lVar9 + 8);
          if (lVar10 != 0) {
            piVar6 = *(int **)(lVar10 + 0x28);
            if (piVar6 != (int *)0x0) {
              LOCK();
              piVar2 = piVar6 + 1;
              *piVar2 = *piVar2 + -1;
              UNLOCK();
              if (*piVar2 == 0) {
                (**(code **)(piVar6 + 2))(piVar6);
              }
              LOCK();
              *piVar6 = *piVar6 + -1;
              UNLOCK();
              if (*piVar6 == 0) {
                operator_delete(piVar6,0x10);
              }
            }
            if (*(long *)(lVar10 + 8) != 0) {
              FUN_00650d50();
            }
            if (*(long *)(lVar10 + 0x10) != 0) {
              FUN_00650d50();
            }
          }
          lVar9 = *(long *)(lVar9 + 0x10);
          if (lVar9 != 0) {
            piVar6 = *(int **)(lVar9 + 0x28);
            if (piVar6 != (int *)0x0) {
              LOCK();
              piVar2 = piVar6 + 1;
              *piVar2 = *piVar2 + -1;
              UNLOCK();
              if (*piVar2 == 0) {
                (**(code **)(piVar6 + 2))(piVar6);
              }
              LOCK();
              *piVar6 = *piVar6 + -1;
              UNLOCK();
              if (*piVar6 == 0) {
                operator_delete(piVar6,0x10);
              }
            }
            if (*(long *)(lVar9 + 8) != 0) {
              FUN_00650d50();
            }
            if (*(long *)(lVar9 + 0x10) != 0) {
              FUN_00650d50();
            }
          }
        }
        lVar8 = *(long *)(lVar8 + 0x10);
        if (lVar8 != 0) {
          piVar6 = *(int **)(lVar8 + 0x28);
          if (piVar6 != (int *)0x0) {
            LOCK();
            piVar2 = piVar6 + 1;
            *piVar2 = *piVar2 + -1;
            UNLOCK();
            if (*piVar2 == 0) {
              (**(code **)(piVar6 + 2))(piVar6);
            }
            LOCK();
            *piVar6 = *piVar6 + -1;
            UNLOCK();
            if (*piVar6 == 0) {
              operator_delete(piVar6,0x10);
            }
          }
          lVar9 = *(long *)(lVar8 + 8);
          if (lVar9 != 0) {
            piVar6 = *(int **)(lVar9 + 0x28);
            if (piVar6 != (int *)0x0) {
              LOCK();
              piVar2 = piVar6 + 1;
              *piVar2 = *piVar2 + -1;
              UNLOCK();
              if (*piVar2 == 0) {
                (**(code **)(piVar6 + 2))(piVar6);
              }
              LOCK();
              *piVar6 = *piVar6 + -1;
              UNLOCK();
              if (*piVar6 == 0) {
                operator_delete(piVar6,0x10);
              }
            }
            if (*(long *)(lVar9 + 8) != 0) {
              FUN_00650d50();
            }
            if (*(long *)(lVar9 + 0x10) != 0) {
              FUN_00650d50();
            }
          }
          lVar8 = *(long *)(lVar8 + 0x10);
          if (lVar8 != 0) {
            piVar6 = *(int **)(lVar8 + 0x28);
            if (piVar6 != (int *)0x0) {
              LOCK();
              piVar2 = piVar6 + 1;
              *piVar2 = *piVar2 + -1;
              UNLOCK();
              if (*piVar2 == 0) {
                (**(code **)(piVar6 + 2))(piVar6);
              }
              LOCK();
              *piVar6 = *piVar6 + -1;
              UNLOCK();
              if (*piVar6 == 0) {
                operator_delete(piVar6,0x10);
              }
            }
            if (*(long *)(lVar8 + 8) != 0) {
              FUN_00650d50();
            }
            if (*(long *)(lVar8 + 0x10) != 0) {
              FUN_00650d50();
            }
          }
        }
      }
      QMapDataBase::freeTree(pQVar16,(int)*(undefined8 *)(pQVar16 + 0x10));
    }
    QMapDataBase::freeData((QMapDataBase *)pQVar16);
  }
  else if (*(int *)pQVar16 != -1) {
    LOCK();
    *(int *)pQVar16 = *(int *)pQVar16 + -1;
    UNLOCK();
    if (*(int *)pQVar16 == 0) {
      pQVar16 = *(QMapNodeBase **)(pKVar5 + 0x10);
      goto LAB_0064f825;
    }
  }
  piVar6 = *(int **)(pKVar5 + 8);
  if (piVar6 != (int *)0x0) {
    LOCK();
    piVar2 = piVar6 + 1;
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (*piVar2 == 0) {
      (**(code **)(piVar6 + 2))(piVar6);
    }
    LOCK();
    *piVar6 = *piVar6 + -1;
    UNLOCK();
    if (*piVar6 == 0) {
      operator_delete(piVar6,0x10);
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    operator_delete(pKVar5,0x38);
    return;
  }
LAB_0064fce7:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



