/* Class KisSwitchCurrentTimeToKeyframeCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSwitchCurrentTimeToKeyframeCommand @ 0036fc30 ======

/* KisSwitchCurrentTimeToKeyframeCommand::KisSwitchCurrentTimeToKeyframeCommand(KisImageAnimationInterface*,
   int, KisSharedPtr<KisNode>, KoID, QSharedPointer<KisKeyframe>, KUndo2Command*) */

void __thiscall
KisSwitchCurrentTimeToKeyframeCommand::KisSwitchCurrentTimeToKeyframeCommand
          (void *this,undefined8 param_2,undefined4 param_3,long *param_4,KoID *param_5,
          undefined8 *param_6,KUndo2Command *param_11)

{
  long lVar1;
  int *piVar2;
  undefined8 uVar3;
  undefined *puVar4;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  ki18ndc((char *)&local_48,"krita","(qtundo-format)");
                    /* try { // try from 0036fca2 to 0036fca6 has its CatchHandler @ 0036fde5 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
                    /* try { // try from 0036fcb5 to 0036fcb9 has its CatchHandler @ 0036fdd9 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_50);
  if (*(int *)local_50 == 0) {
LAB_0036fd90:
    QArrayData::deallocate(local_50,2,8);
  }
  else if (*(int *)local_50 != -1) {
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 == 0) goto LAB_0036fd90;
  }
                    /* try { // try from 0036fce8 to 0036fcec has its CatchHandler @ 0036fdc1 */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_48,param_11);
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_0036fd10;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0036fd10;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0036fd10:
  puVar4 = PTR_vtable_00837d00;
  *(undefined8 *)((long)this + 0x28) = param_2;
  *(undefined4 *)((long)this + 0x30) = param_3;
  *(undefined **)this = puVar4 + 0x10;
  lVar1 = *param_4;
  *(long *)((long)this + 0x38) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 0036fd45 to 0036fd49 has its CatchHandler @ 0036fdcd */
  KoID::KoID((KoID *)((long)this + 0x40),param_5);
  uVar3 = param_6[1];
  *(undefined8 *)((long)this + 0x50) = *param_6;
  *(undefined8 *)((long)this + 0x58) = uVar3;
  piVar2 = *(int **)((long)this + 0x58);
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    piVar2 = (int *)(*(long *)((long)this + 0x58) + 4);
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



