/* Painting functions extracted from kritagridpaintop
 * SPDX-License-Identifier: GPL-2.0-or-later
 * These are the ACTUAL paintDab/paintLine/paintAt/paintBezierCurve
 * implementations - the core of each brush engine's painting loop.
 */

// ====== paintAt @ 00116986 ======

/* KisGridPaintOp::paintAt(KisPaintInformation const&) [clone .cold] */

void __thiscall KisGridPaintOp::paintAt(KisGridPaintOp *this,KisPaintInformation *param_1)

{
  void *pvVar1;
  KisRandomSubAccessor *this_00;
  long unaff_RBP;
  QVariant *unaff_R12;
  QHash<QString,QVariant> *unaff_R15;
  long in_FS_OFFSET;
  
  QString::~QString((QString *)(*(long *)(unaff_RBP + -0x1e0) + 0x10));
  QVariant::~QVariant(unaff_R12);
  QString::~QString(*(QString **)(unaff_RBP + -0x1c0));
  QVariant::~QVariant(*(QVariant **)(unaff_RBP + -0x1d0));
  QHash<QString,QVariant>::~QHash(unaff_R15);
  if (*(long *)(unaff_RBP + -0x1c8) != 0) {
    pvVar1 = *(void **)(*(long *)(unaff_RBP + -0x1c8) + 0x18);
    if (pvVar1 != (void *)0x0) {
      operator_delete__(pvVar1);
    }
    this_00 = *(KisRandomSubAccessor **)(*(long *)(unaff_RBP + -0x1c8) + 0x10);
    if (this_00 != (KisRandomSubAccessor *)0x0) {
      LOCK();
      *(int *)this_00 = *(int *)this_00 + -1;
      UNLOCK();
      if (*(int *)this_00 == 0) {
        KisRandomSubAccessor::~KisRandomSubAccessor(this_00);
        operator_delete(this_00,0x30);
      }
    }
    param_1 = (KisPaintInformation *)0x20;
    operator_delete(*(void **)(unaff_RBP + -0x1c8),0x20);
  }
  QMap<QString,QVariant>::~QMap((QMap<QString,QVariant> *)(unaff_RBP + -0x48));
  KisSharedPtr<KisRandomSource>::deref
            (*(KisSharedPtr **)(unaff_RBP + -0x128),(KisRandomSource *)param_1);
  if (*(long *)(unaff_RBP + -0x38) == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    _Unwind_Resume();
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}


// ====== paintAt @ 00124740 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisGridPaintOp::paintAt(KisPaintInformation const&) */

KisPaintInformation * KisGridPaintOp::paintAt(KisPaintInformation *param_1)

{
  char cVar1;
  undefined4 uVar2;
  code *pcVar3;
  _func_void_Node_ptr_void_ptr ****pppp_Var4;
  long lVar5;
  long lVar6;
  ulong uVar7;
  long lVar8;
  QRect *pQVar9;
  double dVar10;
  double dVar11;
  char cVar12;
  int iVar13;
  int iVar14;
  uint uVar15;
  uint uVar16;
  double *pdVar17;
  long lVar18;
  undefined8 *puVar19;
  long *plVar20;
  void *pvVar21;
  undefined8 uVar22;
  QString *pQVar23;
  long *plVar24;
  _func_void_Node_ptr_void_ptr ****pppp_Var25;
  _func_void_Node_ptr_void_ptr *****ppppp_Var26;
  QMapData *pQVar27;
  ulong *puVar28;
  _func_void_Node_ptr_void_ptr *****ppppp_Var29;
  QMapData *pQVar30;
  long in_RSI;
  QArrayData *pQVar31;
  uchar *in_R8;
  KisRandomSubAccessor *pKVar32;
  undefined this [8];
  _func_void_Node_ptr_void_ptr *****ppppp_Var33;
  QVariant *pQVar34;
  _func_void_Node_ptr_void_ptr *****ppppp_Var35;
  long in_FS_OFFSET;
  ushort in_FPUStatusWord;
  double dVar36;
  double dVar37;
  double dVar38;
  double dVar39;
  double dVar40;
  double dVar41;
  double dVar42;
  double dVar43;
  double local_240;
  int local_1dc;
  undefined8 *local_1d0;
  double local_1a0;
  double local_198;
  KisRandomSubAccessor *local_158;
  double dStack_150;
  double local_148;
  double dStack_140;
  KisRandomSource *local_130;
  _func_void_Node_ptr_void_ptr ****local_128;
  QArrayData *local_120;
  KoColor local_118 [16];
  long *local_108;
  double dStack_100;
  undefined local_f8 [8];
  double dStack_f0;
  long *local_e8;
  double dStack_e0;
  double local_d8;
  double dStack_d0;
  KisRandomSubAccessor *local_c8;
  double dStack_c0;
  double local_b8;
  double dStack_b0;
  undefined2 local_9c;
  undefined2 local_9a;
  uchar *local_98;
  long local_90;
  undefined8 local_88;
  uchar local_80 [40];
  undefined local_58;
  QMapData *local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisPaintInformation::randomSource();
                    /* try { // try from 0012478a to 001247a0 has its CatchHandler @ 001275ef */
  KisPaintOp::painter();
  KisPainter::device();
                    /* try { // try from 001247b9 to 001247bd has its CatchHandler @ 001275a7 */
  KisPaintDevice::defaultBounds();
                    /* try { // try from 001247c8 to 001247ca has its CatchHandler @ 00127692 */
  iVar13 = (**(code **)(*(long *)local_c8 + 0x30))();
  dVar11 = DAT_00151630;
  if (iVar13 < 1) {
    local_240 = DAT_00151630;
  }
  else {
    local_240 = DAT_00151630 / (double)(1 << ((byte)iVar13 & 0x1f));
  }
  if (local_c8 != (KisRandomSubAccessor *)0x0) {
    LOCK();
    plVar24 = (long *)((long)local_c8 + 8);
    *(int *)plVar24 = *(int *)plVar24 + -1;
    UNLOCK();
    if (*(int *)plVar24 == 0) {
      (**(code **)(*(long *)local_c8 + 8))();
    }
  }
  if (local_e8 != (long *)0x0) {
    LOCK();
    plVar24 = local_e8 + 2;
    *(int *)plVar24 = *(int *)plVar24 + -1;
    UNLOCK();
    if (*(int *)plVar24 == 0) {
      (**(code **)(*local_e8 + 0x20))();
    }
  }
                    /* try { // try from 00124835 to 00124aa5 has its CatchHandler @ 001275ef */
  (**(code **)(**(long **)(in_RSI + 0x28) + 0x68))();
  dVar39 = *(double *)(in_RSI + 0x78);
  dVar40 = *(double *)(in_RSI + 0x60);
  dVar41 = *(double *)(in_RSI + 0x68);
  iVar13 = *(int *)(in_RSI + 0x70);
  dVar36 = (double)*(int *)(in_RSI + 0x50) * dVar39 * local_240;
  dVar37 = (double)*(int *)(in_RSI + 0x54) * dVar39 * local_240;
  dVar38 = (double)*(int *)(in_RSI + 0x58) * dVar39 * local_240;
  if (*(char *)(in_RSI + 0x74) != '\0') {
                    /* try { // try from 00126eb4 to 00126eb8 has its CatchHandler @ 001275ef */
    dVar42 = (double)KisPaintInformation::pressure();
    dVar39 = *(double *)(in_RSI + 0x78);
    iVar13 = (int)((double)iVar13 * dVar42);
  }
  dVar42 = DAT_00151600;
  dVar39 = (double)iVar13 * dVar39;
  if (dVar39 < 0.0) {
    iVar13 = (int)(dVar39 - dVar11);
    iVar13 = iVar13 + (int)((dVar39 - (double)iVar13) + DAT_00151600);
  }
  else {
    iVar13 = (int)(dVar39 + DAT_00151600);
  }
  pdVar17 = (double *)KisPaintInformation::pos();
  dVar43 = ((*pdVar17 - dVar36 * dVar42) + dVar37 * dVar42) - dVar40;
  lVar18 = KisPaintInformation::pos();
  dVar42 = ((*(double *)(lVar18 + 8) - dVar36 * dVar42) + dVar42 * dVar38) - dVar41;
  dVar39 = dVar43;
  do {
    dVar39 = dVar39 - (dVar39 / dVar37) * dVar37;
  } while ((in_FPUStatusWord & 0x400) != 0);
  if (NAN(dVar39)) {
    fmod(dVar43,dVar37);
  }
  dVar10 = dVar42;
  do {
    dVar10 = dVar10 - (dVar10 / dVar38) * dVar38;
  } while ((in_FPUStatusWord & 0x400) != 0);
  if (NAN(dVar10)) {
    fmod(dVar42,dVar38);
  }
  dStack_e0 = (dVar42 - dVar10) + dVar41;
  local_e8 = (long *)(dVar40 + (dVar43 - dVar39));
  local_d8 = dVar37;
  dStack_d0 = dVar38;
  local_118 = (KoColor  [16])QRectF::toAlignedRect();
  iVar14 = 1;
  if (0 < iVar13) {
    iVar14 = iVar13;
  }
  dVar38 = dVar38 / (double)iVar14;
  dVar37 = dVar37 / (double)iVar14;
  local_c8 = (KisRandomSubAccessor *)0x0;
  dStack_c0 = 0.0;
  local_b8 = 0.0;
  dStack_b0 = 0.0;
  KisPaintOp::painter();
  puVar19 = (undefined8 *)KisPainter::paintColor();
  local_88 = *puVar19;
  local_58 = *(undefined *)(puVar19 + 6);
  local_50 = (QMapData *)puVar19[7];
  if (*(int *)local_50 == 0) {
                    /* try { // try from 00126e36 to 00126e7c has its CatchHandler @ 001275ef */
    pQVar30 = (QMapData *)QMapDataBase::createData();
    local_50 = pQVar30;
    if (*(QMapNode<QString,QVariant> **)(puVar19[7] + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
      uVar22 = QMapNode<QString,QVariant>::copy
                         (*(QMapNode<QString,QVariant> **)(puVar19[7] + 0x10),pQVar30);
      *(undefined8 *)(pQVar30 + 0x10) = uVar22;
      **(ulong **)(local_50 + 0x10) =
           (ulong)((uint)**(ulong **)(local_50 + 0x10) & 3) | (ulong)(local_50 + 8);
      QMapDataBase::recalcMostLeftNode();
    }
  }
  else if (*(int *)local_50 != -1) {
    LOCK();
    *(int *)local_50 = *(int *)local_50 + 1;
    UNLOCK();
    local_50 = (QMapData *)puVar19[7];
  }
  __memcpy_chk(local_80,puVar19 + 1,local_58,0x38);
  local_1d0 = *(undefined8 **)(in_RSI + 0xb0);
  if (local_1d0 != (undefined8 *)0x0) {
                    /* try { // try from 00124b14 to 00124b18 has its CatchHandler @ 001275fb */
    local_1d0 = (undefined8 *)operator_new(0x20);
                    /* try { // try from 00124b34 to 00124b36 has its CatchHandler @ 001275bf */
    (**(code **)(**(long **)(in_RSI + 0xb0) + 0x60))(&local_108);
    plVar24 = local_108;
    local_1d0[2] = 0;
    if (local_108 != (long *)0x0) {
      LOCK();
      *(int *)(local_108 + 2) = *(int *)(local_108 + 2) + 1;
      UNLOCK();
    }
                    /* try { // try from 00124b56 to 00124b7e has its CatchHandler @ 001275b3 */
    plVar20 = (long *)KisPaintDevice::colorSpace();
    lVar18 = *plVar20;
    *local_1d0 = plVar20;
    local_1d0[1] = local_88;
    uVar15 = (**(code **)(lVar18 + 0x30))(plVar20);
    pvVar21 = operator_new__((ulong)uVar15);
    plVar20 = plVar24 + 2;
    local_1d0[3] = pvVar21;
    LOCK();
    *(int *)(plVar24 + 2) = *(int *)(plVar24 + 2) + 1;
    UNLOCK();
                    /* try { // try from 00124b9a to 00124b9e has its CatchHandler @ 00127686 */
    KisPaintDevice::createRandomSubAccessor();
    pKVar32 = (KisRandomSubAccessor *)local_1d0[2];
    this = (undefined  [8])pKVar32;
    if (local_f8 != (undefined  [8])pKVar32) {
      if (local_f8 != (undefined  [8])0x0) {
        LOCK();
        *(int *)local_f8 = *(int *)local_f8 + 1;
        UNLOCK();
        pKVar32 = (KisRandomSubAccessor *)local_1d0[2];
      }
      local_1d0[2] = local_f8;
      this = local_f8;
      if (pKVar32 != (KisRandomSubAccessor *)0x0) {
        LOCK();
        *(int *)pKVar32 = *(int *)pKVar32 + -1;
        UNLOCK();
        if (*(int *)pKVar32 == 0) {
          KisRandomSubAccessor::~KisRandomSubAccessor(pKVar32);
          operator_delete(pKVar32,0x30);
          this = local_f8;
        }
      }
    }
    if (this != (undefined  [8])0x0) {
      LOCK();
      *(int *)this = *(int *)this + -1;
      UNLOCK();
      if (*(int *)this == 0) {
        KisRandomSubAccessor::~KisRandomSubAccessor((KisRandomSubAccessor *)this);
        operator_delete((void *)this,0x30);
      }
    }
    LOCK();
    *(int *)plVar20 = *(int *)plVar20 + -1;
    UNLOCK();
    if (*(int *)plVar20 == 0) {
      (**(code **)(*plVar24 + 0x20))(plVar24);
    }
    LOCK();
    *(int *)plVar20 = *(int *)plVar20 + -1;
    UNLOCK();
    if (*(int *)plVar20 == 0) {
      (**(code **)(*plVar24 + 0x20))(plVar24);
    }
    if (local_108 != (long *)0x0) {
      LOCK();
      plVar24 = local_108 + 2;
      *(int *)plVar24 = *(int *)plVar24 + -1;
      UNLOCK();
      if (*(int *)plVar24 == 0) {
        (**(code **)(*local_108 + 0x20))();
      }
    }
  }
  local_198 = *(double *)(in_RSI + 0x80) * local_240;
  local_1a0 = local_240 * *(double *)(in_RSI + 0x88);
  if (*(char *)(in_RSI + 0x90) != '\0') {
    if (local_198 == local_1a0) {
                    /* try { // try from 00124c6c to 00124f81 has its CatchHandler @ 001275e3 */
      local_1a0 = (double)KisRandomSource::generateNormalized();
      local_1a0 = local_1a0 * local_198;
      local_198 = local_1a0;
    }
    else {
                    /* try { // try from 00126edb to 00126efb has its CatchHandler @ 001275e3 */
      dVar40 = (double)KisRandomSource::generateNormalized();
      local_198 = dVar40 * local_198;
      dVar40 = (double)KisRandomSource::generateNormalized();
      local_1a0 = dVar40 * local_1a0;
    }
  }
  if (*(char *)(in_RSI + 0x9b) != '\0') {
    pQVar9 = *(QRect **)(in_RSI + 0x28);
                    /* try { // try from 00126e89 to 00126ea7 has its CatchHandler @ 001275e3 */
    KisPaintOp::painter();
    KisPainter::backgroundColor();
    KisPaintDevice::fill(pQVar9,local_118);
  }
  dVar40 = (double)(DAT_00151620 ^ (ulong)local_198);
  dVar39 = (double)(DAT_00151620 ^ (ulong)local_1a0);
  dVar41 = 0.0;
  if (0.0 < dVar36 / dVar38) {
    cVar12 = '\x01';
    local_1dc = 0;
    do {
      if (0.0 < dVar36 / dVar37) {
        iVar13 = 0;
        dVar42 = 0.0;
        do {
          local_c8 = (KisRandomSubAccessor *)(dVar42 * dVar37 + (double)local_e8 + local_198);
          dStack_c0 = dVar41 * dVar38 + dStack_e0 + local_1a0;
          local_b8 = (dVar40 - local_198) + dVar37;
          dStack_b0 = (dVar39 - local_1a0) + dVar38;
          QRectF::normalized();
          local_c8 = local_158;
          dStack_c0 = dStack_150;
          local_b8 = local_148;
          dStack_b0 = dStack_140;
          if (cVar12 != '\0') {
            if ((local_1d0 != (undefined8 *)0x0) && (*(char *)(in_RSI + 0x9a) != '\0')) {
              pKVar32 = (KisRandomSubAccessor *)local_1d0[2];
              dVar42 = DAT_00151600 * dStack_140;
              *(double *)(pKVar32 + 0x18) = DAT_00151600 * local_148 + (double)local_158;
              *(double *)(pKVar32 + 0x20) = dVar42 + dStack_150;
              LOCK();
              *(int *)pKVar32 = *(int *)pKVar32 + 1;
              UNLOCK();
                    /* try { // try from 0012597a to 0012597e has its CatchHandler @ 00127632 */
              KisRandomSubAccessor::sampledOldRawData((uchar *)pKVar32);
              LOCK();
              *(int *)pKVar32 = *(int *)pKVar32 + -1;
              UNLOCK();
              if (*(int *)pKVar32 == 0) {
                KisRandomSubAccessor::~KisRandomSubAccessor(pKVar32);
                operator_delete(pKVar32,0x30);
              }
              in_R8 = (uchar *)0x0;
                    /* try { // try from 001259c0 to 001259c1 has its CatchHandler @ 00127626 */
              (**(code **)(*(long *)*local_1d0 + 0x110))
                        ((long *)*local_1d0,local_1d0[3],local_80,local_1d0[1],1,0,0x2000);
            }
            if (*(char *)(in_RSI + 0x9d) != '\0') {
                    /* try { // try from 00125405 to 00125409 has its CatchHandler @ 001275e3 */
              KisPaintOp::source();
                    /* try { // try from 00125411 to 00125421 has its CatchHandler @ 001276aa */
              plVar24 = (long *)KisPaintDevice::colorSpace();
              plVar24 = (long *)(**(code **)(*plVar24 + 0x1e0))(plVar24);
              if (local_f8 != (undefined  [8])0x0) {
                LOCK();
                pKVar32 = (KisRandomSubAccessor *)((long)local_f8 + 0x10);
                *(int *)pKVar32 = *(int *)pKVar32 + -1;
                UNLOCK();
                if (*(int *)pKVar32 == 0) {
                  (**(code **)(*(long *)local_f8 + 0x20))();
                }
              }
              local_98 = local_80;
                    /* try { // try from 00125451 to 001254d0 has its CatchHandler @ 001275e3 */
              KisPaintOp::painter();
              local_90 = KisPainter::backgroundColor();
              local_90 = local_90 + 8;
              dVar42 = (double)KisPaintInformation::pressure();
              local_9c = (undefined2)(int)(DAT_00151608 * dVar42);
              local_9a = (undefined2)(int)((dVar11 - dVar42) * DAT_00151608);
              in_R8 = local_80;
              (**(code **)(*plVar24 + 0x18))(plVar24,&local_98,&local_9c,2,local_80);
            }
            if (*(char *)(in_RSI + 0x98) != '\0') {
              dVar42 = (double)*(int *)(in_RSI + 0xa0) / _DAT_00151610;
              local_128 = (_func_void_Node_ptr_void_ptr ****)PTR_shared_null_00166f78;
                    /* try { // try from 001252f8 to 0012531a has its CatchHandler @ 001276c9 */
              dVar43 = (double)KisRandomSource::generateNormalized();
              QVariant::QVariant((QVariant *)&local_108,dVar43 * dVar42);
                    /* try { // try from 00125327 to 0012532b has its CatchHandler @ 001276b6 */
              local_120 = (QArrayData *)QString::fromAscii_helper("h",1);
              ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)local_128;
              if (1 < *(uint *)(local_128 + 2)) {
                in_R8 = (uchar *)0x0;
                    /* try { // try from 00125c61 to 00125c65 has its CatchHandler @ 0012767a */
                ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)
                              QHashData::detach_helper
                                        ((_func_void_Node_ptr_void_ptr *)local_128,
                                         QHash<QString,QVariant>::duplicateNode,0x127750,0x28);
                ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)(local_128 + 2);
                if (*(int *)(local_128 + 2) == 0) {
LAB_00126a48:
                    /* try { // try from 00126a60 to 00126a80 has its CatchHandler @ 0012767a */
                  QHashData::free_helper((_func_void_Node_ptr *)local_128);
                }
                else if (*(int *)(local_128 + 2) != -1) {
                  LOCK();
                  *(int *)ppppp_Var33 = *(int *)ppppp_Var33 + -1;
                  UNLOCK();
                  if (*(int *)ppppp_Var33 == 0) goto LAB_00126a48;
                }
              }
              local_128 = (_func_void_Node_ptr_void_ptr ****)ppppp_Var26;
              ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)local_128;
              uVar16 = qHash((QString *)&local_120,*(uint *)((long)local_128 + 0x24));
              uVar15 = *(uint *)((_func_void_Node_ptr_void_ptr ****)ppppp_Var26 + 4);
              if (uVar15 == 0) {
                ppppp_Var33 = &local_128;
LAB_00125b2a:
                uVar15 = *(uint *)(ppppp_Var26 + 4);
              }
              else {
                ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)
                              (((_func_void_Node_ptr_void_ptr ****)ppppp_Var26)[1] +
                              (ulong)uVar16 % (ulong)uVar15);
                ppppp_Var29 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var35;
                ppppp_Var33 = ppppp_Var35;
                if ((_func_void_Node_ptr_void_ptr *****)*ppppp_Var35 != ppppp_Var26) {
                  do {
                    while (ppppp_Var33 = ppppp_Var29, uVar16 != *(uint *)(ppppp_Var33 + 1)) {
                      ppppp_Var29 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var33;
                      ppppp_Var35 = ppppp_Var33;
                      if ((_func_void_Node_ptr_void_ptr *****)*ppppp_Var33 == ppppp_Var26)
                      goto LAB_00125b2a;
                    }
                    cVar12 = operator==((QString *)&local_120,(QString *)(ppppp_Var33 + 2));
                    ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)local_128;
                    if (cVar12 != '\0') {
                      ppppp_Var33 = ppppp_Var35;
                      if (*ppppp_Var35 != local_128) {
                        pQVar34 = (QVariant *)(*ppppp_Var35 + 3);
                        goto LAB_00125505;
                      }
                      break;
                    }
                    ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var35;
                    ppppp_Var29 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var33;
                    ppppp_Var35 = ppppp_Var33;
                  } while (*ppppp_Var33 != local_128);
                  goto LAB_00125b2a;
                }
              }
              if ((int)uVar15 <= *(int *)((long)ppppp_Var26 + 0x14)) {
                QHashData::rehash((int)ppppp_Var26);
                ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)local_128;
                ppppp_Var33 = &local_128;
                if (*(uint *)(local_128 + 4) != 0) {
                  ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)
                                (local_128[1] + (ulong)uVar16 % (ulong)*(uint *)(local_128 + 4));
                  ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var33;
                  if (local_128 != *ppppp_Var33) {
                    do {
                      ppppp_Var29 = ppppp_Var35;
                      if (uVar16 == *(uint *)(ppppp_Var29 + 1)) {
                        cVar12 = operator==((QString *)&local_120,(QString *)(ppppp_Var29 + 2));
                        ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)local_128;
                        if (cVar12 != '\0') break;
                        ppppp_Var29 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var33;
                      }
                      ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var29;
                      ppppp_Var33 = ppppp_Var29;
                    } while ((_func_void_Node_ptr_void_ptr *****)*ppppp_Var29 != ppppp_Var26);
                  }
                }
              }
              local_f8 = (undefined  [8])0x0;
              dStack_f0 = 1.06099789548264e-314;
                    /* try { // try from 00125b59 to 00125b5d has its CatchHandler @ 00127581 */
              pppp_Var25 = (_func_void_Node_ptr_void_ptr ****)
                           QHashData::allocateNode((int)ppppp_Var26);
              pppp_Var4 = *ppppp_Var33;
              *(uint *)(pppp_Var25 + 1) = uVar16;
              pppp_Var25[2] = (_func_void_Node_ptr_void_ptr ***)local_120;
              *pppp_Var25 = (_func_void_Node_ptr_void_ptr ***)pppp_Var4;
              if (1 < *(int *)local_120 + 1U) {
                LOCK();
                *(int *)local_120 = *(int *)local_120 + 1;
                UNLOCK();
              }
              pQVar34 = (QVariant *)(pppp_Var25 + 3);
                    /* try { // try from 00125ba3 to 00125ba7 has its CatchHandler @ 00127569 */
              QVariant::QVariant(pQVar34,(QVariant *)local_f8);
              *ppppp_Var33 = pppp_Var25;
              *(int *)((long)local_128 + 0x14) = *(int *)((long)local_128 + 0x14) + 1;
              QVariant::~QVariant((QVariant *)local_f8);
