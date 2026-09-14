/* Class KisDistanceInformation - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisDistanceInformation @ 00203010 ======

void __thiscall
KisDistanceInformation::KisDistanceInformation
          (KisDistanceInformation *this,QPointF *param_1,double param_2,double param_3,
          double param_4,int param_5)

{
  (*(code *)PTR_KisDistanceInformation_008392d8)();
  return;
}



// ====== KisDistanceInformation @ 00206820 ======

void __thiscall KisDistanceInformation::KisDistanceInformation(KisDistanceInformation *this)

{
  (*(code *)PTR_KisDistanceInformation_0083aee0)();
  return;
}



// ====== KisDistanceInformation @ 00207b60 ======

void __thiscall
KisDistanceInformation::KisDistanceInformation
          (KisDistanceInformation *this,double param_1,double param_2,int param_3)

{
  (*(code *)PTR_KisDistanceInformation_0083b880)();
  return;
}



// ====== KisDistanceInformation @ 00209690 ======

void __thiscall
KisDistanceInformation::KisDistanceInformation
          (KisDistanceInformation *this,QPointF *param_1,double param_2)

{
  (*(code *)PTR_KisDistanceInformation_0083c618)();
  return;
}



// ====== KisDistanceInformation @ 0030c1a0 ======

/* KisDistanceInformation::KisDistanceInformation() */

void __thiscall KisDistanceInformation::KisDistanceInformation(KisDistanceInformation *this)

{
  undefined8 uVar1;
  double dVar2;
  undefined (*pauVar3) [16];
  long in_FS_OFFSET;
  QPointF local_38 [24];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  pauVar3 = (undefined (*) [16])operator_new(200);
  uVar1 = DAT_007227a0;
  pauVar3[1][8] = 1;
  *(undefined8 *)pauVar3[4] = uVar1;
  *(undefined8 *)(pauVar3[4] + 8) = 0;
  uVar1 = DAT_00722788;
  *(undefined8 *)pauVar3[1] = 0;
  *(undefined8 *)pauVar3[3] = 0;
  pauVar3[3][8] = 0;
  pauVar3[5][0] = 0;
  pauVar3[8][8] = 0;
  *pauVar3 = (undefined  [16])0x0;
  pauVar3[2] = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar3[6] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar3[7] + 8) = (undefined  [16])0x0;
  dVar2 = DAT_007227c0;
  local_38[0] = (QPointF)0x0;
  local_38[1] = (QPointF)0x0;
  local_38[2] = (QPointF)0x0;
  local_38[3] = (QPointF)0x0;
  local_38[4] = (QPointF)0x0;
  local_38[5] = (QPointF)0x0;
  local_38[6] = (QPointF)0x0;
  local_38[7] = (QPointF)0x0;
  local_38[8] = (QPointF)0x0;
  local_38[9] = (QPointF)0x0;
  local_38[10] = (QPointF)0x0;
  local_38[0xb] = (QPointF)0x0;
  local_38[0xc] = (QPointF)0x0;
  local_38[0xd] = (QPointF)0x0;
  local_38[0xe] = (QPointF)0x0;
  local_38[0xf] = (QPointF)0x0;
  *(undefined8 *)(pauVar3[5] + 8) = uVar1;
  *(undefined8 *)pauVar3[6] = uVar1;
                    /* try { // try from 0030c232 to 0030c236 has its CatchHandler @ 0030c291 */
  KisPaintInformation::KisPaintInformation((KisPaintInformation *)(pauVar3 + 9),local_38,dVar2);
  pauVar3[9][8] = 0;
  *(undefined8 *)pauVar3[10] = 0;
  pauVar3[10][8] = 0;
  *(undefined8 *)pauVar3[0xb] = 0;
  *(undefined8 *)(pauVar3[0xb] + 8) = 0;
  *(undefined8 *)pauVar3[0xc] = 0;
  *(undefined (**) [16])this = pauVar3;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisDistanceInformation @ 0030c2a0 ======

/* KisDistanceInformation::KisDistanceInformation(double, double, int) */

void __thiscall
KisDistanceInformation::KisDistanceInformation
          (KisDistanceInformation *this,double param_1,double param_2,int param_3)

