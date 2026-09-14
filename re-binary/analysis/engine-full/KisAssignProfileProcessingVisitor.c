/* Class KisAssignProfileProcessingVisitor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisAssignProfileProcessingVisitor @ 00208a50 ======

void __thiscall
KisAssignProfileProcessingVisitor::KisAssignProfileProcessingVisitor
          (KisAssignProfileProcessingVisitor *this,KoColorSpace *param_1,KoColorSpace *param_2)

{
  (*(code *)PTR_KisAssignProfileProcessingVisitor_0083bff8)();
  return;
}



// ====== KisAssignProfileProcessingVisitor @ 0037a130 ======

/* KisAssignProfileProcessingVisitor::KisAssignProfileProcessingVisitor(KoColorSpace const*,
   KoColorSpace const*) */

void __thiscall
KisAssignProfileProcessingVisitor::KisAssignProfileProcessingVisitor
          (KisAssignProfileProcessingVisitor *this,KoColorSpace *param_1,KoColorSpace *param_2)

{
  undefined *puVar1;
  
  KisShared::KisShared((KisShared *)(this + 8));
  puVar1 = PTR_vtable_008372a8;
  *(KoColorSpace **)(this + 0x18) = param_1;
  *(KoColorSpace **)(this + 0x20) = param_2;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



