/* Class KisCropProcessingVisitor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisCropProcessingVisitor @ 00203a00 ======

void __thiscall
KisCropProcessingVisitor::KisCropProcessingVisitor
          (KisCropProcessingVisitor *this,QRect *param_1,bool param_2,bool param_3)

{
  (*(code *)PTR_KisCropProcessingVisitor_008397d0)();
  return;
}



// ====== KisCropProcessingVisitor @ 0037ae50 ======

/* KisCropProcessingVisitor::KisCropProcessingVisitor(QRect const&, bool, bool) */

void __thiscall
KisCropProcessingVisitor::KisCropProcessingVisitor
          (KisCropProcessingVisitor *this,QRect *param_1,bool param_2,bool param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined *puVar3;
  
  KisShared::KisShared((KisShared *)(this + 8));
  puVar3 = PTR_vtable_00836d58;
  uVar1 = *(undefined8 *)param_1;
  uVar2 = *(undefined8 *)(param_1 + 8);
  this[0x28] = (KisCropProcessingVisitor)param_2;
  this[0x29] = (KisCropProcessingVisitor)param_3;
  *(undefined8 *)(this + 0x18) = uVar1;
  *(undefined8 *)(this + 0x20) = uVar2;
  *(undefined **)this = puVar3 + 0x10;
  return;
}



