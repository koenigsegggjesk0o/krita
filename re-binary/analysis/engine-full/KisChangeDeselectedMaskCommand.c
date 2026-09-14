/* Class KisChangeDeselectedMaskCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisChangeDeselectedMaskCommand @ 00202c20 ======

void __thiscall
KisChangeDeselectedMaskCommand::KisChangeDeselectedMaskCommand
          (KisChangeDeselectedMaskCommand *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisChangeDeselectedMaskCommand_008390e0)();
  return;
}



// ====== KisChangeDeselectedMaskCommand @ 002062f0 ======

void __thiscall
KisChangeDeselectedMaskCommand::KisChangeDeselectedMaskCommand
          (KisChangeDeselectedMaskCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisChangeDeselectedMaskCommand_0083ac48)();
  return;
}



// ====== KisChangeDeselectedMaskCommand @ 00369140 ======

/* KisChangeDeselectedMaskCommand::KisChangeDeselectedMaskCommand(KisWeakSharedPtr<KisImage>) */

void __thiscall
KisChangeDeselectedMaskCommand::KisChangeDeselectedMaskCommand
          (KisChangeDeselectedMaskCommand *this,KisWeakSharedPtr param_1)

{
  long lVar1;
  int *piVar2;
  undefined4 in_register_00000034;
  long *plVar3;
  
  plVar3 = (long *)CONCAT44(in_register_00000034,param_1);
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2Command *)0x0);
  lVar1 = *plVar3;
  *(undefined **)this = PTR_vtable_00837b58 + 0x10;
  if (lVar1 == 0) {
    *(undefined8 *)(this + 0x28) = 0;
  }
  else {
    if (((uint *)plVar3[1] == (uint *)0x0) || ((*(uint *)plVar3[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
      *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
      return;
    }
    lVar1 = *plVar3;
    *(long *)(this + 0x28) = lVar1;
    if (lVar1 != 0) {
      piVar2 = *(int **)(lVar1 + 0x58);
      if (piVar2 == (int *)0x0) {
                    /* try { // try from 003691f5 to 003691f9 has its CatchHandler @ 0036920e */
        piVar2 = (int *)operator_new(4);
        *piVar2 = 0;
        *(int **)(lVar1 + 0x58) = piVar2;
        LOCK();
        *piVar2 = *piVar2 + 1;
        UNLOCK();
        piVar2 = *(int **)(lVar1 + 0x58);
      }
      *(int **)(this + 0x30) = piVar2;
      LOCK();
      *piVar2 = *piVar2 + 2;
      UNLOCK();
      *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
      return;
    }
  }
  *(undefined8 *)(this + 0x30) = 0;
  *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
  return;
}



// ====== KisChangeDeselectedMaskCommand @ 00369220 ======

/* KisChangeDeselectedMaskCommand::KisChangeDeselectedMaskCommand(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisSelectionMask>) */

void __thiscall
KisChangeDeselectedMaskCommand::KisChangeDeselectedMaskCommand
          (KisChangeDeselectedMaskCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2)

{
  long lVar1;
  int *piVar2;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long *plVar3;
  
  plVar3 = (long *)CONCAT44(in_register_00000034,param_1);
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2Command *)0x0);
  lVar1 = *plVar3;
  *(undefined **)this = PTR_vtable_00837b58 + 0x10;
  if (lVar1 == 0) {
    *(undefined8 *)(this + 0x28) = 0;
  }
  else {
    if (((uint *)plVar3[1] == (uint *)0x0) || ((*(uint *)plVar3[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
      goto LAB_00369264;
    }
    lVar1 = *plVar3;
    *(long *)(this + 0x28) = lVar1;
    if (lVar1 != 0) {
      piVar2 = *(int **)(lVar1 + 0x58);
      if (piVar2 == (int *)0x0) {
                    /* try { // try from 003692cd to 003692d1 has its CatchHandler @ 003692e6 */
        piVar2 = (int *)operator_new(4);
        *piVar2 = 0;
        *(int **)(lVar1 + 0x58) = piVar2;
        LOCK();
        *piVar2 = *piVar2 + 1;
        UNLOCK();
        piVar2 = *(int **)(lVar1 + 0x58);
      }
      *(int **)(this + 0x30) = piVar2;
      LOCK();
      *piVar2 = *piVar2 + 2;
      UNLOCK();
      goto LAB_00369264;
    }
  }
  *(undefined8 *)(this + 0x30) = 0;
LAB_00369264:
  lVar1 = *(long *)CONCAT44(in_register_00000014,param_2);
  *(long *)(this + 0x38) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  *(undefined8 *)(this + 0x40) = 0;
  return;
}



