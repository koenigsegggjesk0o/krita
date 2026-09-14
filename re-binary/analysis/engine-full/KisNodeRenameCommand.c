/* Class KisNodeRenameCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeRenameCommand @ 00368990 ======

/* KisNodeRenameCommand::KisNodeRenameCommand(KisSharedPtr<KisNode>, QString const&, QString const&)
    */

void __thiscall
KisNodeRenameCommand::KisNodeRenameCommand
          (KisNodeRenameCommand *this,KisSharedPtr param_1,QString *param_2,QString *param_3)

{
  long *plVar1;
  undefined *puVar2;
  undefined *puVar3;
  undefined *puVar4;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_58;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_58 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_58 != (long *)0x0) {
    LOCK();
    *(int *)(local_58 + 2) = *(int *)(local_58 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 003689e8 to 003689ec has its CatchHandler @ 00368b54 */
  ki18ndc((char *)&local_48,"krita","(qtundo-format)");
                    /* try { // try from 003689f8 to 003689fc has its CatchHandler @ 00368b3c */
  KLocalizedString::toString();
  puVar4 = PTR_vtable_00837e78;
  puVar3 = PTR_shared_null_008377d0;
  puVar2 = PTR_vtable_00837e78 + 0xb8;
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
                    /* try { // try from 00368a29 to 00368a2d has its CatchHandler @ 00368b30 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_50);
  if (*(int *)local_50 == 0) {
LAB_00368af0:
    QArrayData::deallocate(local_50,2,8);
  }
  else if (*(int *)local_50 != -1) {
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 == 0) goto LAB_00368af0;
  }
                    /* try { // try from 00368a5c to 00368a60 has its CatchHandler @ 00368b48 */
  KisNodeCommand::KisNodeCommand
            ((KisNodeCommand *)this,(KUndo2MagicString *)&local_48,(KisSharedPtr)&local_58);
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_00368a84;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_00368a84;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_00368a84:
  if (local_58 != (long *)0x0) {
    LOCK();
    plVar1 = local_58 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_58 + 0x20))();
    }
  }
  *(undefined **)this = puVar4 + 0x10;
  puVar4 = PTR_shared_null_008377d0;
  *(undefined **)(this + 0x30) = puVar2;
  *(undefined **)(this + 0x38) = puVar3;
  *(undefined **)(this + 0x40) = puVar4;
  QString::operator=((QString *)(this + 0x38),(QString *)param_2);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    QString::operator=((QString *)(this + 0x40),(QString *)param_3);
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



