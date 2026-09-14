/* Painting functions extracted from kritahairypaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintLine @ 0011a7bc ======

/* KisHairyPaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) [clone .cold] */

void KisHairyPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3,undefined param_4,undefined param_5,undefined param_6
               ,undefined param_7,undefined param_8,undefined param_9,undefined param_10,
               KisSharedPtr *param_11,long param_12)

{
  long in_FS_OFFSET;
  
  KisSharedPtr<KisPaintDevice>::deref(param_11,(KisPaintDevice *)param_2);
  if (param_12 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 0011b30c ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* HairyBrush::paintLine(KisSharedPtr<KisPaintDevice>, KisSharedPtr<KisPaintDevice>,
   KisPaintInformation const&, KisPaintInformation const&, double, double) [clone .cold] */

void HairyBrush::paintLine(void)

{
  HairyBrush *pHVar1;
  long *plVar2;
  uint uVar3;
  long lVar4;
  code *pcVar5;
  KisRandomSource *this;
  undefined8 uVar6;
  float fVar7;
  char cVar8;
  byte bVar9;
  byte bVar10;
  uint *puVar11;
  KoColor *pKVar12;
  long *plVar13;
  void *__dest;
  undefined (*pauVar14) [16];
  undefined (*pauVar15) [16];
  undefined (*pauVar16) [16];
  long lVar17;
  long extraout_RDX;
  undefined4 uVar19;
  ulong uVar18;
  undefined4 extraout_var;
  undefined4 extraout_var_00;
  undefined4 extraout_var_01;
  HairyBrush *unaff_RBX;
  QMapNodeBase *pQVar20;
  long unaff_RBP;
  long lVar21;
  Bristle *pBVar22;
  undefined4 uVar24;
  ulong uVar23;
  QArrayData *pQVar25;
  int iVar26;
  long *unaff_R12;
  long lVar27;
  long lVar28;
  long unaff_R14;
  QArrayData *unaff_R15;
  long in_FS_OFFSET;
  double dVar29;
  double dVar30;
  double dVar31;
  double dVar32;
  double dVar33;
  double dVar34;
  
code_r0x0011b30c:
                    /* try { // try from 0011b30c to 0011b31a has its CatchHandler @ 0011b320 */
  qBadAlloc();
LAB_001425a3:
  if ((*(uint *)(unaff_R15 + 8) & 0x7fffffff) != 0) {
    lVar28 = *unaff_R12;
    lVar27 = *(long *)(unaff_R15 + 0x10);
    uVar6 = *(undefined8 *)(unaff_RBP + -0xe8);
    iVar26 = *(int *)(lVar28 + 4);
    pauVar14 = (undefined (*) [16])(*(long *)(lVar28 + 0x10) + lVar28);
    pauVar15 = pauVar14;
    if (pauVar14 != pauVar14 + iVar26) {
      do {
        pauVar16 = pauVar15 + 1;
        *(undefined (*) [16])(unaff_R15 + (lVar27 - (long)pauVar14) + (long)*pauVar15) = *pauVar15;
        pauVar15 = pauVar16;
      } while (pauVar14 + iVar26 != pauVar16);
      *(undefined8 *)(unaff_RBP + -0xe8) = uVar6;
    }
    *(int *)(unaff_R15 + 4) = iVar26;
  }
LAB_00141ff5:
  lVar28 = *(long *)unaff_RBX;
  uVar24 = *(undefined4 *)(unaff_RBX + 0x98);
  pKVar12 = (KoColor *)(*(long *)(unaff_RBP + -0xe8) + 0x20);
  iVar26 = *(int *)(unaff_RBX + 0x74) + -1 + (uint)(*(char *)(lVar28 + 0x20) == '\0');
  *(long *)(unaff_RBP + -0x138) = unaff_RBP + -0x78;
  __memcpy_chk(unaff_RBP + -0x78,pKVar12,uVar24);
  if (0 < iVar26) {
    *(long *)(unaff_RBP + -0x110) = (long)iVar26 << 4;
    lVar27 = 0;
    do {
      uVar24 = (undefined4)((ulong)pKVar12 >> 0x20);
      if (*(char *)(lVar28 + 0x18) == '\0') {
        cVar8 = KoColor::opacityU8();
        uVar19 = extraout_var;
        if (cVar8 != '\0') {
          KoColor::setOpacity((double)*(float *)(*(long *)(unaff_RBP + -0xe8) + 0x10));
          uVar19 = extraout_var_01;
        }
LAB_00142294:
        lVar28 = *(long *)unaff_RBX;
      }
      else {
        lVar21 = 0;
        iVar26 = *(int *)(*(long *)(unaff_RBP + -0xe8) + 0x5c);
        lVar17 = (long)iVar26;
        lVar4 = *(long *)(lVar28 + 0x10);
        if (iVar26 < *(int *)(unaff_RBP + -0xf8)) {
          dVar31 = *(double *)(lVar4 + lVar17 * 8 + *(long *)(lVar4 + 0x10));
        }
        else {
          lVar21 = *(long *)(unaff_RBP + -0x148);
          dVar31 = *(double *)(lVar4 + lVar21 + *(long *)(lVar4 + 0x10));
        }
        uVar24 = (undefined4)((ulong)lVar21 >> 0x20);
        cVar8 = *(char *)(lVar28 + 0x1b);
        *(float *)(unaff_RBP + -0xf4) = (float)dVar31;
        if ((cVar8 != '\0') && (*(long *)(unaff_RBX + 0xf8) != 0)) {
          pBVar22 = *(Bristle **)(unaff_RBP + -0xe8);
          saturationDepletion(unaff_RBX,pBVar22,*(KoColor **)(unaff_RBP + -0x100),
                              *(double *)(unaff_RBP + -0x118),(double)(float)dVar31);
          uVar24 = (undefined4)((ulong)pBVar22 >> 0x20);
          lVar28 = *(long *)unaff_RBX;
          lVar17 = extraout_RDX;
        }
        uVar19 = (undefined4)((ulong)lVar17 >> 0x20);
        if (*(char *)(lVar28 + 0x1c) != '\0') {
          pKVar12 = *(KoColor **)(unaff_RBP + -0xe8);
          opacityDepletion((Bristle *)unaff_RBX,pKVar12,*(double *)(unaff_RBP + -0x118),
                           (double)*(float *)(unaff_RBP + -0xf4));
          uVar24 = (undefined4)((ulong)pKVar12 >> 0x20);
          uVar19 = extraout_var_00;
          goto LAB_00142294;
        }
      }
      dVar31 = *(double *)(unaff_R15 + lVar27 + *(long *)(unaff_R15 + 0x10));
      if (*(char *)(lVar28 + 0x20) == '\0') {
        if (dVar31 < 0.0) {
          iVar26 = (int)(dVar31 - *(double *)(unaff_RBP + -0x108));
          uVar23 = (ulong)(uint)((int)((dVar31 - (double)iVar26) + DAT_00165b18) + iVar26);
        }
        else {
          uVar23 = CONCAT44(uVar24,(int)(dVar31 + DAT_00165b18));
        }
        dVar31 = *(double *)(unaff_R15 + lVar27 + *(long *)(unaff_R15 + 0x10) + 8);
        if (dVar31 < 0.0) {
          iVar26 = (int)(dVar31 - *(double *)(unaff_RBP + -0x108));
          uVar18 = (ulong)(uint)((int)((dVar31 - (double)iVar26) + DAT_00165b18) + iVar26);
        }
        else {
          uVar18 = CONCAT44(uVar19,(int)(dVar31 + DAT_00165b18));
        }
        pcVar5 = *(code **)(**(long **)(unaff_RBX + 0x88) + 0x30);
        if (*(char *)(lVar28 + 0x21) == '\0') {
          (*pcVar5)(*(long **)(unaff_RBX + 0x88),uVar23,uVar18);
          plVar13 = (long *)KisPaintDevice::colorSpace();
          *(undefined8 *)(unaff_RBP + -0xf0) = *(undefined8 *)(*plVar13 + 0x138);
          pKVar12 = (KoColor *)
                    (**(code **)(*(long *)(*(long *)(unaff_RBX + 0x88) + 0x18) + 0x10))
                              (*(long *)(unaff_RBX + 0x88) + 0x18);
          bVar9 = (**(code **)(unaff_RBP + -0xf0))(plVar13,pKVar12);
          bVar10 = KoColor::opacityU8();
          if (bVar9 < bVar10) {
            uVar3 = *(uint *)(unaff_RBX + 0x98);
            __dest = (void *)(**(code **)(*(long *)(*(long *)(unaff_RBX + 0x88) + 0x18) + 0x10))
                                       (*(long *)(unaff_RBX + 0x88) + 0x18);
            pKVar12 = *(KoColor **)(unaff_RBP + -0x138);
            memcpy(__dest,pKVar12,(ulong)uVar3);
          }
          *(undefined4 *)(unaff_RBP + -0xf0) = 0x3f800000;
        }
        else {
          (*pcVar5)();
          uVar3 = *(uint *)(unaff_RBX + 0x98);
          *(undefined8 *)(unaff_RBP + -0x130) = *(undefined8 *)(unaff_RBX + 0x90);
          *(undefined **)(unaff_RBP + -0xa0) = PTR_shared_null_0017ffc0;
          pKVar12 = (KoColor *)
                    (**(code **)(*(long *)(*(long *)(unaff_RBX + 0x88) + 0x18) + 0x10))
                              (*(long *)(unaff_RBX + 0x88) + 0x18);
          fVar7 = DAT_0016a074;
          *(undefined4 *)(unaff_RBP + -0xf0) = 0x3f800000;
          KoCompositeOp::composite
                    (*(uchar **)(unaff_RBP + -0x130),(int)pKVar12,(uchar *)(ulong)uVar3,
                     (int)*(undefined8 *)(unaff_RBP + -0x138),(uchar *)(ulong)uVar3,0,0,1,fVar7,
                     (QBitArray *)0x1);
          pQVar25 = *(QArrayData **)(unaff_RBP + -0xa0);
          if (*(int *)pQVar25 == 0) {
LAB_00142258:
            pKVar12 = (KoColor *)0x0;
            QArrayData::deallocate(pQVar25,1,8);
          }
          else if (*(int *)pQVar25 != -1) {
            LOCK();
            *(int *)pQVar25 = *(int *)pQVar25 + -1;
            iVar26 = *(int *)pQVar25;
            UNLOCK();
            pQVar25 = *(QArrayData **)(unaff_RBP + -0xa0);
            if (iVar26 == 0) goto LAB_00142258;
          }
        }
      }
      else if (*(char *)(lVar28 + 0x21) == '\0') {
        pKVar12 = *(KoColor **)(unaff_RBP + -0x100);
        paintParticle((QPointF)unaff_RBX,pKVar12,dVar31);
        *(float *)(unaff_RBP + -0xf0) = DAT_0016a074;
      }
      else {
        pKVar12 = *(KoColor **)(unaff_RBP + -0x100);
        paintParticle((QPointF)unaff_RBX,pKVar12);
        *(float *)(unaff_RBP + -0xf0) = DAT_0016a074;
      }
      pBVar22 = *(Bristle **)(unaff_RBP + -0xe8);
      Bristle::setInkAmount(pBVar22,*(float *)(unaff_RBP + -0xf0) - *(float *)(unaff_RBP + -0xf4));
      lVar28 = *(long *)(unaff_RBP + -0x110);
      lVar27 = lVar27 + 0x10;
      pBVar22 = pBVar22 + 0x5c;
      *(int *)pBVar22 = *(int *)pBVar22 + 1;
      if (lVar27 == lVar28) break;
      lVar28 = *(long *)unaff_RBX;
    } while( true );
  }
  if (*(int *)unaff_R15 != 0) {
    if (*(int *)unaff_R15 == -1) goto LAB_00141d58;
    LOCK();
    *(int *)unaff_R15 = *(int *)unaff_R15 + -1;
    UNLOCK();
    if (*(int *)unaff_R15 != 0) goto LAB_00141d58;
  }
  unaff_R14 = unaff_R14 + 8;
  QArrayData::deallocate(unaff_R15,0x10,8);
  if (unaff_R14 != *(long *)(unaff_RBP + -0x128)) {
    do {
      puVar11 = *(uint **)(unaff_RBX + 8);
      if (*(char *)(*(long *)((long)puVar11 + *(long *)(puVar11 + 4) + unaff_R14) + 0x60) != '\0') {
        if (1 < *puVar11) {
          if ((puVar11[2] & 0x7fffffff) == 0) {
            puVar11 = (uint *)QArrayData::allocate(8,8,0,2);
            *(uint **)(unaff_RBX + 8) = puVar11;
          }
          else {
            QVector<Bristle*>::realloc
                      ((QVector<Bristle*> *)(unaff_RBX + 8),puVar11[2] & 0x7fffffff,0);
            puVar11 = *(uint **)(unaff_RBX + 8);
          }
        }
        *(undefined8 *)(unaff_RBP + -0xe8) =
             *(undefined8 *)((long)puVar11 + *(long *)(puVar11 + 4) + unaff_R14);
        dVar29 = (double)KisRandomSource::generateNormalized();
        dVar31 = *(double *)(unaff_RBP + -0x108);
        dVar32 = *(double *)(*(long *)unaff_RBX + 0x30);
        dVar30 = (double)KisRandomSource::generateNormalized();
        pHVar1 = unaff_RBX + 0x10;
        dVar33 = *(double *)(unaff_RBP + -0x108);
        dVar34 = *(double *)(*(long *)unaff_RBX + 0x30);
        *(double *)(unaff_RBP + -0xf0) =
             *(double *)(unaff_RBP + -0x118) * *(double *)(*(long *)unaff_RBX + 0x28);
        QTransform::reset();
        QTransform::rotateRadians
                  ((double)(*(ulong *)(unaff_RBP + -0x170) ^ _DAT_00165b40),(Axis)pHVar1);
        QTransform::scale(*(double *)(unaff_RBP + -0x120),*(double *)(unaff_RBP + -0x120));
        QTransform::translate
                  (((dVar29 + dVar29) - dVar31) * dVar32,((dVar30 + dVar30) - dVar33) * dVar34);
        QTransform::shear(*(double *)(unaff_RBP + -0xf0),*(double *)(unaff_RBP + -0xf0));
        dVar32 = (double)(*(float **)(unaff_RBP + -0xe8))[1];
        dVar31 = (double)**(float **)(unaff_RBP + -0xe8);
        if ((*(int *)(unaff_RBX + 0x9c) == 1) || (*(char *)(*(long *)unaff_RBX + 0x1f) == '\0')) {
          QTransform::map(dVar31,dVar32,(double *)pHVar1,(double *)(unaff_RBP + -0xd0));
          QTransform::map((double)**(float **)(unaff_RBP + -0xe8),
                          (double)(*(float **)(unaff_RBP + -0xe8))[1],(double *)pHVar1,
                          (double *)(unaff_RBP + -0xc0));
        }
        else {
          *(double *)(unaff_RBP + -0xd0) = (double)*(float *)(*(long *)(unaff_RBP + -0xe8) + 8);
          *(double *)(unaff_RBP + -200) = (double)*(float *)(*(long *)(unaff_RBP + -0xe8) + 0xc);
          QTransform::map(dVar31,dVar32,(double *)pHVar1,(double *)(unaff_RBP + -0xc0));
        }
        dVar29 = *(double *)(unaff_RBP + -0x160) + *(double *)(unaff_RBP + -200);
        dVar32 = *(double *)(unaff_RBP + -0xc0) + *(double *)(unaff_RBP + -0x158);
        dVar33 = *(double *)(unaff_RBP + -0xb8) + *(double *)(unaff_RBP + -0x150);
        *(ulong *)(*(long *)(unaff_RBP + -0xe8) + 8) =
             CONCAT44((float)*(double *)(unaff_RBP + -0xb8),(float)*(double *)(unaff_RBP + -0xc0));
        lVar28 = *(long *)unaff_RBX;
        dVar34 = *(double *)(unaff_RBP + -0x168) + *(double *)(unaff_RBP + -0xd0);
        *(double *)(unaff_RBP + -200) = dVar29;
        *(double *)(unaff_RBP + -0xb8) = dVar33;
        dVar31 = *(double *)(lVar28 + 0x40);
        *(double *)(unaff_RBP + -0xc0) = dVar32;
        *(double *)(unaff_RBP + -0xd0) = dVar34;
        if ((dVar31 == 0.0) ||
           (*(double *)(unaff_RBP + -0x178) <=
            (double)*(float *)(*(long *)(unaff_RBP + -0xe8) + 0x10))) goto LAB_00141fa0;
      }
LAB_00141d58:
      unaff_R14 = unaff_R14 + 8;
      if (unaff_R14 == *(long *)(unaff_RBP + -0x128)) break;
    } while( true );
  }
  plVar13 = *(long **)(unaff_RBX + 0x80);
  if (plVar13 != (long *)0x0) {
    *(undefined8 *)(unaff_RBX + 0x80) = 0;
    LOCK();
    plVar2 = plVar13 + 2;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*plVar13 + 0x20))();
    }
  }
  plVar13 = *(long **)(unaff_RBX + 0x88);
  if (plVar13 != (long *)0x0) {
    *(undefined8 *)(unaff_RBX + 0x88) = 0;
    LOCK();
    plVar2 = plVar13 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*plVar13 + 8))();
    }
  }
  this = *(KisRandomSource **)(unaff_RBP + -0xd8);
  if (this != (KisRandomSource *)0x0) {
    LOCK();
    *(int *)this = *(int *)this + -1;
    UNLOCK();
    if (*(int *)this == 0) {
      KisRandomSource::~KisRandomSource(this);
      operator_delete(this,0x18);
    }
  }
  pQVar20 = *(QMapNodeBase **)(unaff_RBP + -0x48);
  if (*(int *)pQVar20 != 0) {
    if (*(int *)pQVar20 == -1) goto LAB_001424cb;
    LOCK();
    *(int *)pQVar20 = *(int *)pQVar20 + -1;
    iVar26 = *(int *)pQVar20;
    UNLOCK();
    pQVar20 = *(QMapNodeBase **)(unaff_RBP + -0x48);
    if (iVar26 != 0) goto LAB_001424cb;
  }
  lVar28 = *(long *)(pQVar20 + 0x10);
  if (lVar28 != 0) {
    pQVar25 = *(QArrayData **)(lVar28 + 0x18);
    if (*(int *)pQVar25 == 0) {
LAB_00142903:
      QArrayData::deallocate(pQVar25,2,8);
    }
    else if (*(int *)pQVar25 != -1) {
      LOCK();
      *(int *)pQVar25 = *(int *)pQVar25 + -1;
      UNLOCK();
      if (*(int *)pQVar25 == 0) {
        pQVar25 = *(QArrayData **)(lVar28 + 0x18);
        goto LAB_00142903;
      }
    }
    QVariant::~QVariant((QVariant *)(lVar28 + 0x20));
    lVar27 = *(long *)(lVar28 + 8);
    if (lVar27 != 0) {
      pQVar25 = *(QArrayData **)(lVar27 + 0x18);
      if (*(int *)pQVar25 == 0) {
LAB_0014266a:
        QArrayData::deallocate(pQVar25,2,8);
      }
      else if (*(int *)pQVar25 != -1) {
        LOCK();
        *(int *)pQVar25 = *(int *)pQVar25 + -1;
        UNLOCK();
        if (*(int *)pQVar25 == 0) {
          pQVar25 = *(QArrayData **)(lVar27 + 0x18);
          goto LAB_0014266a;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar27 + 0x20));
      if (*(QMapNode<QString,QVariant> **)(lVar27 + 8) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar27 + 8));
      }
      if (*(QMapNode<QString,QVariant> **)(lVar27 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar27 + 0x10));
      }
    }
    lVar28 = *(long *)(lVar28 + 0x10);
    if (lVar28 != 0) {
      pQVar25 = *(QArrayData **)(lVar28 + 0x18);
      if (*(int *)pQVar25 == 0) {
LAB_001426c6:
        QArrayData::deallocate(pQVar25,2,8);
      }
      else if (*(int *)pQVar25 != -1) {
        LOCK();
        *(int *)pQVar25 = *(int *)pQVar25 + -1;
        UNLOCK();
        if (*(int *)pQVar25 == 0) {
          pQVar25 = *(QArrayData **)(lVar28 + 0x18);
          goto LAB_001426c6;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar28 + 0x20));
      if (*(QMapNode<QString,QVariant> **)(lVar28 + 8) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar28 + 8));
      }
      if (*(QMapNode<QString,QVariant> **)(lVar28 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar28 + 0x10));
      }
    }
    QMapDataBase::freeTree(pQVar20,(int)*(undefined8 *)(pQVar20 + 0x10));
  }
  QMapDataBase::freeData((QMapDataBase *)pQVar20);
