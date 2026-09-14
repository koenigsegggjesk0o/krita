/* Class KisConvertColorSpaceProcessingVisitor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisConvertColorSpaceProcessingVisitor @ 0020a4a0 ======

void __thiscall
KisConvertColorSpaceProcessingVisitor::KisConvertColorSpaceProcessingVisitor
          (KisConvertColorSpaceProcessingVisitor *this,KoColorSpace *param_1,KoColorSpace *param_2,
          Intent param_3,QFlags param_4)

{
  (*(code *)PTR_KisConvertColorSpaceProcessingVisitor_0083cd20)();
  return;
}



// ====== KisConvertColorSpaceProcessingVisitor @ 00379bd0 ======

/* KisConvertColorSpaceProcessingVisitor::KisConvertColorSpaceProcessingVisitor(KoColorSpace const*,
   KoColorSpace const*, KoColorConversionTransformation::Intent,
   QFlags<KoColorConversionTransformation::ConversionFlag>) */

void __thiscall
KisConvertColorSpaceProcessingVisitor::KisConvertColorSpaceProcessingVisitor
          (KisConvertColorSpaceProcessingVisitor *this,KoColorSpace *param_1,KoColorSpace *param_2,
          Intent param_3,QFlags param_4)

{
  undefined *puVar1;
  
  KisShared::KisShared((KisShared *)(this + 8));
  puVar1 = PTR_vtable_00837b10;
  *(KoColorSpace **)(this + 0x18) = param_1;
  *(KoColorSpace **)(this + 0x20) = param_2;
  *(Intent *)(this + 0x28) = param_3;
  *(undefined **)this = puVar1 + 0x10;
  *(QFlags *)(this + 0x2c) = param_4;
  return;
}



