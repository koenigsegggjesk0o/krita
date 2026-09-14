/* Class KisOptimizedBrushOutline - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisOptimizedBrushOutline @ 00202340 ======

void __thiscall KisOptimizedBrushOutline::KisOptimizedBrushOutline(KisOptimizedBrushOutline *this)

{
  (*(code *)PTR_KisOptimizedBrushOutline_00838c70)();
  return;
}



// ====== KisOptimizedBrushOutline @ 0020b110 ======

void __thiscall
KisOptimizedBrushOutline::KisOptimizedBrushOutline
          (KisOptimizedBrushOutline *this,QPainterPath *param_1,optional *param_2)

{
  (*(code *)PTR_KisOptimizedBrushOutline_0083d358)();
  return;
}



// ====== KisOptimizedBrushOutline @ 0020be80 ======

void __thiscall
KisOptimizedBrushOutline::KisOptimizedBrushOutline
          (KisOptimizedBrushOutline *this,QVector *param_1,optional *param_2)

{
  (*(code *)PTR_KisOptimizedBrushOutline_0083da10)();
  return;
}



// ====== KisOptimizedBrushOutline @ 00359040 ======

/* KisOptimizedBrushOutline::KisOptimizedBrushOutline() */

void __thiscall KisOptimizedBrushOutline::KisOptimizedBrushOutline(KisOptimizedBrushOutline *this)

{
  undefined *puVar1;
  
  this[0x30] = (KisOptimizedBrushOutline)0x0;
  puVar1 = PTR_shared_null_008377d0;
  *(undefined **)this = PTR_shared_null_008377d0;
  *(undefined **)(this + 8) = puVar1;
                    /* try { // try from 00359065 to 00359069 has its CatchHandler @ 00359083 */
  QTransform::QTransform((QTransform *)(this + 0x38));
  *(undefined (*) [16])(this + 0x90) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0xa0) = (undefined  [16])0x0;
  return;
}



// ====== KisOptimizedBrushOutline @ 00359090 ======

/* KisOptimizedBrushOutline::KisOptimizedBrushOutline(QVector<QPolygonF> const&,
   std::optional<QRectF> const&) */

void __thiscall
KisOptimizedBrushOutline::KisOptimizedBrushOutline
          (KisOptimizedBrushOutline *this,QVector *param_1,optional *param_2)

