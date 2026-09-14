/* Class KisFillPainter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisFillPainter @ 00201430 ======

void __thiscall
KisFillPainter::KisFillPainter(KisFillPainter *this,KisSharedPtr param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisFillPainter_008384e8)();
  return;
}



// ====== KisFillPainter @ 00206f40 ======

void __thiscall KisFillPainter::KisFillPainter(KisFillPainter *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisFillPainter_0083b270)();
  return;
}



// ====== KisFillPainter @ 0020b320 ======

void __thiscall KisFillPainter::KisFillPainter(KisFillPainter *this)

{
  (*(code *)PTR_KisFillPainter_0083d460)();
  return;
}



// ====== KisFillPainter @ 0020d060 ======

void __thiscall KisFillPainter::KisFillPainter(KisFillPainter *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisFillPainter_0083e300)();
  return;
}



// ====== KisFillPainter @ 004b5e50 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisFillPainter::KisFillPainter() */

void __thiscall KisFillPainter::KisFillPainter(KisFillPainter *this)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined *puVar3;
  
  KisPainter::KisPainter((KisPainter *)this);
  puVar3 = PTR_vtable_00837a50;
  uVar2 = DAT_00721778;
  uVar1 = _DAT_00721770;
  *(undefined8 *)(this + 0x10) = 0;
  *(undefined8 *)(this + 0x38) = uVar1;
  *(undefined8 *)(this + 0x40) = uVar2;
  *(undefined **)this = puVar3 + 0x10;
                    /* try { // try from 004b5e88 to 004b5e8c has its CatchHandler @ 004b5e9c */
  KoColor::KoColor((KoColor *)(this + 0x50));
                    /* try { // try from 004b5e90 to 004b5e94 has its CatchHandler @ 004b5ea8 */
  initFillPainter(this);
  return;
}



// ====== KisFillPainter @ 004b5ec0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisFillPainter::KisFillPainter(KisSharedPtr<KisPaintDevice>) */

void __thiscall KisFillPainter::KisFillPainter(KisFillPainter *this,KisSharedPtr param_1)

{
  long *plVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_28 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_28 != (long *)0x0) {
    LOCK();
    *(int *)(local_28 + 2) = *(int *)(local_28 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 004b5ef2 to 004b5ef6 has its CatchHandler @ 004b5f7d */
  KisPainter::KisPainter((KisPainter *)this,(KisSharedPtr)&local_28);
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 0x20))();
    }
  }
  puVar4 = PTR_vtable_00837a50;
  uVar3 = DAT_00721778;
  uVar2 = _DAT_00721770;
  *(undefined8 *)(this + 0x10) = 0;
  *(undefined8 *)(this + 0x38) = uVar2;
  *(undefined8 *)(this + 0x40) = uVar3;
  *(undefined **)this = puVar4 + 0x10;
                    /* try { // try from 004b5f2d to 004b5f31 has its CatchHandler @ 004b5f71 */
  KoColor::KoColor((KoColor *)(this + 0x50));
                    /* try { // try from 004b5f35 to 004b5f39 has its CatchHandler @ 004b5f65 */
  initFillPainter(this);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisFillPainter @ 004b5f90 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisFillPainter::KisFillPainter(KisSharedPtr<KisPaintDevice>, KisSharedPtr<KisSelection>) */

void __thiscall
KisFillPainter::KisFillPainter(KisFillPainter *this,KisSharedPtr param_1,KisSharedPtr param_2)

{
  long *plVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_30;
  long *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_28 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_28 != (long *)0x0) {
    LOCK();
    *(int *)(local_28 + 1) = *(int *)(local_28 + 1) + 1;
    UNLOCK();
  }
  local_30 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_30 != (long *)0x0) {
    LOCK();
    *(int *)(local_30 + 2) = *(int *)(local_30 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 004b5fdc to 004b5fe0 has its CatchHandler @ 004b606d */
  KisPainter::KisPainter((KisPainter *)this,(KisSharedPtr)&local_30,(KisSharedPtr)&local_28);
  if (local_30 != (long *)0x0) {
    LOCK();
    plVar1 = local_30 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_30 + 0x20))();
    }
  }
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
  puVar4 = PTR_vtable_00837a50;
  uVar3 = DAT_00721778;
  uVar2 = _DAT_00721770;
  *(undefined8 *)(this + 0x10) = 0;
  *(undefined8 *)(this + 0x38) = uVar2;
  *(undefined8 *)(this + 0x40) = uVar3;
  *(undefined **)this = puVar4 + 0x10;
                    /* try { // try from 004b6029 to 004b602d has its CatchHandler @ 004b6085 */
  KoColor::KoColor((KoColor *)(this + 0x50));
                    /* try { // try from 004b6031 to 004b6035 has its CatchHandler @ 004b6079 */
  initFillPainter(this);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



