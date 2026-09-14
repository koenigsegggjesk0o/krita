/* Class KisSelectionMoveCommand2 - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSelectionMoveCommand2 @ 0036ee30 ======

/* KisSelectionMoveCommand2::KisSelectionMoveCommand2(KisSharedPtr<KisSelection>, QPoint const&,
   QPoint const&, KUndo2Command*) */

void __thiscall
KisSelectionMoveCommand2::KisSelectionMoveCommand2
          (KisSelectionMoveCommand2 *this,KisSharedPtr param_1,QPoint *param_2,QPoint *param_3,
          KUndo2Command *param_4)

{
  long *plVar1;
  long *plVar2;
  undefined8 uVar3;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  plVar2 = *(long **)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (plVar2 != (long *)0x0) {
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 0036ee8a to 0036ee8e has its CatchHandler @ 0036efcd */
  ki18ndc((char *)&local_48,"krita","(qtundo-format)");
                    /* try { // try from 0036ee9a to 0036ee9e has its CatchHandler @ 0036efb5 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
                    /* try { // try from 0036eead to 0036eeb1 has its CatchHandler @ 0036efa9 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_50);
  if (*(int *)local_50 == 0) {
LAB_0036ef68:
    QArrayData::deallocate(local_50,2,8);
  }
  else if (*(int *)local_50 != -1) {
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 == 0) goto LAB_0036ef68;
  }
                    /* try { // try from 0036eee0 to 0036eee4 has its CatchHandler @ 0036efc1 */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_48,param_4);
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_0036ef04;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0036ef04;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0036ef04:
  *(undefined ***)this = &PTR_FUN_00829d68;
  *(undefined8 *)(this + 0x28) = *(undefined8 *)param_2;
  uVar3 = *(undefined8 *)param_3;
  *(long **)(this + 0x38) = plVar2;
  *(undefined8 *)(this + 0x30) = uVar3;
  if (plVar2 != (long *)0x0) {
    plVar1 = plVar2 + 1;
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    LOCK();
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*plVar2 + 8))(plVar2);
    }
  }
  *(undefined **)this = PTR_vtable_00837480 + 0x10;
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



