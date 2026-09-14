/* Class KisCallbackBasedPaintopProperty - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

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



