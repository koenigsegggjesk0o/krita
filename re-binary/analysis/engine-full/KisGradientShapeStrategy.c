/* Class KisGradientShapeStrategy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGradientShapeStrategy @ 00203fa0 ======

void __thiscall
KisGradientShapeStrategy::KisGradientShapeStrategy
          (KisGradientShapeStrategy *this,QPointF *param_1,QPointF *param_2)

{
  (*(code *)PTR_KisGradientShapeStrategy_00839aa0)();
  return;
}



// ====== KisGradientShapeStrategy @ 00209d10 ======

void __thiscall KisGradientShapeStrategy::KisGradientShapeStrategy(KisGradientShapeStrategy *this)

{
  (*(code *)PTR_KisGradientShapeStrategy_0083c958)();
  return;
}



// ====== KisGradientShapeStrategy @ 004daf40 ======

/* KisGradientShapeStrategy::KisGradientShapeStrategy() */

void __thiscall KisGradientShapeStrategy::KisGradientShapeStrategy(KisGradientShapeStrategy *this)

{
  undefined *puVar1;
  
  puVar1 = PTR_vtable_00837428;
  *(undefined (*) [16])(this + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x18) = (undefined  [16])0x0;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



// ====== KisGradientShapeStrategy @ 004daf60 ======

/* KisGradientShapeStrategy::KisGradientShapeStrategy(QPointF const&, QPointF const&) */

void __thiscall
KisGradientShapeStrategy::KisGradientShapeStrategy
          (KisGradientShapeStrategy *this,QPointF *param_1,QPointF *param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined *puVar4;
  
  uVar1 = *(undefined8 *)(param_1 + 8);
  uVar2 = *(undefined8 *)param_2;
  uVar3 = *(undefined8 *)(param_2 + 8);
  puVar4 = PTR_vtable_00837428 + 0x10;
  *(undefined8 *)(this + 8) = *(undefined8 *)param_1;
  *(undefined8 *)(this + 0x10) = uVar1;
  *(undefined **)this = puVar4;
  *(undefined8 *)(this + 0x18) = uVar2;
  *(undefined8 *)(this + 0x20) = uVar3;
  return;
}



