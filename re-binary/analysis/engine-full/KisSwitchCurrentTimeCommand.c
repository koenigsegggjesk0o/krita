/* Class KisSwitchCurrentTimeCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSwitchCurrentTimeCommand @ 00200d00 ======

void __thiscall
KisSwitchCurrentTimeCommand::KisSwitchCurrentTimeCommand
          (KisSwitchCurrentTimeCommand *this,KisImageAnimationInterface *param_1,int param_2,
          int param_3,KUndo2Command *param_4)

{
  (*(code *)PTR_KisSwitchCurrentTimeCommand_00838150)();
  return;
}



// ====== KisSwitchCurrentTimeCommand @ 0036fad0 ======

/* KisSwitchCurrentTimeCommand::KisSwitchCurrentTimeCommand(KisImageAnimationInterface*, int, int,
   KUndo2Command*) */

void __thiscall
KisSwitchCurrentTimeCommand::KisSwitchCurrentTimeCommand
          (KisSwitchCurrentTimeCommand *this,KisImageAnimationInterface *param_1,int param_2,
          int param_3,KUndo2Command *param_4)

{
  undefined *puVar1;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  ki18ndc((char *)&local_48,"krita","(qtundo-format)");
                    /* try { // try from 0036fb30 to 0036fb34 has its CatchHandler @ 0036fbfe */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
                    /* try { // try from 0036fb43 to 0036fb47 has its CatchHandler @ 0036fc0a */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_50);
  if (*(int *)local_50 == 0) {
LAB_0036fbd0:
    QArrayData::deallocate(local_50,2,8);
  }
  else if (*(int *)local_50 != -1) {
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 == 0) goto LAB_0036fbd0;
  }
                    /* try { // try from 0036fb6e to 0036fb72 has its CatchHandler @ 0036fc16 */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_48,param_4);
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_0036fb8e;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0036fb8e;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0036fb8e:
  puVar1 = PTR_vtable_00836ec8;
  *(KisImageAnimationInterface **)(this + 0x28) = param_1;
  *(int *)(this + 0x30) = param_2;
  *(int *)(this + 0x34) = param_3;
  *(undefined **)this = puVar1 + 0x10;
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



