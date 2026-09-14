/* Class KisSavedCommandBase - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSavedCommandBase @ 00202f30 ======

void __thiscall
KisSavedCommandBase::KisSavedCommandBase
          (KisSavedCommandBase *this,KUndo2MagicString *param_1,KisStrokesFacade *param_2)

{
  (*(code *)PTR_KisSavedCommandBase_00839268)();
  return;
}



// ====== KisSavedCommandBase @ 0036adb0 ======

/* KisSavedCommandBase::KisSavedCommandBase(KUndo2MagicString const&, KisStrokesFacade*) */

void __thiscall
KisSavedCommandBase::KisSavedCommandBase
          (KisSavedCommandBase *this,KUndo2MagicString *param_1,KisStrokesFacade *param_2)

{
  undefined *puVar1;
  
  KUndo2Command::KUndo2Command((KUndo2Command *)this,param_1,(KUndo2Command *)0x0);
  puVar1 = PTR_vtable_00836fb8;
  *(KisStrokesFacade **)(this + 0x28) = param_2;
  this[0x30] = (KisSavedCommandBase)0x1;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



