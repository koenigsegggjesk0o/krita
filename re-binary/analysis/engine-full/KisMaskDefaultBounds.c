/* Class KisMaskDefaultBounds - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMaskDefaultBounds @ 002023a0 ======

void __thiscall
KisMaskDefaultBounds::KisMaskDefaultBounds(KisMaskDefaultBounds *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisMaskDefaultBounds_00838ca0)();
  return;
}



// ====== KisMaskDefaultBounds @ 004af080 ======

/* KisMaskDefaultBounds::KisMaskDefaultBounds(KisSharedPtr<KisNode>) */

void __thiscall
KisMaskDefaultBounds::KisMaskDefaultBounds(KisMaskDefaultBounds *this,KisSharedPtr param_1)

{
  long lVar1;
  undefined *puVar2;
  int *piVar3;
  undefined4 in_register_00000034;
  
  KisSelectionDefaultBoundsBase::KisSelectionDefaultBoundsBase
            ((KisSelectionDefaultBoundsBase *)this);
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  puVar2 = PTR_vtable_008372f0 + 0x10;
  *(long *)(this + 0x18) = lVar1;
  *(undefined **)this = puVar2;
  if (lVar1 != 0) {
    piVar3 = *(int **)(lVar1 + 0x18);
    if (piVar3 == (int *)0x0) {
                    /* try { // try from 004af0e5 to 004af0e9 has its CatchHandler @ 004af0fe */
      piVar3 = (int *)operator_new(4);
      *piVar3 = 0;
      *(int **)(lVar1 + 0x18) = piVar3;
      LOCK();
      *piVar3 = *piVar3 + 1;
      UNLOCK();
      piVar3 = *(int **)(lVar1 + 0x18);
    }
    *(int **)(this + 0x20) = piVar3;
    LOCK();
    *piVar3 = *piVar3 + 2;
    UNLOCK();
    return;
  }
  *(undefined8 *)(this + 0x20) = 0;
  return;
}



