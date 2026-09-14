/* FULL Ghidra decompilation: Krita brush engine (libkritaimage.so)
 * Binary: libkritaimage.so.20.0.0 - 7.3MB, 8224 functions, stripped
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ALL functions matching brush engine class prefixes.
 * Virtual methods (paintDab, paintLine) appear via vtable recovery.
 */

// ====== KisPaintInformation @ 002019f0 ======

void __thiscall
KisPaintInformation::KisPaintInformation(KisPaintInformation *this,KisPaintInformation *param_1)

{
  (*(code *)PTR_KisPaintInformation_008387c8)();
  return;
}



// ====== KisOptimizedBrushOutline @ 00202340 ======

void __thiscall KisOptimizedBrushOutline::KisOptimizedBrushOutline(KisOptimizedBrushOutline *this)

{
  (*(code *)PTR_KisOptimizedBrushOutline_00838c70)();
  return;
}



// ====== KisPerStrokeRandomSource @ 00202ed0 ======

void __thiscall
KisPerStrokeRandomSource::KisPerStrokeRandomSource
          (KisPerStrokeRandomSource *this,KisPerStrokeRandomSource *param_1)

{
  (*(code *)PTR_KisPerStrokeRandomSource_00839238)();
  return;
}



// ====== KisPerStrokeRandomSource @ 00203070 ======

void __thiscall KisPerStrokeRandomSource::KisPerStrokeRandomSource(KisPerStrokeRandomSource *this)

{
  (*(code *)PTR_KisPerStrokeRandomSource_00839308)();
  return;
}



// ====== KisPaintOpPreset @ 00206e90 ======

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this)

{
  (*(code *)PTR_KisPaintOpPreset_0083b218)();
  return;
}



// ====== KisCallbackBasedPaintopProperty @ 00207d00 ======

void __thiscall
KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>>::
KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>> *this,Type param_1
          ,KoID *param_2,KisRestrictedSharedPtr param_3,QObject *param_4)

{
  (*(code *)PTR_KisCallbackBasedPaintopProperty_0083b950)();
  return;
}



// ====== KisPaintOpRegistry @ 00208fc0 ======

void __thiscall KisPaintOpRegistry::KisPaintOpRegistry(KisPaintOpRegistry *this)

{
  (*(code *)PTR_KisPaintOpRegistry_0083c2b0)();
  return;
}



// ====== KisPaintInformation @ 0020a3c0 ======

void __thiscall
KisPaintInformation::KisPaintInformation
          (KisPaintInformation *this,QPointF *param_1,double param_2,double param_3,double param_4,
          double param_5,double param_6,double param_7,double param_8,double param_9)

{
  (*(code *)PTR_KisPaintInformation_0083ccb0)();
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



// ====== KisPaintOpPresetUpdateProxy @ 0020b3b0 ======

void __thiscall
KisPaintOpPresetUpdateProxy::KisPaintOpPresetUpdateProxy(KisPaintOpPresetUpdateProxy *this)

{
  (*(code *)PTR_KisPaintOpPresetUpdateProxy_0083d4a8)();
  return;
}



// ====== KisPaintOpPreset @ 0020b8a0 ======

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this,KisPaintOpPreset *param_1)

{
  (*(code *)PTR_KisPaintOpPreset_0083d720)();
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



// ====== KisPaintInformation @ 0020d710 ======

void __thiscall
KisPaintInformation::KisPaintInformation(KisPaintInformation *this,QPointF *param_1,double param_2)

{
  (*(code *)PTR_KisPaintInformation_0083e658)();
  return;
}



// ====== KisPaintOpSettings @ 0020da90 ======

void __thiscall
KisPaintOpSettings::KisPaintOpSettings(KisPaintOpSettings *this,QSharedPointer param_1)

{
  (*(code *)PTR_KisPaintOpSettings_0083e818)();
  return;
}



// ====== KisPaintInformation @ 003301f0 ======

/* KisPaintInformation::KisPaintInformation(QPointF const&, double, double, double, double, double,
   double, double, double) */

void __thiscall
KisPaintInformation::KisPaintInformation
          (KisPaintInformation *this,QPointF *param_1,double param_2,double param_3,double param_4,
          double param_5,double param_6,double param_7,double param_8,double param_9)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 *puVar3;
  
  puVar3 = (undefined8 *)operator_new(0xe8);
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  *(undefined *)(puVar3 + 10) = 0;
  puVar3[2] = param_2;
  puVar3[3] = param_3;
  puVar3[0xd] = 0;
  *(undefined2 *)(puVar3 + 0xe) = 0;
  puVar3[0xf] = 0;
  *(undefined *)(puVar3 + 0x10) = 0;
  puVar3[0x11] = 0;
  *(undefined *)(puVar3 + 0x12) = 0;
  *(undefined *)(puVar3 + 0x13) = 0;
  *(undefined4 *)(puVar3 + 0x1c) = 0;
  *(undefined8 **)this = puVar3;
  *puVar3 = uVar1;
  puVar3[1] = uVar2;
  puVar3[4] = param_4;
  puVar3[5] = param_5;
  puVar3[6] = param_6;
  puVar3[7] = param_7;
  puVar3[8] = param_8;
  puVar3[9] = param_9;
  *(undefined (*) [16])(puVar3 + 0xb) = (undefined  [16])0x0;
  return;
}



// ====== KisPaintInformation @ 003302c0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisPaintInformation::KisPaintInformation(QPointF const&, double, double, double, double) */

void __thiscall
KisPaintInformation::KisPaintInformation
          (KisPaintInformation *this,QPointF *param_1,double param_2,double param_3,double param_4,
          double param_5)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
  
  puVar5 = (undefined8 *)operator_new(0xe8);
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  *(undefined *)(puVar5 + 10) = 0;
  puVar5[2] = param_2;
  puVar5[3] = param_3;
  uVar4 = DAT_007231a8;
  uVar3 = _DAT_007231a0;
  puVar5[0xd] = 0;
  puVar5[6] = uVar3;
  puVar5[7] = uVar4;
  *(undefined (*) [16])(puVar5 + 8) = (undefined  [16])0x0;
  *(undefined2 *)(puVar5 + 0xe) = 0;
  puVar5[0xf] = 0;
  *(undefined *)(puVar5 + 0x10) = 0;
  puVar5[0x11] = 0;
  *(undefined *)(puVar5 + 0x12) = 0;
  *(undefined *)(puVar5 + 0x13) = 0;
  *(undefined4 *)(puVar5 + 0x1c) = 0;
  *(undefined8 **)this = puVar5;
  *puVar5 = uVar1;
  puVar5[1] = uVar2;
  puVar5[4] = param_4;
  puVar5[5] = param_5;
  *(undefined (*) [16])(puVar5 + 0xb) = (undefined  [16])0x0;
  return;
}



// ====== KisPaintInformation @ 00330370 ======

/* KisPaintInformation::KisPaintInformation(QPointF const&, double) */

void __thiscall
KisPaintInformation::KisPaintInformation(KisPaintInformation *this,QPointF *param_1,double param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 *puVar4;
  
  puVar4 = (undefined8 *)operator_new(0xe8);
  uVar3 = DAT_007231a8;
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  puVar4[3] = 0;
  puVar4[2] = param_2;
  puVar4[7] = uVar3;
  puVar4[4] = 0;
  puVar4[5] = 0;
  puVar4[6] = 0;
  puVar4[8] = 0;
  puVar4[9] = 0;
  *(undefined *)(puVar4 + 10) = 0;
  puVar4[0xd] = 0;
  *(undefined2 *)(puVar4 + 0xe) = 0;
  puVar4[0xf] = 0;
  *(undefined *)(puVar4 + 0x10) = 0;
  puVar4[0x11] = 0;
  *(undefined *)(puVar4 + 0x12) = 0;
  *(undefined *)(puVar4 + 0x13) = 0;
  *(undefined4 *)(puVar4 + 0x1c) = 0;
  *(undefined8 **)this = puVar4;
  *puVar4 = uVar1;
  puVar4[1] = uVar2;
  *(undefined (*) [16])(puVar4 + 0xb) = (undefined  [16])0x0;
  return;
}



