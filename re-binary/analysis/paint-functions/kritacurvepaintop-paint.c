/* Painting functions extracted from kritacurvepaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintLine @ 00114d38 ======

/* KisCurvePaintOp::paintLine(KisSharedPtr<KisPaintDevice>, KisPaintInformation const&,
   KisPaintInformation const&) [clone .cold] */

void KisCurvePaintOp::paintLine(void)

{
  QBrush *unaff_R14;
  long in_FS_OFFSET;
  long param_11;
  
  QBrush::~QBrush(unaff_R14);
  if (param_11 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 00114e00 ======

/* KisCurvePaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) [clone .cold] */

void KisCurvePaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3,undefined param_4,undefined param_5,undefined param_6
               ,KisSharedPtr *param_7,undefined param_8,undefined param_9,undefined param_10,
               long param_11)

{
  long in_FS_OFFSET;
  
  KisSharedPtr<KisPaintDevice>::deref(param_7,(KisPaintDevice *)param_2);
  if (param_11 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 0011dfc0 ======

/* KisCurvePaintOp::paintAt(KisPaintInformation const&) */

KisCurvePaintOp * __thiscall
KisCurvePaintOp::paintAt(KisCurvePaintOp *this,KisPaintInformation *param_1)

{
  long lVar1;
  undefined8 uVar2;
  long in_FS_OFFSET;
  
  uVar2 = DAT_0013fee8;
  lVar1 = *(long *)(in_FS_OFFSET + 0x28);
  if (*(code **)(*(long *)param_1 + 0x38) == updateSpacingImpl) {
    *(undefined8 *)this = 1;
    *(undefined8 *)(this + 0x18) = 0;
    this[0x20] = (KisCurvePaintOp)0x0;
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


// ====== paintLine @ 0011e9e0 ======

/* KisCurvePaintOp::paintLine(KisSharedPtr<KisPaintDevice>, KisPaintInformation const&,
   KisPaintInformation const&) */

void __thiscall
KisCurvePaintOp::paintLine
          (KisCurvePaintOp *this,undefined8 *param_2,undefined8 param_3,bool param_4)

{
  long *plVar1;
  long lVar2;
  QList<QPointF> *this_00;
  int iVar3;
  char cVar4;
  int iVar5;
  QPointF *pQVar6;
  uint *puVar7;
  long lVar8;
  KoColor *pKVar9;
  int iVar10;
  uint *puVar11;
  long in_FS_OFFSET;
  double dVar12;
  double dVar13;
  double local_70;
  QPen local_58 [8];
  long *local_50;
  long *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*(long *)(this + 0x100) == 0) {
    pKVar9 = (KoColor *)operator_new(0x10);
    local_48 = (long *)*param_2;
    if (local_48 != (long *)0x0) {
      LOCK();
      *(int *)(local_48 + 2) = *(int *)(local_48 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 0011ee56 to 0011ee5a has its CatchHandler @ 0011eefd */
    KisPainter::KisPainter((KisPainter *)pKVar9,&local_48);
    *(KoColor **)(this + 0x100) = pKVar9;
    if (local_48 != (long *)0x0) {
      LOCK();
      plVar1 = local_48 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*local_48 + 0x20))();
      }
      pKVar9 = *(KoColor **)(this + 0x100);
    }
    KisPaintOp::painter();
    KisPainter::paintColor();
    KisPainter::setPaintColor(pKVar9);
  }
  iVar3 = *(int *)(this + 0x34);
  this_00 = (QList<QPointF> *)(this + 0xf8);
  pQVar6 = (QPointF *)KisPaintInformation::pos();
  QList<QPointF>::append(this_00,pQVar6);
  while( true ) {
    puVar7 = *(uint **)(this + 0xf8);
    iVar10 = (int)this_00;
    if ((int)(puVar7[3] - puVar7[2]) <= iVar3) break;
    if (1 < *puVar7) {
      QList<QPointF>::detach_helper(iVar10);
      puVar7 = *(uint **)(this + 0xf8);
    }
    puVar11 = puVar7 + (long)(int)puVar7[2] * 2 + 4;
    if (1 < *puVar7) {
      if (1 < *puVar7) {
        QList<QPointF>::detach_helper(iVar10);
        puVar7 = *(uint **)(this + 0xf8);
      }
      puVar11 = puVar7 + (long)(int)puVar7[2] * 2 + 4;
    }
    if (*(void **)puVar11 == (void *)0x0) {
      QListData::erase((void **)this_00);
    }
    else {
      operator_delete(*(void **)puVar11,0x10);
      QListData::erase((void **)this_00);
    }
  }
  KisPaintOp::painter();
  KisPainter::device();
                    /* try { // try from 0011eb08 to 0011eb0c has its CatchHandler @ 0011ef15 */
  KisPaintDevice::defaultBounds();
                    /* try { // try from 0011eb15 to 0011eb17 has its CatchHandler @ 0011eef1 */
  iVar5 = (**(code **)(*local_48 + 0x30))();
  dVar13 = DAT_0013fee8;
  if (iVar5 < 1) {
    local_70 = DAT_0013fee8;
  }
  else {
    local_70 = DAT_0013fee8 / (double)(1 << ((byte)iVar5 & 0x1f));
  }
  if (local_48 != (long *)0x0) {
    LOCK();
    plVar1 = local_48 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_48 + 8))();
    }
  }
  if (local_50 != (long *)0x0) {
    LOCK();
    plVar1 = local_50 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_50 + 0x20))();
    }
  }
  cVar4 = KisCurveOption::isChecked();
  if (cVar4 != '\0') {
    dVar12 = (double)KisCurveOption::computeSizeLikeValue
                               ((KisPaintInformation *)(this + 0x88),param_4);
    local_70 = dVar12 * local_70;
  }
  iVar5 = *(int *)(this + 0x38);
  QBrush::QBrush((QBrush *)&local_48,3,1);
                    /* try { // try from 0011ebeb to 0011ebef has its CatchHandler @ 0011eee5 */
  QPen::QPen(local_58,(QBrush *)&local_48,(double)iVar5 * local_70,1,0x10,0x40);
  QBrush::~QBrush((QBrush *)&local_48);
  QPainterPath::QPainterPath((QPainterPath *)&local_50);
  if (this[0x30] != (KisCurvePaintOp)0x0) {
    KisPaintInformation::pos();
    QPainterPath::moveTo((QPointF *)&local_50);
    KisPaintInformation::pos();
    QPainterPath::lineTo((QPointF *)&local_50);
    KisPainter::drawPainterPath(*(QPainterPath **)(this + 0x100),(QPen *)&local_50);
    QPainterPath::QPainterPath((QPainterPath *)&local_48);
    plVar1 = local_50;
    local_50 = local_48;
    local_48 = plVar1;
    QPainterPath::~QPainterPath((QPainterPath *)&local_48);
  }
  puVar7 = *(uint **)(this + 0xf8);
  if (iVar3 <= (int)(puVar7[3] - puVar7[2])) {
    if (1 < *puVar7) {
      QList<QPointF>::detach_helper(iVar10);
    }
                    /* try { // try from 0011ec37 to 0011edfa has its CatchHandler @ 0011ef09 */
    QPainterPath::moveTo((QPointF *)&local_50);
    if (this[0x31] == (KisCurvePaintOp)0x0) {
      if (1 < **(uint **)(this + 0xf8)) {
        QList<QPointF>::detach_helper(iVar10);
      }
      lVar2 = *(long *)(this + 0xf8) + 0x10;
      lVar8 = (long)*(int *)(*(long *)(this + 0xf8) + 8) + (long)((iVar3 / 3) * 2);
      QPainterPath::cubicTo
                ((QPointF *)&local_50,*(QPointF **)(lVar2 + (lVar8 - iVar3 / 3) * 8),
                 *(QPointF **)(lVar2 + lVar8 * 8));
    }
    else {
      if (1 < **(uint **)(this + 0xf8)) {
                    /* try { // try from 0011eec6 to 0011eeda has its CatchHandler @ 0011ef09 */
        QList<QPointF>::detach_helper(iVar10);
      }
      QPainterPath::quadTo
                ((QPointF *)&local_50,
                 *(QPointF **)
                  (*(long *)(this + 0xf8) + 0x10 +
                  ((long)(iVar3 / 2) + (long)*(int *)(*(long *)(this + 0xf8) + 8)) * 8));
    }
    cVar4 = KisCurveOption::isChecked();
    if (cVar4 != '\0') {
      dVar13 = (double)KisCurveOption::computeSizeLikeValue
                                 ((KisPaintInformation *)(this + 0xc0),param_4);
    }
    KisPainter::setOpacityF(dVar13 * *(double *)(this + 0x40));
    KisPainter::drawPainterPath(*(QPainterPath **)(this + 0x100),(QPen *)&local_50);
    KisPainter::setOpacityToUnit();
  }
  QPainterPath::~QPainterPath((QPainterPath *)&local_50);
  QPen::~QPen(local_58);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintLine @ 0011ef30 ======

