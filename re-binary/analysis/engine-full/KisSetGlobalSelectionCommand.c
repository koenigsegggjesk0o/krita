/* Class KisSetGlobalSelectionCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSetGlobalSelectionCommand @ 0020c560 ======

void __thiscall
KisSetGlobalSelectionCommand::KisSetGlobalSelectionCommand
          (KisSetGlobalSelectionCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisSetGlobalSelectionCommand_0083dd80)();
  return;
}



// ====== KisSetGlobalSelectionCommand @ 00367dc0 ======

/* KisSetGlobalSelectionCommand::KisSetGlobalSelectionCommand(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisSelection>) */

void __thiscall
KisSetGlobalSelectionCommand::KisSetGlobalSelectionCommand
          (KisSetGlobalSelectionCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2)

{
  long *plVar1;
  long lVar2;
  long lVar3;
  int *piVar4;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long *plVar5;
  long *plVar6;
  long *plVar7;
  long in_FS_OFFSET;
  long *local_38;
  
  plVar5 = (long *)CONCAT44(in_register_00000034,param_1);
  lVar2 = *(long *)(in_FS_OFFSET + 0x28);
  KisCommandUtils::AggregateCommand::AggregateCommand((AggregateCommand *)this,(KUndo2Command *)0x0)
  ;
  lVar3 = *plVar5;
  *(undefined **)this = PTR_vtable_00837140 + 0x10;
  if (lVar3 == 0) {
    *(undefined8 *)(this + 0x48) = 0;
LAB_00367fe8:
    *(undefined8 *)(this + 0x50) = 0;
    *(undefined (*) [16])(this + 0x58) = (undefined  [16])0x0;
LAB_00367f8c:
    if (((*plVar5 != 0) && ((uint *)plVar5[1] != (uint *)0x0)) &&
       (((*(uint *)plVar5[1] & 1) != 0 && (*plVar5 != 0)))) {
      plVar7 = (long *)0x0;
LAB_00367eab:
                    /* try { // try from 00367eb1 to 00367eb5 has its CatchHandler @ 00368028 */
      KisImage::globalSelection();
      plVar5 = *(long **)(this + 0x60);
      plVar6 = plVar5;
      if (local_38 != plVar5) {
        if (local_38 != (long *)0x0) {
          LOCK();
          *(int *)(local_38 + 1) = *(int *)(local_38 + 1) + 1;
          UNLOCK();
          plVar5 = *(long **)(this + 0x60);
        }
        *(long **)(this + 0x60) = local_38;
        plVar6 = local_38;
        if (plVar5 != (long *)0x0) {
          LOCK();
          plVar1 = plVar5 + 1;
          *(int *)plVar1 = *(int *)plVar1 + -1;
          UNLOCK();
          if (*(int *)plVar1 == 0) {
            (**(code **)(*plVar5 + 8))();
          }
        }
      }
      if (plVar6 != (long *)0x0) {
        LOCK();
        plVar5 = plVar6 + 1;
        *(int *)plVar5 = *(int *)plVar5 + -1;
        UNLOCK();
        if (*(int *)plVar5 == 0) {
          (**(code **)(*plVar6 + 8))();
        }
      }
      plVar5 = *(long **)CONCAT44(in_register_00000014,param_2);
      plVar6 = *(long **)(this + 0x58);
      if (plVar5 != plVar6) {
        if (plVar5 != (long *)0x0) {
          LOCK();
          *(int *)(plVar5 + 1) = *(int *)(plVar5 + 1) + 1;
          UNLOCK();
          plVar6 = *(long **)(this + 0x58);
        }
        *(long **)(this + 0x58) = plVar5;
        if (plVar6 != (long *)0x0) {
          LOCK();
          plVar5 = plVar6 + 1;
          *(int *)plVar5 = *(int *)plVar5 + -1;
          UNLOCK();
          if (*(int *)plVar5 == 0) {
            (**(code **)(*plVar6 + 8))();
          }
        }
      }
      LOCK();
      plVar5 = plVar7 + 10;
      *(int *)plVar5 = *(int *)plVar5 + -1;
      UNLOCK();
      if (*(int *)plVar5 == 0) goto LAB_00367f51;
    }
LAB_00367fbe:
    if (lVar2 == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
  else {
    if (((uint *)plVar5[1] == (uint *)0x0) || ((*(uint *)plVar5[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
      *(undefined (*) [16])(this + 0x58) = (undefined  [16])0x0;
      goto LAB_00367f8c;
    }
    lVar3 = *plVar5;
    *(long *)(this + 0x48) = lVar3;
    if (lVar3 == 0) goto LAB_00367fe8;
    piVar4 = *(int **)(lVar3 + 0x58);
    if (piVar4 == (int *)0x0) {
                    /* try { // try from 00368005 to 00368009 has its CatchHandler @ 00368034 */
      piVar4 = (int *)operator_new(4);
      *piVar4 = 0;
      *(int **)(lVar3 + 0x58) = piVar4;
      LOCK();
      *piVar4 = *piVar4 + 1;
      UNLOCK();
      piVar4 = *(int **)(lVar3 + 0x58);
    }
    *(int **)(this + 0x50) = piVar4;
    LOCK();
    *piVar4 = *piVar4 + 2;
    UNLOCK();
    plVar7 = *(long **)(this + 0x48);
    *(undefined (*) [16])(this + 0x58) = (undefined  [16])0x0;
    if (((*(uint **)(this + 0x50) == (uint *)0x0) || (plVar7 == (long *)0x0)) ||
       ((**(uint **)(this + 0x50) & 1) == 0)) goto LAB_00367f8c;
    plVar6 = plVar7 + 10;
    LOCK();
    *(int *)(plVar7 + 10) = *(int *)(plVar7 + 10) + 1;
    UNLOCK();
    if (((*plVar5 != 0) && ((uint *)plVar5[1] != (uint *)0x0)) &&
       (((*(uint *)plVar5[1] & 1) != 0 && (*plVar5 != 0)))) goto LAB_00367eab;
    LOCK();
    *(int *)plVar6 = *(int *)plVar6 + -1;
    UNLOCK();
    if (*(int *)plVar6 != 0) goto LAB_00367fbe;
LAB_00367f51:
    if (lVar2 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x00367f7a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(*plVar7 + 0x20))(plVar7);
      return;
    }
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



