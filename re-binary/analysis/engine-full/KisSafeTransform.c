/* Class KisSafeTransform - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSafeTransform @ 002022f0 ======

void __thiscall
KisSafeTransform::KisSafeTransform
          (KisSafeTransform *this,QTransform *param_1,QRect *param_2,QRect *param_3)

{
  (*(code *)PTR_KisSafeTransform_00838c48)();
  return;
}



// ====== KisSafeTransform @ 004cf3c0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSafeTransform::KisSafeTransform(QTransform const&, QRect const&, QRect const&) */

void __thiscall
KisSafeTransform::KisSafeTransform
          (KisSafeTransform *this,QTransform *param_1,QRect *param_2,QRect *param_3)

{
  QPointF *pQVar1;
  double dVar2;
  QArrayData *pQVar3;
  int iVar4;
  undefined8 uVar5;
  undefined *puVar6;
  int iVar7;
  uint uVar8;
  undefined *puVar9;
  long lVar10;
  uint uVar11;
  long in_FS_OFFSET;
  double dVar12;
  int iVar13;
  int iVar15;
  undefined8 uVar14;
  undefined auVar16 [16];
  undefined auVar17 [16];
  double local_f8;
  double local_f0;
  double local_e8;
  double local_e0;
  undefined local_c8 [16];
  undefined local_b8 [16];
  undefined local_a8 [16];
  undefined local_98 [16];
  undefined local_88 [16];
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  puVar9 = (undefined *)operator_new(0xd8);
  uVar5 = DAT_00721778;
  uVar14 = _DAT_00721770;
  *puVar9 = 1;
  *(undefined8 *)(puVar9 + 4) = uVar14;
  *(undefined8 *)(puVar9 + 0xc) = uVar5;
                    /* try { // try from 004cf417 to 004cf424 has its CatchHandler @ 004cfc5d */
  QTransform::QTransform((QTransform *)(puVar9 + 0x18));
  QTransform::QTransform((QTransform *)(puVar9 + 0x70));
  *(undefined **)this = puVar9;
  puVar6 = PTR_shared_null_008377d0;
  uVar14 = *(undefined8 *)(param_2 + 8);
  *(undefined8 *)(puVar9 + 4) = *(undefined8 *)param_2;
  *(undefined8 *)(puVar9 + 0xc) = uVar14;
  *(undefined **)(puVar9 + 200) = puVar6;
  *(undefined **)(puVar9 + 0xd0) = puVar6;
  QTransform::operator=((QTransform *)(puVar9 + 0x18),(QTransform *)param_1);
                    /* try { // try from 004cf45d to 004cfa86 has its CatchHandler @ 004cfc51 */
  QTransform::inverted((bool *)local_98);
  lVar10 = *(long *)this;
  *(undefined8 *)(lVar10 + 0x70) = local_98._0_8_;
  *(undefined8 *)(lVar10 + 0x78) = local_98._8_8_;
  *(undefined8 *)(lVar10 + 0x80) = local_88._0_8_;
  *(undefined8 *)(lVar10 + 0x88) = local_88._8_8_;
  *(undefined8 *)(lVar10 + 0x90) = local_78;
  *(undefined8 *)(lVar10 + 0x98) = uStack_70;
  *(undefined8 *)(lVar10 + 0xa0) = local_68;
  *(undefined8 *)(lVar10 + 0xa8) = uStack_60;
  *(undefined8 *)(lVar10 + 0xb0) = local_58;
  *(undefined8 *)(lVar10 + 0xb8) = uStack_50;
  *(undefined8 *)(lVar10 + 0xc0) = local_48;
  iVar7 = QTransform::type();
  lVar10 = *(long *)this;
  *(bool *)lVar10 = 8 < iVar7;
  if (iVar7 < 9) goto LAB_004cf4c7;
  iVar7 = (int)*(undefined8 *)(lVar10 + 4);
  local_98._0_8_ = (undefined8)iVar7;
  iVar4 = (int)((ulong)*(undefined8 *)(lVar10 + 4) >> 0x20);
  local_98._8_8_ = (undefined8)iVar4;
  iVar13 = (int)DAT_00726ba0;
  iVar15 = (int)((ulong)DAT_00726ba0 >> 0x20);
  local_88._0_8_ = (undefined8)(((int)*(undefined8 *)(lVar10 + 0xc) - iVar7) + iVar13);
  local_88._8_8_ =
       (undefined8)(((int)((ulong)*(undefined8 *)(lVar10 + 0xc) >> 0x20) - iVar4) + iVar15);
  QPolygonF::QPolygonF((QPolygonF *)local_b8,(QRectF *)local_98);
  lVar10 = *(long *)this;
  pQVar3 = *(QArrayData **)(lVar10 + 200);
  *(undefined8 *)(lVar10 + 200) = local_b8._0_8_;
  local_b8._0_8_ = pQVar3;
  if (*(int *)pQVar3 == 0) {
LAB_004cfa00:
    QArrayData::deallocate(pQVar3,0x10,8);
    lVar10 = *(long *)this;
  }
  else if (*(int *)pQVar3 != -1) {
    LOCK();
    *(int *)pQVar3 = *(int *)pQVar3 + -1;
    UNLOCK();
    if (*(int *)pQVar3 == 0) goto LAB_004cfa00;
    lVar10 = *(long *)this;
  }
  iVar7 = (int)*(undefined8 *)(lVar10 + 4);
  iVar4 = (int)((ulong)*(undefined8 *)(lVar10 + 4) >> 0x20);
  local_98._8_8_ = (double)iVar4;
  local_98._0_8_ = (double)iVar7;
  local_88._8_8_ = (double)(((int)((ulong)*(undefined8 *)(lVar10 + 0xc) >> 0x20) - iVar4) + iVar15);
  local_88._0_8_ = (double)(((int)*(undefined8 *)(lVar10 + 0xc) - iVar7) + iVar13);
  QPolygonF::QPolygonF((QPolygonF *)local_b8,(QRectF *)local_98);
  lVar10 = *(long *)this;
  pQVar3 = *(QArrayData **)(lVar10 + 0xd0);
  *(undefined8 *)(lVar10 + 0xd0) = local_b8._0_8_;
  local_b8._0_8_ = pQVar3;
  if (*(int *)pQVar3 == 0) {
LAB_004cfa20:
    QArrayData::deallocate(pQVar3,0x10,8);
    lVar10 = *(long *)this;
  }
  else if (*(int *)pQVar3 != -1) {
    LOCK();
    *(int *)pQVar3 = *(int *)pQVar3 + -1;
    UNLOCK();
    if (*(int *)pQVar3 == 0) goto LAB_004cfa20;
    lVar10 = *(long *)this;
  }
  pQVar1 = (QPointF *)(lVar10 + 0x70);
  local_b8 = (undefined  [16])0x0;
  local_a8 = (undefined  [16])0x0;
  dVar2 = *(double *)(lVar10 + 0xa8);
  local_e0 = *(double *)(lVar10 + 0xa0);
  local_f8 = *(double *)(lVar10 + 0x88) / dVar2;
  local_f0 = *(double *)(lVar10 + 0x80) / dVar2;
  if (local_e0 < 0.0) {
    if (_DAT_00724430 <= (double)((ulong)local_e0 ^ DAT_00722790)) goto LAB_004cf64c;
LAB_004cf8d0:
    if (dVar2 < 0.0) {
      if (dVar2 <= _DAT_0072fb18) goto LAB_004cf8e8;
    }
    else if (_DAT_00724430 <= dVar2) {
LAB_004cf8e8:
      local_c8 = (undefined  [16])0x0;
      auVar16 = QTransform::map(pQVar1);
      local_98._8_8_ = 0;
      local_98._0_8_ = DAT_0072fb40;
      auVar17 = QTransform::map(pQVar1);
      local_e8 = (auVar17._8_8_ - auVar16._8_8_) + local_f8;
      local_e0 = (auVar17._0_8_ - auVar16._0_8_) + local_f0;
      lVar10 = *(long *)this;
      goto LAB_004cf678;
    }
    local_b8 = (undefined  [16])0x0;
    local_a8 = (undefined  [16])0x0;
  }
  else {
    if (local_e0 < _DAT_00724430) goto LAB_004cf8d0;
LAB_004cf64c:
    local_e8 = *(double *)(lVar10 + 0x78) / local_e0;
    local_e0 = *(double *)(lVar10 + 0x70) / local_e0;
    if (dVar2 < 0.0) {
      if (_DAT_0072fb18 < dVar2) goto LAB_004cfa4e;
    }
    else if (dVar2 < _DAT_00724430) {
LAB_004cfa4e:
      local_c8 = (undefined  [16])0x0;
      auVar16 = QTransform::map(pQVar1);
      local_98._8_8_ = _DAT_0072fb38;
      local_98._0_8_ = _DAT_0072fb30;
      auVar17 = QTransform::map(pQVar1);
      local_f8 = (auVar17._8_8_ - auVar16._8_8_) + local_e8;
      local_f0 = (auVar17._0_8_ - auVar16._0_8_) + local_e0;
      lVar10 = *(long *)this;
    }
LAB_004cf678:
    uVar8 = *(int *)(param_3 + 8) - *(int *)param_3;
    local_b8._8_8_ = local_e8;
    local_b8._0_8_ = local_e0;
    uVar11 = *(int *)(param_3 + 0xc) - *(int *)(param_3 + 4);
    local_a8._8_8_ = local_f8;
    local_a8._0_8_ = local_f0;
    uVar14 = DAT_007227c0;
    if ((-1 < (int)(uVar8 | uVar11)) &&
       ((((double)(int)(uVar11 + 1) * DAT_007227c8 + (double)*(int *)(param_3 + 4)) - local_e8) *
        (local_f0 - local_e0) -
        (local_f8 - local_e8) *
        (((double)*(int *)param_3 + (double)(int)(uVar8 + 1) * DAT_007227c8) - local_e0) < 0.0)) {
      uVar14 = DAT_0072fb20;
    }
    FUN_004cec50(uVar14,(QRectF *)local_98,(QPolygonF *)local_b8,lVar10 + 4);
    lVar10 = *(long *)this;
    pQVar3 = *(QArrayData **)(lVar10 + 200);
    *(undefined8 *)(lVar10 + 200) = local_98._0_8_;
    local_98._0_8_ = pQVar3;
    if (*(int *)pQVar3 == 0) {
LAB_004cfab8:
      QArrayData::deallocate(pQVar3,0x10,8);
      lVar10 = *(long *)this;
    }
    else if (*(int *)pQVar3 != -1) {
      LOCK();
      *(int *)pQVar3 = *(int *)pQVar3 + -1;
      UNLOCK();
      if (*(int *)pQVar3 == 0) goto LAB_004cfab8;
      lVar10 = *(long *)this;
    }
  }
  pQVar1 = (QPointF *)(lVar10 + 0x18);
  local_98 = (undefined  [16])0x0;
  local_88 = (undefined  [16])0x0;
  dVar2 = *(double *)(lVar10 + 0x50);
  local_e0 = *(double *)(lVar10 + 0x48);
  local_f8 = *(double *)(lVar10 + 0x30) / dVar2;
  local_f0 = *(double *)(lVar10 + 0x28) / dVar2;
  dVar12 = local_e0;
  if (local_e0 < 0.0) {
    dVar12 = (double)((ulong)local_e0 ^ DAT_00722790);
  }
  if (_DAT_00724430 <= dVar12) {
    local_e8 = *(double *)(lVar10 + 0x20) / local_e0;
    local_e0 = *(double *)(lVar10 + 0x18) / local_e0;
    if (dVar2 < 0.0) {
      if (_DAT_0072fb18 < dVar2) goto LAB_004cf99c;
    }
    else if (dVar2 < _DAT_00724430) {
LAB_004cf99c:
      auVar16 = QTransform::map(pQVar1);
      local_c8._8_8_ = _DAT_0072fb38;
      local_c8._0_8_ = _DAT_0072fb30;
      auVar17 = QTransform::map(pQVar1);
      local_f8 = (auVar17._8_8_ - auVar16._8_8_) + local_e8;
      local_f0 = (auVar17._0_8_ - auVar16._0_8_) + local_e0;
    }
LAB_004cf7ef:
    local_98._8_8_ = local_e8;
    local_98._0_8_ = local_e0;
    local_88._8_8_ = local_f8;
    local_88._0_8_ = local_f0;
    auVar16 = mapRectForward((QRect *)this);
    uVar8 = auVar16._8_4_ - auVar16._0_4_;
    uVar11 = auVar16._12_4_ - auVar16._4_4_;
    uVar14 = DAT_007227c0;
    if ((-1 < (int)(uVar8 | uVar11)) &&
       ((((double)(int)(uVar11 + 1) * DAT_007227c8 + (double)auVar16._4_4_) - local_e8) *
        (local_f0 - local_e0) -
        (local_f8 - local_e8) *
        (((double)auVar16._0_4_ + (double)(int)(uVar8 + 1) * DAT_007227c8) - local_e0) < 0.0)) {
      uVar14 = DAT_0072fb20;
    }
    FUN_004cec50(uVar14,local_c8,(QRectF *)local_98,*(long *)this + 4);
    pQVar3 = *(QArrayData **)(*(long *)this + 0xd0);
    *(undefined8 *)(*(long *)this + 0xd0) = local_c8._0_8_;
    local_c8._0_8_ = pQVar3;
    if (*(int *)pQVar3 == 0) {
LAB_004cf890:
      if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
        QArrayData::deallocate(pQVar3,0x10,8);
        return;
      }
      goto LAB_004cfc4c;
    }
    if (*(int *)pQVar3 != -1) {
      LOCK();
      *(int *)pQVar3 = *(int *)pQVar3 + -1;
      UNLOCK();
      if (*(int *)pQVar3 == 0) goto LAB_004cf890;
    }
  }
  else if (dVar2 < 0.0) {
    if (dVar2 <= _DAT_0072fb18) goto LAB_004cf790;
  }
  else if (_DAT_00724430 <= dVar2) {
LAB_004cf790:
    auVar16 = QTransform::map(pQVar1);
    local_c8._8_8_ = 0;
    local_c8._0_8_ = DAT_0072fb40;
    auVar17 = QTransform::map(pQVar1);
    local_e8 = (auVar17._8_8_ - auVar16._8_8_) + local_f8;
    local_e0 = (auVar17._0_8_ - auVar16._0_8_) + local_f0;
    goto LAB_004cf7ef;
  }
LAB_004cf4c7:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_004cfc4c:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



