/* Class KisSavedMacroCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSavedMacroCommand @ 00206400 ======

void __thiscall
KisSavedMacroCommand::KisSavedMacroCommand
          (KisSavedMacroCommand *this,KUndo2MagicString *param_1,KisStrokesFacade *param_2)

{
  (*(code *)PTR_KisSavedMacroCommand_0083acd0)();
  return;
}



// ====== KisSavedMacroCommand @ 0036b040 ======

/* KisSavedMacroCommand::KisSavedMacroCommand(KUndo2MagicString const&, KisStrokesFacade*) */

void __thiscall
KisSavedMacroCommand::KisSavedMacroCommand
          (KisSavedMacroCommand *this,KUndo2MagicString *param_1,KisStrokesFacade *param_2)

{
  undefined *puVar1;
  undefined8 *puVar2;
  
  KisSavedCommandBase::KisSavedCommandBase((KisSavedCommandBase *)this,param_1,param_2);
  *(undefined **)this = PTR_vtable_00836c20 + 0x10;
                    /* try { // try from 0036b065 to 0036b069 has its CatchHandler @ 0036b092 */
  puVar2 = (undefined8 *)operator_new(0x20);
  puVar1 = PTR_shared_null_008377d0;
  puVar2[2] = 0;
  *(undefined4 *)(puVar2 + 1) = 0xffffffff;
  *puVar2 = puVar1;
  puVar2[3] = puVar1;
  *(undefined8 **)(this + 0x38) = puVar2;
  return;
}



