/* Class KisProcessingCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisProcessingCommand @ 00205ac0 ======

void __thiscall
KisProcessingCommand::KisProcessingCommand
          (KisProcessingCommand *this,KisSharedPtr param_1,KisSharedPtr param_2,
          KUndo2Command *param_3)

{
  (*(code *)PTR_KisProcessingCommand_0083a830)();
  return;
}



// ====== KisProcessingCommand @ 0036cdb0 ======

/* KisProcessingCommand::KisProcessingCommand(KisSharedPtr<KisProcessingVisitor>,
   KisSharedPtr<KisNode>, KUndo2Command*) */

void __thiscall
KisProcessingCommand::KisProcessingCommand
          (KisProcessingCommand *this,KisSharedPtr param_1,KisSharedPtr param_2,
          KUndo2Command *param_3)

{
  int iVar1;
  long lVar2;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_48 = (QArrayData *)QString::fromAscii_helper("processing_command",0x12);
                    /* try { // try from 0036ce04 to 0036ce08 has its CatchHandler @ 0036cf0f */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_50,(QString *)&local_48);
                    /* try { // try from 0036ce12 to 0036ce16 has its CatchHandler @ 0036cf03 */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_50,param_3);
  if (*(int *)local_50 == 0) {
LAB_0036cec0:
    QArrayData::deallocate(local_50,2,8);
    iVar1 = *(int *)local_48;
  }
  else {
    if (*(int *)local_50 != -1) {
      LOCK();
      *(int *)local_50 = *(int *)local_50 + -1;
      UNLOCK();
      if (*(int *)local_50 == 0) goto LAB_0036cec0;
    }
    iVar1 = *(int *)local_48;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_0036ce5d;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0036ce5d;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0036ce5d:
  *(undefined **)this = PTR_vtable_00837180 + 0x10;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x28) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 8) = *(int *)(lVar2 + 8) + 1;
    UNLOCK();
  }
  lVar2 = *(long *)CONCAT44(in_register_00000014,param_2);
  *(long *)(this + 0x30) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 0036ce93 to 0036ce97 has its CatchHandler @ 0036cef7 */
  KisSurrogateUndoAdapter::KisSurrogateUndoAdapter((KisSurrogateUndoAdapter *)(this + 0x38));
  this[0x58] = (KisProcessingCommand)0x0;
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