// ====== KisPaintInformation @ 00330440 ======

/* KisPaintInformation::KisPaintInformation(KisPaintInformation const&) */

void __thiscall
KisPaintInformation::KisPaintInformation(KisPaintInformation *this,KisPaintInformation *param_1)

{
  undefined8 uVar1;
  char cVar2;
  undefined uVar3;
  undefined4 uVar4;
  undefined8 *puVar5;
  int *piVar6;
  KisRandomSource *this_00;
  undefined8 uVar7;
  undefined (*pauVar8) [16];
  KisPerStrokeRandomSource *pKVar9;
  KisPerStrokeRandomSource *this_01;
  
  pauVar8 = (undefined (*) [16])operator_new(0xe8);
  puVar5 = *(undefined8 **)param_1;
  *pauVar8 = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar8[5] + 8) = (undefined  [16])0x0;
  *(undefined8 *)(pauVar8[6] + 8) = 0;
  *(undefined8 *)(pauVar8[7] + 8) = 0;
  *(undefined8 *)(pauVar8[8] + 8) = 0;
  uVar1 = *puVar5;
  uVar7 = puVar5[1];
  *(undefined2 *)pauVar8[7] = 0;
  *(undefined8 *)*pauVar8 = uVar1;
  *(undefined8 *)(*pauVar8 + 8) = uVar7;
  uVar1 = puVar5[2];
  pauVar8[8][0] = 0;
  *(undefined8 *)pauVar8[1] = uVar1;
  uVar1 = puVar5[3];
  pauVar8[9][0] = 0;
  *(undefined8 *)(pauVar8[1] + 8) = uVar1;
  uVar1 = puVar5[4];
  pauVar8[9][8] = 0;
  *(undefined8 *)pauVar8[2] = uVar1;
  *(undefined8 *)(pauVar8[2] + 8) = puVar5[5];
  *(undefined8 *)pauVar8[3] = puVar5[6];
  *(undefined8 *)(pauVar8[3] + 8) = puVar5[7];
  *(undefined8 *)pauVar8[4] = puVar5[8];
  *(undefined8 *)(pauVar8[4] + 8) = puVar5[9];
  pauVar8[5][0] = *(undefined *)(puVar5 + 10);
  piVar6 = (int *)puVar5[0xb];
  if (piVar6 == (int *)0x0) {
    pKVar9 = (KisPerStrokeRandomSource *)puVar5[0xc];
    if (pKVar9 == (KisPerStrokeRandomSource *)0x0) goto LAB_00330562;
LAB_00330545:
    LOCK();
    *(int *)pKVar9 = *(int *)pKVar9 + 1;
    UNLOCK();
    this_01 = *(KisPerStrokeRandomSource **)pauVar8[6];
  }
  else {
    LOCK();
    *piVar6 = *piVar6 + 1;
    UNLOCK();
    this_00 = *(KisRandomSource **)(pauVar8[5] + 8);
    *(int **)(pauVar8[5] + 8) = piVar6;
    if (this_00 != (KisRandomSource *)0x0) {
      LOCK();
      *(int *)this_00 = *(int *)this_00 + -1;
      UNLOCK();
      if (*(int *)this_00 == 0) {
        KisRandomSource::~KisRandomSource(this_00);
        operator_delete(this_00,0x18);
      }
    }
    this_01 = *(KisPerStrokeRandomSource **)pauVar8[6];
    pKVar9 = (KisPerStrokeRandomSource *)puVar5[0xc];
    if (pKVar9 == this_01) goto LAB_00330562;
    if (pKVar9 != (KisPerStrokeRandomSource *)0x0) goto LAB_00330545;
  }
  *(KisPerStrokeRandomSource **)pauVar8[6] = pKVar9;
  if (this_01 != (KisPerStrokeRandomSource *)0x0) {
    LOCK();
    *(int *)this_01 = *(int *)this_01 + -1;
    UNLOCK();
    if (*(int *)this_01 == 0) {
      KisPerStrokeRandomSource::~KisPerStrokeRandomSource(this_01);
      operator_delete(this_01,0x18);
    }
  }
LAB_00330562:
  cVar2 = pauVar8[9][8];
  pauVar8[9][0] = 0;
  if (cVar2 == '\0') {
    if (*(char *)(puVar5 + 0x13) != '\0') {
      uVar1 = puVar5[0x15];
      *(undefined8 *)pauVar8[10] = puVar5[0x14];
      *(undefined8 *)(pauVar8[10] + 8) = uVar1;
      uVar1 = puVar5[0x17];
      *(undefined8 *)pauVar8[0xb] = puVar5[0x16];
      *(undefined8 *)(pauVar8[0xb] + 8) = uVar1;
      uVar1 = puVar5[0x19];
      *(undefined8 *)pauVar8[0xc] = puVar5[0x18];
      *(undefined8 *)(pauVar8[0xc] + 8) = uVar1;
      uVar1 = puVar5[0x1a];
      uVar7 = puVar5[0x1b];
      pauVar8[9][8] = 1;
      *(undefined8 *)pauVar8[0xd] = uVar1;
      *(undefined8 *)(pauVar8[0xd] + 8) = uVar7;
    }
  }
  else if (*(char *)(puVar5 + 0x13) == '\0') {
    pauVar8[9][8] = 0;
  }
  else {
    uVar1 = puVar5[0x15];
    *(undefined8 *)pauVar8[10] = puVar5[0x14];
    *(undefined8 *)(pauVar8[10] + 8) = uVar1;
    uVar1 = puVar5[0x17];
    *(undefined8 *)pauVar8[0xb] = puVar5[0x16];
    *(undefined8 *)(pauVar8[0xb] + 8) = uVar1;
    uVar1 = puVar5[0x19];
    *(undefined8 *)pauVar8[0xc] = puVar5[0x18];
    *(undefined8 *)(pauVar8[0xc] + 8) = uVar1;
    uVar1 = puVar5[0x1b];
    *(undefined8 *)pauVar8[0xd] = puVar5[0x1a];
    *(undefined8 *)(pauVar8[0xd] + 8) = uVar1;
  }
  uVar1 = puVar5[0xd];
  pauVar8[7][0] = *(undefined *)(puVar5 + 0xe);
  uVar3 = *(undefined *)((long)puVar5 + 0x71);
  *(undefined8 *)(pauVar8[6] + 8) = uVar1;
  uVar1 = puVar5[0xf];
  pauVar8[7][1] = uVar3;
  cVar2 = *(char *)(puVar5 + 0x10);
  *(undefined8 *)(pauVar8[7] + 8) = uVar1;
  if (cVar2 != '\0') {
    uVar1 = puVar5[0x11];
    pauVar8[8][0] = 1;
    *(undefined8 *)(pauVar8[8] + 8) = uVar1;
  }
  uVar4 = *(undefined4 *)(puVar5 + 0x1c);
  *(undefined (**) [16])this = pauVar8;
  *(undefined4 *)pauVar8[0xe] = uVar4;
  return;
}



// ====== KisPerStrokeRandomSource @ 00336060 ======

/* KisPerStrokeRandomSource::KisPerStrokeRandomSource() */

void __thiscall KisPerStrokeRandomSource::KisPerStrokeRandomSource(KisPerStrokeRandomSource *this)

{
  undefined4 *puVar1;
  void *pvVar2;
  long in_FS_OFFSET;
  undefined4 local_24;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)this);
                    /* try { // try from 00336089 to 0033608d has its CatchHandler @ 003360f8 */
  puVar1 = (undefined4 *)operator_new(0x20);
                    /* try { // try from 00336091 to 003360a7 has its CatchHandler @ 003360ec */
  pvVar2 = (void *)QRandomGenerator64::global();
  QRandomGenerator::_fillRange(pvVar2,&local_24);
  *(undefined8 *)(puVar1 + 6) = 0;
  *(undefined4 **)(this + 0x10) = puVar1;
  *puVar1 = local_24;
  *(undefined **)(puVar1 + 4) = PTR_shared_null_00836c40;
  *(undefined8 *)(puVar1 + 2) = 0xffffffff;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPerStrokeRandomSource @ 00336110 ======

