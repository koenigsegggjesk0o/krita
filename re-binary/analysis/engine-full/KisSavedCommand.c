/* Class KisSavedCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSavedCommand @ 0020a860 ======

void __thiscall
KisSavedCommand::KisSavedCommand
          (KisSavedCommand *this,QSharedPointer param_1,KisStrokesFacade *param_2)

{
  (*(code *)PTR_KisSavedCommand_0083cf00)();
  return;
}



// ====== KisSavedCommand @ 0036af70 ======

/* KisSavedCommand::KisSavedCommand(QSharedPointer<KUndo2Command>, KisStrokesFacade*) */

void __thiscall
KisSavedCommand::KisSavedCommand
          (KisSavedCommand *this,QSharedPointer param_1,KisStrokesFacade *param_2)

{
  int *piVar1;
  undefined8 uVar2;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  QArrayData *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KUndo2Command::text();
                    /* try { // try from 0036afae to 0036afb2 has its CatchHandler @ 0036b02e */
  KisSavedCommandBase::KisSavedCommandBase
            ((KisSavedCommandBase *)this,(KUndo2MagicString *)&local_38,param_2);
  if (*(int *)local_38 != 0) {
    if (*(int *)local_38 == -1) goto LAB_0036afcc;
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 != 0) goto LAB_0036afcc;
  }
  QArrayData::deallocate(local_38,2,8);
LAB_0036afcc:
  *(undefined **)this = PTR_vtable_00837640 + 0x10;
  uVar2 = ((undefined8 *)CONCAT44(in_register_00000034,param_1))[1];
  *(undefined8 *)(this + 0x38) = *(undefined8 *)CONCAT44(in_register_00000034,param_1);
  *(undefined8 *)(this + 0x40) = uVar2;
  piVar1 = *(int **)(this + 0x40);
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x40) + 4) = *(int *)(*(long *)(this + 0x40) + 4) + 1;
    UNLOCK();
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



