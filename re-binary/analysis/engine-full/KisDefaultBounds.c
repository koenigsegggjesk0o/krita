/* Class KisDefaultBounds - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisDefaultBounds @ 00205020 ======

void __thiscall KisDefaultBounds::KisDefaultBounds(KisDefaultBounds *this)

{
  (*(code *)PTR_KisDefaultBounds_0083a2e0)();
  return;
}



// ====== KisDefaultBounds @ 00205050 ======

void __thiscall KisDefaultBounds::KisDefaultBounds(KisDefaultBounds *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisDefaultBounds_0083a2f8)();
  return;
}



// ====== KisDefaultBounds @ 00209a40 ======

void __thiscall KisDefaultBounds::KisDefaultBounds(KisDefaultBounds *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisDefaultBounds_0083c7f0)();
  return;
}



// ====== KisDefaultBounds @ 004aeda0 ======

/* KisDefaultBounds::KisDefaultBounds(KisWeakSharedPtr<KisImage>) */

void __thiscall KisDefaultBounds::KisDefaultBounds(KisDefaultBounds *this,KisWeakSharedPtr param_1)

{
  long lVar1;
  long *plVar2;
  undefined (*pauVar3) [16];
  int *piVar4;
  undefined4 in_register_00000034;
  
  plVar2 = (long *)CONCAT44(in_register_00000034,param_1);
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837190 + 0x10;
                    /* try { // try from 004aedcb to 004aee51 has its CatchHandler @ 004aee68 */
  pauVar3 = (undefined (*) [16])operator_new(0x10);
  lVar1 = *plVar2;
  *(undefined (**) [16])(this + 0x18) = pauVar3;
  *pauVar3 = (undefined  [16])0x0;
  if (lVar1 == 0) {
    *(undefined8 *)*pauVar3 = 0;
  }
  else {
    if (((uint *)plVar2[1] == (uint *)0x0) || ((*(uint *)plVar2[1] & 1) == 0)) {
      *pauVar3 = (undefined  [16])0x0;
      return;
    }
    lVar1 = *plVar2;
    *(long *)*pauVar3 = lVar1;
    if (lVar1 != 0) {
      piVar4 = *(int **)(lVar1 + 0x58);
      if (piVar4 == (int *)0x0) {
        piVar4 = (int *)operator_new(4);
        *piVar4 = 0;
        *(int **)(lVar1 + 0x58) = piVar4;
        LOCK();
        *piVar4 = *piVar4 + 1;
        UNLOCK();
        piVar4 = *(int **)(lVar1 + 0x58);
      }
      *(int **)(*pauVar3 + 8) = piVar4;
      LOCK();
      *piVar4 = *piVar4 + 2;
      UNLOCK();
      return;
    }
  }
  *(undefined8 *)(*pauVar3 + 8) = 0;
  return;
}



// ====== KisDefaultBounds @ 004af1c0 ======

/* KisDefaultBounds::KisDefaultBounds() */

void __thiscall KisDefaultBounds::KisDefaultBounds(KisDefaultBounds *this)

{
  int iVar1;
  undefined auVar2 [16];
  undefined8 uVar3;
  long in_FS_OFFSET;
  undefined local_38 [24];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_38._0_16_ = (undefined  [16])0x0;
                    /* try { // try from 004af1e8 to 004af1ec has its CatchHandler @ 004af243 */
  KisDefaultBounds(this,(KisWeakSharedPtr)local_38);
  uVar3 = local_38._8_8_;
  auVar2._8_8_ = 0;
  auVar2._0_8_ = local_38._8_8_;
  local_38._0_16_ = auVar2 << 0x40;
  if ((int *)uVar3 != (int *)0x0) {
    LOCK();
    iVar1 = *(int *)uVar3;
    *(int *)uVar3 = *(int *)uVar3 + -2;
    UNLOCK();
    if ((iVar1 < 3) && ((int *)uVar3 != (int *)0x0)) {
      operator_delete((void *)uVar3,4);
    }
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



