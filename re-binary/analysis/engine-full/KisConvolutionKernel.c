/* Class KisConvolutionKernel - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisConvolutionKernel @ 00205f90 ======

void __thiscall
KisConvolutionKernel::KisConvolutionKernel
          (KisConvolutionKernel *this,uint param_1,uint param_2,double param_3,double param_4)

{
  (*(code *)PTR_KisConvolutionKernel_0083aa98)();
  return;
}



// ====== KisConvolutionKernel @ 00471970 ======

/* KisConvolutionKernel::KisConvolutionKernel(unsigned int, unsigned int, double, double) */

void __thiscall
KisConvolutionKernel::KisConvolutionKernel
          (KisConvolutionKernel *this,uint param_1,uint param_2,double param_3,double param_4)

{
  double *pdVar1;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837078 + 0x10;
                    /* try { // try from 004719b3 to 004719e9 has its CatchHandler @ 004719f9 */
  pdVar1 = (double *)operator_new(0x28);
  *(double **)(this + 0x18) = pdVar1;
  pdVar1[2] = 0.0;
  *(undefined (*) [16])(pdVar1 + 3) = (undefined  [16])0x0;
  *pdVar1 = param_3;
  pdVar1[1] = param_4;
  setSize(this,param_1,param_2);
  return;
}



