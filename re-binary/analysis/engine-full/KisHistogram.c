/* Class KisHistogram - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisHistogram @ 00509a80 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisHistogram::KisHistogram(KisSharedPtr<KisPaintLayer>, KoHistogramProducer*, enumHistogramType)
    */

void __thiscall
KisHistogram::KisHistogram
          (KisHistogram *this,KisSharedPtr param_1,KoHistogramProducer *param_2,
          enumHistogramType param_3)

{
  uint uVar1;
  long lVar2;
  long *plVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined *puVar6;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  undefined auVar7 [16];
  long *local_58;
  uint *local_50;
  
  lVar2 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)(this + 8));
  plVar3 = *(long **)CONCAT44(in_register_00000034,param_1);
  *(undefined **)this = PTR_vtable_00837508 + 0x10;
                    /* try { // try from 00509ad3 to 00509ad5 has its CatchHandler @ 00509c92 */
  (**(code **)(*plVar3 + 0x70))(this + 0x18);
  uVar5 = DAT_00721778;
  uVar4 = _DAT_00721770;
  this[0x50] = (KisHistogram)0x0;
  *(undefined8 *)(this + 0x30) = 0;
  *(undefined8 *)(this + 0x20) = uVar4;
  *(undefined8 *)(this + 0x28) = uVar5;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  puVar6 = PTR_shared_null_008377d0;
  *(undefined8 *)(this + 0x38) = 0;
  *(undefined **)(this + 0x58) = puVar6;
  *(undefined **)(this + 0x60) = puVar6;
                    /* try { // try from 00509b15 to 00509b19 has its CatchHandler @ 00509c86 */
  KisBaseNode::image();
  if (((local_50 == (uint *)0x0) || (local_58 == (long *)0x0)) || ((*local_50 & 1) == 0)) {
    if (local_50 != (uint *)0x0) {
      LOCK();
      uVar1 = *local_50;
      *local_50 = *local_50 - 2;
      UNLOCK();
      if (((int)uVar1 < 3) && (local_50 != (uint *)0x0)) {
        operator_delete(local_50,4);
      }
    }
    *(enumHistogramType *)(this + 0x38) = param_3;
    *(KoHistogramProducer **)(this + 0x30) = param_2;
    this[0x50] = (KisHistogram)0x0;
    *(undefined4 *)(this + 0x3c) = 0;
                    /* try { // try from 00509c1a to 00509c1e has its CatchHandler @ 00509c86 */
    updateHistogram(this);
  }
  else {
    plVar3 = local_58 + 10;
    LOCK();
    *(int *)(local_58 + 10) = *(int *)(local_58 + 10) + 1;
    UNLOCK();
    if (local_50 != (uint *)0x0) {
      LOCK();
      uVar1 = *local_50;
      *local_50 = *local_50 - 2;
      UNLOCK();
      if (((int)uVar1 < 3) && (local_50 != (uint *)0x0)) {
        operator_delete(local_50,4);
      }
    }
                    /* try { // try from 00509b73 to 00509b9b has its CatchHandler @ 00509c7a */
    auVar7 = (**(code **)(*local_58 + 0xe0))(local_58);
    *(undefined (*) [16])(this + 0x20) = auVar7;
    *(enumHistogramType *)(this + 0x38) = param_3;
    *(KoHistogramProducer **)(this + 0x30) = param_2;
    this[0x50] = (KisHistogram)0x0;
    *(undefined4 *)(this + 0x3c) = 0;
    updateHistogram(this);
    LOCK();
    *(int *)plVar3 = *(int *)plVar3 + -1;
    UNLOCK();
    if (*(int *)plVar3 == 0) {
      if (lVar2 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x00509c51. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        (**(code **)(*local_58 + 0x20))(local_58);
        return;
      }
      goto LAB_00509c75;
    }
  }
  if (lVar2 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_00509c75:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisHistogram @ 00509ca0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisHistogram::KisHistogram(KisSharedPtr<KisPaintDevice>, QRect const&, KoHistogramProducer*,
   enumHistogramType) */

void __thiscall
KisHistogram::KisHistogram
          (KisHistogram *this,KisSharedPtr param_1,QRect *param_2,KoHistogramProducer *param_3,
          enumHistogramType param_4)

{
  long lVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined4 in_register_00000034;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837508 + 0x10;
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x18) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  uVar3 = DAT_00721778;
  uVar2 = _DAT_00721770;
  *(undefined4 *)(this + 0x3c) = 0;
  this[0x50] = (KisHistogram)0x0;
  *(undefined8 *)(this + 0x20) = uVar2;
  *(undefined8 *)(this + 0x28) = uVar3;
  uVar2 = *(undefined8 *)param_2;
  uVar3 = *(undefined8 *)(param_2 + 8);
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  puVar4 = PTR_shared_null_008377d0;
  *(KoHistogramProducer **)(this + 0x30) = param_3;
  *(enumHistogramType *)(this + 0x38) = param_4;
  *(undefined **)(this + 0x58) = puVar4;
  *(undefined **)(this + 0x60) = puVar4;
  *(undefined8 *)(this + 0x20) = uVar2;
  *(undefined8 *)(this + 0x28) = uVar3;
                    /* try { // try from 00509d2f to 00509d33 has its CatchHandler @ 00509d43 */
  updateHistogram(this);
  return;
}



