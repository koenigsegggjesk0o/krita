/* Class KisHoldUIUpdatesCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisHoldUIUpdatesCommand @ 00370d70 ======

/* KisHoldUIUpdatesCommand::KisHoldUIUpdatesCommand(KisUpdatesFacade*,
   KisCommandUtils::FlipFlopCommand::State) */

void __thiscall
KisHoldUIUpdatesCommand::KisHoldUIUpdatesCommand
          (KisHoldUIUpdatesCommand *this,KisUpdatesFacade *param_1,State param_2)

{
  undefined *puVar1;
  undefined4 *puVar2;
  
  KisCommandUtils::FlipFlopCommand::FlipFlopCommand
            ((FlipFlopCommand *)this,param_2,(KUndo2Command *)0x0);
  puVar1 = PTR_vtable_00837d20;
  *(KisUpdatesFacade **)(this + 0x40) = param_1;
  *(undefined **)this = puVar1 + 0x10;
  *(undefined **)(this + 0x30) = puVar1 + 0xc0;
                    /* try { // try from 00370daa to 00370dc2 has its CatchHandler @ 00370dea */
  puVar1 = (undefined *)operator_new(1);
  *puVar1 = 0;
  *(undefined **)(this + 0x48) = puVar1;
  puVar2 = (undefined4 *)operator_new(0x18);
  *(undefined **)(puVar2 + 4) = puVar1;
  *(code **)(puVar2 + 2) = FUN_00371620;
  puVar2[1] = 1;
  *puVar2 = 1;
  *(undefined4 **)(this + 0x50) = puVar2;
  return;
}



