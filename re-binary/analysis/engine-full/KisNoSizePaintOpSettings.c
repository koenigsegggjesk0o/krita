/* Class KisNoSizePaintOpSettings - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

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



