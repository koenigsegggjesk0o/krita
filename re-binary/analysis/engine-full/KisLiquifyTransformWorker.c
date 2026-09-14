/* Class KisLiquifyTransformWorker - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLiquifyTransformWorker @ 00203290 ======

void __thiscall
KisLiquifyTransformWorker::KisLiquifyTransformWorker
          (KisLiquifyTransformWorker *this,QRect *param_1,KoUpdater *param_2,int param_3)

{
  (*(code *)PTR_KisLiquifyTransformWorker_00839418)();
  return;
}



// ====== KisLiquifyTransformWorker @ 0061d360 ======

/* KisLiquifyTransformWorker::KisLiquifyTransformWorker(KisLiquifyTransformWorker const&) */

void __thiscall
KisLiquifyTransformWorker::KisLiquifyTransformWorker
          (KisLiquifyTransformWorker *this,KisLiquifyTransformWorker *param_1)

{
  undefined8 *puVar1;
  undefined4 uVar2;
  int iVar3;
  undefined8 *puVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 *puVar9;
  int *piVar10;
  long lVar11;
  undefined8 *puVar12;
  undefined8 *puVar13;
  long lVar14;
  
  puVar9 = (undefined8 *)operator_new(0x88);
  puVar4 = *(undefined8 **)param_1;
  uVar5 = puVar4[1];
  *puVar9 = *puVar4;
  puVar9[1] = uVar5;
                    /* try { // try from 0061d396 to 0061d39a has its CatchHandler @ 0061d4e8 */
  FUN_00622240(puVar9 + 2,puVar4 + 2);
  piVar10 = (int *)puVar4[3];
  if (*piVar10 == 0) {
    if (*(char *)((long)piVar10 + 0xb) < '\0') {
      lVar11 = QArrayData::allocate(0x10,8,(ulong)(piVar10[2] & 0x7fffffff),0);
      puVar9[3] = lVar11;
      if (lVar11 == 0) {
        qBadAlloc();
        lVar11 = puVar9[3];
      }
      *(byte *)(lVar11 + 0xb) = *(byte *)(lVar11 + 0xb) | 0x80;
    }
    else {
      lVar11 = QArrayData::allocate(0x10,8,(long)piVar10[1],0);
      puVar9[3] = lVar11;
      if (lVar11 == 0) {
        qBadAlloc();
        lVar11 = puVar9[3];
      }
    }
    if ((*(uint *)(lVar11 + 8) & 0x7fffffff) != 0) {
      lVar14 = puVar4[3];
      iVar3 = *(int *)(lVar14 + 4);
      puVar12 = (undefined8 *)(*(long *)(lVar14 + 0x10) + lVar14);
      puVar13 = puVar12 + (long)iVar3 * 2;
      lVar14 = (*(long *)(lVar11 + 0x10) + lVar11) - (long)puVar12;
      for (; puVar12 != puVar13; puVar12 = puVar12 + 2) {
        puVar1 = (undefined8 *)((long)puVar12 + lVar14);
        uVar5 = puVar12[1];
        *puVar1 = *puVar12;
        puVar1[1] = uVar5;
      }
      *(int *)(lVar11 + 4) = iVar3;
    }
  }
  else {
    if (*piVar10 != -1) {
      LOCK();
      *piVar10 = *piVar10 + 1;
      UNLOCK();
      piVar10 = (int *)puVar4[3];
    }
    puVar9[3] = piVar10;
  }
                    /* try { // try from 0061d3bd to 0061d3c1 has its CatchHandler @ 0061d4dc */
  KisSpatialContainer::KisSpatialContainer
            ((KisSpatialContainer *)(puVar9 + 4),(KisSpatialContainer *)(puVar4 + 4));
                    /* try { // try from 0061d3ca to 0061d3ce has its CatchHandler @ 0061d4d0 */
  KisSpatialContainer::KisSpatialContainer
            ((KisSpatialContainer *)(puVar9 + 7),(KisSpatialContainer *)(puVar4 + 7));
  uVar5 = puVar4[10];
  uVar6 = puVar4[0xb];
  uVar7 = puVar4[0xc];
  uVar8 = puVar4[0xd];
  puVar9[0xe] = puVar4[0xe];
  uVar2 = *(undefined4 *)(puVar4 + 0xf);
  puVar9[10] = uVar5;
  puVar9[0xb] = uVar6;
  *(undefined4 *)(puVar9 + 0xf) = uVar2;
  uVar5 = *(undefined8 *)((long)puVar4 + 0x7c);
  puVar9[0xc] = uVar7;
  puVar9[0xd] = uVar8;
  *(undefined8 *)((long)puVar9 + 0x7c) = uVar5;
  *(undefined8 **)this = puVar9;
  return;
}



// ====== KisLiquifyTransformWorker @ 0061eae0 ======

/* KisLiquifyTransformWorker::KisLiquifyTransformWorker(QRect const&, KoUpdater*, int) */

void __thiscall
KisLiquifyTransformWorker::KisLiquifyTransformWorker
          (KisLiquifyTransformWorker *this,QRect *param_1,KoUpdater *param_2,int param_3)

{
  int iVar1;
  undefined8 uVar2;
  undefined *puVar3;
  undefined8 *puVar4;
  
  puVar4 = (undefined8 *)operator_new(0x88);
  puVar3 = PTR_shared_null_008377d0;
  uVar2 = *(undefined8 *)(param_1 + 8);
  *puVar4 = *(undefined8 *)param_1;
  puVar4[1] = uVar2;
  puVar4[2] = puVar3;
  puVar4[3] = puVar3;
                    /* try { // try from 0061eb78 to 0061eb7c has its CatchHandler @ 0061ec43 */
  KisSpatialContainer::KisSpatialContainer(puVar4 + 4,100);
                    /* try { // try from 0061ebbc to 0061ebc0 has its CatchHandler @ 0061ec37 */
  KisSpatialContainer::KisSpatialContainer(puVar4 + 7,100);
  puVar4[0xe] = param_2;
  *(int *)(puVar4 + 0xf) = param_3;
  *(undefined8 *)((long)puVar4 + 0x7c) = 0xffffffffffffffff;
  iVar1 = *(int *)param_1;
  *(undefined8 **)this = puVar4;
  *(undefined (*) [16])(puVar4 + 10) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar4 + 0xc) = (undefined  [16])0x0;
  if ((iVar1 <= *(int *)(param_1 + 8)) && (*(int *)(param_1 + 4) <= *(int *)(param_1 + 0xc))) {
    FUN_0061e650(puVar4);
    return;
  }
                    /* try { // try from 0061ec0b to 0061ec27 has its CatchHandler @ 0061ec4f */
  kis_assert_recoverable
            ("!srcBounds.isEmpty()",
             "/builds/graphics/krita/libs/image/kis_liquify_transform_worker.cpp",0x47);
  return;
}