/* KisPerStrokeRandomSource::KisPerStrokeRandomSource(KisPerStrokeRandomSource const&) */

void __thiscall
KisPerStrokeRandomSource::KisPerStrokeRandomSource
          (KisPerStrokeRandomSource *this,KisPerStrokeRandomSource *param_1)

{
  code *pcVar1;
  undefined4 *puVar2;
  long lVar3;
  _func_void_Node_ptr_void_ptr *p_Var4;
  undefined4 *puVar5;
  undefined8 uVar6;
  _func_void_Node_ptr *p_Var7;
  
  KisShared::KisShared((KisShared *)this);
                    /* try { // try from 0033612e to 00336132 has its CatchHandler @ 003361fa */
  puVar5 = (undefined4 *)operator_new(0x20);
  puVar2 = *(undefined4 **)(param_1 + 0x10);
  *puVar5 = *puVar2;
  *(undefined8 *)(puVar5 + 2) = *(undefined8 *)(puVar2 + 2);
  lVar3 = *(long *)(puVar2 + 4);
  *(long *)(puVar5 + 4) = lVar3;
  if (1 < *(int *)(lVar3 + 0x10) + 1U) {
    LOCK();
    *(int *)(lVar3 + 0x10) = *(int *)(lVar3 + 0x10) + 1;
    UNLOCK();
  }
  p_Var4 = *(_func_void_Node_ptr_void_ptr **)(puVar5 + 4);
  if ((((byte)p_Var4[0x28] & 1) != 0) || (*(uint *)(p_Var4 + 0x10) < 2)) goto LAB_00336164;
                    /* try { // try from 003361a4 to 003361eb has its CatchHandler @ 003361ee */
  uVar6 = QHashData::detach_helper(p_Var4,FUN_00336720,0x336760,0x20);
  p_Var7 = *(_func_void_Node_ptr **)(puVar5 + 4);
  pcVar1 = p_Var7 + 0x10;
  if (*(int *)(p_Var7 + 0x10) == 0) {
LAB_003361e4:
    QHashData::free_helper(p_Var7);
  }
  else if (*(int *)(p_Var7 + 0x10) != -1) {
    LOCK();
    *(int *)pcVar1 = *(int *)pcVar1 + -1;
    UNLOCK();
    if (*(int *)pcVar1 == 0) {
      p_Var7 = *(_func_void_Node_ptr **)(puVar5 + 4);
      goto LAB_003361e4;
    }
  }
  *(undefined8 *)(puVar5 + 4) = uVar6;
LAB_00336164:
  *(undefined8 *)(puVar5 + 6) = 0;
  *(undefined4 **)(this + 0x10) = puVar5;
  return;
}



// ====== KisStrokeRandomSource @ 003367e0 ======

/* KisStrokeRandomSource::KisStrokeRandomSource() */

void __thiscall KisStrokeRandomSource::KisStrokeRandomSource(KisStrokeRandomSource *this)

{
  undefined4 *puVar1;
  KisRandomSource *pKVar2;
  KisPerStrokeRandomSource *pKVar3;
  
  puVar1 = (undefined4 *)operator_new(0x28);
  *puVar1 = 0;
                    /* try { // try from 00336803 to 00336807 has its CatchHandler @ 00336887 */
  pKVar2 = (KisRandomSource *)operator_new(0x18);
                    /* try { // try from 0033680e to 00336812 has its CatchHandler @ 003368cf */
  KisRandomSource::KisRandomSource(pKVar2);
  *(KisRandomSource **)(puVar1 + 2) = pKVar2;
  LOCK();
  *(int *)pKVar2 = *(int *)pKVar2 + 1;
  UNLOCK();
                    /* try { // try from 00336821 to 00336825 has its CatchHandler @ 003368c3 */
  pKVar2 = (KisRandomSource *)operator_new(0x18);
                    /* try { // try from 00336830 to 00336834 has its CatchHandler @ 003368db */
  KisRandomSource::KisRandomSource(pKVar2,*(KisRandomSource **)(puVar1 + 2));
  *(KisRandomSource **)(puVar1 + 4) = pKVar2;
  LOCK();
  *(int *)pKVar2 = *(int *)pKVar2 + 1;
  UNLOCK();
                    /* try { // try from 00336843 to 00336847 has its CatchHandler @ 003368b7 */
  pKVar3 = (KisPerStrokeRandomSource *)operator_new(0x18);
                    /* try { // try from 0033684e to 00336852 has its CatchHandler @ 003368ab */
  KisPerStrokeRandomSource::KisPerStrokeRandomSource(pKVar3);
  *(KisPerStrokeRandomSource **)(puVar1 + 6) = pKVar3;
  LOCK();
  *(int *)pKVar3 = *(int *)pKVar3 + 1;
  UNLOCK();
                    /* try { // try from 00336861 to 00336865 has its CatchHandler @ 0033689f */
  pKVar3 = (KisPerStrokeRandomSource *)operator_new(0x18);
                    /* try { // try from 00336870 to 00336874 has its CatchHandler @ 00336893 */
  KisPerStrokeRandomSource::KisPerStrokeRandomSource
            (pKVar3,*(KisPerStrokeRandomSource **)(puVar1 + 6));
  *(KisPerStrokeRandomSource **)(puVar1 + 8) = pKVar3;
  LOCK();
  *(int *)pKVar3 = *(int *)pKVar3 + 1;
  UNLOCK();
  *(undefined4 **)this = puVar1;
  return;
}



// ====== KisStrokeRandomSource @ 003368f0 ======

/* KisStrokeRandomSource::KisStrokeRandomSource(KisStrokeRandomSource const&) */

void __thiscall
KisStrokeRandomSource::KisStrokeRandomSource
          (KisStrokeRandomSource *this,KisStrokeRandomSource *param_1)

{
  undefined4 *puVar1;
  int *piVar2;
  undefined4 *puVar3;
  
  puVar3 = (undefined4 *)operator_new(0x28);
  puVar1 = *(undefined4 **)param_1;
  *puVar3 = *puVar1;
  piVar2 = *(int **)(puVar1 + 2);
  *(int **)(puVar3 + 2) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  piVar2 = *(int **)(puVar1 + 4);
  *(int **)(puVar3 + 4) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  piVar2 = *(int **)(puVar1 + 6);
  *(int **)(puVar3 + 6) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  piVar2 = *(int **)(puVar1 + 8);
  *(int **)(puVar3 + 8) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  *(undefined4 **)this = puVar3;
  return;
}



// ====== KisPaintOp @ 00336c80 ======

/* KisPaintOp::KisPaintOp(KisPainter*) */

void __thiscall KisPaintOp::KisPaintOp(KisPaintOp *this,KisPainter *param_1)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837ae8 + 0x10;
                    /* try { // try from 00336cad to 00336cb1 has its CatchHandler @ 00336cd9 */
  puVar2 = (undefined8 *)operator_new(0x28);
  uVar1 = DAT_007227c0;
  *puVar2 = this;
  puVar2[1] = 0;
  *(undefined *)(puVar2 + 3) = 0;
  puVar2[4] = uVar1;
  *(undefined8 **)(this + 0x18) = puVar2;
  puVar2[2] = param_1;
  return;
}



// ====== KisPaintOpFactory @ 00337fb0 ======

/* KisPaintOpFactory::KisPaintOpFactory(QStringList const&) */

void __thiscall KisPaintOpFactory::KisPaintOpFactory(KisPaintOpFactory *this,QStringList *param_1)

{
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837760 + 0x10;
                    /* try { // try from 00337fdc to 00337fe0 has its CatchHandler @ 00337ff0 */
  FUN_003380c0(this + 0x10,param_1);
  *(undefined8 *)(this + 0x18) = 100;
  return;
}



