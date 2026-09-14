/* Class KisCurveRectangleMaskGenerator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCurveRectangleMaskGenerator @ 0020c8f0 ======

void __thiscall
KisCurveRectangleMaskGenerator::KisCurveRectangleMaskGenerator
          (KisCurveRectangleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,KisCubicCurve *param_6,bool param_7)

{
  (*(code *)PTR_KisCurveRectangleMaskGenerator_0083df48)();
  return;
}



// ====== KisCurveRectangleMaskGenerator @ 0020d150 ======

void __thiscall
KisCurveRectangleMaskGenerator::KisCurveRectangleMaskGenerator
          (KisCurveRectangleMaskGenerator *this,KisCurveRectangleMaskGenerator *param_1)

{
  (*(code *)PTR_KisCurveRectangleMaskGenerator_0083e378)();
  return;
}



// ====== KisCurveRectangleMaskGenerator @ 005adfb0 ======

/* KisCurveRectangleMaskGenerator::KisCurveRectangleMaskGenerator(KisCurveRectangleMaskGenerator
   const&) */

void __thiscall
KisCurveRectangleMaskGenerator::KisCurveRectangleMaskGenerator
          (KisCurveRectangleMaskGenerator *this,KisCurveRectangleMaskGenerator *param_1)

{
  undefined8 uVar1;
  undefined uVar2;
  undefined8 *puVar3;
  long *plVar4;
  long lVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 *puVar11;
  int *piVar12;
  long *plVar13;
  long lVar14;
  long in_FS_OFFSET;
  KisCurveRectangleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator((KisMaskGenerator *)this,(KisMaskGenerator *)param_1);
  *(undefined **)this = PTR_vtable_00837f20 + 0x10;
                    /* try { // try from 005adfeb to 005adfef has its CatchHandler @ 005ae186 */
  puVar11 = (undefined8 *)operator_new(0x78);
  puVar3 = *(undefined8 **)(param_1 + 0x18);
  uVar6 = puVar3[1];
  uVar1 = puVar3[2];
  *puVar11 = *puVar3;
  puVar11[1] = uVar6;
  puVar11[2] = uVar1;
  piVar12 = (int *)puVar3[3];
  if (*piVar12 == 0) {
    if (*(char *)((long)piVar12 + 0xb) < '\0') {
      lVar14 = QArrayData::allocate(8,8,(ulong)(piVar12[2] & 0x7fffffff),0);
      puVar11[3] = lVar14;
      if (lVar14 == 0) {
        qBadAlloc();
        lVar14 = puVar11[3];
      }
      *(byte *)(lVar14 + 0xb) = *(byte *)(lVar14 + 0xb) | 0x80;
    }
    else {
      lVar14 = QArrayData::allocate(8,8,(long)piVar12[1],0);
      puVar11[3] = lVar14;
      if (lVar14 == 0) {
        qBadAlloc();
        lVar14 = puVar11[3];
      }
    }
    if ((*(uint *)(lVar14 + 8) & 0x7fffffff) != 0) {
      lVar5 = puVar3[3];
      memcpy((void *)(lVar14 + *(long *)(lVar14 + 0x10)),(void *)(lVar5 + *(long *)(lVar5 + 0x10)),
             (long)*(int *)(lVar5 + 4) << 3);
      *(undefined4 *)(puVar11[3] + 4) = *(undefined4 *)(puVar3[3] + 4);
    }
  }
  else {
    if (*piVar12 != -1) {
      LOCK();
      *piVar12 = *piVar12 + 1;
      UNLOCK();
      piVar12 = (int *)puVar3[3];
    }
    puVar11[3] = piVar12;
  }
                    /* try { // try from 005ae02c to 005ae030 has its CatchHandler @ 005ae19e */
  FUN_00497250(puVar11 + 4,puVar3 + 4);
  uVar2 = *(undefined *)(puVar3 + 5);
  uVar1 = puVar3[6];
  uVar6 = puVar3[7];
  puVar11[0xd] = puVar11;
  uVar7 = puVar3[8];
  uVar8 = puVar3[9];
  uVar9 = puVar3[10];
  uVar10 = puVar3[0xb];
  *(undefined *)(puVar11 + 5) = uVar2;
  uVar2 = *(undefined *)(puVar3 + 0xc);
  puVar11[0xe] = 0;
  *(undefined *)(puVar11 + 0xc) = uVar2;
  *(undefined8 **)(this + 0x18) = puVar11;
  puVar11[6] = uVar1;
  puVar11[7] = uVar6;
  puVar11[8] = uVar7;
  puVar11[9] = uVar8;
  puVar11[10] = uVar9;
  puVar11[0xb] = uVar10;
  local_28 = this;
                    /* try { // try from 005ae072 to 005ae076 has its CatchHandler @ 005ae192 */
  plVar13 = (long *)FUN_005ae810(&local_28);
  plVar4 = (long *)puVar11[0xe];
  if ((plVar13 == plVar4) || (puVar11[0xe] = plVar13, plVar4 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x005ae0b4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar4 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCurveRectangleMaskGenerator @ 005ae280 ======

/* KisCurveRectangleMaskGenerator::KisCurveRectangleMaskGenerator(double, double, double, double,
   int, KisCubicCurve const&, bool) */

void __thiscall
KisCurveRectangleMaskGenerator::KisCurveRectangleMaskGenerator
          (KisCurveRectangleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,KisCubicCurve *param_6,bool param_7)

{
  long lVar1;
  QArrayData *pQVar2;
  code *pcVar3;
  double dVar4;
  undefined *puVar5;
  undefined *puVar6;
  int iVar7;
  undefined (*pauVar8) [16];
  long *plVar9;
  long *plVar10;
  QArrayData *pQVar11;
  long in_FS_OFFSET;
  double dVar12;
  double dVar13;
  QArrayData *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator
            ((KisMaskGenerator *)this,param_1,param_2,param_3,param_4,param_5,param_7,1,
             (KoID *)&DAT_00853110);
  *(undefined **)this = PTR_vtable_00837f20 + 0x10;
                    /* try { // try from 005ae2d0 to 005ae2d4 has its CatchHandler @ 005ae666 */
  pauVar8 = (undefined (*) [16])operator_new(0x78);
  puVar5 = PTR_shared_null_008377d0;
  pauVar8[2][8] = 0;
  *(undefined8 *)pauVar8[1] = 0;
  pauVar8[6][0] = param_7;
  puVar6 = PTR_shared_null_00837830;
  *(undefined (**) [16])(pauVar8[6] + 8) = pauVar8;
  *(undefined8 *)pauVar8[7] = 0;
  *(undefined (**) [16])(this + 0x18) = pauVar8;
  *pauVar8 = (undefined  [16])0x0;
  *(undefined **)(pauVar8[1] + 8) = puVar5;
  *(undefined **)pauVar8[2] = puVar6;
  pauVar8[3] = (undefined  [16])0x0;
  pauVar8[4] = (undefined  [16])0x0;
  pauVar8[5] = (undefined  [16])0x0;
                    /* try { // try from 005ae31e to 005ae44e has its CatchHandler @ 005ae67e */
  dVar12 = (double)KisMaskGenerator::height((KisMaskGenerator *)this);
  dVar13 = (double)KisMaskGenerator::width((KisMaskGenerator *)this);
  dVar4 = DAT_007227c0;
  if (dVar12 <= dVar13) {
    dVar12 = dVar13;
  }
  dVar12 = DAT_0072d3c8 * dVar12;
  if (dVar12 < 0.0) {
    iVar7 = (int)((dVar12 - (double)(int)(dVar12 - DAT_007227c0)) + DAT_007227c8) +
            (int)(dVar12 - DAT_007227c0);
  }
  else {
    iVar7 = (int)(dVar12 + DAT_007227c8);
  }
  *(double *)(*(long *)(this + 0x18) + 0x10) = (double)iVar7;
  KisCubicCurve::floatTransfer((int)&local_38);
  lVar1 = *(long *)(this + 0x18);
  pQVar11 = *(QArrayData **)(lVar1 + 0x18);
  if (local_38 != *(QArrayData **)(lVar1 + 0x18)) {
    if (*(int *)local_38 == 0) {
      if ((char)local_38[0xb] < '\0') {
        pQVar11 = (QArrayData *)
                  QArrayData::allocate(8,8,(ulong)(*(uint *)(local_38 + 8) & 0x7fffffff),0);
        if (pQVar11 == (QArrayData *)0x0) {
          qBadAlloc();
        }
        pQVar11[0xb] = (QArrayData)((byte)pQVar11[0xb] | 0x80);
      }
      else {
        pQVar11 = (QArrayData *)QArrayData::allocate(8,8,(long)*(int *)(local_38 + 4),0);
        if (pQVar11 == (QArrayData *)0x0) {
          qBadAlloc();
                    /* WARNING: Does not return */
          pcVar3 = (code *)invalidInstructionException();
          (*pcVar3)();
        }
      }
      if ((*(uint *)(pQVar11 + 8) & 0x7fffffff) != 0) {
        memcpy(pQVar11 + *(long *)(pQVar11 + 0x10),local_38 + *(long *)(local_38 + 0x10),
               (long)*(int *)(local_38 + 4) << 3);
        *(int *)(pQVar11 + 4) = *(int *)(local_38 + 4);
      }
    }
    else {
      pQVar11 = local_38;
      if (*(int *)local_38 != -1) {
        LOCK();
        *(int *)local_38 = *(int *)local_38 + 1;
        UNLOCK();
      }
    }
    pQVar2 = *(QArrayData **)(lVar1 + 0x18);
    *(QArrayData **)(lVar1 + 0x18) = pQVar11;
    if (*(int *)pQVar2 == 0) {
LAB_005ae590:
      QArrayData::deallocate(pQVar2,8,8);
      pQVar11 = local_38;
    }
    else {
      pQVar11 = local_38;
      if (*(int *)pQVar2 != -1) {
        LOCK();
        *(int *)pQVar2 = *(int *)pQVar2 + -1;
        UNLOCK();
        if (*(int *)pQVar2 == 0) goto LAB_005ae590;
      }
    }
  }
  if (*(int *)pQVar11 == 0) {
LAB_005ae560:
    QArrayData::deallocate(local_38,8,8);
  }
  else if (*(int *)pQVar11 != -1) {
    LOCK();
    *(int *)pQVar11 = *(int *)pQVar11 + -1;
    UNLOCK();
    if (*(int *)pQVar11 == 0) goto LAB_005ae560;
  }
  plVar9 = (long *)KisCubicCurve::curvePoints(param_6);
  lVar1 = *(long *)(this + 0x18);
  if (*(long *)(lVar1 + 0x20) != *plVar9) {
    FUN_00497250(&local_38,plVar9);
    pQVar11 = *(QArrayData **)(lVar1 + 0x20);
    *(QArrayData **)(lVar1 + 0x20) = local_38;
    local_38 = pQVar11;
    FUN_004970d0(&local_38);
  }
  KisCubicCurve::toString();
                    /* try { // try from 005ae455 to 005ae459 has its CatchHandler @ 005ae672 */
  KisMaskGenerator::setCurveString((KisMaskGenerator *)this,(QString *)&local_38);
  if (*(int *)local_38 != 0) {
    if (*(int *)local_38 == -1) goto LAB_005ae47d;
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 != 0) goto LAB_005ae47d;
  }
  QArrayData::deallocate(local_38,2,8);
LAB_005ae47d:
  *(undefined *)(*(long *)(this + 0x18) + 0x28) = 0;
                    /* try { // try from 005ae492 to 005ae4a7 has its CatchHandler @ 005ae67e */
  setScale(this,dVar4,dVar4);
  lVar1 = *(long *)(this + 0x18);
  local_38 = (QArrayData *)this;
  plVar10 = (long *)FUN_005ae810(&local_38);
  plVar9 = *(long **)(lVar1 + 0x70);
  if ((plVar10 == plVar9) || (*(long **)(lVar1 + 0x70) = plVar10, plVar9 == (long *)0x0)) {
    if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x005ae4e9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar9 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



