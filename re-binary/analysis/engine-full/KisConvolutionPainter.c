/* Class KisConvolutionPainter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisConvolutionPainter @ 00200d50 ======

void __thiscall
KisConvolutionPainter::KisConvolutionPainter(KisConvolutionPainter *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisConvolutionPainter_00838178)();
  return;
}



// ====== KisConvolutionPainter @ 0020d510 ======

void __thiscall
KisConvolutionPainter::KisConvolutionPainter
          (KisConvolutionPainter *this,KisSharedPtr param_1,EnginePreference param_2)

{
  (*(code *)PTR_KisConvolutionPainter_0083e558)();
  return;
}



// ====== KisConvolutionPainter @ 004727b0 ======

/* KisConvolutionPainter::KisConvolutionPainter() */

void __thiscall KisConvolutionPainter::KisConvolutionPainter(KisConvolutionPainter *this)

{
  undefined *puVar1;
  
  KisPainter::KisPainter((KisPainter *)this);
  puVar1 = PTR_vtable_00837810;
  *(undefined4 *)(this + 0x10) = 0;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



// ====== KisConvolutionPainter @ 004727e0 ======

/* KisConvolutionPainter::KisConvolutionPainter(KisSharedPtr<KisPaintDevice>) */

void __thiscall
KisConvolutionPainter::KisConvolutionPainter(KisConvolutionPainter *this,KisSharedPtr param_1)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_18 != (long *)0x0) {
    LOCK();
    *(int *)(local_18 + 2) = *(int *)(local_18 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 00472811 to 00472815 has its CatchHandler @ 00472865 */
  KisPainter::KisPainter((KisPainter *)this,(KisSharedPtr)&local_18);
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 0x20))();
    }
  }
  puVar2 = PTR_vtable_00837810;
  *(undefined4 *)(this + 0x10) = 0;
  *(undefined **)this = puVar2 + 0x10;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisConvolutionPainter @ 00472880 ======

/* KisConvolutionPainter::KisConvolutionPainter(KisSharedPtr<KisPaintDevice>,
   KisSharedPtr<KisSelection>) */

void __thiscall
KisConvolutionPainter::KisConvolutionPainter
          (KisConvolutionPainter *this,KisSharedPtr param_1,KisSharedPtr param_2)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_20;
  long *local_18;
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  local_18 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_18 != (long *)0x0) {
    LOCK();
    *(int *)(local_18 + 1) = *(int *)(local_18 + 1) + 1;
    UNLOCK();
  }
  local_20 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_20 != (long *)0x0) {
    LOCK();
    *(int *)(local_20 + 2) = *(int *)(local_20 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 004728cb to 004728cf has its CatchHandler @ 0047293d */
  KisPainter::KisPainter((KisPainter *)this,(KisSharedPtr)&local_20,(KisSharedPtr)&local_18);
  if (local_20 != (long *)0x0) {
    LOCK();
    plVar1 = local_20 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_20 + 0x20))();
    }
  }
  if (local_18 != (long *)0x0) {
    LOCK();
    plVar1 = local_18 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_18 + 8))();
    }
  }
  puVar2 = PTR_vtable_00837810;
  *(undefined4 *)(this + 0x10) = 0;
  *(undefined **)this = puVar2 + 0x10;
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisConvolutionPainter @ 00472950 ======

/* KisConvolutionPainter::KisConvolutionPainter(KisSharedPtr<KisPaintDevice>,
   KisConvolutionPainter::EnginePreference) */

void __thiscall
KisConvolutionPainter::KisConvolutionPainter
          (KisConvolutionPainter *this,KisSharedPtr param_1,EnginePreference param_2)

{
  long *plVar1;
  undefined *puVar2;
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
                    /* try { // try from 00472984 to 00472988 has its CatchHandler @ 004729d5 */
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
  puVar2 = PTR_vtable_00837810;
  *(EnginePreference *)(this + 0x10) = param_2;
  *(undefined **)this = puVar2 + 0x10;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



