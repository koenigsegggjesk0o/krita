/* Class KisReselectGlobalSelectionCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisReselectGlobalSelectionCommand @ 00200fa0 ======

void __thiscall
KisReselectGlobalSelectionCommand::KisReselectGlobalSelectionCommand
          (KisReselectGlobalSelectionCommand *this,KisWeakSharedPtr param_1,KUndo2Command *param_2)

{
  (*(code *)PTR_KisReselectGlobalSelectionCommand_008382a0)();
  return;
}



// ====== KisReselectGlobalSelectionCommand @ 003667f0 ======

/* KisReselectGlobalSelectionCommand::KisReselectGlobalSelectionCommand(KisWeakSharedPtr<KisImage>,
   KUndo2Command*) */

void __thiscall
KisReselectGlobalSelectionCommand::KisReselectGlobalSelectionCommand
          (KisReselectGlobalSelectionCommand *this,KisWeakSharedPtr param_1,KUndo2Command *param_2)

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
                    /* try { // try from 00366846 to 0036684a has its CatchHandler @ 003669ae */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_38);
                    /* try { // try from 00366859 to 0036685d has its CatchHandler @ 003669a2 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_38,(QString *)&local_40);
  if (*(int *)local_40 == 0) {
LAB_00366928:
    QArrayData::deallocate(local_40,2,8);
  }
  else if (*(int *)local_40 != -1) {
    LOCK();
    *(int *)local_40 = *(int *)local_40 + -1;
    UNLOCK();
    if (*(int *)local_40 == 0) goto LAB_00366928;
  }
                    /* try { // try from 0036688a to 0036688e has its CatchHandler @ 00366996 */
  KisCommandUtils::AggregateCommand::AggregateCommand
            ((AggregateCommand *)this,(KUndo2MagicString *)&local_38,param_2);
  if (*(int *)local_38 == 0) {
LAB_00366940:
    QArrayData::deallocate(local_38,2,8);
  }
  else if (*(int *)local_38 != -1) {
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 == 0) goto LAB_00366940;
  }
  lVar1 = *plVar3;
  *(undefined **)this = PTR_vtable_00836d20 + 0x10;
  if (lVar1 == 0) {
    *(undefined8 *)(this + 0x48) = 0;
  }
  else {
    if (((uint *)plVar3[1] == (uint *)0x0) || ((*(uint *)plVar3[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
      goto LAB_00366908;
    }
    lVar1 = *plVar3;
    *(long *)(this + 0x48) = lVar1;
    if (lVar1 != 0) {
      piVar2 = *(int **)(lVar1 + 0x58);
      if (piVar2 == (int *)0x0) {
                    /* try { // try from 00366975 to 00366979 has its CatchHandler @ 003669ba */
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
      goto LAB_00366908;
    }
  }
  *(undefined8 *)(this + 0x50) = 0;
LAB_00366908:
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