LAB_001424cb:
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
LAB_00141fa0:
  *(double *)(unaff_RBP + -0xa0) = dVar32;
  *(double *)(unaff_RBP + -0x98) = dVar33;
  *(double *)(unaff_RBP + -0xb0) = dVar34;
  *(double *)(unaff_RBP + -0xa8) = dVar29;
  unaff_R12 = (long *)Trajectory::getLinearTrajectory
                                ((QPointF *)(unaff_RBX + 0x68),(QPointF *)(unaff_RBP + -0xb0),
                                 *(double *)(unaff_RBP + -0x108));
  unaff_R15 = (QArrayData *)*unaff_R12;
  if (*(int *)unaff_R15 != 0) {
    if (*(int *)unaff_R15 != -1) {
      LOCK();
      *(int *)unaff_R15 = *(int *)unaff_R15 + 1;
      UNLOCK();
      unaff_R15 = (QArrayData *)*unaff_R12;
    }
    goto LAB_00141ff5;
  }
  if ((char)unaff_R15[0xb] < '\0') {
    unaff_R15 = (QArrayData *)
                QArrayData::allocate(0x10,8,(ulong)(*(uint *)(unaff_R15 + 8) & 0x7fffffff),0);
    if (unaff_R15 == (QArrayData *)0x0) {
      qBadAlloc();
    }
    unaff_R15[0xb] = (QArrayData)((byte)unaff_R15[0xb] | 0x80);
    goto LAB_001425a3;
  }
  unaff_R15 = (QArrayData *)QArrayData::allocate(0x10,8,(long)*(int *)(unaff_R15 + 4),0);
  if (unaff_R15 == (QArrayData *)0x0) goto code_r0x0011b30c;
  goto LAB_001425a3;
}