// ====== KisPaintOpPreset @ 00338210 ======

/* KisPaintOpPreset::KisPaintOpPreset() */

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this)

{
  undefined *puVar1;
  undefined (*pauVar2) [16];
  undefined8 *puVar3;
  undefined4 *puVar4;
  undefined8 uVar5;
  long in_FS_OFFSET;
  QArrayData *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_28 = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 0033823e to 00338242 has its CatchHandler @ 00338329 */
  KoResource::KoResource((KoResource *)this,(QString *)&local_28);
  if (*(int *)local_28 != 0) {
    if (*(int *)local_28 == -1) goto LAB_00338264;
    LOCK();
    *(int *)local_28 = *(int *)local_28 + -1;
    UNLOCK();
    if (*(int *)local_28 != 0) goto LAB_00338264;
  }
  QArrayData::deallocate(local_28,2,8);
LAB_00338264:
  *(undefined **)this = PTR_vtable_00837f38 + 0x10;
                    /* try { // try from 00338278 to 0033827c has its CatchHandler @ 0033834d */
  pauVar2 = (undefined (*) [16])operator_new(0x30);
  *pauVar2 = (undefined  [16])0x0;
                    /* try { // try from 0033828c to 003382b1 has its CatchHandler @ 00338341 */
  puVar3 = (undefined8 *)operator_new(0x10);
  *puVar3 = &PTR_FUN_0081fc38;
  puVar3[1] = this;
  *(undefined8 **)pauVar2[1] = puVar3;
  puVar4 = (undefined4 *)operator_new(0x18);
  *(undefined8 **)(puVar4 + 4) = puVar3;
  *(code **)(puVar4 + 2) = FUN_00340790;
  puVar4[1] = 1;
  *puVar4 = 1;
  *(undefined4 **)(pauVar2[1] + 8) = puVar4;
                    /* try { // try from 003382de to 003382e2 has its CatchHandler @ 00338335 */
  uVar5 = QString::fromAscii_helper("5.0",3);
  *(undefined8 *)pauVar2[2] = uVar5;
  puVar1 = PTR_shared_null_00837830;
  *(undefined (**) [16])(this + 0x10) = pauVar2;
  *(undefined **)(pauVar2[2] + 8) = puVar1;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPaintOpPreset @ 0033c620 ======

/* KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset const&) */

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this,KisPaintOpPreset *param_1)

{
  QArrayData *pQVar1;
  undefined *puVar2;
  char cVar3;
  char cVar4;
  undefined (*pauVar5) [16];
  undefined8 *puVar6;
  undefined4 *puVar7;
  undefined8 uVar8;
  long in_FS_OFFSET;
  QArrayData *local_58 [5];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KoResource::KoResource((KoResource *)this,(KoResource *)param_1);
  *(undefined **)this = PTR_vtable_00837f38 + 0x10;
                    /* try { // try from 0033c65c to 0033c660 has its CatchHandler @ 0033c840 */
  pauVar5 = (undefined (*) [16])operator_new(0x30);
  *pauVar5 = (undefined  [16])0x0;
                    /* try { // try from 0033c670 to 0033c695 has its CatchHandler @ 0033c87c */
  puVar6 = (undefined8 *)operator_new(0x10);
  *puVar6 = &PTR_FUN_0081fc38;
  puVar6[1] = this;
  *(undefined8 **)pauVar5[1] = puVar6;
  puVar7 = (undefined4 *)operator_new(0x18);
  *(undefined8 **)(puVar7 + 4) = puVar6;
  *(code **)(puVar7 + 2) = FUN_00340790;
  puVar7[1] = 1;
  *puVar7 = 1;
  *(undefined4 **)(pauVar5[1] + 8) = puVar7;
                    /* try { // try from 0033c6c3 to 0033c6c7 has its CatchHandler @ 0033c888 */
  uVar8 = QString::fromAscii_helper("5.0",3);
  *(undefined8 *)pauVar5[2] = uVar8;
  puVar2 = PTR_shared_null_00837830;
  *(undefined (**) [16])(this + 0x10) = pauVar5;
  *(undefined **)(pauVar5[2] + 8) = puVar2;
                    /* try { // try from 0033c6e6 to 0033c709 has its CatchHandler @ 0033c840 */
  settings();
  if (local_58[0] != (QArrayData *)0x0) {
    LOCK();
    pQVar1 = local_58[0] + 8;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (*(int *)pQVar1 == 0) {
      (**(code **)(*(long *)local_58[0] + 8))();
    }
    settings();
                    /* try { // try from 0033c710 to 0033c714 has its CatchHandler @ 0033c864 */
    setSettings(this,(KisPinnedSharedPtr)local_58);
    if (local_58[0] != (QArrayData *)0x0) {
      LOCK();
      pQVar1 = local_58[0] + 8;
      *(int *)pQVar1 = *(int *)pQVar1 + -1;
      UNLOCK();
      if (*(int *)pQVar1 == 0) {
        (**(code **)(*(long *)local_58[0] + 8))();
      }
    }
  }
                    /* try { // try from 0033c72c to 0033c763 has its CatchHandler @ 0033c840 */
  cVar3 = KoResource::isDirty();
  cVar4 = KoResource::isDirty();
  if (cVar3 != cVar4) {
    kis_safe_assert_recoverable
              ("isDirty() == rhs.isDirty()",
               "/builds/graphics/krita/libs/image/brushengine/kis_paintop_preset.cpp",0x6a);
  }
  settings();
                    /* try { // try from 0033c772 to 0033c776 has its CatchHandler @ 0033c84c */
  KoResource::setValid(SUB81(this,0));
  if (local_58[0] != (QArrayData *)0x0) {
    LOCK();
    pQVar1 = local_58[0] + 8;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (*(int *)pQVar1 == 0) {
      (**(code **)(*(long *)local_58[0] + 8))();
    }
  }
                    /* try { // try from 0033c795 to 0033c797 has its CatchHandler @ 0033c840 */
  (**(code **)(*(long *)param_1 + 0x40))(local_58,param_1);
                    /* try { // try from 0033c79e to 0033c7a2 has its CatchHandler @ 0033c858 */
  KoResource::setName((QString *)this);
  if (*(int *)local_58[0] != 0) {
    if (*(int *)local_58[0] == -1) goto LAB_0033c7bc;
    LOCK();
    *(int *)local_58[0] = *(int *)local_58[0] + -1;
    UNLOCK();
    if (*(int *)local_58[0] != 0) goto LAB_0033c7bc;
  }
  QArrayData::deallocate(local_58[0],2,8);
LAB_0033c7bc:
                    /* try { // try from 0033c7c2 to 0033c7c6 has its CatchHandler @ 0033c840 */
  KoResource::image();
                    /* try { // try from 0033c7cd to 0033c7d1 has its CatchHandler @ 0033c870 */
  KoResource::setImage((QImage *)this);
  QImage::~QImage((QImage *)local_58);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPaintOpPreset @ 0033cfa0 ======

/* KisPaintOpPreset::KisPaintOpPreset(QString const&) */

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this,QString *param_1)

