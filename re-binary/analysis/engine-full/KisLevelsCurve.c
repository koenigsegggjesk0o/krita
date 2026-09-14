/* Class KisLevelsCurve - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLevelsCurve @ 002037e0 ======

void __thiscall
KisLevelsCurve::KisLevelsCurve
          (KisLevelsCurve *this,double param_1,double param_2,double param_3,double param_4,
          double param_5)

{
  (*(code *)PTR_KisLevelsCurve_008396c0)();
  return;
}



// ====== KisLevelsCurve @ 00205a70 ======

void __thiscall KisLevelsCurve::KisLevelsCurve(KisLevelsCurve *this)

{
  (*(code *)PTR_KisLevelsCurve_0083a808)();
  return;
}



// ====== KisLevelsCurve @ 004aaa50 ======

/* KisLevelsCurve::KisLevelsCurve(double, double, double, double, double) */

void __thiscall
KisLevelsCurve::KisLevelsCurve
          (KisLevelsCurve *this,double param_1,double param_2,double param_3,double param_4,
          double param_5)

{
  undefined *puVar1;
  
  puVar1 = PTR_shared_null_008377d0;
  *(undefined2 *)(this + 0x58) = 0x101;
  *(double *)this = param_1;
  *(double *)(this + 8) = param_2;
  *(undefined **)(this + 0x50) = puVar1;
  *(double *)(this + 0x10) = param_3;
  *(double *)(this + 0x18) = param_4;
  *(double *)(this + 0x20) = param_5;
  *(double *)(this + 0x28) = param_2 - param_1;
  *(double *)(this + 0x30) = DAT_007231a8 / param_3;
  *(double *)(this + 0x38) = param_5 - param_4;
  *(undefined **)(this + 0x40) = puVar1;
  *(undefined **)(this + 0x48) = puVar1;
  return;
}



// ====== KisLevelsCurve @ 004aaac0 ======

/* KisLevelsCurve::KisLevelsCurve() */

void __thiscall KisLevelsCurve::KisLevelsCurve(KisLevelsCurve *this)

{
  KisLevelsCurve(this,0.0,DAT_007231a8,DAT_007231a8,0.0,DAT_007231a8);
  return;
}



// ====== KisLevelsCurve @ 004ab010 ======

/* KisLevelsCurve::KisLevelsCurve(QString const&) */

void __thiscall KisLevelsCurve::KisLevelsCurve(KisLevelsCurve *this,QString *param_1)

{
  KisLevelsCurve(this);
                    /* try { // try from 004ab02d to 004ab031 has its CatchHandler @ 004ab039 */
  fromString(this,param_1,(bool *)0x0);
  return;
}