/* KisCurvePaintOp::paintLine(KisPaintInformation const&, KisPaintInformation const&,
   KisDistanceInformation*) */

void KisCurvePaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
  long *plVar1;
  long lVar2;
  KisPaintInformation *pKVar3;
  undefined8 uVar4;
  long *plVar5;
  long *plVar6;
  long in_FS_OFFSET;
  long *local_58;
  long *local_50;
  undefined local_48 [16];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  lVar2 = KisPaintOp::painter();
  if (lVar2 != 0) {
    if (*(long **)(param_1 + 0x20) == (long *)0x0) {
      KisPaintOp::source();
                    /* try { // try from 0011f0fa to 0011f0fe has its CatchHandler @ 0011f17d */
      KisPaintDevice::createCompositionSourceDevice();
      plVar5 = *(long **)(param_1 + 0x20);
      plVar6 = plVar5;
      if ((long *)local_48._0_8_ != plVar5) {
        if ((long *)local_48._0_8_ != (long *)0x0) {
          LOCK();
          *(int *)(local_48._0_8_ + 0x10) = *(int *)(local_48._0_8_ + 0x10) + 1;
          UNLOCK();
          plVar5 = *(long **)(param_1 + 0x20);
        }
        *(undefined8 *)(param_1 + 0x20) = local_48._0_8_;
        plVar6 = (long *)local_48._0_8_;
        if (plVar5 != (long *)0x0) {
          LOCK();
          plVar1 = plVar5 + 2;
          *(int *)plVar1 = *(int *)plVar1 + -1;
          UNLOCK();
          if (*(int *)plVar1 == 0) {
            (**(code **)(*plVar5 + 0x20))();
            plVar6 = (long *)local_48._0_8_;
          }
        }
      }
      if (plVar6 != (long *)0x0) {
        LOCK();
        plVar5 = plVar6 + 2;
        *(int *)plVar5 = *(int *)plVar5 + -1;
        UNLOCK();
        if (*(int *)plVar5 == 0) {
          (**(code **)(*plVar6 + 0x20))();
        }
      }
      if (local_50 != (long *)0x0) {
        LOCK();
        plVar5 = local_50 + 2;
        *(int *)plVar5 = *(int *)plVar5 + -1;
        UNLOCK();
        if (*(int *)plVar5 == 0) {
          (**(code **)(*local_50 + 0x20))();
        }
      }
    }
    else {
      (**(code **)(**(long **)(param_1 + 0x20) + 0x68))();
    }
    plVar5 = *(long **)(param_1 + 0x20);
    local_48._0_8_ = plVar5;
    if (plVar5 == (long *)0x0) {
                    /* try { // try from 0011f0bc to 0011f0c0 has its CatchHandler @ 0011f195 */
      paintLine((KisCurvePaintOp *)param_1,local_48,param_2,param_3);
    }
    else {
      LOCK();
      *(int *)(plVar5 + 2) = *(int *)(plVar5 + 2) + 1;
      UNLOCK();
                    /* try { // try from 0011efa7 to 0011efab has its CatchHandler @ 0011f195 */
      paintLine((KisCurvePaintOp *)param_1,local_48,param_2,param_3);
      if (plVar5 != (long *)0x0) {
        LOCK();
        plVar6 = plVar5 + 2;
        *(int *)plVar6 = *(int *)plVar6 + -1;
        UNLOCK();
        if (*(int *)plVar6 == 0) {
          (**(code **)(*plVar5 + 0x20))(plVar5);
        }
      }
    }
    local_48 = KisPaintDevice::extent();
    pKVar3 = (KisPaintInformation *)KisPaintOp::painter();
    KisOpacityOption::apply((KisPainter *)(param_1 + 0x48),pKVar3);
    uVar4 = KisPaintOp::painter();
    local_50 = *(long **)(param_1 + 0x20);
    if (local_50 != (long *)0x0) {
      LOCK();
      *(int *)(local_50 + 2) = *(int *)(local_50 + 2) + 1;
      UNLOCK();
    }
    local_58 = (long *)local_48._0_8_;
                    /* try { // try from 0011f027 to 0011f02b has its CatchHandler @ 0011f1a1 */
    KisPainter::bitBlt(uVar4,&local_58,&local_50,local_48);
    if (local_50 != (long *)0x0) {
      LOCK();
      plVar5 = local_50 + 2;
      *(int *)plVar5 = *(int *)plVar5 + -1;
      UNLOCK();
      if (*(int *)plVar5 == 0) {
        (**(code **)(*local_50 + 0x20))();
      }
    }
    uVar4 = KisPaintOp::painter();
    local_50 = *(long **)(param_1 + 0x20);
    if (local_50 != (long *)0x0) {
      LOCK();
      *(int *)(local_50 + 2) = *(int *)(local_50 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 0011f073 to 0011f077 has its CatchHandler @ 0011f189 */
    KisPainter::renderMirrorMask(uVar4,local_48._0_8_,local_48._8_8_,&local_50);
    if (local_50 != (long *)0x0) {
      LOCK();
      plVar5 = local_50 + 2;
      *(int *)plVar5 = *(int *)plVar5 + -1;
      UNLOCK();
      if (*(int *)plVar5 == 0) {
        (**(code **)(*local_50 + 0x20))();
      }
    }
  }
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}


// ====== paintBezierCurve @ 00158308 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


