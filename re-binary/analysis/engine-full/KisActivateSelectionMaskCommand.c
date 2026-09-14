/* Class KisActivateSelectionMaskCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisActivateSelectionMaskCommand @ 00205e80 ======

void __thiscall
KisActivateSelectionMaskCommand::KisActivateSelectionMaskCommand
          (KisActivateSelectionMaskCommand *this,KisSharedPtr param_1,bool param_2)

{
  (*(code *)PTR_KisActivateSelectionMaskCommand_0083aa10)();
  return;
}



// ====== KisActivateSelectionMaskCommand @ 003707f0 ======

/* KisActivateSelectionMaskCommand::KisActivateSelectionMaskCommand(KisSharedPtr<KisSelectionMask>,
   bool) */

void __thiscall
KisActivateSelectionMaskCommand::KisActivateSelectionMaskCommand
          (KisActivateSelectionMaskCommand *this,KisSharedPtr param_1,bool param_2)

{
  long *plVar1;
  long lVar2;
  KisActivateSelectionMaskCommand KVar3;
  long *plVar4;
  undefined4 in_register_00000034;
  long *plVar5;
  long *plVar6;
  long in_FS_OFFSET;
  long *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2Command *)0x0);
  *(undefined **)this = PTR_vtable_00837870 + 0x10;
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x28) = lVar2;
  if (lVar2 == 0) {
    *(undefined8 *)(this + 0x30) = 0;
    this[0x38] = (KisActivateSelectionMaskCommand)param_2;
  }
  else {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
    *(undefined8 *)(this + 0x30) = 0;
    this[0x38] = (KisActivateSelectionMaskCommand)param_2;
    if (*(long *)(this + 0x28) != 0) {
                    /* try { // try from 00370862 to 00370866 has its CatchHandler @ 00370997 */
      KisNode::parent();
                    /* try { // try from 00370872 to 00370876 has its CatchHandler @ 003709a3 */
      plVar4 = (long *)QMetaObject::cast((QObject *)PTR_staticMetaObject_00837328);
      if (plVar4 == (long *)0x0) {
        if (local_38 != (long *)0x0) {
          LOCK();
          plVar4 = local_38 + 2;
          *(int *)plVar4 = *(int *)plVar4 + -1;
          UNLOCK();
          if (*(int *)plVar4 == 0) {
            (**(code **)(*local_38 + 0x20))();
          }
        }
      }
      else {
        LOCK();
        *(int *)(plVar4 + 2) = *(int *)(plVar4 + 2) + 1;
        UNLOCK();
        if (local_38 != (long *)0x0) {
          LOCK();
          plVar5 = local_38 + 2;
          *(int *)plVar5 = *(int *)plVar5 + -1;
          UNLOCK();
          if (*(int *)plVar5 == 0) {
            (**(code **)(*local_38 + 0x20))();
          }
        }
                    /* try { // try from 003708a6 to 003708ab has its CatchHandler @ 0037098b */
        (**(code **)(*plVar4 + 0x200))(&local_38,plVar4);
        plVar5 = *(long **)(this + 0x30);
        plVar6 = plVar5;
        if (local_38 != plVar5) {
          if (local_38 != (long *)0x0) {
            LOCK();
            *(int *)(local_38 + 2) = *(int *)(local_38 + 2) + 1;
            UNLOCK();
            plVar5 = *(long **)(this + 0x30);
          }
          *(long **)(this + 0x30) = local_38;
          plVar6 = local_38;
          if (plVar5 != (long *)0x0) {
            LOCK();
            plVar1 = plVar5 + 2;
            *(int *)plVar1 = *(int *)plVar1 + -1;
            UNLOCK();
            if (*(int *)plVar1 == 0) {
              (**(code **)(*plVar5 + 0x20))();
              plVar6 = local_38;
            }
          }
        }
        if (plVar6 != (long *)0x0) {
          LOCK();
          plVar5 = plVar6 + 2;
          *(int *)plVar5 = *(int *)plVar5 + -1;
          UNLOCK();
          if (*(int *)plVar5 == 0) {
            (**(code **)(*plVar6 + 0x20))();
          }
        }
        LOCK();
        plVar5 = plVar4 + 2;
        *(int *)plVar5 = *(int *)plVar5 + -1;
        UNLOCK();
        if (*(int *)plVar5 == 0) {
          (**(code **)(*plVar4 + 0x20))(plVar4);
        }
      }
    }
  }
                    /* try { // try from 00370903 to 00370907 has its CatchHandler @ 00370997 */
  KVar3 = (KisActivateSelectionMaskCommand)
          KisSelectionMask::active(*(KisSelectionMask **)CONCAT44(in_register_00000034,param_1));
  this[0x39] = KVar3;
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



