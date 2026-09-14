/* Class KisCubicCurvePoint - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCubicCurvePoint @ 00202e50 ======

void __thiscall
KisCubicCurvePoint::KisCubicCurvePoint(KisCubicCurvePoint *this,QPointF *param_1,bool param_2)

{
  (*(code *)PTR_KisCubicCurvePoint_008391f8)();
  return;
}



// ====== KisCubicCurvePoint @ 00202f90 ======

void __thiscall
KisCubicCurvePoint::KisCubicCurvePoint
          (KisCubicCurvePoint *this,double param_1,double param_2,bool param_3)

{
  (*(code *)PTR_KisCubicCurvePoint_00839298)();
  return;
}



// ====== KisCubicCurvePoint @ 0048f380 ======

/* KisCubicCurvePoint::KisCubicCurvePoint(QPointF const&, bool) */

void __thiscall
KisCubicCurvePoint::KisCubicCurvePoint(KisCubicCurvePoint *this,QPointF *param_1,bool param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  this[0x10] = (KisCubicCurvePoint)param_2;
  *(undefined8 *)this = uVar1;
  *(undefined8 *)(this + 8) = uVar2;
  return;
}



// ====== KisCubicCurvePoint @ 0048f390 ======

/* KisCubicCurvePoint::KisCubicCurvePoint(double, double, bool) */

void __thiscall
KisCubicCurvePoint::KisCubicCurvePoint
          (KisCubicCurvePoint *this,double param_1,double param_2,bool param_3)

{
  this[0x10] = (KisCubicCurvePoint)param_3;
  *(double *)this = param_1;
  *(double *)(this + 8) = param_2;
  return;
}



