/* Class KisGradientPainter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGradientPainter @ 00209240 ======

void __thiscall
KisGradientPainter::KisGradientPainter(KisGradientPainter *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisGradientPainter_0083c3f0)();
  return;
}



// ====== KisGradientPainter @ 004d0bb0 ======

/* KisGradientPainter::KisGradientPainter() */

void __thiscall KisGradientPainter::KisGradientPainter(KisGradientPainter *this)

{
  undefined *puVar1;
  undefined4 *puVar2;
  
  KisPainter::KisPainter((KisPainter *)this);
  *(undefined **)this = PTR_vtable_00837170 + 0x10;
                    /* try { // try from 004d0bd5 to 004d0bd9 has its CatchHandler @ 004d0bf6 */
  puVar2 = (undefined4 *)operator_new(0x10);
  puVar1 = PTR_shared_null_008377d0;
  *puVar2 = 0;
  *(undefined4 **)(this + 0x10) = puVar2;
  *(undefined **)(puVar2 + 2) = puVar1;
  return;
}



// ====== KisGradientPainter @ 004d0c10 ======

/* KisGradientPainter::KisGradientPainter(KisSharedPtr<KisPaintDevice>) */

void __thiscall
KisGradientPainter::KisGradientPainter(KisGradientPainter *this,KisSharedPtr param_1)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 *puVar3;
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
                    /* try { // try from 004d0c42 to 004d0c46 has its CatchHandler @ 004d0cad */
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
  *(undefined **)this = PTR_vtable_00837170 + 0x10;
                    /* try { // try from 004d0c6a to 004d0c6e has its CatchHandler @ 004d0cb9 */
  puVar3 = (undefined4 *)operator_new(0x10);
  puVar2 = PTR_shared_null_008377d0;
  *puVar3 = 0;
  *(undefined4 **)(this + 0x10) = puVar3;
  *(undefined **)(puVar3 + 2) = puVar2;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisGradientPainter @ 004d0cd0 ======

/* KisGradientPainter::KisGradientPainter(KisSharedPtr<KisPaintDevice>, KisSharedPtr<KisSelection>)
    */

void __thiscall
KisGradientPainter::KisGradientPainter
          (KisGradientPainter *this,KisSharedPtr param_1,KisSharedPtr param_2)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 *puVar3;
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
                    /* try { // try from 004d0d1c to 004d0d20 has its CatchHandler @ 004d0dad */
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
  *(undefined **)this = PTR_vtable_00837170 + 0x10;
                    /* try { // try from 004d0d56 to 004d0d5a has its CatchHandler @ 004d0db9 */
  puVar3 = (undefined4 *)operator_new(0x10);
  puVar2 = PTR_shared_null_008377d0;
  *puVar3 = 0;
  *(undefined4 **)(this + 0x10) = puVar3;
  *(undefined **)(puVar3 + 2) = puVar2;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



