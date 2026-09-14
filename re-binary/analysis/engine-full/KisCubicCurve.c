/* Class KisCubicCurve - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCubicCurve @ 00200d20 ======

void __thiscall KisCubicCurve::KisCubicCurve(KisCubicCurve *this,QString *param_1)

{
  (*(code *)PTR_KisCubicCurve_00838160)();
  return;
}



// ====== KisCubicCurve @ 00207c90 ======

void __thiscall KisCubicCurve::KisCubicCurve(KisCubicCurve *this,QList *param_1)

{
  (*(code *)PTR_KisCubicCurve_0083b918)();
  return;
}



// ====== KisCubicCurve @ 00208390 ======

void __thiscall KisCubicCurve::KisCubicCurve(KisCubicCurve *this,KisCubicCurve *param_1)

{
  (*(code *)PTR_KisCubicCurve_0083bc98)();
  return;
}



// ====== KisCubicCurve @ 00208e00 ======

void __thiscall KisCubicCurve::KisCubicCurve(KisCubicCurve *this)

{
  (*(code *)PTR_KisCubicCurve_0083c1d0)();
  return;
}



// ====== KisCubicCurve @ 0048f620 ======

/* KisCubicCurve::KisCubicCurve(KisCubicCurve const&) */

void __thiscall KisCubicCurve::KisCubicCurve(KisCubicCurve *this,KisCubicCurve *param_1)

{
  int *piVar1;
  undefined8 *puVar2;
  
  puVar2 = (undefined8 *)operator_new(8);
  piVar1 = (int *)**(undefined8 **)param_1;
  *puVar2 = piVar1;
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  *(undefined8 **)this = puVar2;
  return;
}



// ====== KisCubicCurve @ 00490140 ======

/* KisCubicCurve::KisCubicCurve() */

void __thiscall KisCubicCurve::KisCubicCurve(KisCubicCurve *this)