{
  undefined8 uVar1;
  double dVar2;
  undefined (*pauVar3) [16];
  long in_FS_OFFSET;
  QPointF local_48 [24];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  pauVar3 = (undefined (*) [16])operator_new(200);
  uVar1 = DAT_007227a0;
  pauVar3[1][8] = 1;
  *(undefined8 *)pauVar3[4] = uVar1;
  *(undefined8 *)(pauVar3[4] + 8) = 0;
  uVar1 = DAT_00722788;
  *(undefined8 *)pauVar3[1] = 0;
  *(undefined8 *)pauVar3[3] = 0;
  pauVar3[3][8] = 0;
  pauVar3[5][0] = 0;
  pauVar3[8][8] = 0;
  *pauVar3 = (undefined  [16])0x0;
  pauVar3[2] = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar3[6] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar3[7] + 8) = (undefined  [16])0x0;
  dVar2 = DAT_007227c0;
  local_48[0] = (QPointF)0x0;
  local_48[1] = (QPointF)0x0;
  local_48[2] = (QPointF)0x0;
  local_48[3] = (QPointF)0x0;
  local_48[4] = (QPointF)0x0;
  local_48[5] = (QPointF)0x0;
  local_48[6] = (QPointF)0x0;
  local_48[7] = (QPointF)0x0;
  local_48[8] = (QPointF)0x0;
  local_48[9] = (QPointF)0x0;
  local_48[10] = (QPointF)0x0;
  local_48[0xb] = (QPointF)0x0;
  local_48[0xc] = (QPointF)0x0;
  local_48[0xd] = (QPointF)0x0;
  local_48[0xe] = (QPointF)0x0;
  local_48[0xf] = (QPointF)0x0;
  *(undefined8 *)(pauVar3[5] + 8) = uVar1;
  *(undefined8 *)pauVar3[6] = uVar1;
                    /* try { // try from 0030c344 to 0030c348 has its CatchHandler @ 0030c3b6 */
  KisPaintInformation::KisPaintInformation((KisPaintInformation *)(pauVar3 + 9),local_48,dVar2);
  pauVar3[9][8] = 0;
  *(undefined8 *)pauVar3[10] = 0;
  pauVar3[10][8] = 0;
  *(undefined8 *)pauVar3[0xb] = 0;
  *(undefined4 *)(pauVar3[0xb] + 0xc) = 0;
  *(undefined8 *)pauVar3[0xc] = 0;
  *(undefined (**) [16])this = pauVar3;
  *(double *)pauVar3[4] = param_1;
  *(double *)pauVar3[6] = param_2;
  *(int *)(pauVar3[0xb] + 8) = param_3;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisDistanceInformation @ 0030c3d0 ======

/* KisDistanceInformation::KisDistanceInformation(QPointF const&, double) */

void __thiscall
KisDistanceInformation::KisDistanceInformation
          (KisDistanceInformation *this,QPointF *param_1,double param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  double dVar3;
  undefined (*pauVar4) [16];
  long in_FS_OFFSET;
  QPointF local_48 [24];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  pauVar4 = (undefined (*) [16])operator_new(200);
  uVar1 = DAT_007227a0;
  pauVar4[1][8] = 1;
  *(undefined8 *)pauVar4[4] = uVar1;
  *(undefined8 *)(pauVar4[4] + 8) = 0;
  uVar1 = DAT_00722788;
  *(undefined8 *)pauVar4[1] = 0;
  *(undefined8 *)pauVar4[3] = 0;
  pauVar4[3][8] = 0;
  pauVar4[5][0] = 0;
  pauVar4[8][8] = 0;
  *pauVar4 = (undefined  [16])0x0;
  pauVar4[2] = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar4[6] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar4[7] + 8) = (undefined  [16])0x0;
  dVar3 = DAT_007227c0;
  local_48[0] = (QPointF)0x0;
  local_48[1] = (QPointF)0x0;
  local_48[2] = (QPointF)0x0;
  local_48[3] = (QPointF)0x0;
  local_48[4] = (QPointF)0x0;
  local_48[5] = (QPointF)0x0;
  local_48[6] = (QPointF)0x0;
  local_48[7] = (QPointF)0x0;
  local_48[8] = (QPointF)0x0;
  local_48[9] = (QPointF)0x0;
  local_48[10] = (QPointF)0x0;
  local_48[0xb] = (QPointF)0x0;
  local_48[0xc] = (QPointF)0x0;
  local_48[0xd] = (QPointF)0x0;
  local_48[0xe] = (QPointF)0x0;
  local_48[0xf] = (QPointF)0x0;
  *(undefined8 *)(pauVar4[5] + 8) = uVar1;
  *(undefined8 *)pauVar4[6] = uVar1;
                    /* try { // try from 0030c46e to 0030c472 has its CatchHandler @ 0030c4e8 */
  KisPaintInformation::KisPaintInformation((KisPaintInformation *)(pauVar4 + 9),local_48,dVar3);
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  pauVar4[9][8] = 0;
  *(undefined8 *)pauVar4[10] = 0;
  pauVar4[10][8] = 0;
  *(undefined8 *)pauVar4[0xb] = 0;
  *(undefined8 *)(pauVar4[0xb] + 8) = 0;
  *(undefined8 *)pauVar4[0xc] = 0;
  *(undefined (**) [16])this = pauVar4;
  *(double *)pauVar4[8] = param_2;
  pauVar4[8][8] = 1;
  *(undefined8 *)pauVar4[7] = uVar1;
  *(undefined8 *)(pauVar4[7] + 8) = uVar2;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisDistanceInformation @ 0030c500 ======

