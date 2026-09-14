/* Class KisCurveCircleMaskGenerator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCurveCircleMaskGenerator @ 00202ea0 ======

void __thiscall
KisCurveCircleMaskGenerator::KisCurveCircleMaskGenerator
          (KisCurveCircleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,KisCubicCurve *param_6,bool param_7)

{
  (*(code *)PTR_KisCurveCircleMaskGenerator_00839220)();
  return;
}



// ====== KisCurveCircleMaskGenerator @ 00208ab0 ======

void __thiscall
KisCurveCircleMaskGenerator::KisCurveCircleMaskGenerator
          (KisCurveCircleMaskGenerator *this,KisCurveCircleMaskGenerator *param_1)

{
  (*(code *)PTR_KisCurveCircleMaskGenerator_0083c028)();
  return;
}



// ====== KisCurveCircleMaskGenerator @ 005ab2e0 ======

/* KisCurveCircleMaskGenerator::KisCurveCircleMaskGenerator(KisCurveCircleMaskGenerator const&) */

void __thiscall
KisCurveCircleMaskGenerator::KisCurveCircleMaskGenerator
          (KisCurveCircleMaskGenerator *this,KisCurveCircleMaskGenerator *param_1)

{
  undefined8 uVar1;
  undefined uVar2;
  undefined8 *puVar3;
  long *plVar4;
  long lVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 *puVar8;
  int *piVar9;
  long *plVar10;
  long lVar11;
  long in_FS_OFFSET;
  KisCurveCircleMaskGenerator *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator((KisMaskGenerator *)this,(KisMaskGenerator *)param_1);
  *(undefined **)this = PTR_vtable_008375d8 + 0x10;
                    /* try { // try from 005ab31b to 005ab31f has its CatchHandler @ 005ab4ae */
  puVar8 = (undefined8 *)operator_new(0x68);
  puVar3 = *(undefined8 **)(param_1 + 0x18);
  uVar6 = puVar3[1];
  uVar1 = puVar3[2];
  *puVar8 = *puVar3;
  puVar8[1] = uVar6;
  puVar8[2] = uVar1;
  piVar9 = (int *)puVar3[3];
  if (*piVar9 == 0) {
    if (*(char *)((long)piVar9 + 0xb) < '\0') {
      lVar11 = QArrayData::allocate(8,8,(ulong)(piVar9[2] & 0x7fffffff),0);
      puVar8[3] = lVar11;
      if (lVar11 == 0) {
        qBadAlloc();
        lVar11 = puVar8[3];
      }
      *(byte *)(lVar11 + 0xb) = *(byte *)(lVar11 + 0xb) | 0x80;
    }
    else {
      lVar11 = QArrayData::allocate(8,8,(long)piVar9[1],0);
      puVar8[3] = lVar11;
      if (lVar11 == 0) {
        qBadAlloc();
        lVar11 = puVar8[3];
      }
    }
    if ((*(uint *)(lVar11 + 8) & 0x7fffffff) != 0) {
      lVar5 = puVar3[3];
      memcpy((void *)(lVar11 + *(long *)(lVar11 + 0x10)),(void *)(lVar5 + *(long *)(lVar5 + 0x10)),
             (long)*(int *)(lVar5 + 4) << 3);
      *(undefined4 *)(puVar8[3] + 4) = *(undefined4 *)(puVar3[3] + 4);
    }
  }
  else {
    if (*piVar9 != -1) {
      LOCK();
      *piVar9 = *piVar9 + 1;
      UNLOCK();
      piVar9 = (int *)puVar3[3];
    }
    puVar8[3] = piVar9;
  }
                    /* try { // try from 005ab35c to 005ab360 has its CatchHandler @ 005ab4c6 */
  FUN_00497250(puVar8 + 4,puVar3 + 4);
  *(undefined *)(puVar8 + 5) = 1;
  uVar2 = *(undefined *)(puVar3 + 7);
  uVar1 = puVar3[6];
  uVar6 = puVar3[8];
  uVar7 = puVar3[9];
  puVar8[0xb] = puVar8;
  *(undefined *)(puVar8 + 7) = uVar2;
  uVar2 = *(undefined *)(puVar3 + 10);
  puVar8[0xc] = 0;
  *(undefined *)(puVar8 + 10) = uVar2;
  *(undefined8 **)(this + 0x18) = puVar8;
  puVar8[6] = uVar1;
  puVar8[8] = uVar6;
  puVar8[9] = uVar7;
  local_28 = this;
                    /* try { // try from 005ab39e to 005ab3a2 has its CatchHandler @ 005ab4ba */
  plVar10 = (long *)FUN_005abff0(&local_28);
  plVar4 = (long *)puVar8[0xc];
  if ((plVar10 == plVar4) || (puVar8[0xc] = plVar10, plVar4 == (long *)0x0)) {
    if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x005ab3e0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar4 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCurveCircleMaskGenerator @ 005aba80 ======

/* KisCurveCircleMaskGenerator::KisCurveCircleMaskGenerator(double, double, double, double, int,
   KisCubicCurve const&, bool) */

void __thiscall
KisCurveCircleMaskGenerator::KisCurveCircleMaskGenerator
          (KisCurveCircleMaskGenerator *this,double param_1,double param_2,double param_3,
          double param_4,int param_5,KisCubicCurve *param_6,bool param_7)

{
  long lVar1;
  QArrayData *pQVar2;
  code *pcVar3;
  undefined *puVar4;
  undefined *puVar5;
  int iVar6;
  undefined (*pauVar7) [16];
  long *plVar8;
  long *plVar9;
  QArrayData *pQVar10;
  long in_FS_OFFSET;
  double dVar11;
  double dVar12;
  QArrayData *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisMaskGenerator::KisMaskGenerator
            ((KisMaskGenerator *)this,param_1,param_2,param_3,param_4,param_5,param_7,0,
             (KoID *)&DAT_008530d0);
  *(undefined **)this = PTR_vtable_008375d8 + 0x10;
                    /* try { // try from 005abacd to 005abad1 has its CatchHandler @ 005abe46 */
  pauVar7 = (undefined (*) [16])operator_new(0x68);
  puVar4 = PTR_shared_null_008377d0;
  pauVar7[2][8] = 0;
  *(undefined8 *)pauVar7[1] = 0;
  *(undefined8 *)pauVar7[3] = 0;
  puVar5 = PTR_shared_null_00837830;
  pauVar7[3][8] = 0;
  pauVar7[5][0] = param_7;
  *(undefined (**) [16])(pauVar7[5] + 8) = pauVar7;
  *(undefined8 *)pauVar7[6] = 0;
  *(undefined (**) [16])(this + 0x18) = pauVar7;
  *pauVar7 = (undefined  [16])0x0;
  *(undefined **)(pauVar7[1] + 8) = puVar4;
  *(undefined **)pauVar7[2] = puVar5;
  pauVar7[4] = (undefined  [16])0x0;
                    /* try { // try from 005abb1f to 005abc3c has its CatchHandler @ 005abe5e */
  dVar11 = (double)KisMaskGenerator::height((KisMaskGenerator *)this);
  dVar12 = (double)KisMaskGenerator::width((KisMaskGenerator *)this);
  if (dVar11 <= dVar12) {
    dVar11 = dVar12;
  }
  dVar11 = DAT_0072d3c8 * dVar11;
  if (dVar11 < 0.0) {
    iVar6 = (int)((dVar11 - (double)(int)(dVar11 - DAT_007227c0)) + DAT_007227c8) +
            (int)(dVar11 - DAT_007227c0);
  }
  else {
    iVar6 = (int)(dVar11 + DAT_007227c8);
  }
  *(double *)(*(long *)(this + 0x18) + 0x10) = (double)iVar6;
  KisCubicCurve::floatTransfer((int)&local_38);
  lVar1 = *(long *)(this + 0x18);
  pQVar10 = *(QArrayData **)(lVar1 + 0x18);
  if (local_38 != *(QArrayData **)(lVar1 + 0x18)) {
    if (*(int *)local_38 == 0) {
      if ((char)local_38[0xb] < '\0') {
        pQVar10 = (QArrayData *)
                  QArrayData::allocate(8,8,(ulong)(*(uint *)(local_38 + 8) & 0x7fffffff),0);
        if (pQVar10 == (QArrayData *)0x0) {
          qBadAlloc();
        }
        pQVar10[0xb] = (QArrayData)((byte)pQVar10[0xb] | 0x80);
      }
      else {
        pQVar10 = (QArrayData *)QArrayData::allocate(8,8,(long)*(int *)(local_38 + 4),0);
        if (pQVar10 == (QArrayData *)0x0) {
          qBadAlloc();
                    /* WARNING: Does not return */
          pcVar3 = (code *)invalidInstructionException();
          (*pcVar3)();
        }
      }
      if ((*(uint *)(pQVar10 + 8) & 0x7fffffff) != 0) {
        memcpy(pQVar10 + *(long *)(pQVar10 + 0x10),local_38 + *(long *)(local_38 + 0x10),
               (long)*(int *)(local_38 + 4) << 3);
        *(int *)(pQVar10 + 4) = *(int *)(local_38 + 4);
      }
    }
    else {
      pQVar10 = local_38;
      if (*(int *)local_38 != -1) {
        LOCK();
        *(int *)local_38 = *(int *)local_38 + 1;
        UNLOCK();
      }
    }
    pQVar2 = *(QArrayData **)(lVar1 + 0x18);
    *(QArrayData **)(lVar1 + 0x18) = pQVar10;
    if (*(int *)pQVar2 == 0) {
LAB_005abd80:
      QArrayData::deallocate(pQVar2,8,8);
      pQVar10 = local_38;
    }
    else {
      pQVar10 = local_38;
      if (*(int *)pQVar2 != -1) {
        LOCK();
        *(int *)pQVar2 = *(int *)pQVar2 + -1;
        UNLOCK();
        if (*(int *)pQVar2 == 0) goto LAB_005abd80;
      }
    }
  }
  if (*(int *)pQVar10 == 0) {
LAB_005abd50:
    QArrayData::deallocate(local_38,8,8);
  }
  else if (*(int *)pQVar10 != -1) {
    LOCK();
    *(int *)pQVar10 = *(int *)pQVar10 + -1;
    UNLOCK();
    if (*(int *)pQVar10 == 0) goto LAB_005abd50;
  }
  plVar8 = (long *)KisCubicCurve::curvePoints(param_6);
  lVar1 = *(long *)(this + 0x18);
  if (*(long *)(lVar1 + 0x20) != *plVar8) {
    FUN_00497250(&local_38,plVar8);
    pQVar10 = *(QArrayData **)(lVar1 + 0x20);
    *(QArrayData **)(lVar1 + 0x20) = local_38;
    local_38 = pQVar10;
    FUN_004970d0(&local_38);
  }
  KisCubicCurve::toString();
                    /* try { // try from 005abc43 to 005abc47 has its CatchHandler @ 005abe52 */
  KisMaskGenerator::setCurveString((KisMaskGenerator *)this,(QString *)&local_38);
  if (*(int *)local_38 != 0) {
    if (*(int *)local_38 == -1) goto LAB_005abc69;
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 != 0) goto LAB_005abc69;
  }
  QArrayData::deallocate(local_38,2,8);
LAB_005abc69:
  dVar11 = DAT_007227c0;
  *(undefined *)(*(long *)(this + 0x18) + 0x28) = 0;
                    /* try { // try from 005abc80 to 005abc94 has its CatchHandler @ 005abe5e */
  setScale(this,dVar11,dVar11);
  lVar1 = *(long *)(this + 0x18);
  local_38 = (QArrayData *)this;
  plVar9 = (long *)FUN_005abff0(&local_38);
  plVar8 = *(long **)(lVar1 + 0x60);
  if ((plVar9 == plVar8) || (*(long **)(lVar1 + 0x60) = plVar9, plVar8 == (long *)0x0)) {
    if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x005abcd6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar8 + 8))();
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