{
  undefined *puVar1;
  int iVar2;
  undefined (*pauVar3) [16];
  undefined8 *puVar4;
  undefined4 *puVar5;
  undefined8 uVar6;
  long in_FS_OFFSET;
  QArrayData *local_48;
  QArrayData *local_40;
  QArrayData *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KoResource::KoResource((KoResource *)this,(QString *)param_1);
  *(undefined **)this = PTR_vtable_00837f38 + 0x10;
                    /* try { // try from 0033cfdc to 0033cfe0 has its CatchHandler @ 0033d1a2 */
  pauVar3 = (undefined (*) [16])operator_new(0x30);
  *pauVar3 = (undefined  [16])0x0;
                    /* try { // try from 0033cff0 to 0033d015 has its CatchHandler @ 0033d1ba */
  puVar4 = (undefined8 *)operator_new(0x10);
  *puVar4 = &PTR_FUN_0081fc38;
  puVar4[1] = this;
  *(undefined8 **)pauVar3[1] = puVar4;
  puVar5 = (undefined4 *)operator_new(0x18);
  *(undefined8 **)(puVar5 + 4) = puVar4;
  *(code **)(puVar5 + 2) = FUN_00340790;
  puVar5[1] = 1;
  *puVar5 = 1;
  *(undefined4 **)(pauVar3[1] + 8) = puVar5;
                    /* try { // try from 0033d042 to 0033d046 has its CatchHandler @ 0033d1ae */
  uVar6 = QString::fromAscii_helper("5.0",3);
  *(undefined8 *)pauVar3[2] = uVar6;
  puVar1 = PTR_shared_null_00837830;
  *(undefined (**) [16])(this + 0x10) = pauVar3;
  *(undefined **)(pauVar3[2] + 8) = puVar1;
                    /* try { // try from 0033d063 to 0033d067 has its CatchHandler @ 0033d1a2 */
  name((KisPaintOpPreset *)&local_48);
                    /* try { // try from 0033d074 to 0033d078 has its CatchHandler @ 0033d18a */
  local_38 = (QArrayData *)QString::fromAscii_helper(" ",1);
                    /* try { // try from 0033d08a to 0033d08e has its CatchHandler @ 0033d179 */
  local_40 = (QArrayData *)QString::fromAscii_helper("_",1);
                    /* try { // try from 0033d0ac to 0033d0bb has its CatchHandler @ 0033d196 */
  QString::replace((QString *)&local_48,(QString *)&local_40,(CaseSensitivity)&local_38);
  KoResource::setName((QString *)this);
  if (*(int *)local_40 == 0) {
LAB_0033d130:
    QArrayData::deallocate(local_40,2,8);
    iVar2 = *(int *)local_38;
    if (iVar2 != 0) goto LAB_0033d0e2;
LAB_0033d14a:
    QArrayData::deallocate(local_38,2,8);
    iVar2 = *(int *)local_48;
  }
  else {
    if (*(int *)local_40 != -1) {
      LOCK();
      *(int *)local_40 = *(int *)local_40 + -1;
      UNLOCK();
      if (*(int *)local_40 == 0) goto LAB_0033d130;
    }
    iVar2 = *(int *)local_38;
    if (iVar2 == 0) goto LAB_0033d14a;
LAB_0033d0e2:
    if (iVar2 != -1) {
      LOCK();
      *(int *)local_38 = *(int *)local_38 + -1;
      UNLOCK();
      if (*(int *)local_38 == 0) goto LAB_0033d14a;
    }
    iVar2 = *(int *)local_48;
  }
  if (iVar2 != 0) {
    if (iVar2 == -1) goto LAB_0033d10b;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0033d10b;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0033d10b:
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisPaintOpRegistry @ 00342c60 ======

/* KisPaintOpRegistry::KisPaintOpRegistry() */

void __thiscall KisPaintOpRegistry::KisPaintOpRegistry(KisPaintOpRegistry *this)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  
  puVar3 = PTR_vtable_00837ad0;
  puVar2 = PTR_shared_null_00837830;
  puVar1 = PTR_vtable_00837ad0 + 0x80;
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = puVar3 + 0x10;
  *(undefined **)(this + 0x10) = puVar1;
  *(undefined **)(this + 0x18) = puVar2;
  puVar1 = PTR_shared_null_00836c40;
  *(undefined **)(this + 0x20) = PTR_shared_null_00836c40;
  *(undefined **)(this + 0x28) = puVar1;
  return;
}



// ====== KisPaintOpSettings @ 00349af0 ======

/* KisPaintOpSettings::KisPaintOpSettings(QSharedPointer<KisResourcesInterface>) */

void __thiscall
KisPaintOpSettings::KisPaintOpSettings(KisPaintOpSettings *this,QSharedPointer param_1)

{
  int *piVar1;
  undefined8 uVar2;
  int *piVar3;
  undefined *puVar4;
  undefined (*pauVar5) [16];
  undefined8 uVar6;
  undefined4 in_register_00000034;
  
  KisPropertiesConfiguration::KisPropertiesConfiguration((KisPropertiesConfiguration *)this);
  *(undefined **)this = PTR_vtable_00837878 + 0x10;
                    /* try { // try from 00349b1d to 00349b21 has its CatchHandler @ 00349c07 */
  pauVar5 = (undefined (*) [16])operator_new(0x80);
  *pauVar5 = (undefined  [16])0x0;
  puVar4 = PTR_shared_null_008377d0;
  *(undefined8 *)(pauVar5[1] + 8) = 0;
  *(undefined **)pauVar5[1] = puVar4;
  puVar4 = PTR_shared_null_00837830;
  *(undefined8 *)pauVar5[2] = 0;
  *(undefined **)(pauVar5[2] + 8) = puVar4;
  *(undefined8 *)pauVar5[3] = 0;
  *(undefined8 *)(pauVar5[3] + 8) = 0;
  *(undefined8 *)pauVar5[4] = 0;
  *(undefined8 *)(pauVar5[4] + 8) = 0;
  *(undefined8 *)pauVar5[5] = 0;
  *(undefined8 *)(pauVar5[5] + 8) = 0;
                    /* try { // try from 00349b8b to 00349b8f has its CatchHandler @ 00349c1f */
  KisRandomSource::KisRandomSource((KisRandomSource *)(pauVar5 + 6),(int)pauVar5);
                    /* try { // try from 00349b93 to 00349b97 has its CatchHandler @ 00349c13 */
  uVar6 = KisRandomSource::generate();
  *(undefined (**) [16])(this + 0x20) = pauVar5;
  uVar2 = *(undefined8 *)CONCAT44(in_register_00000034,param_1);
  piVar3 = (int *)((undefined8 *)CONCAT44(in_register_00000034,param_1))[1];
  *(undefined8 *)(pauVar5[7] + 8) = uVar6;
  if (piVar3 != (int *)0x0) {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
    LOCK();
    piVar3[1] = piVar3[1] + 1;
    UNLOCK();
  }
  piVar1 = *(int **)(pauVar5[3] + 8);
  *(undefined8 *)pauVar5[3] = uVar2;
  *(int **)(pauVar5[3] + 8) = piVar3;
  if (piVar1 != (int *)0x0) {
    LOCK();
    piVar3 = piVar1 + 1;
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      (**(code **)(piVar1 + 2))(piVar1);
    }
    LOCK();
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      operator_delete(piVar1,0x10);
      return;
    }
  }
  return;
}



// ====== KisPaintOpSettings @ 00349df0 ======

/* KisPaintOpSettings::KisPaintOpSettings(KisPaintOpSettings const&) */

void __thiscall
KisPaintOpSettings::KisPaintOpSettings(KisPaintOpSettings *this,KisPaintOpSettings *param_1)

