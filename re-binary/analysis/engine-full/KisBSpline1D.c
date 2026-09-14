/* Class KisBSpline1D - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisBSpline1D @ 0060a3f0 ======

/* KisBSplines::KisBSpline1D::KisBSpline1D(float, float, int, KisBSplines::BorderCondition) */

void __thiscall
KisBSplines::KisBSpline1D::KisBSpline1D
          (KisBSpline1D *this,float param_1,float param_2,int param_3,BorderCondition param_4)

{
  BorderCondition *pBVar1;
  
  pBVar1 = (BorderCondition *)operator_new(0x10);
  *(int *)(this + 0x10) = param_3;
  *(BorderCondition **)this = pBVar1;
  *pBVar1 = param_4;
  pBVar1[2] = 0;
  pBVar1[3] = 0;
  *(ulong *)(this + 8) = CONCAT44(param_2,param_1);
  return;
}



