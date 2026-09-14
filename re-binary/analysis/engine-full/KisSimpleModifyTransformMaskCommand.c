/* Class KisSimpleModifyTransformMaskCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSimpleModifyTransformMaskCommand @ 00201040 ======

void __thiscall
KisSimpleModifyTransformMaskCommand::KisSimpleModifyTransformMaskCommand
          (KisSimpleModifyTransformMaskCommand *this,KisSharedPtr param_1,QSharedPointer param_2,
          QWeakPointer param_3,KUndo2Command *param_4)

{
  (*(code *)PTR_KisSimpleModifyTransformMaskCommand_008382f0)();
  return;
}



// ====== KisSimpleModifyTransformMaskCommand @ 00374f90 ======

/* KisSimpleModifyTransformMaskCommand::KisSimpleModifyTransformMaskCommand(KisSharedPtr<KisTransformMask>,
   QSharedPointer<KisTransformMaskParamsInterface>, QWeakPointer<boost::none_t>, KUndo2Command*) */

void __thiscall
KisSimpleModifyTransformMaskCommand::KisSimpleModifyTransformMaskCommand
          (KisSimpleModifyTransformMaskCommand *this,KisSharedPtr param_1,QSharedPointer param_2,
          QWeakPointer param_3,KUndo2Command *param_4)

{
  long lVar1;
  long lVar2;
  int *piVar3;
  undefined8 uVar4;
  undefined *puVar5;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  
  lVar1 = *(long *)(in_FS_OFFSET + 0x28);
  KUndo2Command::KUndo2Command((KUndo2Command *)this,param_4);
  puVar5 = PTR_vtable_00837b50;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  this[0x28] = (KisSimpleModifyTransformMaskCommand)0x0;
  *(long *)(this + 0x30) = lVar2;
  *(undefined **)this = puVar5 + 0x10;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 00374fee to 00374ff2 has its CatchHandler @ 0037505a */
  KisTransformMask::transformParams();
  uVar4 = ((undefined8 *)CONCAT44(in_register_00000014,param_2))[1];
  *(undefined8 *)(this + 0x48) = *(undefined8 *)CONCAT44(in_register_00000014,param_2);
  *(undefined8 *)(this + 0x50) = uVar4;
  piVar3 = *(int **)(this + 0x50);
  if (piVar3 != (int *)0x0) {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x50) + 4) = *(int *)(*(long *)(this + 0x50) + 4) + 1;
    UNLOCK();
  }
  piVar3 = *(int **)CONCAT44(in_register_0000000c,param_3);
  uVar4 = ((undefined8 *)CONCAT44(in_register_0000000c,param_3))[1];
  *(int **)(this + 0x58) = piVar3;
  *(undefined8 *)(this + 0x60) = uVar4;
  if (piVar3 != (int *)0x0) {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
  }
  *(undefined8 *)(this + 0x78) = 0;
  *(undefined (*) [16])(this + 0x68) = (undefined  [16])0x0;
  if (lVar1 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



