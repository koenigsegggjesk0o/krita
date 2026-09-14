/* Class KisMarkerPainter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMarkerPainter @ 00326e20 ======

/* KisMarkerPainter::KisMarkerPainter(KisSharedPtr<KisPaintDevice>, KoColor const&) */

void __thiscall
KisMarkerPainter::KisMarkerPainter(KisMarkerPainter *this,KisSharedPtr param_1,KoColor *param_2)

{
  long *plVar1;
  long *plVar2;
  long *plVar3;
  undefined4 in_register_00000034;
  
  plVar3 = (long *)operator_new(0x10);
  plVar2 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (plVar2 == (long *)0x0) {
    *plVar3 = 0;
    plVar3[1] = (long)param_2;
    *(long **)this = plVar3;
    return;
  }
  plVar1 = plVar2 + 2;
  LOCK();
  *(int *)(plVar2 + 2) = *(int *)(plVar2 + 2) + 1;
  UNLOCK();
  *plVar3 = (long)plVar2;
  LOCK();
  *(int *)(plVar2 + 2) = *(int *)(plVar2 + 2) + 1;
  UNLOCK();
  plVar3[1] = (long)param_2;
  *(long **)this = plVar3;
  LOCK();
  *(int *)plVar1 = *(int *)plVar1 + -1;
  UNLOCK();
  if (*(int *)plVar1 != 0) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x00326e93. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*plVar2 + 0x20))();
  return;
}



