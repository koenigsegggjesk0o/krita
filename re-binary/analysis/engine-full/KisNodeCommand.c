/* Class KisNodeCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeCommand @ 00201090 ======

void __thiscall
KisNodeCommand::KisNodeCommand(KisNodeCommand *this,KUndo2MagicString *param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisNodeCommand_00838318)();
  return;
}



// ====== KisNodeCommand @ 00361c80 ======

/* KisNodeCommand::KisNodeCommand(KUndo2MagicString const&, KisSharedPtr<KisNode>) */

void __thiscall
KisNodeCommand::KisNodeCommand(KisNodeCommand *this,KUndo2MagicString *param_1,KisSharedPtr param_2)

{
  long lVar1;
  undefined4 in_register_00000014;
  
  KUndo2Command::KUndo2Command((KUndo2Command *)this,param_1,(KUndo2Command *)0x0);
  *(undefined **)this = PTR_vtable_00837c98 + 0x10;
  lVar1 = *(long *)CONCAT44(in_register_00000014,param_2);
  *(long *)(this + 0x28) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  return;
}