// ====== paintAt @ 0012e1f0 ======

/* KisHairyPaintOp::paintAt(KisPaintInformation const&) */

KisHairyPaintOp * __thiscall
KisHairyPaintOp::paintAt(KisHairyPaintOp *this,KisPaintInformation *param_1)

{
  long lVar1;
  undefined8 uVar2;
  long in_FS_OFFSET;
  
  uVar2 = DAT_00165b18;
  lVar1 = *(long *)(in_FS_OFFSET + 0x28);
  if (*(code **)(*(long *)param_1 + 0x38) == updateSpacingImpl) {
    *(undefined8 *)this = 1;
    *(undefined8 *)(this + 0x18) = 0;
    this[0x20] = (KisHairyPaintOp)0x0;
    *(undefined8 *)(this + 8) = uVar2;
    *(undefined8 *)(this + 0x10) = uVar2;
  }
  else {
    (**(code **)(*(long *)param_1 + 0x38))(this);
  }
  if (lVar1 == *(long *)(in_FS_OFFSET + 0x28)) {
    return this;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 0012e270 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisHairyPaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) */

void __thiscall
KisHairyPaintOp::paintLine
          (KisHairyPaintOp *this,KisPaintInformation *param_1,KisPaintInformation *param_2,
          KisDistanceInformation *param_3)

{
  long *plVar1;
  double dVar2;
  char cVar3;
  char cVar4;
  int iVar5;
  long lVar6;
  KisPaintInformation *pKVar7;
  undefined8 uVar8;
  long *plVar9;
  long *plVar10;
  long in_FS_OFFSET;
  double dVar11;
  double local_b8;
  KisPaintInformation local_98 [8];
  KisDistanceInformation local_90 [8];
  undefined local_88 [16];
  long *local_78;
  undefined8 local_70;
  long *local_68;
  undefined local_60 [16];
  undefined8 local_50;
  undefined local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  lVar6 = KisPaintOp::painter();
  if (lVar6 != 0) {
    if (*(long **)(this + 0xc0) == (long *)0x0) {
      KisPaintOp::source();
                    /* try { // try from 0012e667 to 0012e66b has its CatchHandler @ 0012e6f5 */
      KisPaintDevice::createCompositionSourceDevice();
      plVar9 = *(long **)(this + 0xc0);
      plVar10 = plVar9;
      if (local_68 != plVar9) {
        if (local_68 != (long *)0x0) {
          LOCK();
          *(int *)(local_68 + 2) = *(int *)(local_68 + 2) + 1;
          UNLOCK();
          plVar9 = *(long **)(this + 0xc0);
        }
        *(long **)(this + 0xc0) = local_68;
        plVar10 = local_68;
        if (plVar9 != (long *)0x0) {
          LOCK();
          plVar1 = plVar9 + 2;
          *(int *)plVar1 = *(int *)plVar1 + -1;
          UNLOCK();
          if (*(int *)plVar1 == 0) {
            (**(code **)(*plVar9 + 0x20))();
            plVar10 = local_68;
          }
        }
      }
      if (plVar10 != (long *)0x0) {
        LOCK();
        plVar9 = plVar10 + 2;
        *(int *)plVar9 = *(int *)plVar9 + -1;
        UNLOCK();
        if (*(int *)plVar9 == 0) {
          (**(code **)(*plVar10 + 0x20))();
        }
      }
      if (local_78 != (long *)0x0) {
        LOCK();
        plVar9 = local_78 + 2;
        *(int *)plVar9 = *(int *)plVar9 + -1;
        UNLOCK();
        if (*(int *)plVar9 == 0) {
          (**(code **)(*local_78 + 0x20))();
        }
      }
    }
    else {
      (**(code **)(**(long **)(this + 0xc0) + 0x68))();
    }
    KisPaintInformation::KisPaintInformation(local_98,param_2);
                    /* try { // try from 0012e2e8 to 0012e2ec has its CatchHandler @ 0012e70d */
    KisPaintInformation::registerDistanceInformation(local_90);
                    /* try { // try from 0012e2f7 to 0012e328 has its CatchHandler @ 0012e749 */
    cVar3 = KisCurveOption::isChecked();
    local_b8 = DAT_00165b20;
    if (cVar3 != '\0') {
      local_b8 = (double)KisCurveOption::computeSizeLikeValue
                                   ((KisPaintInformation *)(this + 0x210),SUB81(local_98,0));
    }
    KisPaintOp::painter();
    KisPainter::device();
                    /* try { // try from 0012e336 to 0012e33a has its CatchHandler @ 0012e701 */
    KisPaintDevice::defaultBounds();
                    /* try { // try from 0012e343 to 0012e345 has its CatchHandler @ 0012e731 */
    iVar5 = (**(code **)(*local_68 + 0x30))();
    dVar11 = DAT_00165b20;
    if (0 < iVar5) {
      dVar11 = DAT_00165b20 / (double)(1 << ((byte)iVar5 & 0x1f));
    }
    if (local_68 != (long *)0x0) {
      LOCK();
      plVar9 = local_68 + 1;
      *(int *)plVar9 = *(int *)plVar9 + -1;
      UNLOCK();
      if (*(int *)plVar9 == 0) {
        (**(code **)(*local_68 + 8))();
      }
    }
    dVar11 = dVar11 * local_b8;
    if (local_78 != (long *)0x0) {
      LOCK();
      plVar9 = local_78 + 2;
      *(int *)plVar9 = *(int *)plVar9 + -1;
      UNLOCK();
      if (*(int *)plVar9 == 0) {
        (**(code **)(*local_78 + 0x20))();
      }
    }
                    /* try { // try from 0012e3a6 to 0012e3e9 has its CatchHandler @ 0012e749 */
    local_b8 = (double)KisRotationOption::apply((KisPaintInformation *)(this + 0x248));
    pKVar7 = (KisPaintInformation *)KisPaintOp::painter();
    KisOpacityOption::apply((KisPainter *)(this + 0x1d0),pKVar7);
    cVar3 = KisPaintInformation::canvasMirroredH();
    cVar4 = KisPaintInformation::canvasMirroredV();
    if (cVar3 != cVar4) {
      local_b8 = (double)((ulong)local_b8 ^ _DAT_00165b40);
    }
    local_68 = *(long **)(this + 200);
    dVar2 = *(double *)(this + 0x70);
    if (local_68 != (long *)0x0) {
      LOCK();
      *(int *)(local_68 + 2) = *(int *)(local_68 + 2) + 1;
      UNLOCK();
    }
    local_78 = *(long **)(this + 0xc0);
    if (local_78 != (long *)0x0) {
      LOCK();
      *(int *)(local_78 + 2) = *(int *)(local_78 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 0012e451 to 0012e455 has its CatchHandler @ 0012e725 */
    HairyBrush::paintLine
              (dVar11 * dVar2,local_b8,this + 0xd0,&local_78,(KisTimingInformation *)&local_68,
               param_1,local_98);
    if (local_78 != (long *)0x0) {
      LOCK();
      plVar9 = local_78 + 2;
      *(int *)plVar9 = *(int *)plVar9 + -1;
      UNLOCK();
      if (*(int *)plVar9 == 0) {
        (**(code **)(*local_78 + 0x20))();
      }
    }
    if (local_68 != (long *)0x0) {
      LOCK();
      plVar9 = local_68 + 2;
      *(int *)plVar9 = *(int *)plVar9 + -1;
      UNLOCK();
      if (*(int *)plVar9 == 0) {
        (**(code **)(*local_68 + 0x20))();
      }
    }
                    /* try { // try from 0012e487 to 0012e49d has its CatchHandler @ 0012e749 */
    local_88 = KisPaintDevice::extent();
    uVar8 = KisPaintOp::painter();
    local_68 = *(long **)(this + 0xc0);
    if (local_68 != (long *)0x0) {
      LOCK();
      *(int *)(local_68 + 2) = *(int *)(local_68 + 2) + 1;
      UNLOCK();
    }
    local_78 = (long *)local_88._0_8_;
                    /* try { // try from 0012e4cc to 0012e4d0 has its CatchHandler @ 0012e719 */
    KisPainter::bitBlt(uVar8,&local_78,(KisTimingInformation *)&local_68,local_88);
    if (local_68 != (long *)0x0) {
      LOCK();
      plVar9 = local_68 + 2;
      *(int *)plVar9 = *(int *)plVar9 + -1;
      UNLOCK();
      if (*(int *)plVar9 == 0) {
        (**(code **)(*local_68 + 0x20))();
      }
    }
                    /* try { // try from 0012e4e9 to 0012e4ed has its CatchHandler @ 0012e749 */
    uVar8 = KisPaintOp::painter();
    local_68 = *(long **)(this + 0xc0);
    if (local_68 != (long *)0x0) {
      LOCK();
      *(int *)(local_68 + 2) = *(int *)(local_68 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 0012e514 to 0012e518 has its CatchHandler @ 0012e73d */
    KisPainter::renderMirrorMask
              (uVar8,local_88._0_8_,local_88._8_8_,(KisTimingInformation *)&local_68);
    if (local_68 != (long *)0x0) {
      LOCK();
      plVar9 = local_68 + 2;
      *(int *)plVar9 = *(int *)plVar9 + -1;
      UNLOCK();
      if (*(int *)plVar9 == 0) {
        (**(code **)(*local_68 + 0x20))();
      }
    }
    local_78 = (long *)0x0;
    local_70 = DAT_00165b28;
    local_68 = (long *)0x1;
    local_50 = 0;
    local_48 = 0;
    local_60 = (undefined  [16])0x0;
                    /* try { // try from 0012e56f to 0012e5bf has its CatchHandler @ 0012e749 */
    KisDistanceInformation::registerPaintedDab
              ((KisPaintInformation *)param_3,(KisSpacingInformation *)local_98,
               (KisTimingInformation *)&local_68);
    KisPaintInformation::DistanceInformationRegistrar::~DistanceInformationRegistrar
              ((DistanceInformationRegistrar *)local_90);
    KisPaintInformation::~KisPaintInformation(local_98);
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}


// ====== paintLine @ 00141b20 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* HairyBrush::paintLine(KisSharedPtr<KisPaintDevice>, KisSharedPtr<KisPaintDevice>,
   KisPaintInformation const&, KisPaintInformation const&, double, double) */

void HairyBrush::paintLine
               (double param_1,ulong param_2,HairyBrush *param_3,long *param_4,long *param_5)

{
  QArrayData *pQVar1;
  long *plVar2;
  HairyBrush *pHVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  int iVar9;
  uint uVar10;
  Bristle *this;
  long lVar11;
  code *pcVar12;
  uchar *puVar13;
  float fVar14;
  double dVar15;
  char cVar16;
  byte bVar17;
  byte bVar18;
  double *pdVar19;
  long lVar20;
  KoColorSpace *pKVar21;
  long lVar22;
  uint *puVar23;
  long *plVar24;
  KoColor *pKVar25;
  void *__dest;
  undefined (*pauVar26) [16];
  undefined8 *puVar27;
  long lVar28;
  long extraout_RDX;
  undefined4 uVar30;
  ulong uVar29;
  undefined4 extraout_var;
  undefined4 extraout_var_00;
  undefined4 extraout_var_01;
  long lVar31;
  Bristle *pBVar32;
  undefined4 uVar35;
  ulong uVar33;
  undefined (*pauVar34) [16];
  QArrayData *pQVar36;
  int iVar37;
  long lVar38;
  long lVar39;
  long lVar40;
  long in_FS_OFFSET;
  double dVar41;
  double dVar42;
  double dVar43;
  double dVar44;
  QArrayData *pQVar45;
  double dVar46;
  QTextStream *pQVar47;
  double dVar48;
  double local_128;
  float local_fc;
  KisRandomSource *local_e0;
  QArrayData *local_d8;
  double local_d0;
  QArrayData *local_c8;
  QTextStream *local_c0;
  QArrayData *local_b8;
  double dStack_b0;
  QArrayData *local_a8;
  undefined auStack_a0 [16];
  undefined8 local_90;
  KoColor local_88 [8];
  KoColor local_80 [48];
  QMapNodeBase *local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  *(int *)(param_3 + 0x9c) = *(int *)(param_3 + 0x9c) + 1;
  pdVar19 = (double *)KisPaintInformation::pos();
  dVar4 = *pdVar19;
  lVar20 = KisPaintInformation::pos();
  dVar5 = *(double *)(lVar20 + 8);
  pdVar19 = (double *)KisPaintInformation::pos();
  dVar6 = *pdVar19;
  lVar20 = KisPaintInformation::pos();
  dVar15 = DAT_0016a6d8;
  dVar7 = *(double *)(lVar20 + 8);
  dVar42 = DAT_0016a6d8;
  local_128 = param_1;
  if (*(char *)(*(long *)param_3 + 0x1a) != '\0') {
    dVar41 = dVar6 - dVar4;
    dVar42 = dVar7 - dVar5;
    dVar42 = DAT_0016a6d8 - SQRT(dVar41 * dVar41 + dVar42 * dVar42) / _DAT_0016a6e0;
    if (dVar42 < 0.0) {
      dVar42 = 0.0;
    }
    dVar41 = (*(double *)(param_3 + 0xa8) * _DAT_0016a6e8 + _DAT_0016a6f0 + dVar42) / _DAT_0016a6f8;
    dVar42 = DAT_0016a6d8 - dVar41;
    *(double *)(param_3 + 0xa8) = dVar41;
    local_128 = param_1 * dVar42;
  }
  dVar41 = (double)KisPaintInformation::pressure();
  dVar42 = (dVar41 + dVar41) * dVar42;
  pKVar21 = (KoColorSpace *)KisPaintDevice::colorSpace();
  KoColor::KoColor(local_88,pKVar21);
                    /* try { // try from 00141c3c to 00141ce6 has its CatchHandler @ 00142a4e */
  KisPaintDevice::createRandomAccessorNG();
  pQVar45 = *(QArrayData **)(param_3 + 0x88);
  pQVar36 = pQVar45;
  if (local_a8 != pQVar45) {
    if (local_a8 != (QArrayData *)0x0) {
      LOCK();
      *(int *)(local_a8 + 8) = *(int *)(local_a8 + 8) + 1;
      UNLOCK();
      pQVar45 = *(QArrayData **)(param_3 + 0x88);
    }
    *(QArrayData **)(param_3 + 0x88) = local_a8;
    pQVar36 = local_a8;
    if (pQVar45 != (QArrayData *)0x0) {
      LOCK();
      pQVar1 = pQVar45 + 8;
      *(int *)pQVar1 = *(int *)pQVar1 + -1;
      UNLOCK();
      if (*(int *)pQVar1 == 0) {
        (**(code **)(*(long *)pQVar45 + 8))();
        pQVar36 = local_a8;
      }
    }
  }
  if (pQVar36 != (QArrayData *)0x0) {
    LOCK();
    pQVar45 = pQVar36 + 8;
    *(int *)pQVar45 = *(int *)pQVar45 + -1;
    UNLOCK();
    if (*(int *)pQVar45 == 0) {
      (**(code **)(*(long *)pQVar36 + 8))();
    }
  }
  param_4 = (long *)*param_4;
  plVar24 = *(long **)(param_3 + 0x80);
  if (param_4 != plVar24) {
    if (param_4 != (long *)0x0) {
      LOCK();
      *(int *)(param_4 + 2) = *(int *)(param_4 + 2) + 1;
      UNLOCK();
      plVar24 = *(long **)(param_3 + 0x80);
    }
    *(long **)(param_3 + 0x80) = param_4;
    if (plVar24 != (long *)0x0) {
      LOCK();
      plVar2 = plVar24 + 2;
      *(int *)plVar2 = *(int *)plVar2 + -1;
      UNLOCK();
      if (*(int *)plVar2 == 0) {
        (**(code **)(*plVar24 + 0x20))();
      }
    }
  }
  if (*(int *)(param_3 + 0x9c) == 1) {
                    /* try { // try from 00142803 to 0014283d has its CatchHandler @ 00142a4e */
    initAndCache(param_3);
    if (((*(char *)(*(long *)param_3 + 0x18) != '\0') && (*(int *)(param_3 + 0x9c) == 1)) &&
       (*(char *)(*(long *)param_3 + 0x1e) != '\0')) {
      if (*param_5 == 0) {
                    /* try { // try from 00142934 to 00142980 has its CatchHandler @ 00142a4e */
        lVar20 = _41000();
        if (*(char *)(lVar20 + 0x10) != '\0') {
          lVar20 = _41000();
          local_90 = *(undefined8 *)(lVar20 + 8);
          auStack_a0 = (undefined  [16])0x0;
          local_a8 = (QArrayData *)0x2;
          QMessageLogger::debug();
          pQVar47 = local_c0;
                    /* try { // try from 0014299e to 001429a2 has its CatchHandler @ 00142a15 */
          QString::fromUtf8_helper((char *)&local_b8,0x16a680);
                    /* try { // try from 001429a9 to 001429ad has its CatchHandler @ 00142a09 */
          QTextStream::operator<<(pQVar47,(QString *)&local_b8);
          if (*(int *)local_b8 == 0) {
LAB_001429e7:
            QArrayData::deallocate(local_b8,2,8);
          }
          else if (*(int *)local_b8 != -1) {
            LOCK();
            *(int *)local_b8 = *(int *)local_b8 + -1;
            UNLOCK();
            if (*(int *)local_b8 == 0) goto LAB_001429e7;
          }
          if (local_c0[0x20] != (QTextStream)0x0) {
                    /* try { // try from 001429fd to 00142a01 has its CatchHandler @ 00142a15 */
            QTextStream::operator<<(local_c0,' ');
          }
          QDebug::~QDebug((QDebug *)&local_c0);
        }
      }
      else {
        puVar27 = (undefined8 *)KisPaintInformation::pos();
        pQVar45 = (QArrayData *)*param_5;
        local_a8 = pQVar45;
        if (pQVar45 == (QArrayData *)0x0) {
          colorifyBristles(*puVar27,puVar27[1],param_3,&local_a8);
        }
        else {
          LOCK();
          *(int *)(pQVar45 + 0x10) = *(int *)(pQVar45 + 0x10) + 1;
          UNLOCK();
                    /* try { // try from 00142872 to 0014292e has its CatchHandler @ 00142a39 */
          colorifyBristles(*puVar27,puVar27[1],param_3,&local_a8);
          if (pQVar45 != (QArrayData *)0x0) {
            LOCK();
            pQVar36 = pQVar45 + 0x10;
            *(int *)pQVar36 = *(int *)pQVar36 + -1;
            UNLOCK();
            if (*(int *)pQVar36 == 0) {
              (**(code **)(*(long *)pQVar45 + 0x20))(pQVar45);
            }
          }
        }
      }
    }
  }
  KisPaintInformation::randomSource();
  iVar9 = *(int *)(*(long *)(*(long *)param_3 + 0x10) + 4);
  iVar37 = *(int *)(*(long *)(param_3 + 8) + 4);
                    /* try { // try from 00141cfd to 00141fd4 has its CatchHandler @ 00142a45 */
  dVar41 = (double)KisPaintInformation::pressure();
  if (0 < iVar37) {
    iVar9 = iVar9 + -1;
    lVar40 = 0;
    local_fc = 0.0;
    lVar22 = (long)iVar9 * 8;
    lVar20 = (long)iVar37 * 8;
    do {
      while (puVar23 = *(uint **)(param_3 + 8),
            *(char *)(*(long *)((long)puVar23 + *(long *)(puVar23 + 4) + lVar40) + 0x60) == '\0') {
LAB_00141d58:
        lVar40 = lVar40 + 8;
        if (lVar40 == lVar20) goto LAB_00142450;
      }
      if (1 < *puVar23) {
        if ((puVar23[2] & 0x7fffffff) == 0) {
          puVar23 = (uint *)QArrayData::allocate(8,8,0,2);
          *(uint **)(param_3 + 8) = puVar23;
        }
        else {
                    /* try { // try from 0014251e to 0014256e has its CatchHandler @ 00142a45 */
          QVector<Bristle*>::realloc((QVector<Bristle*> *)(param_3 + 8),puVar23[2] & 0x7fffffff,0);
          puVar23 = *(uint **)(param_3 + 8);
        }
      }
      this = *(Bristle **)((long)puVar23 + *(long *)(puVar23 + 4) + lVar40);
      dVar43 = (double)KisRandomSource::generateNormalized();
      dVar46 = *(double *)(*(long *)param_3 + 0x30);
      dVar44 = (double)KisRandomSource::generateNormalized();
      pHVar3 = param_3 + 0x10;
      dVar8 = *(double *)(*(long *)param_3 + 0x30);
      dVar48 = dVar42 * *(double *)(*(long *)param_3 + 0x28);
      QTransform::reset();
      QTransform::rotateRadians((double)(param_2 ^ _DAT_00165b40),(Axis)pHVar3);
      QTransform::scale(local_128,local_128);
      QTransform::translate
                (((dVar43 + dVar43) - dVar15) * dVar46,((dVar44 + dVar44) - dVar15) * dVar8);
      QTransform::shear(dVar48,dVar48);
      if ((*(int *)(param_3 + 0x9c) == 1) || (*(char *)(*(long *)param_3 + 0x1f) == '\0')) {
        QTransform::map((double)*(float *)this,(double)*(float *)(this + 4),(double *)pHVar3,
                        (double *)&local_d8);
        QTransform::map((double)*(float *)this,(double)*(float *)(this + 4),(double *)pHVar3,
                        (double *)&local_c8);
      }
      else {
        local_d8 = (QArrayData *)(double)*(float *)(this + 8);
        local_d0 = (double)*(float *)(this + 0xc);
        QTransform::map((double)*(float *)this,(double)*(float *)(this + 4),(double *)pHVar3,
                        (double *)&local_c8);
      }
      local_d0 = dVar5 + local_d0;
      pQVar45 = (QArrayData *)((double)local_c8 + dVar6);
      pQVar47 = (QTextStream *)((double)local_c0 + dVar7);
      *(ulong *)(this + 8) = CONCAT44((float)(double)local_c0,(float)(double)local_c8);
      local_d8 = (QArrayData *)(dVar4 + (double)local_d8);
      local_c8 = pQVar45;
      local_c0 = pQVar47;
      if ((*(double *)(*(long *)param_3 + 0x40) != 0.0) &&
         ((double)*(float *)(this + 0x10) < dVar15 - dVar41)) goto LAB_00141d58;
      auStack_a0._0_8_ = pQVar47;
      local_b8 = local_d8;
      dStack_b0 = local_d0;
      local_a8 = pQVar45;
      plVar24 = (long *)Trajectory::getLinearTrajectory
                                  ((QPointF *)(param_3 + 0x68),(QPointF *)&local_b8,dVar15);
      pQVar45 = (QArrayData *)*plVar24;
      if (*(int *)pQVar45 == 0) {
        if ((char)pQVar45[0xb] < '\0') {
          pQVar45 = (QArrayData *)
                    QArrayData::allocate(0x10,8,(ulong)(*(uint *)(pQVar45 + 8) & 0x7fffffff),0);
          if (pQVar45 == (QArrayData *)0x0) {
            qBadAlloc();
          }
          pQVar45[0xb] = (QArrayData)((byte)pQVar45[0xb] | 0x80);
        }
        else {
          pQVar45 = (QArrayData *)QArrayData::allocate(0x10,8,(long)*(int *)(pQVar45 + 4),0);
          if (pQVar45 == (QArrayData *)0x0) {
            qBadAlloc();
          }
        }
        if ((*(uint *)(pQVar45 + 8) & 0x7fffffff) != 0) {
          lVar39 = *plVar24;
          iVar37 = *(int *)(lVar39 + 4);
          pauVar26 = (undefined (*) [16])(*(long *)(lVar39 + 0x10) + lVar39);
          pauVar34 = pauVar26 + iVar37;
          lVar39 = *(long *)(pQVar45 + 0x10) - (long)pauVar26;
          for (; pauVar26 != pauVar34; pauVar26 = pauVar26 + 1) {
            *(undefined (*) [16])(pQVar45 + lVar39 + (long)*pauVar26) = *pauVar26;
          }
          *(int *)(pQVar45 + 4) = iVar37;
        }
      }
      else if (*(int *)pQVar45 != -1) {
        LOCK();
        *(int *)pQVar45 = *(int *)pQVar45 + 1;
        UNLOCK();
        pQVar45 = (QArrayData *)*plVar24;
      }
      lVar39 = *(long *)param_3;
      pKVar25 = (KoColor *)(this + 0x20);
      iVar37 = *(int *)(param_3 + 0x74) + -1 + (uint)(*(char *)(lVar39 + 0x20) == '\0');
      __memcpy_chk(local_80,pKVar25,*(undefined4 *)(param_3 + 0x98));
      if (0 < iVar37) {
        lVar38 = 0;
        do {
          uVar35 = (undefined4)((ulong)pKVar25 >> 0x20);
          if (*(char *)(lVar39 + 0x18) == '\0') {
                    /* try { // try from 00142287 to 0014250b has its CatchHandler @ 00142a21 */
            cVar16 = KoColor::opacityU8();
            uVar30 = extraout_var;
            if (cVar16 != '\0') {
              KoColor::setOpacity((double)*(float *)(this + 0x10));
              uVar30 = extraout_var_01;
            }
LAB_00142294:
            lVar39 = *(long *)param_3;
          }
          else {
            lVar31 = 0;
            lVar28 = (long)*(int *)(this + 0x5c);
            lVar11 = *(long *)(lVar39 + 0x10);
            if (*(int *)(this + 0x5c) < iVar9) {
              dVar46 = *(double *)(lVar11 + lVar28 * 8 + *(long *)(lVar11 + 0x10));
            }
            else {
              dVar46 = *(double *)(lVar11 + lVar22 + *(long *)(lVar11 + 0x10));
              lVar31 = lVar22;
            }
            uVar35 = (undefined4)((ulong)lVar31 >> 0x20);
            local_fc = (float)dVar46;
            if ((*(char *)(lVar39 + 0x1b) != '\0') && (*(long *)(param_3 + 0xf8) != 0)) {
              pBVar32 = this;
              saturationDepletion(param_3,this,local_88,dVar42,(double)local_fc);
              uVar35 = (undefined4)((ulong)pBVar32 >> 0x20);
              lVar39 = *(long *)param_3;
              lVar28 = extraout_RDX;
            }
            uVar30 = (undefined4)((ulong)lVar28 >> 0x20);
            if (*(char *)(lVar39 + 0x1c) != '\0') {
              pBVar32 = this;
              opacityDepletion((Bristle *)param_3,(KoColor *)this,dVar42,(double)local_fc);
              uVar35 = (undefined4)((ulong)pBVar32 >> 0x20);
              uVar30 = extraout_var_00;
              goto LAB_00142294;
            }
          }
          dVar46 = *(double *)(pQVar45 + lVar38 + *(long *)(pQVar45 + 0x10));
          if (*(char *)(lVar39 + 0x20) == '\0') {
            if (dVar46 < 0.0) {
              uVar33 = (ulong)(uint)((int)((dVar46 - (double)(int)(dVar46 - dVar15)) + DAT_00165b18)
                                    + (int)(dVar46 - dVar15));
            }
            else {
              uVar33 = CONCAT44(uVar35,(int)(dVar46 + DAT_00165b18));
            }
            dVar46 = *(double *)(pQVar45 + lVar38 + *(long *)(pQVar45 + 0x10) + 8);
            if (dVar46 < 0.0) {
              uVar29 = (ulong)(uint)((int)((dVar46 - (double)(int)(dVar46 - dVar15)) + DAT_00165b18)
                                    + (int)(dVar46 - dVar15));
            }
            else {
              uVar29 = CONCAT44(uVar30,(int)(dVar46 + DAT_00165b18));
            }
            pcVar12 = *(code **)(**(long **)(param_3 + 0x88) + 0x30);
            if (*(char *)(lVar39 + 0x21) == '\0') {
              (*pcVar12)(*(long **)(param_3 + 0x88),uVar33,uVar29);
              plVar24 = (long *)KisPaintDevice::colorSpace();
              pcVar12 = *(code **)(*plVar24 + 0x138);
              pKVar25 = (KoColor *)
                        (**(code **)(*(long *)(*(long *)(param_3 + 0x88) + 0x18) + 0x10))
                                  (*(long *)(param_3 + 0x88) + 0x18);
              bVar17 = (*pcVar12)(plVar24,pKVar25);
              bVar18 = KoColor::opacityU8();
              if (bVar17 < bVar18) {
                uVar10 = *(uint *)(param_3 + 0x98);
                __dest = (void *)(**(code **)(*(long *)(*(long *)(param_3 + 0x88) + 0x18) + 0x10))
                                           (*(long *)(param_3 + 0x88) + 0x18);
                pKVar25 = local_80;
                memcpy(__dest,local_80,(ulong)uVar10);
              }
              fVar14 = 1.0;
            }
            else {
              (*pcVar12)();
              puVar13 = *(uchar **)(param_3 + 0x90);
              uVar10 = *(uint *)(param_3 + 0x98);
              local_a8 = (QArrayData *)PTR_shared_null_0017ffc0;
                    /* try { // try from 001421e8 to 00142227 has its CatchHandler @ 00142a2d */
              pKVar25 = (KoColor *)
                        (**(code **)(*(long *)(*(long *)(param_3 + 0x88) + 0x18) + 0x10))
                                  (*(long *)(param_3 + 0x88) + 0x18);
              fVar14 = 1.0;
              KoCompositeOp::composite
                        (puVar13,(int)pKVar25,(uchar *)(ulong)uVar10,(int)local_80,
                         (uchar *)(ulong)uVar10,0,0,1,DAT_0016a074,(QBitArray *)0x1);
              if (*(int *)local_a8 == 0) {
LAB_00142258:
                pKVar25 = (KoColor *)0x0;
                QArrayData::deallocate(local_a8,1,8);
              }
              else if (*(int *)local_a8 != -1) {
                LOCK();
                *(int *)local_a8 = *(int *)local_a8 + -1;
                UNLOCK();
                if (*(int *)local_a8 == 0) goto LAB_00142258;
              }
            }
          }
          else {
            pKVar25 = local_88;
            if (*(char *)(lVar39 + 0x21) == '\0') {
              paintParticle((QPointF)param_3,local_88,dVar46);
              fVar14 = DAT_0016a074;
            }
            else {
                    /* try { // try from 00142067 to 001421b5 has its CatchHandler @ 00142a21 */
              paintParticle((QPointF)param_3,local_88);
              fVar14 = DAT_0016a074;
            }
          }
          Bristle::setInkAmount(this,fVar14 - local_fc);
          lVar38 = lVar38 + 0x10;
          *(int *)(this + 0x5c) = *(int *)(this + 0x5c) + 1;
          if (lVar38 == (long)iVar37 << 4) break;
          lVar39 = *(long *)param_3;
        } while( true );
      }
      if (*(int *)pQVar45 != 0) {
        if (*(int *)pQVar45 != -1) {
          LOCK();
          *(int *)pQVar45 = *(int *)pQVar45 + -1;
          UNLOCK();
          if (*(int *)pQVar45 == 0) goto LAB_00142428;
        }
        goto LAB_00141d58;
      }
LAB_00142428:
      lVar40 = lVar40 + 8;
      QArrayData::deallocate(pQVar45,0x10,8);
    } while (lVar40 != lVar20);
  }
LAB_00142450:
  plVar24 = *(long **)(param_3 + 0x80);
  if (plVar24 != (long *)0x0) {
    *(undefined8 *)(param_3 + 0x80) = 0;
    LOCK();
    plVar2 = plVar24 + 2;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*plVar24 + 0x20))();
    }
  }
  plVar24 = *(long **)(param_3 + 0x88);
  if (plVar24 != (long *)0x0) {
    *(undefined8 *)(param_3 + 0x88) = 0;
    LOCK();
    plVar2 = plVar24 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*plVar24 + 8))();
    }
  }
  if (local_e0 != (KisRandomSource *)0x0) {
    LOCK();
    *(int *)local_e0 = *(int *)local_e0 + -1;
    UNLOCK();
    if (*(int *)local_e0 == 0) {
      KisRandomSource::~KisRandomSource(local_e0);
      operator_delete(local_e0,0x18);
    }
  }
  if (*(int *)local_50 != 0) {
    if (*(int *)local_50 == -1) goto LAB_001424cb;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_001424cb;
  }
  lVar20 = *(long *)(local_50 + 0x10);
  if (lVar20 != 0) {
    pQVar45 = *(QArrayData **)(lVar20 + 0x18);
    if (*(int *)pQVar45 == 0) {
LAB_00142903:
      QArrayData::deallocate(pQVar45,2,8);
    }
    else if (*(int *)pQVar45 != -1) {
      LOCK();
      *(int *)pQVar45 = *(int *)pQVar45 + -1;
      UNLOCK();
      if (*(int *)pQVar45 == 0) {
        pQVar45 = *(QArrayData **)(lVar20 + 0x18);
        goto LAB_00142903;
      }
    }
    QVariant::~QVariant((QVariant *)(lVar20 + 0x20));
    lVar22 = *(long *)(lVar20 + 8);
    if (lVar22 != 0) {
      pQVar45 = *(QArrayData **)(lVar22 + 0x18);
      if (*(int *)pQVar45 == 0) {
LAB_0014266a:
        QArrayData::deallocate(pQVar45,2,8);
      }
      else if (*(int *)pQVar45 != -1) {
        LOCK();
        *(int *)pQVar45 = *(int *)pQVar45 + -1;
        UNLOCK();
        if (*(int *)pQVar45 == 0) {
          pQVar45 = *(QArrayData **)(lVar22 + 0x18);
          goto LAB_0014266a;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar22 + 0x20));
      if (*(QMapNode<QString,QVariant> **)(lVar22 + 8) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar22 + 8));
      }
      if (*(QMapNode<QString,QVariant> **)(lVar22 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar22 + 0x10));
      }
    }
    lVar20 = *(long *)(lVar20 + 0x10);
    if (lVar20 != 0) {
      pQVar45 = *(QArrayData **)(lVar20 + 0x18);
      if (*(int *)pQVar45 == 0) {
LAB_001426c6:
        QArrayData::deallocate(pQVar45,2,8);
      }
      else if (*(int *)pQVar45 != -1) {
        LOCK();
        *(int *)pQVar45 = *(int *)pQVar45 + -1;
        UNLOCK();
        if (*(int *)pQVar45 == 0) {
          pQVar45 = *(QArrayData **)(lVar20 + 0x18);
          goto LAB_001426c6;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar20 + 0x20));
      if (*(QMapNode<QString,QVariant> **)(lVar20 + 8) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar20 + 8));
      }
      if (*(QMapNode<QString,QVariant> **)(lVar20 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar20 + 0x10));
      }
    }
    QMapDataBase::freeTree(local_50,(int)*(undefined8 *)(local_50 + 0x10));
  }
  QMapDataBase::freeData((QMapDataBase *)local_50);
LAB_001424cb:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}


// ====== paintBezierCurve @ 00182440 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