{
  long lVar1;
  int *piVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined (*pauVar5) [16];
  
  KisPropertiesConfiguration::KisPropertiesConfiguration
            ((KisPropertiesConfiguration *)this,(KisPropertiesConfiguration *)param_1);
  *(undefined **)this = PTR_vtable_00837878 + 0x10;
                    /* try { // try from 00349e17 to 00349e1b has its CatchHandler @ 00349ed9 */
  pauVar5 = (undefined (*) [16])operator_new(0x80);
  lVar1 = *(long *)(param_1 + 0x20);
  *pauVar5 = (undefined  [16])0x0;
  piVar2 = *(int **)(lVar1 + 0x10);
  *(int **)pauVar5[1] = piVar2;
  if (1 < *piVar2 + 1U) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  puVar4 = PTR_shared_null_00837830;
  *(undefined (*) [16])(pauVar5[1] + 8) = (undefined  [16])0x0;
  *(undefined **)(pauVar5[2] + 8) = puVar4;
  *(undefined8 *)pauVar5[3] = *(undefined8 *)(lVar1 + 0x30);
  piVar2 = *(int **)(lVar1 + 0x38);
  *(int **)(pauVar5[3] + 8) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    piVar2 = (int *)(*(long *)(pauVar5[3] + 8) + 4);
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  uVar3 = *(undefined8 *)(lVar1 + 0x48);
  piVar2 = *(int **)(lVar1 + 0x48);
  *(undefined8 *)pauVar5[4] = *(undefined8 *)(lVar1 + 0x40);
  *(undefined8 *)(pauVar5[4] + 8) = uVar3;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    piVar2 = (int *)(*(long *)(pauVar5[4] + 8) + 4);
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  uVar3 = *(undefined8 *)(lVar1 + 0x58);
  piVar2 = *(int **)(lVar1 + 0x58);
  *(undefined8 *)pauVar5[5] = *(undefined8 *)(lVar1 + 0x50);
  *(undefined8 *)(pauVar5[5] + 8) = uVar3;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    piVar2 = (int *)(*(long *)(pauVar5[5] + 8) + 4);
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
                    /* try { // try from 00349eb9 to 00349ebd has its CatchHandler @ 00349ee5 */
  KisRandomSource::KisRandomSource((KisRandomSource *)(pauVar5 + 6),(int)pauVar5);
  uVar3 = *(undefined8 *)(lVar1 + 0x78);
  *(undefined (**) [16])(this + 0x20) = pauVar5;
  *(undefined8 *)(pauVar5[7] + 8) = uVar3;
  return;
}



// ====== KisPaintOpPresetUpdateProxy @ 0034d080 ======

/* KisPaintOpPresetUpdateProxy::KisPaintOpPresetUpdateProxy() */

void __thiscall
KisPaintOpPresetUpdateProxy::KisPaintOpPresetUpdateProxy(KisPaintOpPresetUpdateProxy *this)

{
  KisSignalCompressor *this_00;
  long in_FS_OFFSET;
  QObject aQStack_28 [8];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837b78 + 0x10;
                    /* try { // try from 0034d0b9 to 0034d0bd has its CatchHandler @ 0034d141 */
  this_00 = (KisSignalCompressor *)operator_new(0x70);
                    /* try { // try from 0034d0d0 to 0034d0d4 has its CatchHandler @ 0034d135 */
  KisSignalCompressor::KisSignalCompressor(this_00,100,2,(QObject *)0x0);
  *(KisSignalCompressor **)(this + 0x10) = this_00;
  *(undefined8 *)(this_00 + 0x68) = 0;
                    /* try { // try from 0034d0fe to 0034d102 has its CatchHandler @ 0034d129 */
  QObject::connect(aQStack_28,(char *)this_00,(QObject *)"2timeout()",(char *)this,0x723830);
  QMetaObject::Connection::~Connection((Connection *)aQStack_28);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisNoSizePaintOpSettings @ 0034ed80 ======

/* KisNoSizePaintOpSettings::KisNoSizePaintOpSettings(QSharedPointer<KisResourcesInterface>) */

void __thiscall
KisNoSizePaintOpSettings::KisNoSizePaintOpSettings
          (KisNoSizePaintOpSettings *this,QSharedPointer param_1)

{
  int *piVar1;
  int *piVar2;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  undefined8 local_38;
  int *piStack_30;
  long local_20;
  
  local_38 = *(undefined8 *)CONCAT44(in_register_00000034,param_1);
  piStack_30 = (int *)((undefined8 *)CONCAT44(in_register_00000034,param_1))[1];
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (piStack_30 != (int *)0x0) {
    LOCK();
    *piStack_30 = *piStack_30 + 1;
    UNLOCK();
    LOCK();
    piStack_30[1] = piStack_30[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 0034edc3 to 0034edc7 has its CatchHandler @ 0034ee24 */
  KisPaintOpSettings::KisPaintOpSettings((KisPaintOpSettings *)this,(QSharedPointer)&local_38);
  piVar2 = piStack_30;
  if (piStack_30 != (int *)0x0) {
    LOCK();
    piVar1 = piStack_30 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piStack_30 + 2))(piStack_30);
    }
    LOCK();
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (*piVar2 == 0) {
      operator_delete(piVar2,0x10);
    }
  }
  *(undefined **)this = PTR_vtable_00836d38 + 0x10;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPaintOpConfigWidget @ 00352c00 ======

/* KisPaintOpConfigWidget::KisPaintOpConfigWidget(QWidget*, QFlags<Qt::WindowType>) */

void __thiscall
KisPaintOpConfigWidget::KisPaintOpConfigWidget
          (KisPaintOpConfigWidget *this,QWidget *param_1,QFlags param_2)

{
  undefined *puVar1;
  
  KisConfigWidget::KisConfigWidget((KisConfigWidget *)this,param_1,param_2,100);
  puVar1 = PTR_vtable_00836fc0;
  *(undefined4 *)(this + 0xd8) = 0;
  *(undefined (*) [16])(this + 0xa8) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0xb8) = (undefined  [16])0x0;
  *(undefined **)this = puVar1 + 0x10;
  *(undefined **)(this + 0x10) = puVar1 + 0x228;
  *(undefined (*) [16])(this + 200) = (undefined  [16])0x0;
  return;
}



// ====== KisCallbackBasedPaintopProperty @ 00353a60 ======

/* KisCallbackBasedPaintopProperty<KisUniformPaintOpProperty>::KisCallbackBasedPaintopProperty(KisUniformPaintOpProperty::Type,
   KisUniformPaintOpProperty::SubType, KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisUniformPaintOpProperty>::KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisUniformPaintOpProperty> *this,Type param_1,
          SubType param_2,KoID *param_3,KisRestrictedSharedPtr param_4,QObject *param_5)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  long *local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long **)CONCAT44(in_register_00000084,param_4);
  if (local_18 != (long *)0x0) {
    LOCK();
    *(int *)(local_18 + 1) = *(int *)(local_18 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 00353a91 to 00353a95 has its CatchHandler @ 00353b36 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,param_3,
             (KisRestrictedSharedPtr)&local_18,param_5);
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar2 = PTR_vtable_00836c10;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined8 *)(this + 0x30) = 0;
  *(undefined8 *)(this + 0x48) = 0;
  *(undefined **)this = puVar2 + 0x10;
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined8 *)(this + 0x68) = 0;
  *(undefined8 *)(this + 0x70) = 0;
  *(undefined (*) [16])(this + 0x18) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x58) = (undefined  [16])0x0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 00353b50 ======

/* KisCallbackBasedPaintopProperty<KisUniformPaintOpProperty>::KisCallbackBasedPaintopProperty(KisUniformPaintOpProperty::Type,
   KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisUniformPaintOpProperty>::KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisUniformPaintOpProperty> *this,Type param_1,
          KoID *param_2,KisRestrictedSharedPtr param_3,QObject *param_4)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 in_register_0000000c;
  long in_FS_OFFSET;
  long *local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (local_18 != (long *)0x0) {
    LOCK();
    *(int *)(local_18 + 1) = *(int *)(local_18 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 00353b81 to 00353b85 has its CatchHandler @ 00353c26 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,(KisRestrictedSharedPtr)&local_18,
             param_4);
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar2 = PTR_vtable_00836c10;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined8 *)(this + 0x30) = 0;
  *(undefined8 *)(this + 0x48) = 0;
  *(undefined **)this = puVar2 + 0x10;
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined8 *)(this + 0x68) = 0;
  *(undefined8 *)(this + 0x70) = 0;
  *(undefined (*) [16])(this + 0x18) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x58) = (undefined  [16])0x0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 00353c40 ======

/* KisCallbackBasedPaintopProperty<KisUniformPaintOpProperty>::KisCallbackBasedPaintopProperty(KoID
   const&, KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisUniformPaintOpProperty>::KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisUniformPaintOpProperty> *this,KoID *param_1,
          KisRestrictedSharedPtr param_2,QObject *param_3)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  long *local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_18 != (long *)0x0) {
    LOCK();
    *(int *)(local_18 + 1) = *(int *)(local_18 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 00353c71 to 00353c75 has its CatchHandler @ 00353d16 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,(KisRestrictedSharedPtr)&local_18,param_3);
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar2 = PTR_vtable_00836c10;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined8 *)(this + 0x30) = 0;
  *(undefined8 *)(this + 0x48) = 0;
  *(undefined **)this = puVar2 + 0x10;
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined8 *)(this + 0x68) = 0;
  *(undefined8 *)(this + 0x70) = 0;
  *(undefined (*) [16])(this + 0x18) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x58) = (undefined  [16])0x0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 003546c0 ======

/* KisCallbackBasedPaintopProperty<KisComboBasedPaintOpProperty>::KisCallbackBasedPaintopProperty(KoID
   const&, KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisComboBasedPaintOpProperty>::KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisComboBasedPaintOpProperty> *this,KoID *param_1,
          KisRestrictedSharedPtr param_2,QObject *param_3)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  long *local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_18 != (long *)0x0) {
    LOCK();
    *(int *)(local_18 + 1) = *(int *)(local_18 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 003546f1 to 003546f5 has its CatchHandler @ 00354796 */
  KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty
            ((KisComboBasedPaintOpProperty *)this,param_1,(KisRestrictedSharedPtr)&local_18,param_3)
  ;
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar2 = PTR_vtable_00837298;
  *(undefined8 *)(this + 0x30) = 0;
  *(undefined8 *)(this + 0x38) = 0;
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined **)this = puVar2 + 0x10;
  *(undefined8 *)(this + 0x58) = 0;
  *(undefined8 *)(this + 0x70) = 0;
  *(undefined8 *)(this + 0x78) = 0;
  *(undefined (*) [16])(this + 0x20) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x60) = (undefined  [16])0x0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 00354d50 ======

