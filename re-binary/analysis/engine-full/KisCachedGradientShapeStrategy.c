/* Class KisCachedGradientShapeStrategy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCachedGradientShapeStrategy @ 0020d680 ======

void __thiscall
KisCachedGradientShapeStrategy::KisCachedGradientShapeStrategy
          (KisCachedGradientShapeStrategy *this,QRect *param_1,double param_2,double param_3,
          KisGradientShapeStrategy *param_4)

{
  (*(code *)PTR_KisCachedGradientShapeStrategy_0083e610)();
  return;
}



// ====== KisCachedGradientShapeStrategy @ 004db110 ======

/* KisCachedGradientShapeStrategy::KisCachedGradientShapeStrategy(QRect const&, double, double,
   KisGradientShapeStrategy*) */

void __thiscall
KisCachedGradientShapeStrategy::KisCachedGradientShapeStrategy
          (KisCachedGradientShapeStrategy *this,QRect *param_1,double param_2,double param_3,
          KisGradientShapeStrategy *param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  KisCachedGradientShapeStrategy *this_00;
  KisBSpline2D *pKVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  QTextStream *pQVar8;
  QArrayData *pQVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  undefined (*pauVar13) [16];
  KisBSpline2D *this_01;
  undefined8 *puVar14;
  long lVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  uint uVar19;
  int iVar20;
  long in_FS_OFFSET;
  double dVar21;
  double dVar22;
  QTextStream *local_a8;
  QTextStream *local_a0;
  QArrayData *local_98;
  QArrayData *local_90;
  double local_88;
  undefined local_80 [16];
  undefined8 local_70;
  undefined local_68 [16];
  code *local_58;
  code *pcStack_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisGradientShapeStrategy::KisGradientShapeStrategy((KisGradientShapeStrategy *)this);
  *(undefined **)this = PTR_vtable_00837b00 + 0x10;
                    /* try { // try from 004db165 to 004db169 has its CatchHandler @ 004dc75b */
  pauVar13 = (undefined (*) [16])operator_new(0x30);
  iVar16 = *(int *)(param_1 + 8);
  *(undefined (**) [16])(this + 0x28) = pauVar13;
  *pauVar13 = (undefined  [16])0x0;
  iVar18 = *(int *)param_1;
  *(undefined8 *)(*pauVar13 + 8) = 0xffffffffffffffff;
  pauVar13[1] = (undefined  [16])0x0;
  pauVar13[2] = (undefined  [16])0x0;
  if ((iVar16 - iVar18 < 2) ||
     (iVar18 = *(int *)(param_1 + 0xc), iVar18 - *(int *)(param_1 + 4) < 2)) {
                    /* try { // try from 004db1d7 to 004db368 has its CatchHandler @ 004dc74f */
    kis_assert_recoverable
              ("rc.width() >= 3 && rc.height() >= 3",
               "/builds/graphics/krita/libs/image/kis_cached_gradient_shape_strategy.cpp",0x28);
    puVar14 = *(undefined8 **)(this + 0x28);
    uVar6 = *(undefined8 *)(param_1 + 8);
    this_00 = (KisCachedGradientShapeStrategy *)puVar14[4];
    *puVar14 = *(undefined8 *)param_1;
    puVar14[1] = uVar6;
    puVar14[2] = param_2;
    puVar14[3] = param_3;
    if ((param_4 != (KisGradientShapeStrategy *)this_00) &&
       (puVar14[4] = param_4, this_00 != (KisCachedGradientShapeStrategy *)0x0)) {
      if (*(code **)(*(long *)this_00 + 8) == ~KisCachedGradientShapeStrategy) {
        ~KisCachedGradientShapeStrategy(this_00);
        operator_delete(this_00,0x30);
        iVar16 = *(int *)(param_1 + 8);
        iVar18 = *(int *)(param_1 + 0xc);
        goto LAB_004db23e;
      }
      (**(code **)(*(long *)this_00 + 8))();
    }
    iVar16 = *(int *)(param_1 + 8);
    iVar18 = *(int *)(param_1 + 0xc);
  }
  else {
    uVar6 = *(undefined8 *)param_1;
    uVar7 = *(undefined8 *)(param_1 + 8);
    *(double *)pauVar13[1] = param_2;
    *(double *)(pauVar13[1] + 8) = param_3;
    *(undefined8 *)*pauVar13 = uVar6;
    *(undefined8 *)(*pauVar13 + 8) = uVar7;
    if (param_4 != (KisGradientShapeStrategy *)0x0) {
      *(KisGradientShapeStrategy **)pauVar13[2] = param_4;
    }
  }
LAB_004db23e:
  iVar11 = *(int *)param_1;
  iVar17 = *(int *)(param_1 + 4);
  iVar16 = (iVar16 - iVar11) + 1;
  iVar18 = (iVar18 - iVar17) + 1;
  dVar21 = (double)iVar16 / param_2;
  if ((double)((ulong)dVar21 & DAT_00722ba0) < DAT_00722b30) {
    dVar21 = (double)((ulong)((double)(long)dVar21 +
                             (double)(-(ulong)((double)(long)dVar21 < dVar21) & DAT_007227c0)) |
                     ~DAT_00722ba0 & (ulong)dVar21);
  }
  iVar20 = (int)dVar21;
  dVar21 = (double)iVar18 / param_3;
  if ((double)((ulong)dVar21 & DAT_00722ba0) < DAT_00722b30) {
    dVar21 = (double)((ulong)((double)(long)dVar21 +
                             (double)(-(ulong)((double)(long)dVar21 < dVar21) & DAT_007227c0)) |
                     ~DAT_00722ba0 & (ulong)dVar21);
  }
  iVar10 = (int)dVar21;
  if ((iVar20 < 2) || (iVar12 = iVar10, iVar10 < 2)) {
                    /* try { // try from 004db6a4 to 004db733 has its CatchHandler @ 004dc74f */
    lVar15 = _41000();
    if (*(char *)(lVar15 + 0x11) != '\0') {
      lVar15 = _41000();
      local_70 = *(undefined8 *)(lVar15 + 8);
      local_80 = (undefined  [16])0x0;
      local_88 = 9.88131291682493e-324;
      QMessageLogger::warning();
      QDebug::~QDebug((QDebug *)&local_90);
    }
    lVar15 = _41000();
    if (*(char *)(lVar15 + 0x11) != '\0') {
      lVar15 = _41000();
      local_70 = *(undefined8 *)(lVar15 + 8);
      local_80 = (undefined  [16])0x0;
      local_88 = 9.88131291682493e-324;
      QMessageLogger::warning();
      pQVar9 = local_98;
                    /* try { // try from 004db74d to 004db751 has its CatchHandler @ 004dc7bb */
      QString::fromUtf8_helper((char *)&local_90,0x73015c);
                    /* try { // try from 004db758 to 004db75c has its CatchHandler @ 004dc7c7 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc01b:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc01b;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc4de to 004dc4e2 has its CatchHandler @ 004dc7bb */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      QDebug::~QDebug((QDebug *)&local_98);
    }
                    /* try { // try from 004db797 to 004db7dc has its CatchHandler @ 004dc74f */
    lVar15 = _41000();
    if (*(char *)(lVar15 + 0x11) != '\0') {
      lVar15 = _41000();
      local_70 = *(undefined8 *)(lVar15 + 8);
      local_80 = (undefined  [16])0x0;
      local_88 = 9.88131291682493e-324;
      QMessageLogger::warning();
      pQVar9 = local_98;
                    /* try { // try from 004db7f6 to 004db7fa has its CatchHandler @ 004dc78b */
      QString::fromUtf8_helper((char *)&local_90,0x730118);
                    /* try { // try from 004db801 to 004db805 has its CatchHandler @ 004dc797 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc06b:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc06b;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc3e1 to 004dc3e5 has its CatchHandler @ 004dc78b */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004db848 to 004db84c has its CatchHandler @ 004dc78b */
      QString::fromUtf8_helper((char *)&local_90,0x730169);
                    /* try { // try from 004db853 to 004db857 has its CatchHandler @ 004dc887 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc389:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc389;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc605 to 004dc634 has its CatchHandler @ 004dc78b */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004db8a2 to 004db8a6 has its CatchHandler @ 004dc78b */
      QString::fromUtf8_helper((char *)&local_90,0x7312d2);
                    /* try { // try from 004db8ad to 004db8b1 has its CatchHandler @ 004dc87b */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc325:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc325;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
                    /* try { // try from 004db8e7 to 004db90f has its CatchHandler @ 004dc78b */
      QTextStream::operator<<((QTextStream *)local_98,iVar20);
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
      QString::fromUtf8_helper((char *)&local_90,0x730175);
                    /* try { // try from 004db916 to 004db91a has its CatchHandler @ 004dc72b */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc339:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc339;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004db95b to 004db95f has its CatchHandler @ 004dc78b */
      QString::fromUtf8_helper((char *)&local_90,0x7312d2);
                    /* try { // try from 004db966 to 004db96a has its CatchHandler @ 004dc71f */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc34d:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc34d;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
                    /* try { // try from 004db9a0 to 004db9a4 has its CatchHandler @ 004dc78b */
      QTextStream::operator<<((QTextStream *)local_98,iVar10);
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc56f to 004dc599 has its CatchHandler @ 004dc78b */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      QDebug::~QDebug((QDebug *)&local_98);
    }
                    /* try { // try from 004db9bc to 004dba01 has its CatchHandler @ 004dc74f */
    lVar15 = _41000();
    if (*(char *)(lVar15 + 0x11) != '\0') {
      lVar15 = _41000();
      local_70 = *(undefined8 *)(lVar15 + 8);
      local_80 = (undefined  [16])0x0;
      local_88 = 9.88131291682493e-324;
      QMessageLogger::warning();
      pQVar9 = local_98;
                    /* try { // try from 004dba1b to 004dba1f has its CatchHandler @ 004dc737 */
      QString::fromUtf8_helper((char *)&local_90,0x72daf9);
                    /* try { // try from 004dba26 to 004dba2a has its CatchHandler @ 004dc743 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc02f:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc02f;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004dba6d to 004dba71 has its CatchHandler @ 004dc737 */
      QString::fromUtf8_helper((char *)&local_90,0x74c420);
                    /* try { // try from 004dba78 to 004dba7c has its CatchHandler @ 004dc80f */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc3b1:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc3b1;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc3f8 to 004dc427 has its CatchHandler @ 004dc737 */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004dbac7 to 004dbacb has its CatchHandler @ 004dc737 */
      QString::fromUtf8_helper((char *)&local_90,0x7312d2);
                    /* try { // try from 004dbad2 to 004dbad6 has its CatchHandler @ 004dc803 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc39d:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc39d;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      *(int *)(local_98 + 0x18) = *(int *)(local_98 + 0x18) + 1;
      local_a0 = (QTextStream *)local_98;
                    /* try { // try from 004dbb2a to 004dbb2e has its CatchHandler @ 004dc7f7 */
      ::operator<<((QDebug)(QDebug *)&local_a8,(QRect *)&local_a0);
      pQVar8 = local_a8;
                    /* try { // try from 004dbb43 to 004dbb47 has its CatchHandler @ 004dc7eb */
      QString::fromUtf8_helper((char *)&local_90,0x730181);
                    /* try { // try from 004dbb4e to 004dbb52 has its CatchHandler @ 004dc86f */
      QTextStream::operator<<(pQVar8,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc3c5:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc3c5;
      }
      if (local_a8[0x20] != (QTextStream)0x0) {
                    /* try { // try from 004dc49f to 004dc4a3 has its CatchHandler @ 004dc7eb */
        QTextStream::operator<<(local_a8,' ');
      }
      pQVar8 = local_a8;
                    /* try { // try from 004dbb92 to 004dbb96 has its CatchHandler @ 004dc7eb */
      QString::fromUtf8_helper((char *)&local_90,0x7312d2);
                    /* try { // try from 004dbb9d to 004dbba1 has its CatchHandler @ 004dc863 */
      QTextStream::operator<<(pQVar8,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc294:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc294;
      }
      if (local_a8[0x20] != (QTextStream)0x0) {
        QTextStream::operator<<(local_a8,' ');
      }
                    /* try { // try from 004dbbda to 004dbc01 has its CatchHandler @ 004dc7eb */
      QTextStream::operator<<(local_a8,param_2);
      if (local_a8[0x20] != (QTextStream)0x0) {
        QTextStream::operator<<(local_a8,' ');
      }
      pQVar8 = local_a8;
      QString::fromUtf8_helper((char *)&local_90,0x730187);
                    /* try { // try from 004dbc08 to 004dbc0c has its CatchHandler @ 004dc857 */
      QTextStream::operator<<(pQVar8,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc375:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc375;
      }
      if (local_a8[0x20] != (QTextStream)0x0) {
                    /* try { // try from 004dc5ac to 004dc5db has its CatchHandler @ 004dc7eb */
        QTextStream::operator<<(local_a8,' ');
      }
      pQVar8 = local_a8;
                    /* try { // try from 004dbc4c to 004dbc50 has its CatchHandler @ 004dc7eb */
      QString::fromUtf8_helper((char *)&local_90,0x7312d2);
                    /* try { // try from 004dbc57 to 004dbc5b has its CatchHandler @ 004dc84b */
      QTextStream::operator<<(pQVar8,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc361:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc361;
      }
      if (local_a8[0x20] != (QTextStream)0x0) {
        QTextStream::operator<<(local_a8,' ');
      }
                    /* try { // try from 004dbc94 to 004dbc98 has its CatchHandler @ 004dc7eb */
      QTextStream::operator<<(local_a8,param_3);
      if (local_a8[0x20] != (QTextStream)0x0) {
                    /* try { // try from 004dc4ed to 004dc500 has its CatchHandler @ 004dc7eb */
        QTextStream::operator<<(local_a8,' ');
      }
      QDebug::~QDebug((QDebug *)&local_a8);
      QDebug::~QDebug((QDebug *)&local_a0);
      QDebug::~QDebug((QDebug *)&local_98);
    }
                    /* try { // try from 004dbcc2 to 004dbd2f has its CatchHandler @ 004dc74f */
    lVar15 = _41000();
    if (*(char *)(lVar15 + 0x11) != '\0') {
      lVar15 = _41000();
      local_70 = *(undefined8 *)(lVar15 + 8);
      local_80 = (undefined  [16])0x0;
      local_88 = 9.88131291682493e-324;
      QMessageLogger::warning();
      pQVar9 = local_98;
                    /* try { // try from 004dc0cd to 004dc0d1 has its CatchHandler @ 004dc713 */
      QString::fromUtf8_helper((char *)&local_90,0x72daf9);
                    /* try { // try from 004dc0d8 to 004dc0dc has its CatchHandler @ 004dc707 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc533:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc533;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc647 to 004dc6c7 has its CatchHandler @ 004dc713 */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004dc11e to 004dc122 has its CatchHandler @ 004dc713 */
      QString::fromUtf8_helper((char *)&local_90,0x730169);
                    /* try { // try from 004dc129 to 004dc12d has its CatchHandler @ 004dc6fb */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc51f:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc51f;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004dc177 to 004dc17b has its CatchHandler @ 004dc713 */
      QString::fromUtf8_helper((char *)&local_90,0x7312d2);
                    /* try { // try from 004dc182 to 004dc186 has its CatchHandler @ 004dc6ef */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc50b:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc50b;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
                    /* try { // try from 004dc1bc to 004dc1e3 has its CatchHandler @ 004dc713 */
      QTextStream::operator<<((QTextStream *)local_98,iVar20);
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
      QString::fromUtf8_helper((char *)&local_90,0x730175);
                    /* try { // try from 004dc1ea to 004dc1ee has its CatchHandler @ 004dc6e3 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc432:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc432;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004dc22e to 004dc232 has its CatchHandler @ 004dc713 */
      QString::fromUtf8_helper((char *)&local_90,0x7312d2);
                    /* try { // try from 004dc239 to 004dc23d has its CatchHandler @ 004dc6d7 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc483:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc483;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
                    /* try { // try from 004dc273 to 004dc277 has its CatchHandler @ 004dc713 */
      QTextStream::operator<<((QTextStream *)local_98,iVar10);
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      QDebug::~QDebug((QDebug *)&local_98);
    }
    if (iVar20 < 2) {
      iVar20 = 2;
    }
    iVar12 = 2;
    if (1 < iVar10) {
      iVar12 = iVar10;
    }
    lVar15 = _41000();
    if (*(char *)(lVar15 + 0x11) != '\0') {
      lVar15 = _41000();
      local_70 = *(undefined8 *)(lVar15 + 8);
      local_80 = (undefined  [16])0x0;
      local_88 = 9.88131291682493e-324;
      QMessageLogger::warning();
      pQVar9 = local_98;
                    /* try { // try from 004dbd49 to 004dbd4d has its CatchHandler @ 004dc7af */
      QString::fromUtf8_helper((char *)&local_90,0x73018d);
                    /* try { // try from 004dbd54 to 004dbd58 has its CatchHandler @ 004dc7a3 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc057:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc057;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc5ee to 004dc5f2 has its CatchHandler @ 004dc7af */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004dbd9a to 004dbd9e has its CatchHandler @ 004dc7af */
      QString::fromUtf8_helper((char *)&local_90,0x730169);
                    /* try { // try from 004dbda5 to 004dbda9 has its CatchHandler @ 004dc83f */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc2e4:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc2e4;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc44e to 004dc469 has its CatchHandler @ 004dc7af */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004dbdf3 to 004dbdf7 has its CatchHandler @ 004dc7af */
      QString::fromUtf8_helper((char *)&local_90,0x7312d2);
                    /* try { // try from 004dbdfe to 004dbe02 has its CatchHandler @ 004dc833 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc2a8:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc2a8;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
                    /* try { // try from 004dbe38 to 004dbe5f has its CatchHandler @ 004dc7af */
      QTextStream::operator<<((QTextStream *)local_98,iVar20);
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc4b6 to 004dc4ce has its CatchHandler @ 004dc7af */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
      QString::fromUtf8_helper((char *)&local_90,0x730175);
                    /* try { // try from 004dbe66 to 004dbe6a has its CatchHandler @ 004dc827 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc2bc:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc2bc;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      pQVar9 = local_98;
                    /* try { // try from 004dbeaa to 004dbeae has its CatchHandler @ 004dc7af */
      QString::fromUtf8_helper((char *)&local_90,0x7312d2);
                    /* try { // try from 004dbeb5 to 004dbeb9 has its CatchHandler @ 004dc81b */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc2d0:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc2d0;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
                    /* try { // try from 004dbeef to 004dbef3 has its CatchHandler @ 004dc7af */
      QTextStream::operator<<((QTextStream *)local_98,iVar12);
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc54c to 004dc55f has its CatchHandler @ 004dc7af */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      QDebug::~QDebug((QDebug *)&local_98);
    }
                    /* try { // try from 004dbf0b to 004dbf50 has its CatchHandler @ 004dc74f */
    lVar15 = _41000();
    if (*(char *)(lVar15 + 0x11) != '\0') {
      lVar15 = _41000();
      local_70 = *(undefined8 *)(lVar15 + 8);
      local_80 = (undefined  [16])0x0;
      local_88 = 9.88131291682493e-324;
      QMessageLogger::warning();
      pQVar9 = local_98;
                    /* try { // try from 004dbf6a to 004dbf6e has its CatchHandler @ 004dc7df */
      QString::fromUtf8_helper((char *)&local_90,0x73015c);
                    /* try { // try from 004dbf75 to 004dbf79 has its CatchHandler @ 004dc773 */
      QTextStream::operator<<((QTextStream *)pQVar9,(QString *)&local_90);
      if (*(int *)local_90 == 0) {
LAB_004dc043:
        QArrayData::deallocate(local_90,2,8);
      }
      else if (*(int *)local_90 != -1) {
        LOCK();
        *(int *)local_90 = *(int *)local_90 + -1;
        UNLOCK();
        if (*(int *)local_90 == 0) goto LAB_004dc043;
      }
      if (*(QTextStream *)(local_98 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 004dc479 to 004dc47d has its CatchHandler @ 004dc7df */
        QTextStream::operator<<((QTextStream *)local_98,' ');
      }
      QDebug::~QDebug((QDebug *)&local_98);
    }
                    /* try { // try from 004dbfb4 to 004dc0b3 has its CatchHandler @ 004dc74f */
    lVar15 = _41000();
    if (*(char *)(lVar15 + 0x11) != '\0') {
      lVar15 = _41000();
      local_70 = *(undefined8 *)(lVar15 + 8);
      local_80 = (undefined  [16])0x0;
      local_88 = 9.88131291682493e-324;
      QMessageLogger::warning();
      QDebug::~QDebug((QDebug *)&local_90);
    }
  }
  lVar15 = *(long *)(this + 0x28);
  this_01 = (KisBSpline2D *)operator_new(0x20);
                    /* try { // try from 004db3a8 to 004db3ac has its CatchHandler @ 004dc767 */
  KisBSplines::KisBSpline2D::KisBSpline2D
            (this_01,(float)iVar11,(float)(iVar11 + iVar16),iVar20,4,(float)iVar17,
             (float)(iVar17 + iVar18),iVar12,4);
  pKVar5 = *(KisBSpline2D **)(lVar15 + 0x28);
  if ((this_01 != pKVar5) &&
     (*(KisBSpline2D **)(lVar15 + 0x28) = this_01, pKVar5 != (KisBSpline2D *)0x0)) {
    KisBSplines::KisBSpline2D::~KisBSpline2D(pKVar5);
    operator_delete(pKVar5,0x20);
  }
  pcStack_50 = (code *)0x0;
  local_68 = (undefined  [16])0x0;
  uVar6 = *(undefined8 *)(*(long *)(this + 0x28) + 0x20);
  local_58 = (code *)0x0;
                    /* try { // try from 004db405 to 004db409 has its CatchHandler @ 004dc77f */
  puVar14 = (undefined8 *)operator_new(0x18);
  puVar14[2] = uVar6;
  *puVar14 = 0x11;
  puVar14[1] = 0;
  local_68._0_8_ = puVar14;
  pKVar5 = *(KisBSpline2D **)(*(long *)(this + 0x28) + 0x28);
  local_58 = FUN_004dc8d0;
  pcStack_50 = FUN_004dc8a0;
  iVar11 = *(int *)(pKVar5 + 0x10);
  fVar1 = *(float *)(pKVar5 + 0xc);
  fVar2 = *(float *)(pKVar5 + 8);
  iVar18 = *(int *)(pKVar5 + 0x1c);
  iVar16 = iVar11 + -1;
  uVar19 = iVar18 * iVar11;
  fVar3 = *(float *)(pKVar5 + 0x18);
  fVar4 = *(float *)(pKVar5 + 0x14);
  if ((int)uVar19 < 1) {
    local_98 = (QArrayData *)PTR_shared_null_008377d0;
  }
  else {
    local_98 = (QArrayData *)QArrayData::allocate(4,8,(long)(int)uVar19,0);
    if (local_98 == (QArrayData *)0x0) {
                    /* try { // try from 0022cbfe to 0022cc02 has its CatchHandler @ 0022cc35 */
      qBadAlloc();
    }
    *(uint *)(local_98 + 4) = uVar19;
    memset(local_98 + *(long *)(local_98 + 0x10),0,(long)(int)uVar19 * 4);
    iVar11 = *(int *)(pKVar5 + 0x10);
  }
  if (0 < iVar11) {
    iVar11 = *(int *)(pKVar5 + 0x1c);
    iVar17 = 0;
    do {
      if (iVar11 < 1) break;
      iVar20 = 0;
      dVar21 = (double)((float)iVar17 * ((fVar1 - fVar2) / (float)iVar16) + *(float *)(pKVar5 + 8));
      do {
        local_90 = (QArrayData *)
                   (double)((float)iVar20 * ((fVar3 - fVar4) / (float)(iVar18 + -1)) +
                           *(float *)(pKVar5 + 0x14));
        local_88 = dVar21;
        if (local_58 == (code *)0x0) {
          if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
                    /* try { // try from 004dc30f to 004dc313 has its CatchHandler @ 004dc7d3 */
            std::__throw_bad_function_call();
          }
          goto LAB_004dc6d2;
        }
                    /* try { // try from 004db5b8 to 004db638 has its CatchHandler @ 004dc7d3 */
        dVar22 = (double)(*pcStack_50)(local_68,&local_88,&local_90);
        iVar10 = *(int *)(pKVar5 + 0x1c) * iVar17 + iVar20;
        if (1 < *(uint *)local_98) {
          if ((*(uint *)(local_98 + 8) & 0x7fffffff) == 0) {
            local_98 = (QArrayData *)QArrayData::allocate(4,8,0,2);
          }
          else {
            FUN_0048cf30(&local_98,*(uint *)(local_98 + 8) & 0x7fffffff,0);
          }
        }
        iVar20 = iVar20 + 1;
        iVar11 = *(int *)(pKVar5 + 0x1c);
        *(float *)(local_98 + *(long *)(local_98 + 0x10) + (long)iVar10 * 4) = (float)dVar22;
      } while (iVar20 < iVar11);
      iVar17 = iVar17 + 1;
    } while (iVar17 < *(int *)(pKVar5 + 0x10));
  }
  KisBSplines::KisBSpline2D::initializeSplineImpl(pKVar5,(QVector *)&local_98);
  if (*(int *)local_98 != 0) {
    if (*(int *)local_98 == -1) goto LAB_004db65c;
    LOCK();
    *(int *)local_98 = *(int *)local_98 + -1;
    UNLOCK();
    if (*(int *)local_98 != 0) goto LAB_004db65c;
  }
  QArrayData::deallocate(local_98,4,8);
LAB_004db65c:
  if (local_58 != (code *)0x0) {
    (*local_58)(local_68,local_68,3);
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_004dc6d2:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