{
  Data *pDVar1;
  int iVar2;
  undefined *puVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined8 *puVar6;
  int *piVar7;
  int *piVar8;
  QArrayData *pQVar9;
  Data *pDVar10;
  Data *pDVar11;
  long in_FS_OFFSET;
  KisCubicCurvePoint local_48 [24];
  long local_30;
  
  puVar5 = PTR_shared_null_00837830;
  puVar3 = PTR_shared_null_008377d0;
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  puVar6 = (undefined8 *)operator_new(8);
  *puVar6 = 0;
  *(undefined8 **)this = puVar6;
  piVar7 = (int *)operator_new(0x60);
  puVar4 = PTR_shared_null_008377d0;
  puVar6 = *(undefined8 **)this;
  *piVar7 = 0;
  piVar8 = (int *)*puVar6;
  *(undefined *)(piVar7 + 10) = 0;
  *(undefined *)(piVar7 + 0xe) = 0;
  *(undefined *)(piVar7 + 0x12) = 0;
  *(undefined *)(piVar7 + 0x16) = 0;
  *(undefined **)(piVar7 + 2) = puVar3;
  *(undefined **)(piVar7 + 4) = puVar5;
  *(undefined **)(piVar7 + 6) = puVar5;
  *(undefined **)(piVar7 + 8) = puVar5;
  *(undefined **)(piVar7 + 0xc) = puVar4;
  *(undefined **)(piVar7 + 0x10) = puVar4;
  *(undefined **)(piVar7 + 0x14) = puVar4;
  if (piVar7 != piVar8) {
    LOCK();
    *piVar7 = *piVar7 + 1;
    UNLOCK();
    piVar8 = (int *)*puVar6;
    *puVar6 = piVar7;
    if (piVar8 == (int *)0x0) {
LAB_00490216:
      puVar6 = *(undefined8 **)this;
      piVar8 = (int *)*puVar6;
    }
    else {
      LOCK();
      *piVar8 = *piVar8 + -1;
      UNLOCK();
      if (*piVar8 != 0) goto LAB_00490216;
      pQVar9 = *(QArrayData **)(piVar8 + 0x14);
      if (*(int *)pQVar9 == 0) {
LAB_00490450:
        QArrayData::deallocate(pQVar9,8,8);
      }
      else if (*(int *)pQVar9 != -1) {
        LOCK();
        *(int *)pQVar9 = *(int *)pQVar9 + -1;
        UNLOCK();
        if (*(int *)pQVar9 == 0) {
          pQVar9 = *(QArrayData **)(piVar8 + 0x14);
          goto LAB_00490450;
        }
      }
      pQVar9 = *(QArrayData **)(piVar8 + 0x10);
      if (*(int *)pQVar9 == 0) {
LAB_00490430:
        QArrayData::deallocate(pQVar9,2,8);
      }
      else if (*(int *)pQVar9 != -1) {
        LOCK();
        *(int *)pQVar9 = *(int *)pQVar9 + -1;
        UNLOCK();
        if (*(int *)pQVar9 == 0) {
          pQVar9 = *(QArrayData **)(piVar8 + 0x10);
          goto LAB_00490430;
        }
      }
      pQVar9 = *(QArrayData **)(piVar8 + 0xc);
      if (*(int *)pQVar9 == 0) {
LAB_00490410:
        QArrayData::deallocate(pQVar9,1,8);
      }
      else if (*(int *)pQVar9 != -1) {
        LOCK();
        *(int *)pQVar9 = *(int *)pQVar9 + -1;
        UNLOCK();
        if (*(int *)pQVar9 == 0) {
          pQVar9 = *(QArrayData **)(piVar8 + 0xc);
          goto LAB_00490410;
        }
      }
      FUN_004970d0(piVar8 + 8);
      pDVar11 = *(Data **)(piVar8 + 6);
      if (*(int *)pDVar11 == 0) {
LAB_004903b8:
        iVar2 = *(int *)(pDVar11 + 8);
        pDVar10 = pDVar11 + (long)*(int *)(pDVar11 + 0xc) * 8 + 0x10;
        if ((long)*(int *)(pDVar11 + 0xc) * 8 != (long)iVar2 * 8) {
          do {
            pDVar1 = pDVar10 + -8;
            pDVar10 = pDVar10 + -8;
            if (*(void **)pDVar1 != (void *)0x0) {
              operator_delete(*(void **)pDVar1,0x20);
            }
          } while (pDVar11 + (long)iVar2 * 8 + 0x10 != pDVar10);
        }
        QListData::dispose(pDVar11);
      }
      else if (*(int *)pDVar11 != -1) {
        LOCK();
        *(int *)pDVar11 = *(int *)pDVar11 + -1;
        UNLOCK();
        if (*(int *)pDVar11 == 0) {
          pDVar11 = *(Data **)(piVar8 + 6);
          goto LAB_004903b8;
        }
      }
      FUN_004970d0(piVar8 + 4);
      pQVar9 = *(QArrayData **)(piVar8 + 2);
      if (*(int *)pQVar9 == 0) {
LAB_004903a0:
        QArrayData::deallocate(pQVar9,2,8);
      }
      else if (*(int *)pQVar9 != -1) {
        LOCK();
        *(int *)pQVar9 = *(int *)pQVar9 + -1;
        UNLOCK();
        if (*(int *)pQVar9 == 0) {
          pQVar9 = *(QArrayData **)(piVar8 + 2);
          goto LAB_004903a0;
        }
      }
      operator_delete(piVar8,0x60);
      puVar6 = *(undefined8 **)this;
      piVar8 = (int *)*puVar6;
    }
    if (piVar8 == (int *)0x0) goto LAB_00490223;
  }
  if (*piVar8 == 1) {
    piVar8 = (int *)*puVar6;
  }
  else {
    FUN_00497d60(puVar6);
    piVar8 = (int *)*puVar6;
  }
LAB_00490223:
  KisCubicCurvePoint::KisCubicCurvePoint(local_48,0.0,0.0,false);
  FUN_00497860(piVar8 + 8);
  puVar6 = *(undefined8 **)this;
  piVar8 = (int *)*puVar6;
  if ((piVar8 != (int *)0x0) && (*piVar8 != 1)) {
    FUN_00497d60(puVar6);
    piVar8 = (int *)*puVar6;
  }
  KisCubicCurvePoint::KisCubicCurvePoint(local_48,DAT_007227c0,DAT_007227c0,false);
  FUN_00497860(piVar8 + 8,local_48);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCubicCurve @ 00490470 ======

/* WARNING: Removing unreachable block (ram,0x00490688) */
/* WARNING: Removing unreachable block (ram,0x00490690) */
/* WARNING: Removing unreachable block (ram,0x00490920) */
/* WARNING: Removing unreachable block (ram,0x00490949) */
/* WARNING: Removing unreachable block (ram,0x004906ce) */
/* WARNING: Removing unreachable block (ram,0x004906d6) */
/* WARNING: Removing unreachable block (ram,0x004906f2) */
/* WARNING: Removing unreachable block (ram,0x004906f8) */
/* WARNING: Removing unreachable block (ram,0x00490721) */
/* WARNING: Removing unreachable block (ram,0x00490725) */
/* WARNING: Removing unreachable block (ram,0x00490736) */
/* WARNING: Removing unreachable block (ram,0x0049073a) */
/* KisCubicCurve::KisCubicCurve(QString const&) */

void __thiscall KisCubicCurve::KisCubicCurve(KisCubicCurve *this,QString *param_1)

{
  Data *pDVar1;
  bool *pbVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  undefined *puVar6;
  undefined *puVar7;
  undefined *puVar8;
  undefined8 *puVar9;
  int *piVar10;
  QArrayData *pQVar11;
  Data *pDVar12;
  Data *pDVar13;
  long in_FS_OFFSET;
  QList *local_c8;
  int **local_c0;
  int *local_a0;
  undefined *local_98;
  long local_90;
  QLocale local_88 [32];
  int *local_68;
  int *local_60;
  int *local_58;
  undefined4 local_50;
  long local_40;
  
  puVar8 = PTR_shared_null_00837830;
  puVar6 = PTR_shared_null_008377d0;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  puVar9 = (undefined8 *)operator_new(8);
  *puVar9 = 0;
  *(undefined8 **)this = puVar9;
  piVar10 = (int *)operator_new(0x60);
  puVar7 = PTR_shared_null_008377d0;
  puVar9 = *(undefined8 **)this;
  *piVar10 = 0;
  *(undefined *)(piVar10 + 10) = 0;
  *(undefined *)(piVar10 + 0xe) = 0;
  *(undefined *)(piVar10 + 0x12) = 0;
  *(undefined *)(piVar10 + 0x16) = 0;
  *(undefined **)(piVar10 + 2) = puVar6;
  *(undefined **)(piVar10 + 4) = puVar8;
  *(undefined **)(piVar10 + 6) = puVar8;
  *(undefined **)(piVar10 + 8) = puVar8;
  *(undefined **)(piVar10 + 0xc) = puVar7;
  *(undefined **)(piVar10 + 0x10) = puVar7;
  *(undefined **)(piVar10 + 0x14) = puVar7;
  if (piVar10 != (int *)*puVar9) {
    LOCK();
    *piVar10 = *piVar10 + 1;
    UNLOCK();
    piVar5 = (int *)*puVar9;
    *puVar9 = piVar10;
    if (piVar5 != (int *)0x0) {
      LOCK();
      *piVar5 = *piVar5 + -1;
      UNLOCK();
      if (*piVar5 == 0) {
        pQVar11 = *(QArrayData **)(piVar5 + 0x14);
        if (*(int *)pQVar11 == 0) {
LAB_00490b71:
          QArrayData::deallocate(pQVar11,8,8);
        }
        else if (*(int *)pQVar11 != -1) {
          LOCK();
          *(int *)pQVar11 = *(int *)pQVar11 + -1;
          UNLOCK();
          if (*(int *)pQVar11 == 0) {
            pQVar11 = *(QArrayData **)(piVar5 + 0x14);
            goto LAB_00490b71;
          }
        }
        pQVar11 = *(QArrayData **)(piVar5 + 0x10);
        if (*(int *)pQVar11 == 0) {
LAB_00490b59:
          QArrayData::deallocate(pQVar11,2,8);
        }
        else if (*(int *)pQVar11 != -1) {
          LOCK();
          *(int *)pQVar11 = *(int *)pQVar11 + -1;
          UNLOCK();
          if (*(int *)pQVar11 == 0) {
            pQVar11 = *(QArrayData **)(piVar5 + 0x10);
            goto LAB_00490b59;
          }
        }
        pQVar11 = *(QArrayData **)(piVar5 + 0xc);
        if (*(int *)pQVar11 == 0) {
LAB_00490b41:
          QArrayData::deallocate(pQVar11,1,8);
        }
        else if (*(int *)pQVar11 != -1) {
          LOCK();
          *(int *)pQVar11 = *(int *)pQVar11 + -1;
          UNLOCK();
          if (*(int *)pQVar11 == 0) {
            pQVar11 = *(QArrayData **)(piVar5 + 0xc);
            goto LAB_00490b41;
          }
        }
        FUN_004970d0(piVar5 + 8);
        pDVar13 = *(Data **)(piVar5 + 6);
        if (*(int *)pDVar13 == 0) {
LAB_00490af2:
          iVar3 = *(int *)(pDVar13 + 8);
          pDVar12 = pDVar13 + (long)*(int *)(pDVar13 + 0xc) * 8 + 0x10;
          if ((long)*(int *)(pDVar13 + 0xc) * 8 != (long)iVar3 * 8) {
            do {
              pDVar1 = pDVar12 + -8;
              pDVar12 = pDVar12 + -8;
              if (*(void **)pDVar1 != (void *)0x0) {
                operator_delete(*(void **)pDVar1,0x20);
              }
            } while (pDVar13 + (long)iVar3 * 8 + 0x10 != pDVar12);
          }
          QListData::dispose(pDVar13);
        }
        else if (*(int *)pDVar13 != -1) {
          LOCK();
          *(int *)pDVar13 = *(int *)pDVar13 + -1;
          UNLOCK();
          if (*(int *)pDVar13 == 0) {
            pDVar13 = *(Data **)(piVar5 + 6);
            goto LAB_00490af2;
          }
        }
        FUN_004970d0(piVar5 + 4);
        pQVar11 = *(QArrayData **)(piVar5 + 2);
        if (*(int *)pQVar11 == 0) {
LAB_00490add:
          QArrayData::deallocate(pQVar11,2,8);
        }
        else if (*(int *)pQVar11 != -1) {
          LOCK();
          *(int *)pQVar11 = *(int *)pQVar11 + -1;
          UNLOCK();
          if (*(int *)pQVar11 == 0) {
            pQVar11 = *(QArrayData **)(piVar5 + 2);
            goto LAB_00490add;
          }
        }
        operator_delete(piVar5,0x60);
      }
    }
  }
  if (*(int *)(*(long *)param_1 + 4) == 0) {
    kis_safe_assert_recoverable
              ("!curveString.isEmpty()","/builds/graphics/krita/libs/image/kis_cubic_curve.cpp",0xd3
              );
    KisCubicCurve((KisCubicCurve *)&local_68);
                    /* try { // try from 004909b3 to 004909b7 has its CatchHandler @ 00490bd2 */
    operator=(this,(KisCubicCurve *)&local_68);
    ~KisCubicCurve((KisCubicCurve *)&local_68);
    goto LAB_004909c2;
  }
  QString::split((QChar)&local_a0,(QFlags)param_1,0x3b);
  local_98 = PTR_shared_null_00837830;
  local_68 = local_a0;
  if (*local_a0 == 0) {
    QListData::detach((int)&local_68);
    local_a0 = local_a0 + (long)local_a0[2] * 2 + 4;
    iVar3 = local_68[3];
    local_60 = local_68 + (long)local_68[2] * 2 + 4;
    local_58 = local_60;
    if ((long)iVar3 * 8 != (long)local_68[2] * 8) {
      do {
        while( true ) {
          piVar10 = *(int **)local_a0;
          *(int **)local_60 = piVar10;
          if (1 < *piVar10 + 1U) break;
          local_60 = local_60 + 2;
          local_a0 = local_a0 + 2;
          if (local_68 + (long)iVar3 * 2 + 4 == local_60) goto LAB_0049086e;
        }
        LOCK();
        *piVar10 = *piVar10 + 1;
        UNLOCK();
        local_60 = local_60 + 2;
        local_a0 = local_a0 + 2;
      } while (local_68 + (long)iVar3 * 2 + 4 != local_60);
LAB_0049086e:
      iVar3 = local_68[2];
      iVar4 = local_68[3];
      goto LAB_004905cf;
    }
  }
  else {
    if (*local_a0 != -1) {
      LOCK();
      *local_a0 = *local_a0 + 1;
      UNLOCK();
    }
    iVar3 = local_a0[2];
    iVar4 = local_a0[3];
LAB_004905cf:
    local_c0 = &local_68;
    local_60 = local_68 + (long)iVar3 * 2 + 4;
    local_58 = local_68 + (long)iVar4 * 2 + 4;
    if ((long)iVar3 * 8 != (long)iVar4 * 8) {
      local_50 = 1;
                    /* try { // try from 00490633 to 00490637 has its CatchHandler @ 00490bde */
      QString::split((QChar)&local_90,(QFlags)local_60,0x2c);
      if (*(int *)(local_90 + 0xc) - *(int *)(local_90 + 8) < 2) {
                    /* try { // try from 00490aad to 00490abe has its CatchHandler @ 00490bae */
        kis_safe_assert_recoverable
                  ("entryData.size() > 1","/builds/graphics/krita/libs/image/kis_cubic_curve.cpp",
                   0xdd);
        KisCubicCurve((KisCubicCurve *)local_88);
                    /* try { // try from 00490ac7 to 00490acb has its CatchHandler @ 00490b8a */
        operator=(this,(KisCubicCurve *)local_88);
        ~KisCubicCurve((KisCubicCurve *)local_88);
      }
      else {
        pbVar2 = (bool *)(local_90 + 0x10 + (long)*(int *)(local_90 + 8) * 8);
                    /* try { // try from 00490663 to 00490667 has its CatchHandler @ 00490bae */
        QLocale::QLocale(local_88,0x2a,0);
                    /* try { // try from 00490673 to 00490677 has its CatchHandler @ 00490bea */
        QString::toDouble(pbVar2);
                    /* try { // try from 00490899 to 0049089d has its CatchHandler @ 00490bea */
        QLocale::toDouble((QString *)local_88,pbVar2);
        QLocale::~QLocale(local_88);
                    /* try { // try from 004908cc to 004908d8 has its CatchHandler @ 00490bae */
        kis_safe_assert_recoverable
                  ("ok","/builds/graphics/krita/libs/image/kis_cubic_curve.cpp",0xe3);
        KisCubicCurve((KisCubicCurve *)local_88);
                    /* try { // try from 004908e1 to 004908e5 has its CatchHandler @ 00490bc6 */
        operator=(this,(KisCubicCurve *)local_88);
        ~KisCubicCurve((KisCubicCurve *)local_88);
      }
      FUN_003412b0(&local_90);
      FUN_003412b0(local_c0);
      FUN_004970d0(&local_98);
      FUN_003412b0(&local_a0);
      goto LAB_004909c2;
    }
  }
  local_50 = 1;
  local_c0 = &local_68;
  local_c8 = (QList *)&local_98;
  FUN_003412b0(local_c0);
                    /* try { // try from 0049079b to 004907d5 has its CatchHandler @ 00490bba */
  setPoints(this,local_c8);
  FUN_004970d0(local_c8);
  FUN_003412b0(&local_a0);
LAB_004909c2:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCubicCurve @ 00491940 ======

/* KisCubicCurve::KisCubicCurve(QList<QPointF> const&) */

void __thiscall KisCubicCurve::KisCubicCurve(KisCubicCurve *this,QList *param_1)

{
  Data *pDVar1;
  int iVar2;
  undefined *puVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined8 *puVar6;
  int *piVar7;
  int *piVar8;
  long lVar9;
  long lVar10;
  QArrayData *pQVar11;
  Data *pDVar12;
  Data *pDVar13;
  long in_FS_OFFSET;
  undefined8 local_98;
  undefined8 uStack_90;
  KisCubicCurvePoint local_88 [32];
  long local_68;
  long *local_60;
  long *local_58;
  undefined4 local_50;
  long local_40;
  
  puVar5 = PTR_shared_null_00837830;
  puVar3 = PTR_shared_null_008377d0;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  puVar6 = (undefined8 *)operator_new(8);
  *puVar6 = 0;
  *(undefined8 **)this = puVar6;
  piVar7 = (int *)operator_new(0x60);
  puVar4 = PTR_shared_null_008377d0;
  puVar6 = *(undefined8 **)this;
  *piVar7 = 0;
  piVar8 = (int *)*puVar6;
  *(undefined *)(piVar7 + 10) = 0;
  *(undefined *)(piVar7 + 0xe) = 0;
  *(undefined *)(piVar7 + 0x12) = 0;
  *(undefined *)(piVar7 + 0x16) = 0;
  *(undefined **)(piVar7 + 2) = puVar3;
  *(undefined **)(piVar7 + 4) = puVar5;
  *(undefined **)(piVar7 + 6) = puVar5;
  *(undefined **)(piVar7 + 8) = puVar5;
  *(undefined **)(piVar7 + 0xc) = puVar4;
  *(undefined **)(piVar7 + 0x10) = puVar4;
  *(undefined **)(piVar7 + 0x14) = puVar4;
  if (piVar7 != piVar8) {
    LOCK();
    *piVar7 = *piVar7 + 1;
    UNLOCK();
    piVar8 = (int *)*puVar6;
    *puVar6 = piVar7;
    if (piVar8 == (int *)0x0) {
LAB_00491a20:
      puVar6 = *(undefined8 **)this;
      piVar8 = (int *)*puVar6;
    }
    else {
      LOCK();
      *piVar8 = *piVar8 + -1;
      UNLOCK();
      if (*piVar8 != 0) goto LAB_00491a20;
      pQVar11 = *(QArrayData **)(piVar8 + 0x14);
      if (*(int *)pQVar11 == 0) {
LAB_00491cf6:
        QArrayData::deallocate(pQVar11,8,8);
      }
      else if (*(int *)pQVar11 != -1) {
        LOCK();
        *(int *)pQVar11 = *(int *)pQVar11 + -1;
        UNLOCK();
        if (*(int *)pQVar11 == 0) {
          pQVar11 = *(QArrayData **)(piVar8 + 0x14);
          goto LAB_00491cf6;
        }
      }
      pQVar11 = *(QArrayData **)(piVar8 + 0x10);
      if (*(int *)pQVar11 == 0) {
LAB_00491cde:
        QArrayData::deallocate(pQVar11,2,8);
      }
      else if (*(int *)pQVar11 != -1) {
        LOCK();
        *(int *)pQVar11 = *(int *)pQVar11 + -1;
        UNLOCK();
        if (*(int *)pQVar11 == 0) {
          pQVar11 = *(QArrayData **)(piVar8 + 0x10);
          goto LAB_00491cde;
        }
      }
      pQVar11 = *(QArrayData **)(piVar8 + 0xc);
      if (*(int *)pQVar11 == 0) {
LAB_00491cc6:
        QArrayData::deallocate(pQVar11,1,8);
      }
      else if (*(int *)pQVar11 != -1) {
        LOCK();
        *(int *)pQVar11 = *(int *)pQVar11 + -1;
        UNLOCK();
        if (*(int *)pQVar11 == 0) {
          pQVar11 = *(QArrayData **)(piVar8 + 0xc);
          goto LAB_00491cc6;
        }
      }
      FUN_004970d0(piVar8 + 8);
      pDVar13 = *(Data **)(piVar8 + 6);
      if (*(int *)pDVar13 == 0) {
LAB_00491c71:
        iVar2 = *(int *)(pDVar13 + 8);
        pDVar12 = pDVar13 + (long)*(int *)(pDVar13 + 0xc) * 8 + 0x10;
        if ((long)*(int *)(pDVar13 + 0xc) * 8 != (long)iVar2 * 8) {
          do {
            pDVar1 = pDVar12 + -8;
            pDVar12 = pDVar12 + -8;
            if (*(void **)pDVar1 != (void *)0x0) {
              operator_delete(*(void **)pDVar1,0x20);
            }
          } while (pDVar13 + (long)iVar2 * 8 + 0x10 != pDVar12);
        }
        QListData::dispose(pDVar13);
      }
      else if (*(int *)pDVar13 != -1) {
        LOCK();
        *(int *)pDVar13 = *(int *)pDVar13 + -1;
        UNLOCK();
        if (*(int *)pDVar13 == 0) {
          pDVar13 = *(Data **)(piVar8 + 6);
          goto LAB_00491c71;
        }
      }
      FUN_004970d0(piVar8 + 4);
      pQVar11 = *(QArrayData **)(piVar8 + 2);
      if (*(int *)pQVar11 == 0) {
LAB_00491c5c:
        QArrayData::deallocate(pQVar11,2,8);
      }
      else if (*(int *)pQVar11 != -1) {
        LOCK();
        *(int *)pQVar11 = *(int *)pQVar11 + -1;
        UNLOCK();
        if (*(int *)pQVar11 == 0) {
          pQVar11 = *(QArrayData **)(piVar8 + 2);
          goto LAB_00491c5c;
        }
      }
      operator_delete(piVar8,0x60);
      puVar6 = *(undefined8 **)this;
      piVar8 = (int *)*puVar6;
    }
    if (piVar8 == (int *)0x0) goto LAB_00491a31;
  }
  if (*piVar8 == 1) {
    piVar8 = (int *)*puVar6;
  }
  else {
    FUN_00497d60(puVar6);
    piVar8 = (int *)*puVar6;
  }
LAB_00491a31:
  if ((int)(*(uint **)(piVar8 + 8))[1] <
      *(int *)(*(long *)param_1 + 0xc) - *(int *)(*(long *)param_1 + 8)) {
    if (**(uint **)(piVar8 + 8) < 2) {
      QListData::realloc((int)piVar8 + 0x20);
    }
    else {
      FUN_00497940();
    }
  }
  FUN_004992f0(&local_68,param_1);
  local_50 = 1;
  lVar9 = (long)*(int *)(local_68 + 8) * 8;
  lVar10 = (long)*(int *)(local_68 + 0xc) * 8;
  local_60 = (long *)(local_68 + 0x10 + lVar9);
  local_58 = (long *)(local_68 + 0x10 + lVar10);
  if (lVar9 != lVar10) {
    do {
      puVar6 = *(undefined8 **)this;
      local_98 = *(undefined8 *)*local_60;
      uStack_90 = ((undefined8 *)*local_60)[1];
      piVar8 = (int *)*puVar6;
      if ((piVar8 != (int *)0x0) && (*piVar8 != 1)) {
                    /* try { // try from 00491aba to 00491ae2 has its CatchHandler @ 00491d0f */
        FUN_00497d60(puVar6);
        piVar8 = (int *)*puVar6;
      }
      KisCubicCurvePoint::KisCubicCurvePoint(local_88,(QPointF *)&local_98,false);
      FUN_00497860(piVar8 + 8,local_88);
      local_60 = local_60 + 1;
    } while (local_60 != local_58);
  }
  FUN_004971d0(&local_68);
  puVar6 = *(undefined8 **)this;
  piVar8 = (int *)*puVar6;
  if ((piVar8 != (int *)0x0) && (*piVar8 != 1)) {
    FUN_00497d60(puVar6);
    piVar8 = (int *)*puVar6;
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    FUN_004915b0();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail(piVar8);
}



// ====== KisCubicCurve @ 00491d20 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisCubicCurve::KisCubicCurve(QList<KisCubicCurvePoint> const&) */

void __thiscall KisCubicCurve::KisCubicCurve(KisCubicCurve *this,QList *param_1)

{
  Data *pDVar1;
  int iVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined *puVar6;
  undefined8 *puVar7;
  int *piVar8;
  long lVar9;
  int *piVar10;
  QArrayData *pQVar11;
  long *plVar12;
  Data *pDVar13;
  Data *pDVar14;
  long in_FS_OFFSET;
  undefined8 local_48;
  long local_40;
  
  puVar6 = PTR_shared_null_00837830;
  puVar4 = PTR_shared_null_008377d0;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  puVar7 = (undefined8 *)operator_new(8);
  *puVar7 = 0;
  *(undefined8 **)this = puVar7;
  piVar8 = (int *)operator_new(0x60);
  puVar5 = PTR_shared_null_008377d0;
  plVar12 = *(long **)this;
  *piVar8 = 0;
  piVar10 = (int *)*plVar12;
  *(undefined *)(piVar8 + 10) = 0;
  *(undefined *)(piVar8 + 0xe) = 0;
  *(undefined *)(piVar8 + 0x12) = 0;
  *(undefined *)(piVar8 + 0x16) = 0;
  *(undefined **)(piVar8 + 2) = puVar4;
  *(undefined **)(piVar8 + 4) = puVar6;
  *(undefined **)(piVar8 + 6) = puVar6;
  *(undefined **)(piVar8 + 8) = puVar6;
  *(undefined **)(piVar8 + 0xc) = puVar5;
  *(undefined **)(piVar8 + 0x10) = puVar5;
  *(undefined **)(piVar8 + 0x14) = puVar5;
  if (piVar8 == piVar10) {
LAB_00491e80:
    if (*piVar10 == 1) {
      lVar9 = *plVar12;
      if (*(long *)(lVar9 + 0x20) != *(long *)param_1) goto LAB_00491e18;
LAB_00491e9a:
      puVar7 = *(undefined8 **)this;
      piVar10 = (int *)*puVar7;
    }
    else {
      FUN_00497d60(plVar12);
      lVar9 = *plVar12;
      if (*(long *)(lVar9 + 0x20) == *(long *)param_1) goto LAB_00491e9a;
LAB_00491e18:
      FUN_00497250(&local_48,param_1);
      uVar3 = *(undefined8 *)(lVar9 + 0x20);
      *(undefined8 *)(lVar9 + 0x20) = local_48;
      local_48 = uVar3;
      FUN_004970d0(&local_48);
      puVar7 = *(undefined8 **)this;
      piVar10 = (int *)*puVar7;
    }
    if (piVar10 != (int *)0x0) {
      if (*piVar10 != 1) {
        FUN_00497d60(puVar7);
        piVar10 = (int *)*puVar7;
      }
      goto LAB_00491e54;
    }
  }
  else {
    LOCK();
    *piVar8 = *piVar8 + 1;
    UNLOCK();
    piVar10 = (int *)*plVar12;
    *plVar12 = (long)piVar8;
    if (piVar10 == (int *)0x0) {
LAB_00491dfa:
      plVar12 = *(long **)this;
      piVar10 = (int *)*plVar12;
    }
    else {
      LOCK();
      *piVar10 = *piVar10 + -1;
      UNLOCK();
      if (*piVar10 != 0) goto LAB_00491dfa;
      pQVar11 = *(QArrayData **)(piVar10 + 0x14);
      if (*(int *)pQVar11 == 0) {
LAB_00492050:
        QArrayData::deallocate(pQVar11,8,8);
      }
      else if (*(int *)pQVar11 != -1) {
        LOCK();
        *(int *)pQVar11 = *(int *)pQVar11 + -1;
        UNLOCK();
        if (*(int *)pQVar11 == 0) {
          pQVar11 = *(QArrayData **)(piVar10 + 0x14);
          goto LAB_00492050;
        }
      }
      pQVar11 = *(QArrayData **)(piVar10 + 0x10);
      if (*(int *)pQVar11 == 0) {
LAB_00492030:
        QArrayData::deallocate(pQVar11,2,8);
      }
      else if (*(int *)pQVar11 != -1) {
        LOCK();
        *(int *)pQVar11 = *(int *)pQVar11 + -1;
        UNLOCK();
        if (*(int *)pQVar11 == 0) {
          pQVar11 = *(QArrayData **)(piVar10 + 0x10);
          goto LAB_00492030;
        }
      }
      pQVar11 = *(QArrayData **)(piVar10 + 0xc);
      if (*(int *)pQVar11 == 0) {
LAB_00492010:
        QArrayData::deallocate(pQVar11,1,8);
      }
      else if (*(int *)pQVar11 != -1) {
        LOCK();
        *(int *)pQVar11 = *(int *)pQVar11 + -1;
        UNLOCK();
        if (*(int *)pQVar11 == 0) {
          pQVar11 = *(QArrayData **)(piVar10 + 0xc);
          goto LAB_00492010;
        }
      }
      FUN_004970d0(piVar10 + 8);
      pDVar14 = *(Data **)(piVar10 + 6);
      if (*(int *)pDVar14 == 0) {
LAB_00491fb8:
        iVar2 = *(int *)(pDVar14 + 8);
        pDVar13 = pDVar14 + (long)*(int *)(pDVar14 + 0xc) * 8 + 0x10;
        if ((long)*(int *)(pDVar14 + 0xc) * 8 != (long)iVar2 * 8) {
          do {
            pDVar1 = pDVar13 + -8;
            pDVar13 = pDVar13 + -8;
            if (*(void **)pDVar1 != (void *)0x0) {
              operator_delete(*(void **)pDVar1,0x20);
            }
          } while (pDVar14 + (long)iVar2 * 8 + 0x10 != pDVar13);
        }
        QListData::dispose(pDVar14);
      }
      else if (*(int *)pDVar14 != -1) {
        LOCK();
        *(int *)pDVar14 = *(int *)pDVar14 + -1;
        UNLOCK();
        if (*(int *)pDVar14 == 0) {
          pDVar14 = *(Data **)(piVar10 + 6);
          goto LAB_00491fb8;
        }
      }
      FUN_004970d0(piVar10 + 4);
      pQVar11 = *(QArrayData **)(piVar10 + 2);
      if (*(int *)pQVar11 == 0) {
LAB_00491fa0:
        QArrayData::deallocate(pQVar11,2,8);
      }
      else if (*(int *)pQVar11 != -1) {
        LOCK();
        *(int *)pQVar11 = *(int *)pQVar11 + -1;
        UNLOCK();
        if (*(int *)pQVar11 == 0) {
          pQVar11 = *(QArrayData **)(piVar10 + 2);
          goto LAB_00491fa0;
        }
      }
      operator_delete(piVar10,0x60);
      plVar12 = *(long **)this;
      piVar10 = (int *)*plVar12;
    }
    if (piVar10 != (int *)0x0) goto LAB_00491e80;
    lVar9 = 0;
    if (_DAT_00000020 != *(long *)param_1) goto LAB_00491e18;
  }
  piVar10 = (int *)0x0;
LAB_00491e54:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail(piVar10);
  }
  FUN_004915b0();
  return;
}



