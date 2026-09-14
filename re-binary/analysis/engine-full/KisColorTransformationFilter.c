/* Class KisColorTransformationFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisColorTransformationFilter @ 00384500 ======

/* KisColorTransformationFilter::KisColorTransformationFilter(KoID const&, KoID const&, QString
   const&) */

void __thiscall
KisColorTransformationFilter::KisColorTransformationFilter
          (KisColorTransformationFilter *this,KoID *param_1,KoID *param_2,QString *param_3)

{
  KisFilter::KisFilter((KisFilter *)this,param_1,param_2,param_3);
  *(undefined **)this = PTR_vtable_008377e8 + 0x10;
                    /* try { // try from 00384528 to 0038452c has its CatchHandler @ 00384534 */
  KisFilter::setSupportsLevelOfDetail((KisFilter *)this,true);
  return;
}



