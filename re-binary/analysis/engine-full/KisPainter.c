/* Class KisPainter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPainter @ 00202620 ======

void __thiscall KisPainter::KisPainter(KisPainter *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisPainter_00838de0)();
  return;
}



// ====== KisPainter @ 002026f0 ======

void __thiscall KisPainter::KisPainter(KisPainter *this,KisSharedPtr param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisPainter_00838e48)();
  return;
}



// ====== KisPainter @ 00205280 ======

void __thiscall KisPainter::KisPainter(KisPainter *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisPainter_0083a410)();
  return;
}



// ====== KisPainter @ 00207310 ======

void __thiscall KisPainter::KisPainter(KisPainter *this)

{
  (*(code *)PTR_KisPainter_0083b458)();
  return;
}



// ====== KisPainter @ 00208610 ======

void __thiscall KisPainter::KisPainter(KisPainter *this)

{
  (*(code *)PTR_KisPainter_0083bdd8)();
  return;
}



// ====== KisPainter @ 0031d520 ======

/* KisPainter::KisPainter() */

void __thiscall KisPainter::KisPainter(KisPainter *this)

{
  undefined8 uVar1;
  undefined *puVar2;
  undefined8 *puVar3;
  
  *(undefined **)this = PTR_vtable_00837e90 + 0x10;
  puVar3 = (undefined8 *)operator_new(0x2a0);
  puVar2 = PTR_shared_null_008377d0;
  *puVar3 = this;
  puVar3[1] = 0;
  puVar3[2] = 0;
  puVar3[3] = 0;
  puVar3[4] = 0;
  puVar3[5] = puVar2;
  puVar3[6] = 0;
                    /* try { // try from 0031d580 to 0031d584 has its CatchHandler @ 0031d6a3 */
  KoColor::KoColor((KoColor *)(puVar3 + 7));
                    /* try { // try from 0031d589 to 0031d58d has its CatchHandler @ 0031d6d3 */
  KoColor::KoColor((KoColor *)(puVar3 + 0xf));
                    /* try { // try from 0031d595 to 0031d599 has its CatchHandler @ 0031d6bb */
  KoColor::KoColor((KoColor *)(puVar3 + 0x17));
  uVar1 = DAT_00721bd8;
  puVar3[0x2c] = puVar2;
  *(undefined *)(puVar3 + 0x22) = 1;
  puVar3[0x21] = uVar1;
  *(undefined4 *)(puVar3 + 0x27) = 0;
  puVar3[0x2d] = 0;
  puVar3[0x2e] = 0;
  puVar3[0x2f] = 0;
  puVar3[0x30] = 0;
  *(undefined (*) [16])(puVar3 + 0x1f) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar3 + 0x23) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar3 + 0x25) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar3 + 0x28) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar3 + 0x2a) = (undefined  [16])0x0;
  QImage::QImage((QImage *)(puVar3 + 0x31));
  uVar1 = DAT_00722b70;
  *(undefined *)((long)puVar3 + 0x1da) = 1;
  puVar3[0x37] = 0;
  puVar3[0x38] = uVar1;
  *(undefined2 *)(puVar3 + 0x3b) = 0;
  *(undefined (*) [16])(puVar3 + 0x35) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar3 + 0x39) = (undefined  [16])0x0;
                    /* try { // try from 0031d66c to 0031d670 has its CatchHandler @ 0031d6c7 */
  KoCompositeOp::ParameterInfo::ParameterInfo((ParameterInfo *)(puVar3 + 0x3c));
  puVar3[0x46] = 0;
  *(undefined (*) [16])(puVar3 + 0x47) = (undefined  [16])0x0;
                    /* try { // try from 0031d68e to 0031d692 has its CatchHandler @ 0031d6af */
  QTransform::QTransform((QTransform *)(puVar3 + 0x49));
  *(undefined8 **)(this + 8) = puVar3;
  init(this);
  return;
}



// ====== KisPainter @ 0031d6e0 ======

/* KisPainter::KisPainter(KisSharedPtr<KisPaintDevice>, KisSharedPtr<KisSelection>) */

