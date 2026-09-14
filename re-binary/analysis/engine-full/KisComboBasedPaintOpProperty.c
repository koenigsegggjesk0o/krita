/* Class KisComboBasedPaintOpProperty - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisComboBasedPaintOpProperty @ 00201bf0 ======

void __thiscall
KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty
          (KisComboBasedPaintOpProperty *this,KoID *param_1,KisRestrictedSharedPtr param_2,
          QObject *param_3)

{
  (*(code *)PTR_KisComboBasedPaintOpProperty_008388c8)();
  return;
}



// ====== KisComboBasedPaintOpProperty @ 00206c30 ======

void __thiscall
KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty
          (KisComboBasedPaintOpProperty *this,Type param_1,KoID *param_2,
          KisRestrictedSharedPtr param_3,QObject *param_4)

{
  (*(code *)PTR_KisComboBasedPaintOpProperty_0083b0e8)();
  return;
}



// ====== KisComboBasedPaintOpProperty @ 00208aa0 ======

void __thiscall
KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty
          (KisComboBasedPaintOpProperty *this,Type param_1,SubType param_2,KoID *param_3,
          KisRestrictedSharedPtr param_4,QObject *param_5)

{
  (*(code *)PTR_KisComboBasedPaintOpProperty_0083c020)();
  return;
}



// ====== KisComboBasedPaintOpProperty @ 00354050 ======

/* KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty(KoID const&,
   KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty
          (KisComboBasedPaintOpProperty *this,KoID *param_1,KisRestrictedSharedPtr param_2,
          QObject *param_3)

{
  long *plVar1;
  undefined *puVar2;
  undefined8 *puVar3;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  long *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_28 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_28 != (long *)0x0) {
    LOCK();
    *(int *)(local_28 + 1) = *(int *)(local_28 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 0035408d to 00354091 has its CatchHandler @ 003540f5 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,3,param_1,(KisRestrictedSharedPtr)&local_28,param_3);
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
  *(undefined **)this = PTR_vtable_00837660 + 0x10;
                    /* try { // try from 003540b5 to 003540b9 has its CatchHandler @ 00354101 */
  puVar3 = (undefined8 *)operator_new(0x10);
  puVar2 = PTR_shared_null_00837830;
  *(undefined8 **)(this + 0x18) = puVar3;
  *puVar3 = puVar2;
  puVar3[1] = puVar2;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisComboBasedPaintOpProperty @ 003543f0 ======

/* KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty(KisUniformPaintOpProperty::Type, KoID
   const&, KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty
          (KisComboBasedPaintOpProperty *this,Type param_1,KoID *param_2,
          KisRestrictedSharedPtr param_3,QObject *param_4)

{
  long *plVar1;
  undefined *puVar2;
  undefined8 *puVar3;
  undefined4 in_register_0000000c;
  long in_FS_OFFSET;
  long *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_28 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (local_28 != (long *)0x0) {
    LOCK();
    *(int *)(local_28 + 1) = *(int *)(local_28 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 00354429 to 0035442d has its CatchHandler @ 003544c1 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,3,param_2,(KisRestrictedSharedPtr)&local_28,param_4);
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
  *(undefined **)this = PTR_vtable_00837660 + 0x10;
                    /* try { // try from 00354451 to 00354455 has its CatchHandler @ 003544b5 */
  puVar3 = (undefined8 *)operator_new(0x10);
  puVar2 = PTR_shared_null_00837830;
  *(undefined8 **)(this + 0x18) = puVar3;
  *puVar3 = puVar2;
  puVar3[1] = puVar2;
  if (param_1 != 3) {
                    /* try { // try from 0035449b to 0035449f has its CatchHandler @ 003544cd */
    kis_assert_recoverable
              ("type == Combo",
               "/builds/graphics/krita/libs/image/brushengine/kis_combo_based_paintop_property.cpp",
               0x1d);
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisComboBasedPaintOpProperty @ 003544e0 ======

/* KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty(KisUniformPaintOpProperty::Type,
   KisUniformPaintOpProperty::SubType, KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisComboBasedPaintOpProperty::KisComboBasedPaintOpProperty
          (KisComboBasedPaintOpProperty *this,Type param_1,SubType param_2,KoID *param_3,
          KisRestrictedSharedPtr param_4,QObject *param_5)

{
  long *plVar1;
  undefined *puVar2;
  undefined8 *puVar3;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  long *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_28 = *(long **)CONCAT44(in_register_00000084,param_4);
  if (local_28 != (long *)0x0) {
    LOCK();
    *(int *)(local_28 + 1) = *(int *)(local_28 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 00354519 to 0035451d has its CatchHandler @ 003545b1 */
  KisUniformPaintOpProperty::KisUniformPaintOpProperty
            ((KisUniformPaintOpProperty *)this,3,param_2,param_3,(KisRestrictedSharedPtr)&local_28,
             param_5);
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
  *(undefined **)this = PTR_vtable_00837660 + 0x10;
                    /* try { // try from 00354541 to 00354545 has its CatchHandler @ 003545a5 */
  puVar3 = (undefined8 *)operator_new(0x10);
  puVar2 = PTR_shared_null_00837830;
  *(undefined8 **)(this + 0x18) = puVar3;
  *puVar3 = puVar2;
  puVar3[1] = puVar2;
  if (param_1 != 3) {
                    /* try { // try from 0035458b to 0035458f has its CatchHandler @ 003545bd */
    kis_assert_recoverable
              ("type == Combo",
               "/builds/graphics/krita/libs/image/brushengine/kis_combo_based_paintop_property.cpp",
               0x24);
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