/* KisCallbackBasedPaintopProperty<KisComboBasedPaintOpProperty>::KisCallbackBasedPaintopProperty(KisUniformPaintOpProperty::Type,
   KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisComboBasedPaintOpProperty>::KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisComboBasedPaintOpProperty> *this,Type param_1,
          KoID *param_2,KisRestrictedSharedPtr param_3,QObject *param_4)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 in_register_0000000c;
  long in_FS_OFFSET;
  long *local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (local_18 != (long *)0x0) {
    LOCK();
    *(int *)(local_18 + 1) = *(int *)(local_18 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 00354d81 to 00354d85 has its CatchHandler @ 00354e26 */
  KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty
            ((KisComboBasedPaintOpProperty *)this,param_1,param_2,(KisRestrictedSharedPtr)&local_18,
             param_4);
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar2 = PTR_vtable_00837298;
  *(undefined8 *)(this + 0x30) = 0;
  *(undefined8 *)(this + 0x38) = 0;
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined **)this = puVar2 + 0x10;
  *(undefined8 *)(this + 0x58) = 0;
  *(undefined8 *)(this + 0x70) = 0;
  *(undefined8 *)(this + 0x78) = 0;
  *(undefined (*) [16])(this + 0x20) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x60) = (undefined  [16])0x0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 00354e40 ======

/* KisCallbackBasedPaintopProperty<KisComboBasedPaintOpProperty>::KisCallbackBasedPaintopProperty(KisUniformPaintOpProperty::Type,
   KisUniformPaintOpProperty::SubType, KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisComboBasedPaintOpProperty>::KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisComboBasedPaintOpProperty> *this,Type param_1,
          SubType param_2,KoID *param_3,KisRestrictedSharedPtr param_4,QObject *param_5)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  long *local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long **)CONCAT44(in_register_00000084,param_4);
  if (local_18 != (long *)0x0) {
    LOCK();
    *(int *)(local_18 + 1) = *(int *)(local_18 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 00354e71 to 00354e75 has its CatchHandler @ 00354f16 */
  KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty
            ((KisComboBasedPaintOpProperty *)this,param_1,param_2,param_3,
             (KisRestrictedSharedPtr)&local_18,param_5);
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar2 = PTR_vtable_00837298;
  *(undefined8 *)(this + 0x30) = 0;
  *(undefined8 *)(this + 0x38) = 0;
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined **)this = puVar2 + 0x10;
  *(undefined8 *)(this + 0x58) = 0;
  *(undefined8 *)(this + 0x70) = 0;
  *(undefined8 *)(this + 0x78) = 0;
  *(undefined (*) [16])(this + 0x20) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x60) = (undefined  [16])0x0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 00355dc0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<int>
   >::KisCallbackBasedPaintopProperty(KisUniformPaintOpProperty::Type,
   KisUniformPaintOpProperty::SubType, KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<int>>::KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<int>> *this,Type param_1,
          SubType param_2,KoID *param_3,KisRestrictedSharedPtr param_4,QObject *param_5)

{
  long *plVar1;
  long *plVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined *puVar5;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  long *local_28;
  long local_20;
  
  plVar2 = *(long **)CONCAT44(in_register_00000084,param_4);
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (plVar2 == (long *)0x0) {
    local_28 = (long *)0x0;
  }
  else {
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    local_28 = plVar2;
  }
                    /* try { // try from 00355dfd to 00355e01 has its CatchHandler @ 00355f31 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,param_3,
             (KisRestrictedSharedPtr)&local_28,param_5);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
  puVar5 = PTR_vtable_00837dd0;
  uVar4 = _UNK_00723cd8;
  uVar3 = _DAT_00723cd0;
  *(undefined4 *)(this + 0x30) = 2;
  *(undefined8 *)(this + 0x18) = uVar3;
  *(undefined8 *)(this + 0x20) = uVar4;
  *(undefined **)this = puVar5 + 0x10;
  *(undefined8 *)(this + 0x28) = DAT_00723cf0;
  *(undefined **)(this + 0x38) = PTR_shared_null_008377d0;
  if (plVar2 != (long *)0x0) {
    LOCK();
    plVar1 = plVar2 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*plVar2 + 8))(plVar2);
    }
  }
  puVar5 = PTR_vtable_00836c08;
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined8 *)(this + 0x58) = 0;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  *(undefined **)this = puVar5 + 0x10;
  *(undefined8 *)(this + 0x70) = 0;
  *(undefined8 *)(this + 0x78) = 0;
  *(undefined8 *)(this + 0x90) = 0;
  *(undefined8 *)(this + 0x98) = 0;
  *(undefined (*) [16])(this + 0x60) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x80) = (undefined  [16])0x0;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 00355f50 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<int>
   >::KisCallbackBasedPaintopProperty(KisUniformPaintOpProperty::Type, KoID const&,
   KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<int>>::KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<int>> *this,Type param_1,
          KoID *param_2,KisRestrictedSharedPtr param_3,QObject *param_4)

{
  long *plVar1;
  long *plVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined *puVar5;
  undefined4 in_register_0000000c;
  long in_FS_OFFSET;
  long *local_28;
  long local_20;
  
  plVar2 = *(long **)CONCAT44(in_register_0000000c,param_3);
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (plVar2 == (long *)0x0) {
    local_28 = (long *)0x0;
  }
  else {
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    local_28 = plVar2;
  }
                    /* try { // try from 00355f8d to 00355f91 has its CatchHandler @ 003560c1 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,(KisRestrictedSharedPtr)&local_28,
             param_4);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
  puVar5 = PTR_vtable_00837dd0;
  uVar4 = _UNK_00723cd8;
  uVar3 = _DAT_00723cd0;
  *(undefined4 *)(this + 0x30) = 2;
  *(undefined8 *)(this + 0x18) = uVar3;
  *(undefined8 *)(this + 0x20) = uVar4;
  *(undefined **)this = puVar5 + 0x10;
  *(undefined8 *)(this + 0x28) = DAT_00723cf0;
  *(undefined **)(this + 0x38) = PTR_shared_null_008377d0;
  if (plVar2 != (long *)0x0) {
    LOCK();
    plVar1 = plVar2 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*plVar2 + 8))(plVar2);
    }
  }
  puVar5 = PTR_vtable_00836c08;
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined8 *)(this + 0x58) = 0;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  *(undefined **)this = puVar5 + 0x10;
  *(undefined8 *)(this + 0x70) = 0;
  *(undefined8 *)(this + 0x78) = 0;
  *(undefined8 *)(this + 0x90) = 0;
  *(undefined8 *)(this + 0x98) = 0;
  *(undefined (*) [16])(this + 0x60) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x80) = (undefined  [16])0x0;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 003560e0 ======

/* KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<int>
   >::KisCallbackBasedPaintopProperty(KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<int>>::KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<int>> *this,KoID *param_1,
          KisRestrictedSharedPtr param_2,QObject *param_3)

{
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  long local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long *)CONCAT44(in_register_00000014,param_2);
  if (local_18 != 0) {
    LOCK();
    *(int *)(local_18 + 8) = *(int *)(local_18 + 8) + 1;
    UNLOCK();
  }
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* try { // try from 0035611b to 0035611f has its CatchHandler @ 0035613e */
    KisSliderBasedPaintOpProperty<int>::KisSliderBasedPaintOpProperty
              ((KisSliderBasedPaintOpProperty<int> *)this,param_1,(KisRestrictedSharedPtr)&local_18,
               param_3);
    FUN_00354f30(local_18);
    if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
      _Unwind_Resume();
    }
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 00356450 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>
   >::KisCallbackBasedPaintopProperty(KisUniformPaintOpProperty::Type,
   KisUniformPaintOpProperty::SubType, KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>>::
KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>> *this,Type param_1
          ,SubType param_2,KoID *param_3,KisRestrictedSharedPtr param_4,QObject *param_5)

