/* Class KisDeselectGlobalSelectionCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisDeselectGlobalSelectionCommand @ 00206a60 ======

void __thiscall
KisDeselectGlobalSelectionCommand::KisDeselectGlobalSelectionCommand
          (KisDeselectGlobalSelectionCommand *this,KisWeakSharedPtr param_1,KUndo2Command *param_2)

{
  (*(code *)PTR_KisDeselectGlobalSelectionCommand_0083b000)();
  return;
}



// ====== KisDeselectGlobalSelectionCommand @ 0035b120 ======

/* KisDeselectGlobalSelectionCommand::KisDeselectGlobalSelectionCommand(KisWeakSharedPtr<KisImage>,
   KUndo2Command*) */

void __thiscall
KisDeselectGlobalSelectionCommand::KisDeselectGlobalSelectionCommand
          (KisDeselectGlobalSelectionCommand *this,KisWeakSharedPtr param_1,KUndo2Command *param_2)

{
  long lVar1;
  int *piVar2;
  undefined4 in_register_00000034;
  long *plVar3;
  long in_FS_OFFSET;
  QArrayData *local_40;
  QArrayData *local_38;
  long local_30;
  
  plVar3 = (long *)CONCAT44(in_register_00000034,param_1);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  ki18ndc((char *)&local_38,"krita","(qtundo-format)");
                    /* try { // try from 0035b176 to 0035b17a has its CatchHandler @ 0035b2de */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_38);
                    /* try { // try from 0035b189 to 0035b18d has its CatchHandler @ 0035b2d2 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_38,(QString *)&local_40);
  if (*(int *)local_40 == 0) {
LAB_0035b258:
    QArrayData::deallocate(local_40,2,8);
  }
  else if (*(int *)local_40 != -1) {
    LOCK();
    *(int *)local_40 = *(int *)local_40 + -1;
    UNLOCK();
    if (*(int *)local_40 == 0) goto LAB_0035b258;
  }
                    /* try { // try from 0035b1ba to 0035b1be has its CatchHandler @ 0035b2c6 */
  KisCommandUtils::AggregateCommand::AggregateCommand
            ((AggregateCommand *)this,(KUndo2MagicString *)&local_38,param_2);
  if (*(int *)local_38 == 0) {
LAB_0035b270:
    QArrayData::deallocate(local_38,2,8);
  }
  else if (*(int *)local_38 != -1) {
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 == 0) goto LAB_0035b270;
  }
  lVar1 = *plVar3;
  *(undefined **)this = PTR_vtable_00836e38 + 0x10;
  if (lVar1 == 0) {
    *(undefined8 *)(this + 0x48) = 0;
  }
  else {
    if (((uint *)plVar3[1] == (uint *)0x0) || ((*(uint *)plVar3[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
      goto LAB_0035b238;
    }
    lVar1 = *plVar3;
    *(long *)(this + 0x48) = lVar1;
    if (lVar1 != 0) {
      piVar2 = *(int **)(lVar1 + 0x58);
      if (piVar2 == (int *)0x0) {
                    /* try { // try from 0035b2a5 to 0035b2a9 has its CatchHandler @ 0035b2ea */
        piVar2 = (int *)operator_new(4);
        *piVar2 = 0;
        *(int **)(lVar1 + 0x58) = piVar2;
        LOCK();
        *piVar2 = *piVar2 + 1;
        UNLOCK();
        piVar2 = *(int **)(lVar1 + 0x58);
      }
      *(int **)(this + 0x50) = piVar2;
      LOCK();
      *piVar2 = *piVar2 + 2;
      UNLOCK();
      goto LAB_0035b238;
    }
  }
  *(undefined8 *)(this + 0x50) = 0;
LAB_0035b238:
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