LAB_00125505:
              local_f8 = *(undefined (*) [8])pQVar34;
              uVar2 = *(undefined4 *)(pQVar34 + 8);
              *(long **)pQVar34 = local_108;
              *(undefined4 *)(pQVar34 + 8) = dStack_100._0_4_;
              dStack_f0 = (double)CONCAT44(dStack_f0._4_4_,uVar2);
              dStack_100 = (double)CONCAT44(dStack_100._4_4_,uVar2);
              local_108 = (long *)local_f8;
              if (*(int *)local_120 == 0) {
LAB_00125c00:
                QArrayData::deallocate(local_120,2,8);
              }
              else if (*(int *)local_120 != -1) {
                LOCK();
                *(int *)local_120 = *(int *)local_120 + -1;
                UNLOCK();
                if (*(int *)local_120 == 0) goto LAB_00125c00;
              }
              QVariant::~QVariant((QVariant *)&local_108);
              dVar42 = (double)*(int *)(in_RSI + 0xa4) / DAT_00151618;
                    /* try { // try from 0012559c to 001255b0 has its CatchHandler @ 001276c9 */
              dVar43 = (double)KisRandomSource::generateNormalized();
              QVariant::QVariant((QVariant *)&local_108,dVar43 * dVar42);
                    /* try { // try from 001255bd to 001255c1 has its CatchHandler @ 0012769e */
              local_120 = (QArrayData *)QString::fromAscii_helper("s",1);
              ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)local_128;
              if (1 < *(uint *)(local_128 + 2)) {
                in_R8 = (uchar *)0x0;
                    /* try { // try from 00125d24 to 00125d28 has its CatchHandler @ 0012755d */
                ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)
                              QHashData::detach_helper
                                        ((_func_void_Node_ptr_void_ptr *)local_128,
                                         QHash<QString,QVariant>::duplicateNode,0x127750,0x28);
                ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)(local_128 + 2);
                if (*(int *)(local_128 + 2) == 0) {
LAB_00126a28:
                    /* try { // try from 00126a2b to 00126a2f has its CatchHandler @ 0012755d */
                  QHashData::free_helper((_func_void_Node_ptr *)local_128);
                }
                else if (*(int *)(local_128 + 2) != -1) {
                  LOCK();
                  *(int *)ppppp_Var33 = *(int *)ppppp_Var33 + -1;
                  UNLOCK();
                  if (*(int *)ppppp_Var33 == 0) goto LAB_00126a28;
                }
              }
              local_128 = (_func_void_Node_ptr_void_ptr ****)ppppp_Var26;
              ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)local_128;
              uVar16 = qHash((QString *)&local_120,*(uint *)((long)local_128 + 0x24));
              uVar15 = *(uint *)((_func_void_Node_ptr_void_ptr ****)ppppp_Var33 + 4);
              ppppp_Var26 = &local_128;
              if (uVar15 == 0) {
LAB_00125a7b:
                uVar15 = *(uint *)(ppppp_Var33 + 4);
              }
              else {
                ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)
                              (((_func_void_Node_ptr_void_ptr ****)ppppp_Var33)[1] +
                              (ulong)uVar16 % (ulong)uVar15);
                ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26;
                if (ppppp_Var33 != (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26) {
                  do {
                    ppppp_Var29 = ppppp_Var35;
                    if (uVar16 == *(uint *)(ppppp_Var29 + 1)) {
                      cVar12 = operator==((QString *)&local_120,(QString *)(ppppp_Var29 + 2));
                      ppppp_Var29 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26;
                      ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)local_128;
                      if (cVar12 != '\0') {
                        if ((_func_void_Node_ptr_void_ptr *****)local_128 != ppppp_Var29) {
                          pQVar34 = (QVariant *)(ppppp_Var29 + 3);
                          goto LAB_00125660;
                        }
                        break;
                      }
                    }
                    ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var29;
                    ppppp_Var26 = ppppp_Var29;
                  } while (ppppp_Var33 != (_func_void_Node_ptr_void_ptr *****)*ppppp_Var29);
                  goto LAB_00125a7b;
                }
              }
              if ((int)uVar15 <= *(int *)((long)ppppp_Var33 + 0x14)) {
                    /* try { // try from 00126b8c to 00126b90 has its CatchHandler @ 0012755d */
                QHashData::rehash((int)ppppp_Var33);
                ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)local_128;
                ppppp_Var26 = &local_128;
                if (*(uint *)(local_128 + 4) != 0) {
                  ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)
                                (local_128[1] + (ulong)uVar16 % (ulong)*(uint *)(local_128 + 4));
                  ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26;
                  if (local_128 != *ppppp_Var26) {
                    do {
                      ppppp_Var29 = ppppp_Var35;
                      if (uVar16 == *(uint *)(ppppp_Var29 + 1)) {
                        cVar12 = operator==((QString *)&local_120,(QString *)(ppppp_Var29 + 2));
                        ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)local_128;
                        if (cVar12 != '\0') break;
                        ppppp_Var29 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26;
                      }
                      ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var29;
                      ppppp_Var26 = ppppp_Var29;
                    } while (ppppp_Var33 != (_func_void_Node_ptr_void_ptr *****)*ppppp_Var29);
                  }
                }
              }
              local_f8 = (undefined  [8])0x0;
              dStack_f0 = 1.06099789548264e-314;
                    /* try { // try from 00125aaa to 00125aae has its CatchHandler @ 00127594 */
              pppp_Var25 = (_func_void_Node_ptr_void_ptr ****)
                           QHashData::allocateNode((int)ppppp_Var33);
              pppp_Var4 = *ppppp_Var26;
              *(uint *)(pppp_Var25 + 1) = uVar16;
              pppp_Var25[2] = (_func_void_Node_ptr_void_ptr ***)local_120;
              *pppp_Var25 = (_func_void_Node_ptr_void_ptr ***)pppp_Var4;
              if (1 < *(int *)local_120 + 1U) {
                LOCK();
                *(int *)local_120 = *(int *)local_120 + 1;
                UNLOCK();
              }
              pQVar34 = (QVariant *)(pppp_Var25 + 3);
                    /* try { // try from 00125af4 to 00125af8 has its CatchHandler @ 00127575 */
              QVariant::QVariant(pQVar34,(QVariant *)local_f8);
              *ppppp_Var26 = pppp_Var25;
              *(int *)((long)local_128 + 0x14) = *(int *)((long)local_128 + 0x14) + 1;
              QVariant::~QVariant((QVariant *)local_f8);