void __thiscall KisPainter::KisPainter(KisPainter *this,KisSharedPtr param_1,KisSharedPtr param_2)

{
  long lVar1;
  long *plVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined8 *puVar5;
  KoColorSpace *pKVar6;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long *plVar7;
  long in_FS_OFFSET;
  long *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  *(undefined **)this = PTR_vtable_00837e90 + 0x10;
  puVar5 = (undefined8 *)operator_new(0x2a0);
                    /* try { // try from 0031d728 to 0031d72c has its CatchHandler @ 0031d994 */
  pKVar6 = (KoColorSpace *)
           KisPaintDevice::colorSpace(*(KisPaintDevice **)CONCAT44(in_register_00000034,param_1));
  puVar4 = PTR_shared_null_008377d0;
  *puVar5 = this;
  puVar5[1] = 0;
  puVar5[2] = 0;
  puVar5[3] = 0;
  puVar5[4] = 0;
  puVar5[5] = puVar4;
  puVar5[6] = 0;
                    /* try { // try from 0031d76d to 0031d771 has its CatchHandler @ 0031d988 */
  KoColor::KoColor((KoColor *)(puVar5 + 7),pKVar6);
                    /* try { // try from 0031d779 to 0031d77d has its CatchHandler @ 0031d97c */
  KoColor::KoColor((KoColor *)(puVar5 + 0xf),pKVar6);
                    /* try { // try from 0031d785 to 0031d789 has its CatchHandler @ 0031d970 */
  KoColor::KoColor((KoColor *)(puVar5 + 0x17));
  uVar3 = DAT_00721bd8;
  *(undefined *)(puVar5 + 0x22) = 1;
  puVar5[0x2c] = puVar4;
  puVar5[0x21] = uVar3;
  *(undefined4 *)(puVar5 + 0x27) = 0;
  puVar5[0x2d] = 0;
  puVar5[0x2e] = 0;
  puVar5[0x2f] = 0;
  puVar5[0x30] = 0;
  *(undefined (*) [16])(puVar5 + 0x1f) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar5 + 0x23) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar5 + 0x25) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar5 + 0x28) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar5 + 0x2a) = (undefined  [16])0x0;
  QImage::QImage((QImage *)(puVar5 + 0x31));
  uVar3 = DAT_00722b70;
  *(undefined *)((long)puVar5 + 0x1da) = 1;
  puVar5[0x37] = 0;
  puVar5[0x38] = uVar3;
  *(undefined2 *)(puVar5 + 0x3b) = 0;
  *(undefined (*) [16])(puVar5 + 0x35) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar5 + 0x39) = (undefined  [16])0x0;
                    /* try { // try from 0031d85c to 0031d860 has its CatchHandler @ 0031d964 */
  KoCompositeOp::ParameterInfo::ParameterInfo((ParameterInfo *)(puVar5 + 0x3c));
  puVar5[0x46] = 0;
  *(undefined (*) [16])(puVar5 + 0x47) = (undefined  [16])0x0;
                    /* try { // try from 0031d87e to 0031d882 has its CatchHandler @ 0031d958 */
  QTransform::QTransform((QTransform *)(puVar5 + 0x49));
  *(undefined8 **)(this + 8) = puVar5;
  init(this);
  local_48 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_48 != (long *)0x0) {
    LOCK();
    *(int *)(local_48 + 2) = *(int *)(local_48 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 0031d8a7 to 0031d8ab has its CatchHandler @ 0031d94c */
  begin(this,(KisSharedPtr)&local_48);
  if (local_48 != (long *)0x0) {
    LOCK();
    plVar2 = local_48 + 2;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if (*(int *)plVar2 == 0) {
      (**(code **)(*local_48 + 0x20))();
    }
  }
  lVar1 = *(long *)(this + 8);
  plVar2 = *(long **)CONCAT44(in_register_00000014,param_2);
  plVar7 = *(long **)(lVar1 + 0x10);
  if (plVar2 != plVar7) {
    if (plVar2 != (long *)0x0) {
      LOCK();
      *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
      UNLOCK();
      plVar7 = *(long **)(lVar1 + 0x10);
    }
    *(long **)(lVar1 + 0x10) = plVar2;
    if (plVar7 != (long *)0x0) {
      LOCK();
      plVar2 = plVar7 + 1;
      *(int *)plVar2 = *(int *)plVar2 + -1;
      UNLOCK();
      if (*(int *)plVar2 == 0) {
        if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0031d945. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          (**(code **)(*plVar7 + 8))();
          return;
        }
        goto LAB_0031d947;
      }
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_0031d947:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPainter @ 003208e0 ======

/* KisPainter::KisPainter(KisSharedPtr<KisPaintDevice>) */

void __thiscall KisPainter::KisPainter(KisPainter *this,KisSharedPtr param_1)

{
  long *plVar1;
  undefined8 uVar2;
  undefined *puVar3;
  undefined8 *puVar4;
  KoColorSpace *pKVar5;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  *(undefined **)this = PTR_vtable_00837e90 + 0x10;
  puVar4 = (undefined8 *)operator_new(0x2a0);
                    /* try { // try from 00320923 to 00320927 has its CatchHandler @ 00320b2d */
  pKVar5 = (KoColorSpace *)
           KisPaintDevice::colorSpace(*(KisPaintDevice **)CONCAT44(in_register_00000034,param_1));
  puVar3 = PTR_shared_null_008377d0;
  *puVar4 = this;
  puVar4[1] = 0;
  puVar4[2] = 0;
  puVar4[3] = 0;
  puVar4[4] = 0;
  puVar4[5] = puVar3;
  puVar4[6] = 0;
                    /* try { // try from 00320968 to 0032096c has its CatchHandler @ 00320b21 */
  KoColor::KoColor((KoColor *)(puVar4 + 7),pKVar5);
                    /* try { // try from 00320974 to 00320978 has its CatchHandler @ 00320b15 */
  KoColor::KoColor((KoColor *)(puVar4 + 0xf),pKVar5);
                    /* try { // try from 00320980 to 00320984 has its CatchHandler @ 00320b09 */
  KoColor::KoColor((KoColor *)(puVar4 + 0x17));
  uVar2 = DAT_00721bd8;
  *(undefined *)(puVar4 + 0x22) = 1;
  puVar4[0x2c] = puVar3;
  puVar4[0x21] = uVar2;
  *(undefined4 *)(puVar4 + 0x27) = 0;
  puVar4[0x2d] = 0;
  puVar4[0x2e] = 0;
  puVar4[0x2f] = 0;
  puVar4[0x30] = 0;
  *(undefined (*) [16])(puVar4 + 0x1f) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar4 + 0x23) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar4 + 0x25) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar4 + 0x28) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar4 + 0x2a) = (undefined  [16])0x0;
  QImage::QImage((QImage *)(puVar4 + 0x31));
  uVar2 = DAT_00722b70;
  *(undefined *)((long)puVar4 + 0x1da) = 1;
  puVar4[0x37] = 0;
  puVar4[0x38] = uVar2;
  *(undefined2 *)(puVar4 + 0x3b) = 0;
  *(undefined (*) [16])(puVar4 + 0x35) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar4 + 0x39) = (undefined  [16])0x0;
                    /* try { // try from 00320a57 to 00320a5b has its CatchHandler @ 00320afd */
  KoCompositeOp::ParameterInfo::ParameterInfo((ParameterInfo *)(puVar4 + 0x3c));
  puVar4[0x46] = 0;
  *(undefined (*) [16])(puVar4 + 0x47) = (undefined  [16])0x0;
                    /* try { // try from 00320a79 to 00320a7d has its CatchHandler @ 00320af1 */
  QTransform::QTransform((QTransform *)(puVar4 + 0x49));
  *(undefined8 **)(this + 8) = puVar4;
  init(this);
  local_38 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_38 != (long *)0x0) {
    LOCK();
    *(int *)(local_38 + 2) = *(int *)(local_38 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 00320aa2 to 00320aa6 has its CatchHandler @ 00320ae5 */
  begin(this,(KisSharedPtr)&local_38);
  if (local_38 != (long *)0x0) {
    LOCK();
    plVar1 = local_38 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_38 + 0x20))();
    }
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