{
  undefined8 *puVar1;
  long *plVar2;
  long *plVar3;
  undefined8 uVar4;
  long lVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  long lVar10;
  int *piVar11;
  long lVar12;
  undefined8 *puVar13;
  int iVar14;
  long *plVar15;
  long *plVar16;
  long *plVar17;
  undefined8 *puVar18;
  long lVar19;
  
  piVar11 = *(int **)param_1;
  if (*piVar11 == 0) {
    if (*(char *)((long)piVar11 + 0xb) < '\0') {
      lVar10 = QArrayData::allocate(8,8,(ulong)(piVar11[2] & 0x7fffffff),0);
      *(long *)this = lVar10;
      if (lVar10 == 0) {
        qBadAlloc();
        lVar10 = *(long *)this;
      }
      *(byte *)(lVar10 + 0xb) = *(byte *)(lVar10 + 0xb) | 0x80;
    }
    else {
      lVar10 = QArrayData::allocate(8,8,(long)piVar11[1],0);
      *(long *)this = lVar10;
      if (lVar10 == 0) {
        qBadAlloc();
        lVar10 = *(long *)this;
      }
    }
    if ((*(uint *)(lVar10 + 8) & 0x7fffffff) != 0) {
      lVar12 = *(long *)param_1;
      lVar5 = *(long *)(lVar10 + 0x10);
      iVar14 = *(int *)(lVar12 + 4);
      plVar15 = (long *)(*(long *)(lVar12 + 0x10) + lVar12);
      plVar2 = plVar15 + iVar14;
      if (plVar15 != plVar2) {
        plVar16 = plVar15;
        do {
          while( true ) {
            plVar3 = (long *)((long)plVar16 + ((lVar5 + lVar10) - (long)plVar15));
            plVar17 = plVar16 + 1;
            piVar11 = (int *)*plVar16;
            if (*piVar11 == 0) break;
            if (*piVar11 != -1) {
              LOCK();
              *piVar11 = *piVar11 + 1;
              UNLOCK();
              piVar11 = (int *)*plVar16;
            }
            *plVar3 = (long)piVar11;
LAB_0035918f:
            plVar16 = plVar17;
            if (plVar2 == plVar17) goto LAB_00359244;
          }
          if (*(char *)((long)piVar11 + 0xb) < '\0') {
            lVar12 = QArrayData::allocate(0x10,8,(ulong)(piVar11[2] & 0x7fffffff),0);
            *plVar3 = lVar12;
            if (lVar12 == 0) {
              qBadAlloc();
              lVar12 = *plVar3;
            }
            *(byte *)(lVar12 + 0xb) = *(byte *)(lVar12 + 0xb) | 0x80;
          }
          else {
            lVar12 = QArrayData::allocate(0x10,8,(long)piVar11[1],0);
            *plVar3 = lVar12;
            if (lVar12 == 0) {
              qBadAlloc();
              lVar12 = *plVar3;
            }
          }
          if ((*(uint *)(lVar12 + 8) & 0x7fffffff) == 0) goto LAB_0035918f;
          lVar19 = *plVar16;
          iVar14 = *(int *)(lVar19 + 4);
          puVar13 = (undefined8 *)(*(long *)(lVar19 + 0x10) + lVar19);
          puVar18 = puVar13 + (long)iVar14 * 2;
          lVar19 = (*(long *)(lVar12 + 0x10) + lVar12) - (long)puVar13;
          for (; puVar13 != puVar18; puVar13 = puVar13 + 2) {
            puVar1 = (undefined8 *)((long)puVar13 + lVar19);
            uVar4 = puVar13[1];
            *puVar1 = *puVar13;
            puVar1[1] = uVar4;
          }
          *(int *)(lVar12 + 4) = iVar14;
          plVar16 = plVar17;
        } while (plVar2 != plVar17);
LAB_00359244:
        lVar10 = *(long *)this;
        iVar14 = *(int *)(*(long *)param_1 + 4);
      }
      *(int *)(lVar10 + 4) = iVar14;
    }
  }
  else {
    if (*piVar11 != -1) {
      LOCK();
      *piVar11 = *piVar11 + 1;
      UNLOCK();
      piVar11 = *(int **)param_1;
    }
    *(int **)this = piVar11;
  }
  uVar6 = *(undefined8 *)param_2;
  uVar7 = *(undefined8 *)(param_2 + 8);
  uVar8 = *(undefined8 *)(param_2 + 0x10);
  uVar9 = *(undefined8 *)(param_2 + 0x18);
  *(undefined **)(this + 8) = PTR_shared_null_008377d0;
  uVar4 = *(undefined8 *)(param_2 + 0x20);
  *(undefined8 *)(this + 0x10) = uVar6;
  *(undefined8 *)(this + 0x18) = uVar7;
  *(undefined8 *)(this + 0x30) = uVar4;
  *(undefined8 *)(this + 0x20) = uVar8;
  *(undefined8 *)(this + 0x28) = uVar9;
                    /* try { // try from 003590eb to 003590ef has its CatchHandler @ 003592e4 */
  QTransform::QTransform((QTransform *)(this + 0x38));
  *(undefined (*) [16])(this + 0x90) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0xa0) = (undefined  [16])0x0;
  return;
}



// ====== KisOptimizedBrushOutline @ 00359990 ======

/* KisOptimizedBrushOutline::KisOptimizedBrushOutline(QPainterPath const&, std::optional<QRectF>
   const&) */