LAB_00125660:
              local_f8 = *(undefined (*) [8])pQVar34;
              uVar2 = *(undefined4 *)(pQVar34 + 8);
              *(long **)pQVar34 = local_108;
              *(undefined4 *)(pQVar34 + 8) = dStack_100._0_4_;
              dStack_f0 = (double)CONCAT44(dStack_f0._4_4_,uVar2);
              dStack_100 = (double)CONCAT44(dStack_100._4_4_,uVar2);
              local_108 = (long *)local_f8;
              if (*(int *)local_120 == 0) {
LAB_00125be8:
                QArrayData::deallocate(local_120,2,8);
              }
              else if (*(int *)local_120 != -1) {
                LOCK();
                *(int *)local_120 = *(int *)local_120 + -1;
                UNLOCK();
                if (*(int *)local_120 == 0) goto LAB_00125be8;
              }
              QVariant::~QVariant((QVariant *)&local_108);
              dVar42 = (double)*(int *)(in_RSI + 0xa8) / DAT_00151618;
                    /* try { // try from 001256f7 to 0012570b has its CatchHandler @ 001276c9 */
              dVar43 = (double)KisRandomSource::generateNormalized();
              QVariant::QVariant((QVariant *)&local_108,dVar43 * dVar42);
                    /* try { // try from 00125718 to 0012571c has its CatchHandler @ 00127662 */
              local_120 = (QArrayData *)QString::fromAscii_helper("v",1);
              ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)local_128;
              if (1 < *(uint *)(local_128 + 2)) {
                in_R8 = (uchar *)0x0;
                    /* try { // try from 00125cc4 to 00125cc8 has its CatchHandler @ 0012766e */
                ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)
                              QHashData::detach_helper
                                        ((_func_void_Node_ptr_void_ptr *)local_128,
                                         QHash<QString,QVariant>::duplicateNode,0x127750,0x28);
                ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)(local_128 + 2);
                if (*(int *)(local_128 + 2) == 0) {
LAB_00126a38:
                    /* try { // try from 00126a3b to 00126a3f has its CatchHandler @ 0012766e */
                  QHashData::free_helper((_func_void_Node_ptr *)local_128);
                }
                else if (*(int *)(local_128 + 2) != -1) {
                  LOCK();
                  *(int *)ppppp_Var33 = *(int *)ppppp_Var33 + -1;
                  UNLOCK();
                  if (*(int *)ppppp_Var33 == 0) goto LAB_00126a38;
                }
              }
              local_128 = (_func_void_Node_ptr_void_ptr ****)ppppp_Var26;
              ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)local_128;
              uVar16 = qHash((QString *)&local_120,*(uint *)((long)local_128 + 0x24));
              uVar15 = *(uint *)((_func_void_Node_ptr_void_ptr ****)ppppp_Var33 + 4);
              ppppp_Var26 = &local_128;
              if (uVar15 == 0) {
LAB_001259d3:
                uVar15 = *(uint *)(ppppp_Var33 + 4);
              }
              else {
                ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)
                              (((_func_void_Node_ptr_void_ptr ****)ppppp_Var33)[1] +
                              (ulong)uVar16 % (ulong)uVar15);
                ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26;
                if (ppppp_Var33 != (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26) {
                  do {
                    ppppp_Var29 = ppppp_Var35;
                    if (uVar16 == *(uint *)(ppppp_Var29 + 1)) {
                      cVar12 = operator==((QString *)&local_120,(QString *)(ppppp_Var29 + 2));
                      ppppp_Var29 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26;
                      ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)local_128;
                      if (cVar12 != '\0') {
                        if (ppppp_Var29 != (_func_void_Node_ptr_void_ptr *****)local_128) {
                          pQVar34 = (QVariant *)(ppppp_Var29 + 3);
                          goto LAB_001257b8;
                        }
                        break;
                      }
                    }
                    ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var29;
                    ppppp_Var26 = ppppp_Var29;
                  } while ((_func_void_Node_ptr_void_ptr *****)*ppppp_Var29 != ppppp_Var33);
                  goto LAB_001259d3;
                }
              }
              if ((int)uVar15 <= *(int *)((long)ppppp_Var33 + 0x14)) {
                    /* try { // try from 00126b04 to 00126b08 has its CatchHandler @ 0012766e */
                QHashData::rehash((int)ppppp_Var33);
                ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)local_128;
                ppppp_Var26 = &local_128;
                if (*(uint *)(local_128 + 4) != 0) {
                  ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)
                                (local_128[1] + (ulong)uVar16 % (ulong)*(uint *)(local_128 + 4));
                  ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26;
                  if (local_128 != *ppppp_Var26) {
                    do {
                      ppppp_Var29 = ppppp_Var35;
                      if (uVar16 == *(uint *)(ppppp_Var29 + 1)) {
                        cVar12 = operator==((QString *)&local_120,(QString *)(ppppp_Var29 + 2));
                        ppppp_Var33 = (_func_void_Node_ptr_void_ptr *****)local_128;
                        if (cVar12 != '\0') break;
                        ppppp_Var29 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var26;
                      }
                      ppppp_Var35 = (_func_void_Node_ptr_void_ptr *****)*ppppp_Var29;
                      ppppp_Var26 = ppppp_Var29;
                    } while ((_func_void_Node_ptr_void_ptr *****)*ppppp_Var29 != ppppp_Var33);
                  }
                }
              }
              local_f8 = (undefined  [8])0x0;
              dStack_f0 = 1.06099789548264e-314;
                    /* try { // try from 00125a02 to 00125a06 has its CatchHandler @ 00127613 */
              pppp_Var25 = (_func_void_Node_ptr_void_ptr ****)
                           QHashData::allocateNode((int)ppppp_Var33);
              pppp_Var4 = *ppppp_Var26;
              *(uint *)(pppp_Var25 + 1) = uVar16;
              pppp_Var25[2] = (_func_void_Node_ptr_void_ptr ***)local_120;
              *pppp_Var25 = (_func_void_Node_ptr_void_ptr ***)pppp_Var4;
              if (1 < *(int *)local_120 + 1U) {
                LOCK();
                *(int *)local_120 = *(int *)local_120 + 1;
                UNLOCK();
              }
              pQVar34 = (QVariant *)(pppp_Var25 + 3);
                    /* try { // try from 00125a4c to 00125a50 has its CatchHandler @ 00127607 */
              QVariant::QVariant(pQVar34,(QVariant *)local_f8);
              *ppppp_Var26 = pppp_Var25;
              *(int *)((long)local_128 + 0x14) = *(int *)((long)local_128 + 0x14) + 1;
              QVariant::~QVariant((QVariant *)local_f8);
