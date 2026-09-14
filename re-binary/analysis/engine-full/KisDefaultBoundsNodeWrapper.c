/* Class KisDefaultBoundsNodeWrapper - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisDefaultBoundsNodeWrapper @ 002015a0 ======

void __thiscall
KisDefaultBoundsNodeWrapper::KisDefaultBoundsNodeWrapper
          (KisDefaultBoundsNodeWrapper *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisDefaultBoundsNodeWrapper_008385a0)();
  return;
}



// ====== KisDefaultBoundsNodeWrapper @ 004aff80 ======

/* KisDefaultBoundsNodeWrapper::KisDefaultBoundsNodeWrapper(KisWeakSharedPtr<KisBaseNode>) */

void __thiscall
KisDefaultBoundsNodeWrapper::KisDefaultBoundsNodeWrapper
          (KisDefaultBoundsNodeWrapper *this,KisWeakSharedPtr param_1)

{
  long lVar1;
  long *plVar2;
  undefined (*pauVar3) [16];
  int *piVar4;
  undefined4 in_register_00000034;
  
  plVar2 = (long *)CONCAT44(in_register_00000034,param_1);
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837270 + 0x10;
                    /* try { // try from 004affab to 004b0031 has its CatchHandler @ 004b0048 */
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
      piVar4 = *(int **)(lVar1 + 0x18);
      if (piVar4 == (int *)0x0) {
        piVar4 = (int *)operator_new(4);
        *piVar4 = 0;
        *(int **)(lVar1 + 0x18) = piVar4;
        LOCK();
        *piVar4 = *piVar4 + 1;
        UNLOCK();
        piVar4 = *(int **)(lVar1 + 0x18);
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



// ====== KisDefaultBoundsNodeWrapper @ 004b0060 ======

/* KisDefaultBoundsNodeWrapper::KisDefaultBoundsNodeWrapper(KisDefaultBoundsNodeWrapper&) */

void __thiscall
KisDefaultBoundsNodeWrapper::KisDefaultBoundsNodeWrapper
          (KisDefaultBoundsNodeWrapper *this,KisDefaultBoundsNodeWrapper *param_1)

{
  long *plVar1;
  long lVar2;
  undefined (*pauVar3) [16];
  int *piVar4;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837270 + 0x10;
                    /* try { // try from 004b008b to 004b0111 has its CatchHandler @ 004b0128 */
  pauVar3 = (undefined (*) [16])operator_new(0x10);
  *(undefined (**) [16])(this + 0x18) = pauVar3;
  *pauVar3 = (undefined  [16])0x0;
  plVar1 = *(long **)(param_1 + 0x18);
  if (*plVar1 == 0) {
    *(undefined8 *)*pauVar3 = 0;
  }
  else {
    if (((uint *)plVar1[1] == (uint *)0x0) || ((*(uint *)plVar1[1] & 1) == 0)) {
      *pauVar3 = (undefined  [16])0x0;
      return;
    }
    lVar2 = *plVar1;
    *(long *)*pauVar3 = lVar2;
    if (lVar2 != 0) {
      piVar4 = *(int **)(lVar2 + 0x18);
      if (piVar4 == (int *)0x0) {
        piVar4 = (int *)operator_new(4);
        *piVar4 = 0;
        *(int **)(lVar2 + 0x18) = piVar4;
        LOCK();
        *piVar4 = *piVar4 + 1;
        UNLOCK();
        piVar4 = *(int **)(lVar2 + 0x18);
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