{
  long *plVar1;
  long *plVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined *puVar6;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  long *local_28;
  long local_20;
  
  plVar2 = *(long **)CONCAT44(in_register_00000084,param_4);
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (plVar2 == (long *)0x0) {
    local_28 = (long *)0x0;
  }
  else {
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    local_28 = plVar2;
  }
                    /* try { // try from 0035648d to 00356491 has its CatchHandler @ 003565d1 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,param_3,
             (KisRestrictedSharedPtr)&local_28,param_5);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
  puVar6 = PTR_vtable_00837c18;
  uVar4 = _UNK_00723ce8;
  uVar3 = _DAT_00723ce0;
  *(undefined4 *)(this + 0x40) = 2;
  *(undefined8 *)(this + 0x18) = uVar3;
  *(undefined8 *)(this + 0x20) = uVar4;
  uVar5 = _UNK_00723cf8;
  uVar3 = DAT_00723cf0;
  *(undefined **)this = puVar6 + 0x10;
  uVar4 = DAT_00723cf0;
  *(undefined8 *)(this + 0x28) = uVar3;
  *(undefined8 *)(this + 0x30) = uVar5;
  *(undefined8 *)(this + 0x38) = uVar4;
  *(undefined **)(this + 0x48) = PTR_shared_null_008377d0;
  if (plVar2 != (long *)0x0) {
    LOCK();
    plVar1 = plVar2 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*plVar2 + 8))(plVar2);
    }
  }
  puVar6 = PTR_vtable_00837718;
  *(undefined8 *)(this + 0x60) = 0;
  *(undefined8 *)(this + 0x68) = 0;
  *(undefined (*) [16])(this + 0x50) = (undefined  [16])0x0;
  *(undefined **)this = puVar6 + 0x10;
  *(undefined8 *)(this + 0x80) = 0;
  *(undefined8 *)(this + 0x88) = 0;
  *(undefined8 *)(this + 0xa0) = 0;
  *(undefined8 *)(this + 0xa8) = 0;
  *(undefined (*) [16])(this + 0x70) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x90) = (undefined  [16])0x0;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 003565f0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>
   >::KisCallbackBasedPaintopProperty(KisUniformPaintOpProperty::Type, KoID const&,
   KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>>::
KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>> *this,Type param_1
          ,KoID *param_2,KisRestrictedSharedPtr param_3,QObject *param_4)

{
  long *plVar1;
  long *plVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined *puVar6;
  undefined4 in_register_0000000c;
  long in_FS_OFFSET;
  long *local_28;
  long local_20;
  
  plVar2 = *(long **)CONCAT44(in_register_0000000c,param_3);
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (plVar2 == (long *)0x0) {
    local_28 = (long *)0x0;
  }
  else {
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    local_28 = plVar2;
  }
                    /* try { // try from 0035662d to 00356631 has its CatchHandler @ 00356771 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,(KisRestrictedSharedPtr)&local_28,
             param_4);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
  puVar6 = PTR_vtable_00837c18;
  uVar4 = _UNK_00723ce8;
  uVar3 = _DAT_00723ce0;
  *(undefined4 *)(this + 0x40) = 2;
  *(undefined8 *)(this + 0x18) = uVar3;
  *(undefined8 *)(this + 0x20) = uVar4;
  uVar5 = _UNK_00723cf8;
  uVar3 = DAT_00723cf0;
  *(undefined **)this = puVar6 + 0x10;
  uVar4 = DAT_00723cf0;
  *(undefined8 *)(this + 0x28) = uVar3;
  *(undefined8 *)(this + 0x30) = uVar5;
  *(undefined8 *)(this + 0x38) = uVar4;
  *(undefined **)(this + 0x48) = PTR_shared_null_008377d0;
  if (plVar2 != (long *)0x0) {
    LOCK();
    plVar1 = plVar2 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*plVar2 + 8))(plVar2);
    }
  }
  puVar6 = PTR_vtable_00837718;
  *(undefined8 *)(this + 0x60) = 0;
  *(undefined8 *)(this + 0x68) = 0;
  *(undefined (*) [16])(this + 0x50) = (undefined  [16])0x0;
  *(undefined **)this = puVar6 + 0x10;
  *(undefined8 *)(this + 0x80) = 0;
  *(undefined8 *)(this + 0x88) = 0;
  *(undefined8 *)(this + 0xa0) = 0;
  *(undefined8 *)(this + 0xa8) = 0;
  *(undefined (*) [16])(this + 0x70) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x90) = (undefined  [16])0x0;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisCallbackBasedPaintopProperty @ 00356790 ======

/* KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>
   >::KisCallbackBasedPaintopProperty(KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>>::
KisCallbackBasedPaintopProperty
          (KisCallbackBasedPaintopProperty<KisSliderBasedPaintOpProperty<double>> *this,
          KoID *param_1,KisRestrictedSharedPtr param_2,QObject *param_3)

{
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  long local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long *)CONCAT44(in_register_00000014,param_2);
  if (local_18 != 0) {
    LOCK();
    *(int *)(local_18 + 8) = *(int *)(local_18 + 8) + 1;
    UNLOCK();
  }
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* try { // try from 003567cb to 003567cf has its CatchHandler @ 003567ee */
    KisSliderBasedPaintOpProperty<double>::KisSliderBasedPaintOpProperty
              ((KisSliderBasedPaintOpProperty<double> *)this,param_1,
               (KisRestrictedSharedPtr)&local_18,param_3);
    FUN_00354f30(local_18);
    if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
      _Unwind_Resume();
    }
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisStrokeSpeedMeasurer @ 00357be0 ======

/* KisStrokeSpeedMeasurer::KisStrokeSpeedMeasurer(int) */

void __thiscall
KisStrokeSpeedMeasurer::KisStrokeSpeedMeasurer(KisStrokeSpeedMeasurer *this,int param_1)

{
  undefined *puVar1;
  int *piVar2;
  
  piVar2 = (int *)operator_new(0x30);
  puVar1 = PTR_shared_null_00837830;
  piVar2[8] = 0;
  piVar2[10] = 0;
  piVar2[0xb] = 0;
  *(undefined **)(piVar2 + 2) = puVar1;
  *(int **)this = piVar2;
  *piVar2 = param_1;
  *(undefined (*) [16])(piVar2 + 4) = (undefined  [16])0x0;
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