LAB_001257b8:
              local_f8 = *(undefined (*) [8])pQVar34;
              uVar2 = *(undefined4 *)(pQVar34 + 8);
              *(long **)pQVar34 = local_108;
              *(undefined4 *)(pQVar34 + 8) = dStack_100._0_4_;
              dStack_f0 = (double)CONCAT44(dStack_f0._4_4_,uVar2);
              dStack_100 = (double)CONCAT44(dStack_100._4_4_,uVar2);
              local_108 = (long *)local_f8;
              if (*(int *)local_120 == 0) {
LAB_00125c18:
                QArrayData::deallocate(local_120,2,8);
              }
              else if (*(int *)local_120 != -1) {
                LOCK();
                *(int *)local_120 = *(int *)local_120 + -1;
                UNLOCK();
                if (*(int *)local_120 == 0) goto LAB_00125c18;
              }
              QVariant::~QVariant((QVariant *)&local_108);
                    /* try { // try from 0012582d to 00125845 has its CatchHandler @ 001276c9 */
              pQVar23 = (QString *)KisPaintDevice::colorSpace();
              local_f8 = (undefined  [8])QString::fromAscii_helper("hsv_adjustment",0xe);
                    /* try { // try from 0012585d to 00125861 has its CatchHandler @ 0012764a */
              plVar24 = (long *)KoColorSpace::createColorTransformation(pQVar23,(QHash *)local_f8);
              if (*(int *)local_f8 == 0) {
LAB_00125bd0:
                QArrayData::deallocate((QArrayData *)local_f8,2,8);
              }
              else if (*(int *)local_f8 != -1) {
                LOCK();
                *(int *)local_f8 = *(int *)local_f8 + -1;
                UNLOCK();
                if (*(int *)local_f8 == 0) goto LAB_00125bd0;
              }
              pcVar3 = *(code **)(*plVar24 + 0x28);
                    /* try { // try from 0012589c to 001258a0 has its CatchHandler @ 001276c9 */
              QVariant::QVariant((QVariant *)local_f8,1);
                    /* try { // try from 001258ac to 001258ae has its CatchHandler @ 00127656 */
              (*pcVar3)(plVar24,3,(QVariant *)local_f8);
              QVariant::~QVariant((QVariant *)local_f8);
              pcVar3 = *(code **)(*plVar24 + 0x28);
                    /* try { // try from 001258c4 to 001258c8 has its CatchHandler @ 001276c9 */
              QVariant::QVariant((QVariant *)local_f8,false);
                    /* try { // try from 001258d4 to 001258d6 has its CatchHandler @ 0012763e */
              (*pcVar3)(plVar24,4,(QVariant *)local_f8);
              QVariant::~QVariant((QVariant *)local_f8);
                    /* try { // try from 001258f5 to 001258f7 has its CatchHandler @ 001276c9 */
              (**(code **)(*plVar24 + 0x10))(plVar24,local_80,local_80,1);
              ppppp_Var26 = (_func_void_Node_ptr_void_ptr *****)(local_128 + 2);
              if (*(int *)(local_128 + 2) == 0) {
LAB_00125928:
                QHashData::free_helper((_func_void_Node_ptr *)local_128);
              }
              else if (*(int *)(local_128 + 2) != -1) {
                LOCK();
                *(int *)ppppp_Var26 = *(int *)ppppp_Var26 + -1;
                UNLOCK();
                if (*(int *)ppppp_Var26 == 0) goto LAB_00125928;
              }
            }
            if (*(char *)(in_RSI + 0x99) != '\0') {
              dVar42 = (double)KisRandomSource::generateNormalized();
              KoColor::setOpacity(dVar42);
              KisPainter::setOpacityF(dVar42);
            }
            cVar12 = *(char *)(in_RSI + 0x9c);
            KisPainter::setPaintColor(*(KoColor **)(in_RSI + 0x30));
          }
          switch(*(undefined4 *)(in_RSI + 0xac)) {
          case 0:
            KisPainter::paintEllipse(*(QRectF **)(in_RSI + 0x30));
            break;
          case 1:
            in_R8 = (uchar *)CONCAT44((int)((ulong)in_R8 >> 0x20),(int)dStack_b0);
            KisPaintDevice::fill
                      ((int)*(undefined8 *)(in_RSI + 0x28),(int)(double)local_c8,(int)dStack_c0,
                       (int)local_b8,in_R8);
            break;
          case 2:
            local_108 = (long *)((double)local_c8 + local_b8);
            dStack_f0 = dStack_b0 + dStack_c0;
            dStack_100 = dStack_c0;
            local_f8 = (undefined  [8])local_c8;
                    /* try { // try from 0012515b to 001252ba has its CatchHandler @ 001275e3 */
            KisPainter::drawDDALine(*(QPointF **)(in_RSI + 0x30),(QPointF *)&local_108);
            cVar1 = *(char *)(in_RSI + 0x9c);
            goto joined_r0x00125167;
          case 3:
            local_108 = (long *)((double)local_c8 + local_b8);
            dStack_f0 = dStack_b0 + dStack_c0;
            dStack_100 = dStack_c0;
            local_f8 = (undefined  [8])local_c8;
            KisPainter::drawLine(*(QPointF **)(in_RSI + 0x30),(QPointF *)&local_108);
            break;
          case 4:
            in_R8 = (uchar *)0x0;
            local_108 = (long *)((double)local_c8 + local_b8);
            dStack_f0 = dStack_b0 + dStack_c0;
            dStack_100 = dStack_c0;
            local_f8 = (undefined  [8])local_c8;
            KisPainter::drawThickLine
                      (*(QPointF **)(in_RSI + 0x30),(QPointF *)&local_108,(int)local_f8,1);
          }
          cVar1 = *(char *)(in_RSI + 0x9c);
