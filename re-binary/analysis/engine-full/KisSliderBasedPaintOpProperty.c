/* Class KisSliderBasedPaintOpProperty - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSliderBasedPaintOpProperty @ 00204fa0 ======

void __thiscall
KisSliderBasedPaintOpProperty<double>::KisSliderBasedPaintOpProperty
          (KisSliderBasedPaintOpProperty<double> *this,KoID *param_1,KisRestrictedSharedPtr param_2,
          QObject *param_3)

{
  (*(code *)PTR_KisSliderBasedPaintOpProperty_0083a2a0)();
  return;
}



// ====== KisSliderBasedPaintOpProperty @ 0020a490 ======

void __thiscall
KisSliderBasedPaintOpProperty<int>::KisSliderBasedPaintOpProperty
          (KisSliderBasedPaintOpProperty<int> *this,KoID *param_1,KisRestrictedSharedPtr param_2,
          QObject *param_3)

{
  (*(code *)PTR_KisSliderBasedPaintOpProperty_0083cd18)();
  return;
}



// ====== KisSliderBasedPaintOpProperty @ 00355610 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSliderBasedPaintOpProperty<int>::KisSliderBasedPaintOpProperty(KisUniformPaintOpProperty::Type,
   KisUniformPaintOpProperty::SubType, KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisSliderBasedPaintOpProperty<int>::KisSliderBasedPaintOpProperty
          (KisSliderBasedPaintOpProperty<int> *this,Type param_1,SubType param_2,KoID *param_3,
          KisRestrictedSharedPtr param_4,QObject *param_5)

{
  long *plVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
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
                    /* try { // try from 00355641 to 00355645 has its CatchHandler @ 003556de */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,param_3,
             (KisRestrictedSharedPtr)&local_18,param_5);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar4 = PTR_vtable_00837dd0;
  uVar3 = _UNK_00723cd8;
  uVar2 = _DAT_00723cd0;
  *(undefined4 *)(this + 0x30) = 2;
  *(undefined8 *)(this + 0x18) = uVar2;
  *(undefined8 *)(this + 0x20) = uVar3;
  *(undefined **)this = puVar4 + 0x10;
  *(undefined8 *)(this + 0x28) = DAT_00723cf0;
  *(undefined **)(this + 0x38) = PTR_shared_null_008377d0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSliderBasedPaintOpProperty @ 003556f0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSliderBasedPaintOpProperty<int>::KisSliderBasedPaintOpProperty(KisUniformPaintOpProperty::Type,
   KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisSliderBasedPaintOpProperty<int>::KisSliderBasedPaintOpProperty
          (KisSliderBasedPaintOpProperty<int> *this,Type param_1,KoID *param_2,
          KisRestrictedSharedPtr param_3,QObject *param_4)

{
  long *plVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
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
                    /* try { // try from 00355721 to 00355725 has its CatchHandler @ 003557be */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,(KisRestrictedSharedPtr)&local_18,
             param_4);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar4 = PTR_vtable_00837dd0;
  uVar3 = _UNK_00723cd8;
  uVar2 = _DAT_00723cd0;
  *(undefined4 *)(this + 0x30) = 2;
  *(undefined8 *)(this + 0x18) = uVar2;
  *(undefined8 *)(this + 0x20) = uVar3;
  *(undefined **)this = puVar4 + 0x10;
  *(undefined8 *)(this + 0x28) = DAT_00723cf0;
  *(undefined **)(this + 0x38) = PTR_shared_null_008377d0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSliderBasedPaintOpProperty @ 003557d0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSliderBasedPaintOpProperty<int>::KisSliderBasedPaintOpProperty(KoID const&,
   KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisSliderBasedPaintOpProperty<int>::KisSliderBasedPaintOpProperty
          (KisSliderBasedPaintOpProperty<int> *this,KoID *param_1,KisRestrictedSharedPtr param_2,
          QObject *param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined *puVar3;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  long local_48;
  undefined local_40 [16];
  char *local_30;
  undefined8 local_20;
  
  local_20 = *(undefined8 *)(in_FS_OFFSET + 0x28);
  local_48 = *(long *)CONCAT44(in_register_00000014,param_2);
  if (local_48 != 0) {
    LOCK();
    *(int *)(local_48 + 8) = *(int *)(local_48 + 8) + 1;
    UNLOCK();
  }
                    /* try { // try from 0035580d to 00355811 has its CatchHandler @ 0035588e */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,0,param_1,(KisRestrictedSharedPtr)&local_48,param_3);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  FUN_00354f30(local_48);
  puVar3 = PTR_vtable_00837dd0;
  uVar2 = _UNK_00723cd8;
  uVar1 = _DAT_00723cd0;
  *(undefined4 *)(this + 0x30) = 2;
  *(undefined8 *)(this + 0x18) = uVar1;
  *(undefined8 *)(this + 0x20) = uVar2;
  *(undefined **)this = puVar3 + 0x10;
  local_48 = 2;
  *(undefined8 *)(this + 0x28) = DAT_00723cf0;
  local_40 = (undefined  [16])0x0;
  *(undefined **)(this + 0x38) = PTR_shared_null_008377d0;
  local_30 = "default";
                    /* WARNING: Subroutine does not return */
  QMessageLogger::fatal((char *)&local_48,"Should have never been called!");
}



// ====== KisSliderBasedPaintOpProperty @ 003559c0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSliderBasedPaintOpProperty<double>::KisSliderBasedPaintOpProperty(KisUniformPaintOpProperty::Type,
   KisUniformPaintOpProperty::SubType, KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisSliderBasedPaintOpProperty<double>::KisSliderBasedPaintOpProperty
          (KisSliderBasedPaintOpProperty<double> *this,Type param_1,SubType param_2,KoID *param_3,
          KisRestrictedSharedPtr param_4,QObject *param_5)

{
  long *plVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined *puVar5;
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
                    /* try { // try from 003559f1 to 003559f5 has its CatchHandler @ 00355a96 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,param_3,
             (KisRestrictedSharedPtr)&local_18,param_5);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar5 = PTR_vtable_00837c18;
  uVar3 = _UNK_00723ce8;
  uVar2 = _DAT_00723ce0;
  *(undefined4 *)(this + 0x40) = 2;
  *(undefined8 *)(this + 0x18) = uVar2;
  *(undefined8 *)(this + 0x20) = uVar3;
  uVar4 = _UNK_00723cf8;
  uVar2 = DAT_00723cf0;
  *(undefined **)this = puVar5 + 0x10;
  uVar3 = DAT_00723cf0;
  *(undefined8 *)(this + 0x28) = uVar2;
  *(undefined8 *)(this + 0x30) = uVar4;
  *(undefined8 *)(this + 0x38) = uVar3;
  *(undefined **)(this + 0x48) = PTR_shared_null_008377d0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSliderBasedPaintOpProperty @ 00355ab0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSliderBasedPaintOpProperty<double>::KisSliderBasedPaintOpProperty(KisUniformPaintOpProperty::Type,
   KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisSliderBasedPaintOpProperty<double>::KisSliderBasedPaintOpProperty
          (KisSliderBasedPaintOpProperty<double> *this,Type param_1,KoID *param_2,
          KisRestrictedSharedPtr param_3,QObject *param_4)

{
  long *plVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined *puVar5;
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
                    /* try { // try from 00355ae1 to 00355ae5 has its CatchHandler @ 00355b86 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,param_1,param_2,(KisRestrictedSharedPtr)&local_18,
             param_4);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar5 = PTR_vtable_00837c18;
  uVar3 = _UNK_00723ce8;
  uVar2 = _DAT_00723ce0;
  *(undefined4 *)(this + 0x40) = 2;
  *(undefined8 *)(this + 0x18) = uVar2;
  *(undefined8 *)(this + 0x20) = uVar3;
  uVar4 = _UNK_00723cf8;
  uVar2 = DAT_00723cf0;
  *(undefined **)this = puVar5 + 0x10;
  uVar3 = DAT_00723cf0;
  *(undefined8 *)(this + 0x28) = uVar2;
  *(undefined8 *)(this + 0x30) = uVar4;
  *(undefined8 *)(this + 0x38) = uVar3;
  *(undefined **)(this + 0x48) = PTR_shared_null_008377d0;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSliderBasedPaintOpProperty @ 00355ba0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSliderBasedPaintOpProperty<double>::KisSliderBasedPaintOpProperty(KoID const&,
   KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisSliderBasedPaintOpProperty<double>::KisSliderBasedPaintOpProperty
          (KisSliderBasedPaintOpProperty<double> *this,KoID *param_1,KisRestrictedSharedPtr param_2,
          QObject *param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  long local_48;
  undefined local_40 [16];
  char *local_30;
  undefined8 local_20;
  
  local_20 = *(undefined8 *)(in_FS_OFFSET + 0x28);
  local_48 = *(long *)CONCAT44(in_register_00000014,param_2);
  if (local_48 != 0) {
    LOCK();
    *(int *)(local_48 + 8) = *(int *)(local_48 + 8) + 1;
    UNLOCK();
  }
                    /* try { // try from 00355bdd to 00355be1 has its CatchHandler @ 00355c6a */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,0,param_1,(KisRestrictedSharedPtr)&local_48,param_3);
  *(undefined **)this = PTR_vtable_00836f48 + 0x10;
  FUN_00354f30(local_48);
  puVar4 = PTR_vtable_00837c18;
  uVar2 = _UNK_00723ce8;
  uVar1 = _DAT_00723ce0;
  *(undefined4 *)(this + 0x40) = 2;
  *(undefined8 *)(this + 0x18) = uVar1;
  *(undefined8 *)(this + 0x20) = uVar2;
  uVar3 = _UNK_00723cf8;
  uVar1 = DAT_00723cf0;
  *(undefined **)this = puVar4 + 0x10;
  uVar2 = DAT_00723cf0;
  *(undefined8 *)(this + 0x28) = uVar1;
  *(undefined8 *)(this + 0x30) = uVar3;
  *(undefined8 *)(this + 0x38) = uVar2;
  local_48 = 2;
  *(undefined **)(this + 0x48) = PTR_shared_null_008377d0;
  local_30 = "default";
  local_40 = (undefined  [16])0x0;
                    /* WARNING: Subroutine does not return */
  QMessageLogger::fatal((char *)&local_48,"Should have never been called!");
}



