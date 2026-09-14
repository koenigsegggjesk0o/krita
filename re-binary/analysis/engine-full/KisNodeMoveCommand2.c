/* Class KisNodeMoveCommand2 - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeMoveCommand2 @ 0020b030 ======

void __thiscall
KisNodeMoveCommand2::KisNodeMoveCommand2
          (KisNodeMoveCommand2 *this,KisSharedPtr param_1,QPoint *param_2,QPoint *param_3,
          KUndo2Command *param_4)

{
  (*(code *)PTR_KisNodeMoveCommand2_0083d2e8)();
  return;
}



// ====== KisNodeMoveCommand2 @ 0036e160 ======

/* KisNodeMoveCommand2::KisNodeMoveCommand2(KisSharedPtr<KisNode>, QPoint const&, QPoint const&,
   KUndo2Command*) */

void __thiscall
KisNodeMoveCommand2::KisNodeMoveCommand2
          (KisNodeMoveCommand2 *this,KisSharedPtr param_1,QPoint *param_2,QPoint *param_3,
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
    *(int *)(plVar2 + 2) = *(int *)(plVar2 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 0036e1ba to 0036e1be has its CatchHandler @ 0036e2fd */
  ki18ndc((char *)&local_48,"krita","(qtundo-format)");
                    /* try { // try from 0036e1ca to 0036e1ce has its CatchHandler @ 0036e2e5 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
                    /* try { // try from 0036e1dd to 0036e1e1 has its CatchHandler @ 0036e2d9 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_50);
  if (*(int *)local_50 == 0) {
LAB_0036e298:
    QArrayData::deallocate(local_50,2,8);
  }
  else if (*(int *)local_50 != -1) {
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 == 0) goto LAB_0036e298;
  }
                    /* try { // try from 0036e210 to 0036e214 has its CatchHandler @ 0036e2f1 */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_48,param_4);
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_0036e234;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0036e234;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0036e234:
  *(undefined ***)this = &PTR_FUN_00829b40;
  *(undefined8 *)(this + 0x28) = *(undefined8 *)param_2;
  uVar3 = *(undefined8 *)param_3;
  *(long **)(this + 0x38) = plVar2;
  *(undefined8 *)(this + 0x30) = uVar3;
  if (plVar2 != (long *)0x0) {
    plVar1 = plVar2 + 2;
    LOCK();
    *(int *)(plVar2 + 2) = *(int *)(plVar2 + 2) + 1;
    UNLOCK();
    LOCK();
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*plVar2 + 0x20))(plVar2);
    }
  }
  *(undefined **)this = PTR_vtable_00837698 + 0x10;
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