void __thiscall
KisOptimizedBrushOutline::KisOptimizedBrushOutline
          (KisOptimizedBrushOutline *this,QPainterPath *param_1,optional *param_2)

{
  int iVar1;
  QArrayData *pQVar2;
  undefined8 *puVar3;
  QArrayData *pQVar4;
  Data *pDVar5;
  QArrayData *pQVar6;
  QArrayData *pQVar7;
  Data *pDVar8;
  long in_FS_OFFSET;
  QArrayData *local_98;
  Data *local_90;
  QTransform local_88 [88];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QTransform::QTransform(local_88);
  QPainterPath::toSubpathPolygons((QTransform *)&local_90);
                    /* try { // try from 003599e2 to 003599e6 has its CatchHandler @ 00359ba9 */
  FUN_0035a380(&local_98,&local_90);
                    /* try { // try from 003599f0 to 003599f4 has its CatchHandler @ 00359b9d */
  KisOptimizedBrushOutline(this,(QVector *)&local_98,param_2);
  if (*(int *)local_98 == 0) {
LAB_00359a60:
    pQVar6 = local_98 + *(long *)(local_98 + 0x10);
    pQVar4 = pQVar6 + (long)*(int *)(local_98 + 4) * 8;
joined_r0x00359a72:
    pQVar7 = pQVar6;
    if (pQVar6 != pQVar4) {
      do {
        pQVar6 = pQVar7 + 8;
        pQVar2 = *(QArrayData **)pQVar7;
        if (*(int *)pQVar2 == 0) {
          QArrayData::deallocate(pQVar2,0x10,8);
        }
        else {
          if (*(int *)pQVar2 == -1) goto joined_r0x00359a72;
          LOCK();
          *(int *)pQVar2 = *(int *)pQVar2 + -1;
          UNLOCK();
          if (*(int *)pQVar2 != 0) goto joined_r0x00359a72;
          QArrayData::deallocate(*(QArrayData **)pQVar7,0x10,8);
        }
        pQVar7 = pQVar6;
        if (pQVar4 == pQVar6) break;
      } while( true );
    }
    QArrayData::deallocate(local_98,8,8);
    iVar1 = *(int *)local_90;
    pDVar5 = local_90;
  }
  else {
    if (*(int *)local_98 != -1) {
      LOCK();
      *(int *)local_98 = *(int *)local_98 + -1;
      UNLOCK();
      if (*(int *)local_98 == 0) goto LAB_00359a60;
    }
    iVar1 = *(int *)local_90;
    pDVar5 = local_90;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_00359a37;
    LOCK();
    *(int *)pDVar5 = *(int *)pDVar5 + -1;
    UNLOCK();
    if (*(int *)pDVar5 != 0) goto LAB_00359a37;
  }
  iVar1 = *(int *)(pDVar5 + 8);
  pDVar8 = pDVar5 + (long)*(int *)(pDVar5 + 0xc) * 8 + 0x10;
  if ((long)*(int *)(pDVar5 + 0xc) * 8 != (long)iVar1 * 8) {
    do {
      puVar3 = *(undefined8 **)(pDVar8 + -8);
      pDVar8 = pDVar8 + -8;
      if (puVar3 != (undefined8 *)0x0) {
        pQVar4 = (QArrayData *)*puVar3;
        if (*(int *)pQVar4 == 0) {
          QArrayData::deallocate(pQVar4,0x10,8);
        }
        else if (*(int *)pQVar4 != -1) {
          LOCK();
          *(int *)pQVar4 = *(int *)pQVar4 + -1;
          UNLOCK();
          if (*(int *)pQVar4 == 0) {
            QArrayData::deallocate((QArrayData *)*puVar3,0x10,8);
          }
        }
        operator_delete(puVar3,8);
      }
    } while (pDVar5 + (long)iVar1 * 8 + 0x10 != pDVar8);
  }
  QListData::dispose(pDVar5);
LAB_00359a37:
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



