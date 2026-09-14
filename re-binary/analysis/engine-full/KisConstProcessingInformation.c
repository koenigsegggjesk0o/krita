/* Class KisConstProcessingInformation - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisConstProcessingInformation @ 00200db0 ======

void __thiscall
KisConstProcessingInformation::KisConstProcessingInformation
          (KisConstProcessingInformation *this,KisSharedPtr param_1,QPoint *param_2,
          KisSharedPtr param_3)

{
  (*(code *)PTR_KisConstProcessingInformation_008381a8)();
  return;
}



// ====== KisConstProcessingInformation @ 0020b2c0 ======

void __thiscall
KisConstProcessingInformation::KisConstProcessingInformation
          (KisConstProcessingInformation *this,KisConstProcessingInformation *param_1)

{
  (*(code *)PTR_KisConstProcessingInformation_0083d430)();
  return;
}



// ====== KisConstProcessingInformation @ 005e7d40 ======

/* KisConstProcessingInformation::KisConstProcessingInformation(KisSharedPtr<KisPaintDevice>, QPoint
   const&, KisSharedPtr<KisSelection>) */

void __thiscall
KisConstProcessingInformation::KisConstProcessingInformation
          (KisConstProcessingInformation *this,KisSharedPtr param_1,QPoint *param_2,
          KisSharedPtr param_3)

{
  long *plVar1;
  long lVar2;
  undefined (*pauVar3) [16];
  undefined4 in_register_0000000c;
  long *plVar4;
  undefined4 in_register_00000034;
  long *plVar5;
  
  plVar4 = (long *)CONCAT44(in_register_0000000c,param_3);
  pauVar3 = (undefined (*) [16])operator_new(0x18);
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(undefined8 *)pauVar3[1] = 0;
  *(undefined (**) [16])this = pauVar3;
  *pauVar3 = (undefined  [16])0x0;
  if (lVar2 == 0) {
    plVar4 = (long *)*plVar4;
    if (plVar4 == (long *)0x0) goto LAB_005e7dcb;
LAB_005e7de9:
    LOCK();
    *(int *)(plVar4 + 1) = *(int *)(plVar4 + 1) + 1;
    UNLOCK();
    plVar5 = *(long **)(*pauVar3 + 8);
  }
  else {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
    plVar5 = *(long **)*pauVar3;
    *(long *)*pauVar3 = lVar2;
    if (plVar5 == (long *)0x0) {
LAB_005e7d96:
      pauVar3 = *(undefined (**) [16])this;
      plVar4 = (long *)*plVar4;
      plVar5 = *(long **)(*pauVar3 + 8);
      if (plVar4 == plVar5) goto LAB_005e7dcb;
    }
    else {
      LOCK();
      plVar1 = plVar5 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 != 0) goto LAB_005e7d96;
      (**(code **)(*plVar5 + 0x20))();
      pauVar3 = *(undefined (**) [16])this;
      plVar4 = (long *)*plVar4;
      plVar5 = *(long **)(*pauVar3 + 8);
      if (plVar4 == plVar5) goto LAB_005e7dcb;
    }
    if (plVar4 != (long *)0x0) goto LAB_005e7de9;
  }
  *(long **)(*pauVar3 + 8) = plVar4;
  if (plVar5 != (long *)0x0) {
    LOCK();
    plVar4 = plVar5 + 1;
    *(int *)plVar4 = *(int *)plVar4 + -1;
    UNLOCK();
    if (*(int *)plVar4 == 0) {
      (**(code **)(*plVar5 + 8))();
    }
  }
  pauVar3 = *(undefined (**) [16])this;
LAB_005e7dcb:
  *(undefined8 *)pauVar3[1] = *(undefined8 *)param_2;
  return;
}



// ====== KisConstProcessingInformation @ 005e7e10 ======

/* KisConstProcessingInformation::KisConstProcessingInformation(KisConstProcessingInformation
   const&) */

void __thiscall
KisConstProcessingInformation::KisConstProcessingInformation
          (KisConstProcessingInformation *this,KisConstProcessingInformation *param_1)

{
  long *plVar1;
  long lVar2;
  undefined (*pauVar3) [16];
  long *plVar4;
  long *plVar5;
  
  pauVar3 = (undefined (*) [16])operator_new(0x18);
  plVar1 = *(long **)param_1;
  *(undefined8 *)pauVar3[1] = 0;
  *(undefined (**) [16])this = pauVar3;
  *pauVar3 = (undefined  [16])0x0;
  lVar2 = *plVar1;
  if (lVar2 == 0) {
    plVar4 = (long *)plVar1[1];
    if (plVar4 == (long *)0x0) goto LAB_005e7e8d;
LAB_005e7e74:
    LOCK();
    *(int *)(plVar4 + 1) = *(int *)(plVar4 + 1) + 1;
    UNLOCK();
    plVar5 = *(long **)(*pauVar3 + 8);
  }
  else {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
    plVar4 = *(long **)*pauVar3;
    *(long *)*pauVar3 = lVar2;
    if (plVar4 == (long *)0x0) {
LAB_005e7e62:
      plVar5 = *(long **)(*pauVar3 + 8);
      plVar4 = (long *)plVar1[1];
      if (plVar4 == plVar5) goto LAB_005e7e8d;
    }
    else {
      LOCK();
      plVar5 = plVar4 + 2;
      *(int *)plVar5 = *(int *)plVar5 + -1;
      UNLOCK();
      if (*(int *)plVar5 != 0) goto LAB_005e7e62;
      (**(code **)(*plVar4 + 0x20))();
      plVar5 = *(long **)(*pauVar3 + 8);
      plVar4 = (long *)plVar1[1];
      if (plVar4 == plVar5) goto LAB_005e7e8d;
    }
    if (plVar4 != (long *)0x0) goto LAB_005e7e74;
  }
  *(long **)(*pauVar3 + 8) = plVar4;
  if (plVar5 != (long *)0x0) {
    LOCK();
    plVar4 = plVar5 + 1;
    *(int *)plVar4 = *(int *)plVar4 + -1;
    UNLOCK();
    if (*(int *)plVar4 == 0) {
      (**(code **)(*plVar5 + 8))();
      *(long *)pauVar3[1] = plVar1[2];
      return;
    }
  }
LAB_005e7e8d:
  *(long *)pauVar3[1] = plVar1[2];
  return;
}