/* KisDistanceInformation::KisDistanceInformation(QPointF const&, double, double, double, int) */

void __thiscall
KisDistanceInformation::KisDistanceInformation
          (KisDistanceInformation *this,QPointF *param_1,double param_2,double param_3,
          double param_4,int param_5)

{
  long lVar1;
  
  KisDistanceInformation(this,param_1,param_2);
  lVar1 = *(long *)this;
  *(int *)(lVar1 + 0xb8) = param_5;
  *(double *)(lVar1 + 0x40) = param_3;
  *(double *)(lVar1 + 0x60) = param_4;
  return;
}



// ====== KisDistanceInformation @ 0030c5a0 ======

/* KisDistanceInformation::KisDistanceInformation(KisDistanceInformation const&) */

void __thiscall
KisDistanceInformation::KisDistanceInformation
          (KisDistanceInformation *this,KisDistanceInformation *param_1)

{
  undefined8 uVar1;
  undefined uVar2;
  undefined8 *puVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  undefined8 uVar13;
  undefined8 uVar14;
  undefined8 uVar15;
  undefined8 uVar16;
  undefined8 *puVar17;
  
  puVar17 = (undefined8 *)operator_new(200);
  puVar3 = *(undefined8 **)param_1;
  uVar1 = *puVar3;
  uVar4 = puVar3[1];
  uVar5 = puVar3[3];
  uVar6 = puVar3[4];
  uVar7 = puVar3[5];
  uVar8 = puVar3[6];
  puVar17[2] = puVar3[2];
  uVar9 = puVar3[8];
  uVar10 = puVar3[9];
  uVar11 = puVar3[10];
  uVar12 = puVar3[0xb];
  *puVar17 = uVar1;
  puVar17[1] = uVar4;
  uVar13 = puVar3[0xc];
  uVar14 = puVar3[0xd];
  uVar15 = puVar3[0xe];
  uVar16 = puVar3[0xf];
  puVar17[3] = uVar5;
  puVar17[4] = uVar6;
  uVar1 = puVar3[0x10];
  puVar17[5] = uVar7;
  puVar17[6] = uVar8;
  uVar4 = puVar3[7];
  puVar17[8] = uVar9;
  puVar17[9] = uVar10;
  puVar17[7] = uVar4;
  uVar2 = *(undefined *)(puVar3 + 0x11);
  puVar17[10] = uVar11;
  puVar17[0xb] = uVar12;
  *(undefined *)(puVar17 + 0x11) = uVar2;
  puVar17[0xc] = uVar13;
  puVar17[0xd] = uVar14;
  puVar17[0xe] = uVar15;
  puVar17[0xf] = uVar16;
  puVar17[0x10] = uVar1;
                    /* try { // try from 0030c63a to 0030c63e has its CatchHandler @ 0030c692 */
  KisPaintInformation::KisPaintInformation
            ((KisPaintInformation *)(puVar17 + 0x12),(KisPaintInformation *)(puVar3 + 0x12));
  uVar1 = puVar3[0x14];
  uVar5 = puVar3[0x15];
  uVar6 = puVar3[0x16];
  *(undefined *)(puVar17 + 0x13) = *(undefined *)(puVar3 + 0x13);
  uVar4 = puVar3[0x17];
  puVar17[0x14] = uVar1;
  uVar1 = puVar3[0x18];
  puVar17[0x17] = uVar4;
  *(undefined8 **)this = puVar17;
  puVar17[0x15] = uVar5;
  puVar17[0x16] = uVar6;
  puVar17[0x18] = uVar1;
  return;
}



