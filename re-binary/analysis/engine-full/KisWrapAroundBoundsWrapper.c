/* Class KisWrapAroundBoundsWrapper - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisWrapAroundBoundsWrapper @ 0020a050 ======

void __thiscall KisWrapAroundBoundsWrapper::KisWrapAroundBoundsWrapper(void)

{
  (*(code *)PTR_KisWrapAroundBoundsWrapper_0083caf8)();
  return;
}



// ====== KisWrapAroundBoundsWrapper @ 004af110 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisWrapAroundBoundsWrapper::KisWrapAroundBoundsWrapper(KisSharedPtr<KisDefaultBoundsBase>, QRect)
    */

void __thiscall
KisWrapAroundBoundsWrapper::KisWrapAroundBoundsWrapper
          (KisWrapAroundBoundsWrapper *this,long *param_2,long param_3,long param_4)

{
  long lVar1;
  long *plVar2;
  long lVar3;
  long lVar4;
  long *plVar5;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837040 + 0x10;
                    /* try { // try from 004af146 to 004af14a has its CatchHandler @ 004af1a8 */
  plVar5 = (long *)operator_new(0x18);
  lVar4 = DAT_00721778;
  lVar3 = _DAT_00721770;
  lVar1 = *param_2;
  *plVar5 = 0;
  *(long **)(this + 0x18) = plVar5;
  plVar5[1] = lVar3;
  plVar5[2] = lVar4;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 8) = *(int *)(lVar1 + 8) + 1;
    UNLOCK();
    plVar2 = (long *)*plVar5;
    *plVar5 = lVar1;
    if (plVar2 != (long *)0x0) {
      LOCK();
      plVar5 = plVar2 + 1;
      *(int *)plVar5 = *(int *)plVar5 + -1;
      UNLOCK();
      if (*(int *)plVar5 == 0) {
        (**(code **)(*plVar2 + 8))();
      }
    }
    plVar5 = *(long **)(this + 0x18);
  }
  plVar5[1] = param_3;
  plVar5[2] = param_4;
  return;
}



