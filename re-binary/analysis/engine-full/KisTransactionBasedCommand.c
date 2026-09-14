/* Class KisTransactionBasedCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTransactionBasedCommand @ 0020a2b0 ======

void __thiscall
KisTransactionBasedCommand::KisTransactionBasedCommand
          (KisTransactionBasedCommand *this,KUndo2MagicString *param_1,KUndo2Command *param_2)

{
  (*(code *)PTR_KisTransactionBasedCommand_0083cc28)();
  return;
}



// ====== KisTransactionBasedCommand @ 00370ae0 ======

/* KisTransactionBasedCommand::KisTransactionBasedCommand(KUndo2MagicString const&, KUndo2Command*)
    */

void __thiscall
KisTransactionBasedCommand::KisTransactionBasedCommand
          (KisTransactionBasedCommand *this,KUndo2MagicString *param_1,KUndo2Command *param_2)

{
  undefined *puVar1;
  
  KUndo2Command::KUndo2Command((KUndo2Command *)this,param_1,param_2);
  puVar1 = PTR_vtable_00837980;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



