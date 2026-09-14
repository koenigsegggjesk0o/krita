/* Class KisImageCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageCommand @ 0020b4d0 ======

void __thiscall
KisImageCommand::KisImageCommand
          (KisImageCommand *this,KUndo2MagicString *param_1,KisWeakSharedPtr param_2,
          KUndo2Command *param_3)

{
  (*(code *)PTR_KisImageCommand_0083d538)();
  return;
}



// ====== KisImageCommand @ 0035c290 ======

/* KisImageCommand::KisImageCommand(KUndo2MagicString const&, KisWeakSharedPtr<KisImage>,
   KUndo2Command*) */

void __thiscall
KisImageCommand::KisImageCommand
          (KisImageCommand *this,KUndo2MagicString *param_1,KisWeakSharedPtr param_2,
          KUndo2Command *param_3)

{
  long lVar1;
  int *piVar2;
  undefined4 in_register_00000014;
  long *plVar3;
  
  plVar3 = (long *)CONCAT44(in_register_00000014,param_2);
  KUndo2Command::KUndo2Command((KUndo2Command *)this,param_1,param_3);
  lVar1 = *plVar3;
  *(undefined **)this = PTR_vtable_00837a98 + 0x10;
  if (lVar1 == 0) {
    *(undefined8 *)(this + 0x28) = 0;
  }
  else {
    if (((uint *)plVar3[1] == (uint *)0x0) || ((*(uint *)plVar3[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
      return;
    }
    lVar1 = *plVar3;
    *(long *)(this + 0x28) = lVar1;
    if (lVar1 != 0) {
      piVar2 = *(int **)(lVar1 + 0x58);
      if (piVar2 == (int *)0x0) {
                    /* try { // try from 0035c335 to 0035c339 has its CatchHandler @ 0035c34e */
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
      return;
    }
  }
  *(undefined8 *)(this + 0x30) = 0;
  return;
}



