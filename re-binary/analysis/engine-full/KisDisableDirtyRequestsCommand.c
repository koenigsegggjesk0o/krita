/* Class KisDisableDirtyRequestsCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisDisableDirtyRequestsCommand @ 00375cc0 ======

/* KisDisableDirtyRequestsCommand::KisDisableDirtyRequestsCommand(KisUpdatesFacade*,
   KisCommandUtils::FlipFlopCommand::State) */

void __thiscall
KisDisableDirtyRequestsCommand::KisDisableDirtyRequestsCommand
          (KisDisableDirtyRequestsCommand *this,KisUpdatesFacade *param_1,State param_2)

{
  undefined *puVar1;
  
  KisCommandUtils::FlipFlopCommand::FlipFlopCommand
            ((FlipFlopCommand *)this,param_2,(KUndo2Command *)0x0);
  puVar1 = PTR_vtable_00837070;
  *(KisUpdatesFacade **)(this + 0x30) = param_1;
  *(undefined **)this = puVar1 + 0x10;
  return;
}



