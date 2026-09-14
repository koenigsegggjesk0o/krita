/* Class KisStroke - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisStroke @ 00209ab0 ======

void __thiscall
KisStroke::KisStroke(KisStroke *this,KisStrokeStrategy *param_1,Type param_2,int param_3)

{
  (*(code *)PTR_KisStroke_0083c828)();
  return;
}



// ====== KisStroke @ 004ec0a0 ======

/* KisStroke::KisStroke(KisStrokeStrategy*, KisStroke::Type, int) */

void __thiscall
KisStroke::KisStroke(KisStroke *this,KisStrokeStrategy *param_1,Type param_2,int param_3)

{
  long lVar1;
  long *plVar2;
  long *plVar3;
  KisStrokeJobData *pKVar4;
  
  *(undefined **)(this + 0x38) = PTR_shared_null_00837830;
  *(undefined (*) [16])(this + 0x50) = (undefined  [16])0x0;
  lVar1 = *(long *)param_1;
  *(KisStrokeStrategy **)this = param_1;
  *(undefined8 *)(this + 8) = 0;
  *(undefined8 *)(this + 0x10) = 0;
  *(undefined8 *)(this + 0x18) = 0;
  *(undefined8 *)(this + 0x20) = 0;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined8 *)(this + 0x30) = 0;
  *(undefined4 *)(this + 0x40) = 0;
  *(int *)(this + 0x44) = param_3;
  *(Type *)(this + 0x48) = param_2;
                    /* try { // try from 004ec104 to 004ec1eb has its CatchHandler @ 004ec201 */
  plVar3 = (long *)(**(code **)(lVar1 + 0x20))(param_1);
  plVar2 = *(long **)(this + 8);
  if ((plVar3 != plVar2) && (*(long **)(this + 8) = plVar3, plVar2 != (long *)0x0)) {
    (**(code **)(*plVar2 + 8))();
  }
  plVar3 = (long *)(**(code **)(**(long **)this + 0x38))();
  plVar2 = *(long **)(this + 0x10);
  if ((plVar3 != plVar2) && (*(long **)(this + 0x10) = plVar3, plVar2 != (long *)0x0)) {
    (**(code **)(*plVar2 + 8))();
  }
  plVar3 = (long *)(**(code **)(**(long **)this + 0x30))();
  plVar2 = *(long **)(this + 0x18);
  if ((plVar3 != plVar2) && (*(long **)(this + 0x18) = plVar3, plVar2 != (long *)0x0)) {
    (**(code **)(*plVar2 + 8))();
  }
  plVar3 = (long *)(**(code **)(**(long **)this + 0x28))();
  plVar2 = *(long **)(this + 0x20);
  if ((plVar3 != plVar2) && (*(long **)(this + 0x20) = plVar3, plVar2 != (long *)0x0)) {
    (**(code **)(*plVar2 + 8))();
  }
  plVar3 = (long *)(**(code **)(**(long **)this + 0x40))();
  plVar2 = *(long **)(this + 0x28);
  if ((plVar3 != plVar2) && (*(long **)(this + 0x28) = plVar3, plVar2 != (long *)0x0)) {
    (**(code **)(*plVar2 + 8))();
  }
  plVar3 = (long *)(**(code **)(**(long **)this + 0x48))();
  plVar2 = *(long **)(this + 0x30);
  if ((plVar3 != plVar2) && (*(long **)(this + 0x30) = plVar3, plVar2 != (long *)0x0)) {
    (**(code **)(*plVar2 + 8))();
  }
  (**(code **)(**(long **)this + 0x10))();
  if (*(long *)(this + 8) != 0) {
    pKVar4 = (KisStrokeJobData *)(**(code **)(**(long **)this + 0x50))();
    enqueue(this,*(KisStrokeJobStrategy **)(this + 8),pKVar4);
    return;
  }
  this[0x40] = (KisStroke)0x1;
  return;
}



