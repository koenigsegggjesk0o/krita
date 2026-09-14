/* Class KisImageAnimSettingCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageAnimSettingCommand @ 00371a70 ======

/* KisImageAnimSettingCommand::KisImageAnimSettingCommand(KisImageAnimationInterface*,
   KisImageAnimSettingCommand::Settings, KUndo2Command*) */

void __thiscall
KisImageAnimSettingCommand::KisImageAnimSettingCommand
          (KisImageAnimSettingCommand *this,KisImageAnimationInterface *param_1,Settings param_2,
          KUndo2Command *param_3)

{
  undefined4 uVar1;
  undefined *puVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  long lVar5;
  undefined4 in_register_00000014;
  KUndo2Command *in_R8;
  long in_FS_OFFSET;
  QArrayData *local_40;
  QArrayData *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  ki18ndc((char *)&local_38,"krita","(qtundo-format)");
                    /* try { // try from 00371ace to 00371ad2 has its CatchHandler @ 00371c25 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_38);
                    /* try { // try from 00371ae1 to 00371ae5 has its CatchHandler @ 00371c19 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_38,(QString *)&local_40);
  if (*(int *)local_40 == 0) {
LAB_00371bd0:
    QArrayData::deallocate(local_40,2,8);
  }
  else if (*(int *)local_40 != -1) {
    LOCK();
    *(int *)local_40 = *(int *)local_40 + -1;
    UNLOCK();
    if (*(int *)local_40 == 0) goto LAB_00371bd0;
  }
                    /* try { // try from 00371b12 to 00371b16 has its CatchHandler @ 00371c01 */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_38,in_R8);
  if (*(int *)local_38 != 0) {
    if (*(int *)local_38 == -1) goto LAB_00371b3a;
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 != 0) goto LAB_00371b3a;
  }
  QArrayData::deallocate(local_38,2,8);
LAB_00371b3a:
  puVar2 = PTR_vtable_00836e48;
  *(KisImageAnimationInterface **)(this + 0x30) = param_1;
  *(undefined8 *)(this + 0x38) = 0;
  *(undefined4 *)(this + 0x40) = 0x18;
  *(undefined **)(this + 0x28) = puVar2 + 0xb8;
  *(undefined **)this = puVar2 + 0x10;
  *(ulong *)(this + 0x44) = CONCAT44(in_register_00000014,param_2);
  *(int *)(this + 0x4c) = (int)param_3;
                    /* try { // try from 00371b77 to 00371b91 has its CatchHandler @ 00371c0d */
  uVar3 = KisImageAnimationInterface::framerate(param_1);
  puVar4 = (undefined4 *)KisImageAnimationInterface::documentPlaybackRange(param_1);
  uVar1 = *puVar4;
  lVar5 = KisImageAnimationInterface::documentPlaybackRange(param_1);
  *(undefined4 *)(this + 0x40) = *(undefined4 *)(lVar5 + 4);
  *(ulong *)(this + 0x38) = CONCAT44(uVar1,uVar3);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



