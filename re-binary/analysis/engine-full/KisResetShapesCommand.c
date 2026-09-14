/* Class KisResetShapesCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisResetShapesCommand @ 002088c0 ======

void __thiscall
KisResetShapesCommand::KisResetShapesCommand(KisResetShapesCommand *this,KisSharedPtr param_1)

{
  (*(code *)PTR_KisResetShapesCommand_0083bf30)();
  return;
}



// ====== KisResetShapesCommand @ 0036d840 ======

/* KisResetShapesCommand::KisResetShapesCommand(KisSharedPtr<KisNode>) */

void __thiscall
KisResetShapesCommand::KisResetShapesCommand(KisResetShapesCommand *this,KisSharedPtr param_1)

{
  int iVar1;
  long lVar2;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  QArrayData *local_40;
  QArrayData *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  local_38 = (QArrayData *)QString::fromAscii_helper("RESET_SHAPES_COMMAND",0x14);
                    /* try { // try from 0036d88a to 0036d88e has its CatchHandler @ 0036d94c */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_40,(QString *)&local_38);
                    /* try { // try from 0036d897 to 0036d89b has its CatchHandler @ 0036d940 */
  KUndo2Command::KUndo2Command
            ((KUndo2Command *)this,(KUndo2MagicString *)&local_40,(KUndo2Command *)0x0);
  if (*(int *)local_40 == 0) {
LAB_0036d910:
    QArrayData::deallocate(local_40,2,8);
    iVar1 = *(int *)local_38;
  }
  else {
    if (*(int *)local_40 != -1) {
      LOCK();
      *(int *)local_40 = *(int *)local_40 + -1;
      UNLOCK();
      if (*(int *)local_40 == 0) goto LAB_0036d910;
    }
    iVar1 = *(int *)local_38;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_0036d8d2;
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 != 0) goto LAB_0036d8d2;
  }
  QArrayData::deallocate(local_38,2,8);
LAB_0036d8d2:
  *(undefined **)this = PTR_vtable_00836c00 + 0x10;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x28) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