// ====== KisDistanceInformation @ 0030c6a0 ======

/* KisDistanceInformation::KisDistanceInformation(KisDistanceInformation const&, int) */

void __thiscall
KisDistanceInformation::KisDistanceInformation
          (KisDistanceInformation *this,KisDistanceInformation *param_1,int param_2)

{
  undefined uVar1;
  char cVar2;
  undefined8 *puVar3;
  undefined8 uVar4;
  long lVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  undefined8 uVar13;
  undefined8 uVar14;
  undefined8 uVar15;
  undefined8 uVar16;
  undefined8 uVar17;
  undefined8 *puVar18;
  long in_FS_OFFSET;
  undefined8 uVar19;
  double dVar20;
  QTransform local_98 [88];
  int local_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  puVar18 = (undefined8 *)operator_new(200);
  puVar3 = *(undefined8 **)param_1;
  uVar4 = puVar3[1];
  uVar19 = puVar3[2];
  uVar6 = puVar3[3];
  uVar7 = puVar3[4];
  uVar8 = puVar3[5];
  uVar9 = puVar3[6];
  *puVar18 = *puVar3;
  puVar18[1] = uVar4;
  uVar10 = puVar3[8];
  uVar11 = puVar3[9];
  uVar12 = puVar3[10];
  uVar13 = puVar3[0xb];
  puVar18[2] = uVar19;
  uVar14 = puVar3[0xc];
  uVar15 = puVar3[0xd];
  uVar16 = puVar3[0xe];
  uVar17 = puVar3[0xf];
  puVar18[3] = uVar6;
  puVar18[4] = uVar7;
  uVar19 = puVar3[0x10];
  puVar18[5] = uVar8;
  puVar18[6] = uVar9;
  uVar4 = puVar3[7];
  puVar18[8] = uVar10;
  puVar18[9] = uVar11;
  puVar18[7] = uVar4;
  uVar1 = *(undefined *)(puVar3 + 0x11);
  puVar18[10] = uVar12;
  puVar18[0xb] = uVar13;
  *(undefined *)(puVar18 + 0x11) = uVar1;
  puVar18[0xc] = uVar14;
  puVar18[0xd] = uVar15;
  puVar18[0xe] = uVar16;
  puVar18[0xf] = uVar17;
  puVar18[0x10] = uVar19;
                    /* try { // try from 0030c759 to 0030c75d has its CatchHandler @ 0030c8c6 */
  KisPaintInformation::KisPaintInformation
            ((KisPaintInformation *)(puVar18 + 0x12),(KisPaintInformation *)(puVar3 + 0x12));
  cVar2 = *(char *)(puVar3 + 0x13);
  uVar6 = puVar3[0x15];
  uVar7 = puVar3[0x16];
  uVar4 = puVar3[0x17];
  puVar18[0x14] = puVar3[0x14];
  uVar19 = puVar3[0x18];
  *(char *)(puVar18 + 0x13) = cVar2;
  puVar18[0x17] = uVar4;
  *(undefined8 **)this = puVar18;
  puVar18[0x15] = uVar6;
  puVar18[0x16] = uVar7;
  puVar18[0x18] = uVar19;
  if (cVar2 != '\0') {
    kis_assert_recoverable
              ("!m_d->lastPaintInfoValid && \"The distance information \" \"should be cloned before the \" \"actual painting is started\""
               ,"/builds/graphics/krita/libs/image/kis_distance_information.cpp",0x117);
    puVar18 = *(undefined8 **)this;
  }
  *(int *)((long)puVar18 + 0xbc) = param_2;
  QTransform::QTransform(local_98);
  dVar20 = DAT_007227c0;
  if (0 < param_2) {
    dVar20 = DAT_007227c0 / (double)(1 << ((byte)param_2 & 0x1f));
  }
  QTransform::fromScale(dVar20,dVar20);
  lVar5 = *(long *)this;
  local_40 = param_2;
  uVar19 = QTransform::map((QPointF *)local_98);
  *(undefined8 *)(lVar5 + 0x70) = uVar19;
  *(double *)(lVar5 + 0x78) = dVar20;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



