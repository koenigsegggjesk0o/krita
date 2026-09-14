/* Class KisSimpleUpdateCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSimpleUpdateCommand @ 00549de0 ======

/* KisLayerUtils::KisSimpleUpdateCommand::KisSimpleUpdateCommand(QList<KisSharedPtr<KisNode> >,
   bool, KUndo2Command*) */

void __thiscall
KisLayerUtils::KisSimpleUpdateCommand::KisSimpleUpdateCommand
          (KisSimpleUpdateCommand *this,undefined8 param_2,bool param_3,KUndo2Command *param_4)

{
  KisCommandUtils::FlipFlopCommand::FlipFlopCommand((FlipFlopCommand *)this,param_3,param_4);
  *(undefined **)this = PTR_vtable_00837e30 + 0x10;
                    /* try { // try from 00549e10 to 00549e14 has its CatchHandler @ 00549e1c */
  FUN_002de570(this + 0x30,param_2);
  return;
}



