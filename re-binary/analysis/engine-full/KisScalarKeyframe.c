/* Class KisScalarKeyframe - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisScalarKeyframe @ 002030e0 ======

void __thiscall KisScalarKeyframe::KisScalarKeyframe(void)

{
  (*(code *)PTR_KisScalarKeyframe_00839340)();
  return;
}



// ====== KisScalarKeyframe @ 0020ad90 ======

void __thiscall
KisScalarKeyframe::KisScalarKeyframe(KisScalarKeyframe *this,double param_1,QSharedPointer param_2)

{
  (*(code *)PTR_KisScalarKeyframe_0083d198)();
  return;
}



// ====== KisScalarKeyframe @ 00653a70 ======

/* KisScalarKeyframe::KisScalarKeyframe(double, QSharedPointer<ScalarKeyframeLimits>) */

void __thiscall
KisScalarKeyframe::KisScalarKeyframe(KisScalarKeyframe *this,double param_1,QSharedPointer param_2)

{
  undefined8 uVar1;
  int *piVar2;
  undefined4 in_register_00000034;
  
  KisKeyframe::KisKeyframe((KisKeyframe *)this);
  *(undefined **)this = PTR_vtable_00837bf8 + 0x10;
                    /* try { // try from 00653a9a to 00653a9e has its CatchHandler @ 00653ade */
  QMetaObject::Connection::Connection((Connection *)(this + 0x18));
  uVar1 = DAT_00721bd8;
  *(double *)(this + 0x20) = param_1;
  *(undefined (*) [16])(this + 0x30) = (undefined  [16])0x0;
  *(undefined8 *)(this + 0x28) = uVar1;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  uVar1 = *(undefined8 *)CONCAT44(in_register_00000034,param_2);
  piVar2 = (int *)((undefined8 *)CONCAT44(in_register_00000034,param_2))[1];
  *(int **)(this + 0x50) = piVar2;
  *(undefined8 *)(this + 0x58) = uVar1;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  return;
}



// ====== KisScalarKeyframe @ 00653af0 ======

/* KisScalarKeyframe::KisScalarKeyframe(double, KisScalarKeyframe::InterpolationMode,
   KisScalarKeyframe::TangentsMode, QPointF, QPointF, QSharedPointer<ScalarKeyframeLimits>) */

void __thiscall
KisScalarKeyframe::KisScalarKeyframe
          (undefined8 param_1,undefined8 param_2_00,undefined8 param_3_00,undefined8 param_4,
          undefined8 param_5,KisScalarKeyframe *this,undefined4 param_2,undefined4 param_3,
          undefined8 *param_9)

{
  undefined8 uVar1;
  int *piVar2;
  
  KisKeyframe::KisKeyframe((KisKeyframe *)this);
  *(undefined **)this = PTR_vtable_00837bf8 + 0x10;
                    /* try { // try from 00653b41 to 00653b45 has its CatchHandler @ 00653b9f */
  QMetaObject::Connection::Connection((Connection *)(this + 0x18));
  *(undefined8 *)(this + 0x20) = param_1;
  *(undefined4 *)(this + 0x28) = param_2;
  *(undefined8 *)(this + 0x38) = param_3_00;
  *(undefined4 *)(this + 0x2c) = param_3;
  *(undefined8 *)(this + 0x40) = param_4;
  *(undefined8 *)(this + 0x30) = param_2_00;
  *(undefined8 *)(this + 0x48) = param_5;
  uVar1 = *param_9;
  piVar2 = (int *)param_9[1];
  *(int **)(this + 0x50) = piVar2;
  *(undefined8 *)(this + 0x58) = uVar1;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  return;
}



