/* Class KisNotifySelectionChangedCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNotifySelectionChangedCommand @ 002031c0 ======

void __thiscall
KisNotifySelectionChangedCommand::KisNotifySelectionChangedCommand
          (KisNotifySelectionChangedCommand *this,KisWeakSharedPtr param_1,State param_2)

{
  (*(code *)PTR_KisNotifySelectionChangedCommand_008393b0)();
  return;
}



// ====== KisNotifySelectionChangedCommand @ 003694b0 ======

/* KisNotifySelectionChangedCommand::KisNotifySelectionChangedCommand(KisWeakSharedPtr<KisImage>,
   KisCommandUtils::FlipFlopCommand::State) */

void __thiscall
KisNotifySelectionChangedCommand::KisNotifySelectionChangedCommand
          (KisNotifySelectionChangedCommand *this,KisWeakSharedPtr param_1,State param_2)

{
  long lVar1;
  int *piVar2;
  undefined4 in_register_00000034;
  long *plVar3;
  
  plVar3 = (long *)CONCAT44(in_register_00000034,param_1);
  KisCommandUtils::FlipFlopCommand::FlipFlopCommand
            ((FlipFlopCommand *)this,param_2,(KUndo2Command *)0x0);
  lVar1 = *plVar3;
  *(undefined **)this = PTR_vtable_00837a10 + 0x10;
  if (lVar1 == 0) {
    *(undefined8 *)(this + 0x30) = 0;
  }
  else {
    if (((uint *)plVar3[1] == (uint *)0x0) || ((*(uint *)plVar3[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x30) = (undefined  [16])0x0;
      return;
    }
    lVar1 = *plVar3;
    *(long *)(this + 0x30) = lVar1;
    if (lVar1 != 0) {
      piVar2 = *(int **)(lVar1 + 0x58);
      if (piVar2 == (int *)0x0) {
                    /* try { // try from 00369555 to 00369559 has its CatchHandler @ 0036956e */
        piVar2 = (int *)operator_new(4);
        *piVar2 = 0;
        *(int **)(lVar1 + 0x58) = piVar2;
        LOCK();
        *piVar2 = *piVar2 + 1;
        UNLOCK();
        piVar2 = *(int **)(lVar1 + 0x58);
      }
      *(int **)(this + 0x38) = piVar2;
      LOCK();
      *piVar2 = *piVar2 + 2;
      UNLOCK();
      return;
    }
  }
  *(undefined8 *)(this + 0x38) = 0;
  return;
}



