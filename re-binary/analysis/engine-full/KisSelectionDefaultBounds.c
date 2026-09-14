/* Class KisSelectionDefaultBounds - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSelectionDefaultBounds @ 002045c0 ======

void __thiscall
KisSelectionDefaultBounds::KisSelectionDefaultBounds
          (KisSelectionDefaultBounds *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisSelectionDefaultBounds_00839db0)();
  return;
}



// ====== KisSelectionDefaultBounds @ 004aeff0 ======

/* KisSelectionDefaultBounds::KisSelectionDefaultBounds(KisSharedPtr<KisPaintDevice>) */

void __thiscall
KisSelectionDefaultBounds::KisSelectionDefaultBounds
          (KisSelectionDefaultBounds *this,KisSharedPtr param_1)

{
  long lVar1;
  undefined *puVar2;
  int *piVar3;
  undefined4 in_register_00000034;
  
  KisSelectionDefaultBoundsBase::KisSelectionDefaultBoundsBase
            ((KisSelectionDefaultBoundsBase *)this);
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  puVar2 = PTR_vtable_00837478 + 0x10;
  *(long *)(this + 0x18) = lVar1;
  *(undefined **)this = puVar2;
  if (lVar1 != 0) {
    piVar3 = *(int **)(lVar1 + 0x18);
    if (piVar3 == (int *)0x0) {
                    /* try { // try from 004af055 to 004af059 has its CatchHandler @ 004af06e */
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