joined_r0x00125167:
          if (cVar1 != '\0') {
            KisPaintOp::painter();
            puVar19 = (undefined8 *)KisPainter::paintColor();
            if (puVar19 != &local_88) {
              local_88 = *puVar19;
              pQVar30 = (QMapData *)puVar19[7];
              pQVar27 = local_50;
              if (local_50 != pQVar30) {
                if (*(int *)pQVar30 == 0) {
                    /* try { // try from 001269e0 to 00126a1d has its CatchHandler @ 001275e3 */
                  pQVar27 = (QMapData *)QMapDataBase::createData();
                  if (*(QMapNode<QString,QVariant> **)(puVar19[7] + 0x10) !=
                      (QMapNode<QString,QVariant> *)0x0) {
                    puVar28 = (ulong *)QMapNode<QString,QVariant>::copy
                                                 (*(QMapNode<QString,QVariant> **)
                                                   (puVar19[7] + 0x10),pQVar27);
                    uVar7 = *puVar28;
                    *(ulong **)(pQVar27 + 0x10) = puVar28;
                    *puVar28 = (ulong)((uint)uVar7 & 3) | (ulong)(pQVar27 + 8);
                    QMapDataBase::recalcMostLeftNode();
                  }
                }
                else {
                  if (*(int *)pQVar30 != -1) {
                    LOCK();
                    *(int *)pQVar30 = *(int *)pQVar30 + 1;
                    UNLOCK();
                  }
                  pQVar27 = (QMapData *)puVar19[7];
                }
                pQVar30 = local_50;
                if (*(int *)local_50 == 0) {
LAB_00125d68:
                  lVar18 = *(long *)(local_50 + 0x10);
                  local_50 = pQVar27;
                  if (lVar18 != 0) {
                    pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                    if (*(int *)pQVar31 == 0) {
LAB_00126c4e:
                      QArrayData::deallocate(pQVar31,2,8);
                    }
                    else if (*(int *)pQVar31 != -1) {
                      LOCK();
                      *(int *)pQVar31 = *(int *)pQVar31 + -1;
                      UNLOCK();
                      if (*(int *)pQVar31 == 0) {
                        pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                        goto LAB_00126c4e;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar18 + 0x20));
                    lVar8 = *(long *)(lVar18 + 8);
                    if (lVar8 != 0) {
                      pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                      if (*(int *)pQVar31 == 0) {
LAB_00126e22:
                        QArrayData::deallocate(pQVar31,2,8);
                      }
                      else if (*(int *)pQVar31 != -1) {
                        LOCK();
                        *(int *)pQVar31 = *(int *)pQVar31 + -1;
                        UNLOCK();
                        if (*(int *)pQVar31 == 0) {
                          pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                          goto LAB_00126e22;
                        }
                      }
                      QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                      lVar5 = *(long *)(lVar8 + 8);
                      if (lVar5 != 0) {
                        pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                        if (*(int *)pQVar31 == 0) {
LAB_0012713e:
                          QArrayData::deallocate(pQVar31,2,8);
                        }
                        else if (*(int *)pQVar31 != -1) {
                          LOCK();
                          *(int *)pQVar31 = *(int *)pQVar31 + -1;
                          UNLOCK();
                          if (*(int *)pQVar31 == 0) {
                            pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                            goto LAB_0012713e;
                          }
                        }
                        QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                        in_R8 = *(uchar **)(lVar5 + 8);
                        if (in_R8 != (uchar *)0x0) {
                          pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                          if (*(int *)pQVar31 == 0) {
LAB_00126f82:
                            QArrayData::deallocate(pQVar31,2,8);
                          }
                          else if (*(int *)pQVar31 != -1) {
                            LOCK();
                            *(int *)pQVar31 = *(int *)pQVar31 + -1;
                            UNLOCK();
                            if (*(int *)pQVar31 == 0) {
                              pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                              goto LAB_00126f82;
                            }
                          }
                          QVariant::~QVariant((QVariant *)(in_R8 + 0x20));
                          lVar6 = *(long *)(in_R8 + 8);
                          if (lVar6 != 0) {
                            pQVar31 = *(QArrayData **)(lVar6 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_001272d8:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar6 + 0x18);
                                goto LAB_001272d8;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar6 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar6 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar6 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar6 + 0x10));
                            }
                          }
                          in_R8 = *(uchar **)(in_R8 + 0x10);
                          if (in_R8 != (uchar *)0x0) {
                            pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_001272a4:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                                goto LAB_001272a4;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(in_R8 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(in_R8 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(in_R8 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(in_R8 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(in_R8 + 0x10));
                            }
                          }
                        }
                        lVar5 = *(long *)(lVar5 + 0x10);
                        if (lVar5 != 0) {
                          pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                          if (*(int *)pQVar31 == 0) {
LAB_00126fb6:
                            QArrayData::deallocate(pQVar31,2,8);
                          }
                          else if (*(int *)pQVar31 != -1) {
                            LOCK();
                            *(int *)pQVar31 = *(int *)pQVar31 + -1;
                            UNLOCK();
                            if (*(int *)pQVar31 == 0) {
                              pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                              goto LAB_00126fb6;
                            }
                          }
                          QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                          in_R8 = *(uchar **)(lVar5 + 8);
                          if (in_R8 != (uchar *)0x0) {
                            pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_0012718a:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                                goto LAB_0012718a;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(in_R8 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(in_R8 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(in_R8 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(in_R8 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(in_R8 + 0x10));
                            }
                          }
                          lVar5 = *(long *)(lVar5 + 0x10);
                          if (lVar5 != 0) {
                            pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_0012724a:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                                goto LAB_0012724a;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10));
                            }
                          }
                        }
                      }
                      lVar8 = *(long *)(lVar8 + 0x10);
                      if (lVar8 != 0) {
                        pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                        if (*(int *)pQVar31 == 0) {
LAB_00127164:
                          QArrayData::deallocate(pQVar31,2,8);
                        }
                        else if (*(int *)pQVar31 != -1) {
                          LOCK();
                          *(int *)pQVar31 = *(int *)pQVar31 + -1;
                          UNLOCK();
                          if (*(int *)pQVar31 == 0) {
                            pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                            goto LAB_00127164;
                          }
                        }
                        QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                        lVar5 = *(long *)(lVar8 + 8);
                        if (lVar5 != 0) {
                          pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                          if (*(int *)pQVar31 == 0) {
LAB_00126fdc:
                            QArrayData::deallocate(pQVar31,2,8);
                          }
                          else if (*(int *)pQVar31 != -1) {
                            LOCK();
                            *(int *)pQVar31 = *(int *)pQVar31 + -1;
                            UNLOCK();
                            if (*(int *)pQVar31 == 0) {
                              pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                              goto LAB_00126fdc;
                            }
                          }
                          QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                          in_R8 = *(uchar **)(lVar5 + 8);
                          if (in_R8 != (uchar *)0x0) {
                            pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_00127410:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                                goto LAB_00127410;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(in_R8 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(in_R8 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(in_R8 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(in_R8 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(in_R8 + 0x10));
                            }
                          }
                          lVar5 = *(long *)(lVar5 + 0x10);
                          if (lVar5 != 0) {
                            pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_0012731a:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                                goto LAB_0012731a;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10));
                            }
                          }
                        }
                        lVar8 = *(long *)(lVar8 + 0x10);
                        if (lVar8 != 0) {
                          pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                          if (*(int *)pQVar31 == 0) {
LAB_00127082:
                            QArrayData::deallocate(pQVar31,2,8);
                          }
                          else if (*(int *)pQVar31 != -1) {
                            LOCK();
                            *(int *)pQVar31 = *(int *)pQVar31 + -1;
                            UNLOCK();
                            if (*(int *)pQVar31 == 0) {
                              pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                              goto LAB_00127082;
                            }
                          }
                          QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                          lVar5 = *(long *)(lVar8 + 8);
                          if (lVar5 != 0) {
                            pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_001273b6:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                                goto LAB_001273b6;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10));
                            }
                          }
                          lVar8 = *(long *)(lVar8 + 0x10);
                          if (lVar8 != 0) {
                            pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_00127390:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                                goto LAB_00127390;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar8 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar8 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar8 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar8 + 0x10));
                            }
                          }
                        }
                      }
                    }
                    lVar18 = *(long *)(lVar18 + 0x10);
                    if (lVar18 != 0) {
                      pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                      if (*(int *)pQVar31 == 0) {
LAB_00126e03:
                        QArrayData::deallocate(pQVar31,2,8);
                      }
                      else if (*(int *)pQVar31 != -1) {
                        LOCK();
                        *(int *)pQVar31 = *(int *)pQVar31 + -1;
                        UNLOCK();
                        if (*(int *)pQVar31 == 0) {
                          pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                          goto LAB_00126e03;
                        }
                      }
                      QVariant::~QVariant((QVariant *)(lVar18 + 0x20));
                      lVar8 = *(long *)(lVar18 + 8);
                      if (lVar8 != 0) {
                        pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                        if (*(int *)pQVar31 == 0) {
LAB_00127100:
                          QArrayData::deallocate(pQVar31,2,8);
                        }
                        else if (*(int *)pQVar31 != -1) {
                          LOCK();
                          *(int *)pQVar31 = *(int *)pQVar31 + -1;
                          UNLOCK();
                          if (*(int *)pQVar31 == 0) {
                            pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                            goto LAB_00127100;
                          }
                        }
                        QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                        lVar5 = *(long *)(lVar8 + 8);
                        if (lVar5 != 0) {
                          pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                          if (*(int *)pQVar31 == 0) {
LAB_00127010:
                            QArrayData::deallocate(pQVar31,2,8);
                          }
                          else if (*(int *)pQVar31 != -1) {
                            LOCK();
                            *(int *)pQVar31 = *(int *)pQVar31 + -1;
                            UNLOCK();
                            if (*(int *)pQVar31 == 0) {
                              pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                              goto LAB_00127010;
                            }
                          }
                          QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                          in_R8 = *(uchar **)(lVar5 + 8);
                          if (in_R8 != (uchar *)0x0) {
                            pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_0012734e:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(in_R8 + 0x18);
                                goto LAB_0012734e;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(in_R8 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(in_R8 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(in_R8 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(in_R8 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(in_R8 + 0x10));
                            }
                          }
                          lVar5 = *(long *)(lVar5 + 0x10);
                          if (lVar5 != 0) {
                            pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_001271be:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                                goto LAB_001271be;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10));
                            }
                          }
                        }
                        lVar8 = *(long *)(lVar8 + 0x10);
                        if (lVar8 != 0) {
                          pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                          if (*(int *)pQVar31 == 0) {
LAB_001270a8:
                            QArrayData::deallocate(pQVar31,2,8);
                          }
                          else if (*(int *)pQVar31 != -1) {
                            LOCK();
                            *(int *)pQVar31 = *(int *)pQVar31 + -1;
                            UNLOCK();
                            if (*(int *)pQVar31 == 0) {
                              pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                              goto LAB_001270a8;
                            }
                          }
                          QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                          lVar5 = *(long *)(lVar8 + 8);
                          if (lVar5 != 0) {
                            pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_00127478:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                                goto LAB_00127478;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10));
                            }
                          }
                          lVar8 = *(long *)(lVar8 + 0x10);
                          if (lVar8 != 0) {
                            pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_00127452:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                                goto LAB_00127452;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar8 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar8 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar8 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar8 + 0x10));
                            }
                          }
                        }
                      }
                      lVar18 = *(long *)(lVar18 + 0x10);
                      if (lVar18 != 0) {
                        pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                        if (*(int *)pQVar31 == 0) {
LAB_00127126:
                          QArrayData::deallocate(pQVar31,2,8);
                        }
                        else if (*(int *)pQVar31 != -1) {
                          LOCK();
                          *(int *)pQVar31 = *(int *)pQVar31 + -1;
                          UNLOCK();
                          if (*(int *)pQVar31 == 0) {
                            pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                            goto LAB_00127126;
                          }
                        }
                        QVariant::~QVariant((QVariant *)(lVar18 + 0x20));
                        lVar8 = *(long *)(lVar18 + 8);
                        if (lVar8 != 0) {
                          pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                          if (*(int *)pQVar31 == 0) {
LAB_00127044:
                            QArrayData::deallocate(pQVar31,2,8);
                          }
                          else if (*(int *)pQVar31 != -1) {
                            LOCK();
                            *(int *)pQVar31 = *(int *)pQVar31 + -1;
                            UNLOCK();
                            if (*(int *)pQVar31 == 0) {
                              pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                              goto LAB_00127044;
                            }
                          }
                          QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                          lVar5 = *(long *)(lVar8 + 8);
                          if (lVar5 != 0) {
                            pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_00127270:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar5 + 0x18);
                                goto LAB_00127270;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar5 + 0x10));
                            }
                          }
                          lVar8 = *(long *)(lVar8 + 0x10);
                          if (lVar8 != 0) {
                            pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_001273ea:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                                goto LAB_001273ea;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar8 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar8 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar8 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar8 + 0x10));
                            }
                          }
                        }
                        lVar18 = *(long *)(lVar18 + 0x10);
                        if (lVar18 != 0) {
                          pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                          if (*(int *)pQVar31 == 0) {
LAB_0012706a:
                            QArrayData::deallocate(pQVar31,2,8);
                          }
                          else if (*(int *)pQVar31 != -1) {
                            LOCK();
                            *(int *)pQVar31 = *(int *)pQVar31 + -1;
                            UNLOCK();
                            if (*(int *)pQVar31 == 0) {
                              pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                              goto LAB_0012706a;
                            }
                          }
                          QVariant::~QVariant((QVariant *)(lVar18 + 0x20));
                          lVar8 = *(long *)(lVar18 + 8);
                          if (lVar8 != 0) {
                            pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_00127224:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar8 + 0x18);
                                goto LAB_00127224;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar8 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar8 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar8 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar8 + 0x10));
                            }
                          }
                          lVar18 = *(long *)(lVar18 + 0x10);
                          if (lVar18 != 0) {
                            pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                            if (*(int *)pQVar31 == 0) {
LAB_0012720c:
                              QArrayData::deallocate(pQVar31,2,8);
                            }
                            else if (*(int *)pQVar31 != -1) {
                              LOCK();
                              *(int *)pQVar31 = *(int *)pQVar31 + -1;
                              UNLOCK();
                              if (*(int *)pQVar31 == 0) {
                                pQVar31 = *(QArrayData **)(lVar18 + 0x18);
                                goto LAB_0012720c;
                              }
                            }
                            QVariant::~QVariant((QVariant *)(lVar18 + 0x20));
                            if (*(QMapNode<QString,QVariant> **)(lVar18 + 8) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar18 + 8));
                            }
                            if (*(QMapNode<QString,QVariant> **)(lVar18 + 0x10) !=
                                (QMapNode<QString,QVariant> *)0x0) {
                              QMapNode<QString,QVariant>::destroySubTree
                                        (*(QMapNode<QString,QVariant> **)(lVar18 + 0x10));
                            }
                          }
                        }
                      }
                    }
                    QMapDataBase::freeTree
                              ((QMapNodeBase *)pQVar30,(int)*(undefined8 *)(pQVar30 + 0x10));
                  }
                  QMapDataBase::freeData((QMapDataBase *)pQVar30);
                  pQVar27 = local_50;
                }
                else if (*(int *)local_50 != -1) {
                  LOCK();
                  *(int *)local_50 = *(int *)local_50 + -1;
                  UNLOCK();
                  if (*(int *)local_50 == 0) goto LAB_00125d68;
                }
              }
              local_50 = pQVar27;
              local_58 = *(undefined *)(puVar19 + 6);
              __memcpy_chk(local_80,puVar19 + 1,local_58,0x38);
            }
          }
          iVar13 = iVar13 + 1;
          dVar42 = (double)iVar13;
        } while (dVar42 < dVar36 / dVar37);
      }
      local_1dc = local_1dc + 1;
      dVar41 = (double)local_1dc;
    } while (dVar41 < dVar36 / dVar38);
  }
  _local_f8 = KisPaintDevice::extent();
  uVar22 = KisPaintOp::painter();
  local_108 = *(long **)(in_RSI + 0x28);
  if (local_108 != (long *)0x0) {
    LOCK();
    *(int *)(local_108 + 2) = *(int *)(local_108 + 2) + 1;
    UNLOCK();
  }
  local_120 = (QArrayData *)local_f8;
                    /* try { // try from 00124fc7 to 00124fcb has its CatchHandler @ 001275d7 */
  KisPainter::bitBlt(uVar22,&local_120,&local_108,local_f8);
  if (local_108 != (long *)0x0) {
    LOCK();
    plVar24 = local_108 + 2;
    *(int *)plVar24 = *(int *)plVar24 + -1;
    UNLOCK();
    if (*(int *)plVar24 == 0) {
      (**(code **)(*local_108 + 0x20))();
    }
  }
                    /* try { // try from 00124fe6 to 00124fea has its CatchHandler @ 001275e3 */
  uVar22 = KisPaintOp::painter();
  local_108 = *(long **)(in_RSI + 0x28);
  if (local_108 != (long *)0x0) {
    LOCK();
    *(int *)(local_108 + 2) = *(int *)(local_108 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 00125018 to 0012501c has its CatchHandler @ 001275cb */
  KisPainter::renderMirrorMask(uVar22,local_f8,dStack_f0,&local_108);
  if (local_108 != (long *)0x0) {
    LOCK();
    plVar24 = local_108 + 2;
    *(int *)plVar24 = *(int *)plVar24 + -1;
    UNLOCK();
    if (*(int *)plVar24 == 0) {
      (**(code **)(*local_108 + 0x20))();
    }
  }
  local_240 = local_240 * *(double *)(in_RSI + 0x48);
  *param_1 = (KisPaintInformation)0x1;
  *(undefined8 *)(param_1 + 0x18) = 0;
  param_1[0x20] = (KisPaintInformation)0x0;
  *(double *)(param_1 + 8) = local_240;
  *(double *)(param_1 + 0x10) = local_240;
  if (local_1d0 != (undefined8 *)0x0) {
    if ((void *)local_1d0[3] != (void *)0x0) {
      operator_delete__((void *)local_1d0[3]);
    }
    pKVar32 = (KisRandomSubAccessor *)local_1d0[2];
    if (pKVar32 != (KisRandomSubAccessor *)0x0) {
      LOCK();
      *(int *)pKVar32 = *(int *)pKVar32 + -1;
      UNLOCK();
      if (*(int *)pKVar32 == 0) {
        KisRandomSubAccessor::~KisRandomSubAccessor(pKVar32);
        operator_delete(pKVar32,0x30);
      }
    }
    operator_delete(local_1d0,0x20);
  }
  pQVar30 = local_50;
  if (*(int *)local_50 != 0) {
    if (*(int *)local_50 == -1) goto LAB_001250c5;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_001250c5;
  }
  lVar18 = *(long *)(local_50 + 0x10);
  if (lVar18 != 0) {
    pQVar31 = *(QArrayData **)(lVar18 + 0x18);
    if (*(int *)pQVar31 == 0) {
LAB_001270ce:
      QArrayData::deallocate(pQVar31,2,8);
    }
    else if (*(int *)pQVar31 != -1) {
      LOCK();
      *(int *)pQVar31 = *(int *)pQVar31 + -1;
      UNLOCK();
      if (*(int *)pQVar31 == 0) {
        pQVar31 = *(QArrayData **)(lVar18 + 0x18);
        goto LAB_001270ce;
      }
    }
    QVariant::~QVariant((QVariant *)(lVar18 + 0x20));
    lVar8 = *(long *)(lVar18 + 8);
    if (lVar8 != 0) {
      pQVar31 = *(QArrayData **)(lVar8 + 0x18);
      if (*(int *)pQVar31 == 0) {
LAB_00126cf6:
        QArrayData::deallocate(pQVar31,2,8);
      }
      else if (*(int *)pQVar31 != -1) {
        LOCK();
        *(int *)pQVar31 = *(int *)pQVar31 + -1;
        UNLOCK();
        if (*(int *)pQVar31 == 0) {
          pQVar31 = *(QArrayData **)(lVar8 + 0x18);
          goto LAB_00126cf6;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar8 + 0x20));
      if (*(QMapNode<QString,QVariant> **)(lVar8 + 8) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar8 + 8));
      }
      if (*(QMapNode<QString,QVariant> **)(lVar8 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar8 + 0x10));
      }
    }
    lVar18 = *(long *)(lVar18 + 0x10);
    if (lVar18 != 0) {
      pQVar31 = *(QArrayData **)(lVar18 + 0x18);
      if (*(int *)pQVar31 == 0) {
LAB_00126d52:
        QArrayData::deallocate(pQVar31,2,8);
      }
      else if (*(int *)pQVar31 != -1) {
        LOCK();
        *(int *)pQVar31 = *(int *)pQVar31 + -1;
        UNLOCK();
        if (*(int *)pQVar31 == 0) {
          pQVar31 = *(QArrayData **)(lVar18 + 0x18);
          goto LAB_00126d52;
        }
      }
      QVariant::~QVariant((QVariant *)(lVar18 + 0x20));
      if (*(QMapNode<QString,QVariant> **)(lVar18 + 8) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar18 + 8));
      }
      if (*(QMapNode<QString,QVariant> **)(lVar18 + 0x10) != (QMapNode<QString,QVariant> *)0x0) {
        QMapNode<QString,QVariant>::destroySubTree(*(QMapNode<QString,QVariant> **)(lVar18 + 0x10));
      }
    }
    QMapDataBase::freeTree((QMapNodeBase *)pQVar30,(int)*(undefined8 *)(pQVar30 + 0x10));
  }
  QMapDataBase::freeData((QMapDataBase *)pQVar30);
LAB_001250c5:
  if (local_130 != (KisRandomSource *)0x0) {
    LOCK();
    *(int *)local_130 = *(int *)local_130 + -1;
    UNLOCK();
    if (*(int *)local_130 == 0) {
      KisRandomSource::~KisRandomSource(local_130);
      operator_delete(local_130,0x18);
    }
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return param_1;
}


// ====== paintBezierCurve @ 00168360 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintBezierCurve
               (KisPaintInformation *param_1,QPointF *param_2,QPointF *param_3,
               KisPaintInformation *param_4,KisDistanceInformation *param_5)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


// ====== paintLine @ 00168808 ======

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void KisPaintOp::paintLine
               (KisPaintInformation *param_1,KisPaintInformation *param_2,
               KisDistanceInformation *param_3)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


