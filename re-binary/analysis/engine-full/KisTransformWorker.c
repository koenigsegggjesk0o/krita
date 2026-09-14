/* Class KisTransformWorker - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTransformWorker @ 002042a0 ======

void __thiscall
KisTransformWorker::KisTransformWorker
          (KisTransformWorker *this,KisSharedPtr param_1,double param_2,double param_3,
          double param_4,double param_5,double param_6,double param_7,double param_8,
          QPointer param_9,KisFilterStrategy *param_10)

{
  (*(code *)PTR_KisTransformWorker_00839c20)();
  return;
}



// ====== KisTransformWorker @ 00602120 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTransformWorker::KisTransformWorker(KisSharedPtr<KisPaintDevice>, double, double, double,
   double, double, double, double, QPointer<KoUpdater>, KisFilterStrategy*) */

void __thiscall
KisTransformWorker::KisTransformWorker
          (KisTransformWorker *this,KisSharedPtr param_1,double param_2,double param_3,
          double param_4,double param_5,double param_6,double param_7,double param_8,
          QPointer param_9,KisFilterStrategy *param_10)

{
  long *plVar1;
  long lVar2;
  long *plVar3;
  int *piVar4;
  int *piVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  
  *(undefined (*) [16])(this + 0x50) = (undefined  [16])0x0;
  uVar7 = DAT_00721778;
  uVar6 = _DAT_00721770;
  *(undefined8 *)this = 0;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  this[0x78] = (KisTransformWorker)0x0;
  *(undefined8 *)(this + 0x68) = uVar6;
  *(undefined8 *)(this + 0x70) = uVar7;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
    plVar3 = *(long **)this;
    *(long *)this = lVar2;
    if (plVar3 != (long *)0x0) {
      LOCK();
      plVar1 = plVar3 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar3 + 0x20))();
      }
    }
  }
  *(double *)(this + 8) = param_2;
  *(double *)(this + 0x10) = param_3;
  piVar5 = *(int **)CONCAT44(in_register_00000014,param_9);
  uVar6 = ((undefined8 *)CONCAT44(in_register_00000014,param_9))[1];
  *(double *)(this + 0x18) = param_4;
  *(double *)(this + 0x20) = param_5;
  *(double *)(this + 0x28) = param_6;
  *(double *)(this + 0x40) = param_7;
  *(double *)(this + 0x48) = param_8;
  if (piVar5 != (int *)0x0) {
    LOCK();
    *piVar5 = *piVar5 + 1;
    UNLOCK();
  }
  piVar4 = *(int **)(this + 0x50);
  *(int **)(this + 0x50) = piVar5;
  *(undefined8 *)(this + 0x58) = uVar6;
  if (piVar4 != (int *)0x0) {
    LOCK();
    *piVar4 = *piVar4 + -1;
    UNLOCK();
    if (*piVar4 == 0) {
      operator_delete(piVar4,0x10);
    }
  }
  *(KisFilterStrategy **)(this + 0x60) = param_10;
  return;
}



