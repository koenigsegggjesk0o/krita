/* Class KisBSpline2D - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisBSpline2D @ 002035e0 ======

void __thiscall
KisBSplines::KisBSpline2D::KisBSpline2D
          (KisBSpline2D *this,float param_1,float param_2,int param_3,BorderCondition param_4,
          float param_5,float param_6,int param_7,BorderCondition param_8)

{
  (*(code *)PTR_KisBSpline2D_008395c0)();
  return;
}



// ====== KisBSpline2D @ 0060a6a0 ======

/* KisBSplines::KisBSpline2D::KisBSpline2D(float, float, int, KisBSplines::BorderCondition, float,
   float, int, KisBSplines::BorderCondition) */

void __thiscall
KisBSplines::KisBSpline2D::KisBSpline2D
          (KisBSpline2D *this,float param_1,float param_2,int param_3,BorderCondition param_4,
          float param_5,float param_6,int param_7,BorderCondition param_8)

{
  BorderCondition *pBVar1;
  
  pBVar1 = (BorderCondition *)operator_new(0x10);
  *(int *)(this + 0x10) = param_3;
  *(BorderCondition **)this = pBVar1;
  *(int *)(this + 0x1c) = param_7;
  *pBVar1 = param_4;
  pBVar1[1] = param_8;
  pBVar1[2] = 0;
  pBVar1[3] = 0;
  *(ulong *)(this + 8) = CONCAT44(param_2,param_1);
  *(ulong *)(this + 0x14) = CONCAT44(param_6,param_5);
  return;
}



