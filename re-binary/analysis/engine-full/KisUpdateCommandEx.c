/* Class KisUpdateCommandEx - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisUpdateCommandEx @ 0020d0e0 ======

void __thiscall
KisUpdateCommandEx::KisUpdateCommandEx
          (KisUpdateCommandEx *this,QSharedPointer param_1,KisUpdatesFacade *param_2,State param_3,
          QWeakPointer param_4)

{
  (*(code *)PTR_KisUpdateCommandEx_0083e340)();
  return;
}



// ====== KisUpdateCommandEx @ 00375b50 ======

/* KisUpdateCommandEx::KisUpdateCommandEx(QSharedPointer<KisBatchNodeUpdate>, KisUpdatesFacade*,
   KisCommandUtils::FlipFlopCommand::State, QWeakPointer<boost::none_t>) */

void __thiscall
KisUpdateCommandEx::KisUpdateCommandEx
          (KisUpdateCommandEx *this,QSharedPointer param_1,KisUpdatesFacade *param_2,State param_3,
          QWeakPointer param_4)

{
  int *piVar1;
  undefined8 uVar2;
  undefined4 in_register_00000034;
  undefined4 in_register_00000084;
  
  KisCommandUtils::FlipFlopCommand::FlipFlopCommand
            ((FlipFlopCommand *)this,param_3,(KUndo2Command *)0x0);
  *(undefined **)this = PTR_vtable_00837308 + 0x10;
  uVar2 = ((undefined8 *)CONCAT44(in_register_00000034,param_1))[1];
  *(undefined8 *)(this + 0x30) = *(undefined8 *)CONCAT44(in_register_00000034,param_1);
  *(undefined8 *)(this + 0x38) = uVar2;
  piVar1 = *(int **)(this + 0x38);
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x38) + 4) = *(int *)(*(long *)(this + 0x38) + 4) + 1;
    UNLOCK();
  }
  piVar1 = *(int **)CONCAT44(in_register_00000084,param_4);
  uVar2 = ((undefined8 *)CONCAT44(in_register_00000084,param_4))[1];
  *(int **)(this + 0x40) = piVar1;
  *(undefined8 *)(this + 0x48) = uVar2;
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  *(KisUpdatesFacade **)(this + 0x50) = param_2;
  return;
}



// ====== KisUpdateCommandEx @ 00375bd0 ======

/* KisUpdateCommandEx::KisUpdateCommandEx(QSharedPointer<KisBatchNodeUpdate>, KisUpdatesFacade*,
   KisCommandUtils::FlipFlopCommand::State) */

void __thiscall
KisUpdateCommandEx::KisUpdateCommandEx
          (KisUpdateCommandEx *this,QSharedPointer param_1,KisUpdatesFacade *param_2,State param_3)

{
  int *piVar1;
  int *piVar2;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  undefined8 local_48;
  int *piStack_40;
  undefined local_38 [16];
  long local_20;
  
  local_48 = *(undefined8 *)CONCAT44(in_register_00000034,param_1);
  piStack_40 = (int *)((undefined8 *)CONCAT44(in_register_00000034,param_1))[1];
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_38 = (undefined  [16])0x0;
  if (piStack_40 != (int *)0x0) {
    LOCK();
    *piStack_40 = *piStack_40 + 1;
    UNLOCK();
    LOCK();
    piStack_40[1] = piStack_40[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 00375c1b to 00375c1f has its CatchHandler @ 00375c94 */
  KisUpdateCommandEx(this,(QSharedPointer)&local_48,param_2,param_3,(QWeakPointer)local_38);
  piVar2 = piStack_40;
  if (piStack_40 != (int *)0x0) {
    LOCK();
    piVar1 = piStack_40 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piStack_40 + 2))(piStack_40);
    }
    LOCK();
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (*piVar2 == 0) {
      operator_delete(piVar2,0x10);
    }
  }
  if ((int *)local_38._0_8_ != (int *)0x0) {
    LOCK();
    *(int *)local_38._0_8_ = *(int *)local_38._0_8_ + -1;
    UNLOCK();
    if ((*(int *)local_38._0_8_ == 0) && ((int *)local_38._0_8_ != (int *)0x0)) {
      operator_delete((void *)local_38._0_8_,0x10);
    }
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



