/* Class KisUpdateCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisUpdateCommand @ 0036f210 ======

/* KisUpdateCommand::KisUpdateCommand(KisSharedPtr<KisNode>, QRect, KisUpdatesFacade*, bool) */

void __thiscall
KisUpdateCommand::KisUpdateCommand
          (KisUpdateCommand *this,long *param_2,undefined8 param_3,undefined8 param_4,
          undefined8 param_5,KisUpdateCommand param_6)

{
  int iVar1;
  long lVar2;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_48 = (QArrayData *)QString::fromAscii_helper("UPDATE_COMMAND",0xe);
                    /* try { // try from 0036f271 to 0036f275 has its CatchHandler @ 0036f363 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_50,(QString *)&local_48);
                    /* try { // try from 0036f27e to 0036f282 has its CatchHandler @ 0036f357 */
  KUndo2Command::KUndo2Command
            ((KUndo2Command *)this,(KUndo2MagicString *)&local_50,(KUndo2Command *)0x0);
  if (*(int *)local_50 == 0) {
LAB_0036f320:
    QArrayData::deallocate(local_50,2,8);
    iVar1 = *(int *)local_48;
  }
  else {
    if (*(int *)local_50 != -1) {
      LOCK();
      *(int *)local_50 = *(int *)local_50 + -1;
      UNLOCK();
      if (*(int *)local_50 == 0) goto LAB_0036f320;
    }
    iVar1 = *(int *)local_48;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_0036f2c1;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0036f2c1;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0036f2c1:
  *(undefined **)this = PTR_vtable_00837f48 + 0x10;
  lVar2 = *param_2;
  *(long *)(this + 0x28) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
  *(undefined8 *)(this + 0x30) = param_3;
  *(undefined8 *)(this + 0x38) = param_4;
  *(undefined8 *)(this + 0x50) = param_5;
  this[0x58] = param_6;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisUpdateCommand @ 0036f370 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisUpdateCommand::KisUpdateCommand(KisSharedPtr<KisNode>, QSharedPointer<QRect>,
   KisUpdatesFacade*, bool) */

void __thiscall
KisUpdateCommand::KisUpdateCommand
          (KisUpdateCommand *this,KisSharedPtr param_1,QSharedPointer param_2,
          KisUpdatesFacade *param_3,bool param_4)

{
  int iVar1;
  long lVar2;
  int *piVar3;
  undefined8 uVar4;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_48 = (QArrayData *)QString::fromAscii_helper("UPDATE_COMMAND",0xe);
                    /* try { // try from 0036f3cc to 0036f3d0 has its CatchHandler @ 0036f4e3 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_50,(QString *)&local_48);
                    /* try { // try from 0036f3d9 to 0036f3dd has its CatchHandler @ 0036f4d7 */
  KUndo2Command::KUndo2Command
            ((KUndo2Command *)this,(KUndo2MagicString *)&local_50,(KUndo2Command *)0x0);
  if (*(int *)local_50 == 0) {
LAB_0036f4a0:
    QArrayData::deallocate(local_50,2,8);
    iVar1 = *(int *)local_48;
  }
  else {
    if (*(int *)local_50 != -1) {
      LOCK();
      *(int *)local_50 = *(int *)local_50 + -1;
      UNLOCK();
      if (*(int *)local_50 == 0) goto LAB_0036f4a0;
    }
    iVar1 = *(int *)local_48;
  }
  if (iVar1 != 0) {
    if (iVar1 == -1) goto LAB_0036f424;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0036f424;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0036f424:
  *(undefined **)this = PTR_vtable_00837f48 + 0x10;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x28) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
  uVar4 = DAT_00721778;
  *(undefined8 *)(this + 0x30) = _DAT_00721770;
  *(undefined8 *)(this + 0x38) = uVar4;
  uVar4 = ((undefined8 *)CONCAT44(in_register_00000014,param_2))[1];
  *(undefined8 *)(this + 0x40) = *(undefined8 *)CONCAT44(in_register_00000014,param_2);
  *(undefined8 *)(this + 0x48) = uVar4;
  piVar3 = *(int **)(this + 0x48);
  if (piVar3 != (int *)0x0) {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x48) + 4) = *(int *)(*(long *)(this + 0x48) + 4) + 1;
    UNLOCK();
  }
  *(KisUpdatesFacade **)(this + 0x50) = param_3;
  this[0x58] = (KisUpdateCommand)param_4;
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



