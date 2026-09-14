/* Class KisEncloseAndFillPainter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisEncloseAndFillPainter @ 006e7290 ======

/* KisEncloseAndFillPainter::KisEncloseAndFillPainter(QSize const&) */

void __thiscall
KisEncloseAndFillPainter::KisEncloseAndFillPainter(KisEncloseAndFillPainter *this,QSize *param_1)

{
  int iVar1;
  undefined8 uVar2;
  int iVar3;
  int iVar4;
  undefined8 *puVar5;
  
  KisFillPainter::KisFillPainter((KisFillPainter *)this);
  *(undefined **)this = PTR_vtable_00837ac8 + 0x10;
                    /* try { // try from 006e72b7 to 006e72bb has its CatchHandler @ 006e731d */
  puVar5 = (undefined8 *)operator_new(0x68);
  *puVar5 = this;
  *(undefined4 *)(puVar5 + 1) = 0;
                    /* try { // try from 006e72cd to 006e72d1 has its CatchHandler @ 006e7329 */
  KoColor::KoColor((KoColor *)(puVar5 + 2));
  iVar1 = *(int *)(param_1 + 4);
  *(undefined *)((long)puVar5 + 0x52) = 1;
  iVar4 = DAT_00721778._4_4_;
  iVar3 = (int)DAT_00721778;
  *(undefined2 *)(puVar5 + 10) = 0x100;
  uVar2 = *(undefined8 *)param_1;
  *(undefined8 **)(this + 0x98) = puVar5;
  *(undefined8 *)((long)puVar5 + 0x54) = 0;
  *(undefined8 *)(this + 0x30) = uVar2;
  *(ulong *)((long)puVar5 + 0x5c) = CONCAT44(iVar1 + iVar4,*(int *)param_1 + iVar3);
  return;
}



// ====== KisEncloseAndFillPainter @ 006e7340 ======

/* KisEncloseAndFillPainter::KisEncloseAndFillPainter(KisSharedPtr<KisPaintDevice>, QSize const&) */

void __thiscall
KisEncloseAndFillPainter::KisEncloseAndFillPainter
          (KisEncloseAndFillPainter *this,KisSharedPtr param_1,QSize *param_2)

{
  long *plVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
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
                    /* try { // try from 006e7377 to 006e737b has its CatchHandler @ 006e7448 */
  KisFillPainter::KisFillPainter((KisFillPainter *)this,(KisSharedPtr)&local_28);
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 0x20))();
    }
  }
  *(undefined **)this = PTR_vtable_00837ac8 + 0x10;
                    /* try { // try from 006e73a4 to 006e73a8 has its CatchHandler @ 006e743c */
  puVar5 = (undefined8 *)operator_new(0x68);
  *puVar5 = this;
  *(undefined4 *)(puVar5 + 1) = 0;
                    /* try { // try from 006e73ba to 006e73be has its CatchHandler @ 006e7430 */
  KoColor::KoColor((KoColor *)(puVar5 + 2));
  iVar2 = *(int *)(param_2 + 4);
  *(undefined *)((long)puVar5 + 0x52) = 1;
  uVar4 = DAT_00721778;
  *(undefined2 *)(puVar5 + 10) = 0x100;
  uVar3 = *(undefined8 *)param_2;
  *(undefined8 **)(this + 0x98) = puVar5;
  *(undefined8 *)((long)puVar5 + 0x54) = 0;
  *(undefined8 *)(this + 0x30) = uVar3;
  *(ulong *)((long)puVar5 + 0x5c) =
       CONCAT44(iVar2 + (int)((ulong)uVar4 >> 0x20),*(int *)param_2 + (int)uVar4);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisEncloseAndFillPainter @ 006e7460 ======

/* KisEncloseAndFillPainter::KisEncloseAndFillPainter(KisSharedPtr<KisPaintDevice>,
   KisSharedPtr<KisSelection>, QSize const&) */

void __thiscall
KisEncloseAndFillPainter::KisEncloseAndFillPainter
          (KisEncloseAndFillPainter *this,KisSharedPtr param_1,KisSharedPtr param_2,QSize *param_3)

{
  long *plVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
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
                    /* try { // try from 006e74b1 to 006e74b5 has its CatchHandler @ 006e7590 */
  KisFillPainter::KisFillPainter
            ((KisFillPainter *)this,(KisSharedPtr)&local_30,(KisSharedPtr)&local_28);
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
  *(undefined **)this = PTR_vtable_00837ac8 + 0x10;
                    /* try { // try from 006e74f4 to 006e74f8 has its CatchHandler @ 006e75a8 */
  puVar5 = (undefined8 *)operator_new(0x68);
  *puVar5 = this;
  *(undefined4 *)(puVar5 + 1) = 0;
                    /* try { // try from 006e750a to 006e750e has its CatchHandler @ 006e759c */
  KoColor::KoColor((KoColor *)(puVar5 + 2));
  iVar2 = *(int *)(param_3 + 4);
  *(undefined *)((long)puVar5 + 0x52) = 1;
  uVar4 = DAT_00721778;
  *(undefined2 *)(puVar5 + 10) = 0x100;
  uVar3 = *(undefined8 *)param_3;
  *(undefined8 **)(this + 0x98) = puVar5;
  *(undefined8 *)((long)puVar5 + 0x54) = 0;
  *(undefined8 *)(this + 0x30) = uVar3;
  *(ulong *)((long)puVar5 + 0x5c) =
       CONCAT44(iVar2 + (int)((ulong)uVar4 >> 0x20),*(int *)param_3 + (int)uVar4);
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



