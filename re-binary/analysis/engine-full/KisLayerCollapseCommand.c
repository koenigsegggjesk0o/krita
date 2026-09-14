/* Class KisLayerCollapseCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLayerCollapseCommand @ 00207c80 ======

void __thiscall
KisLayerCollapseCommand::KisLayerCollapseCommand
          (KisLayerCollapseCommand *this,KisSharedPtr param_1,bool param_2,bool param_3,
          KUndo2Command *param_4)

{
  (*(code *)PTR_KisLayerCollapseCommand_0083b910)();
  return;
}



// ====== KisLayerCollapseCommand @ 00378030 ======

/* KisLayerCollapseCommand::KisLayerCollapseCommand(KisSharedPtr<KisNode>, bool, bool,
   KUndo2Command*) */

void __thiscall
KisLayerCollapseCommand::KisLayerCollapseCommand
          (KisLayerCollapseCommand *this,KisSharedPtr param_1,bool param_2,bool param_3,
          KUndo2Command *param_4)

{
  long lVar1;
  bool bVar2;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  QArrayData *local_68;
  QArrayData *local_60;
  QArrayData *local_58;
  KLocalizedString local_50 [8];
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (param_3) {
                    /* try { // try from 0037807a to 0037807e has its CatchHandler @ 00378311 */
    QObject::objectName();
                    /* try { // try from 0037809c to 003780a0 has its CatchHandler @ 00378371 */
    ki18ndc((char *)local_50,"krita","(qtundo-format)");
                    /* try { // try from 003780b7 to 003780bb has its CatchHandler @ 00378335 */
    KLocalizedString::subs((QString *)&local_48,(int)local_50,(QChar)&local_68);
                    /* try { // try from 003780c7 to 003780cb has its CatchHandler @ 00378329 */
    KLocalizedString::toString();
    KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
    KLocalizedString::~KLocalizedString(local_50);
                    /* try { // try from 003780e2 to 003780e6 has its CatchHandler @ 0037831d */
    KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_58);
    if (*(int *)local_58 == 0) {
LAB_00378102:
      QArrayData::deallocate(local_58,2,8);
    }
    else if (*(int *)local_58 != -1) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + -1;
      UNLOCK();
      if (*(int *)local_58 == 0) goto LAB_00378102;
    }
    bVar2 = false;
  }
  else {
                    /* try { // try from 003781bd to 003781c1 has its CatchHandler @ 00378389 */
    QObject::objectName();
                    /* try { // try from 003781df to 003781e3 has its CatchHandler @ 00378341 */
    ki18ndc((char *)local_50,"krita","(qtundo-format)");
                    /* try { // try from 003781fc to 00378200 has its CatchHandler @ 0037834d */
    KLocalizedString::subs((QString *)&local_48,(int)local_50,(QChar)&local_60);
                    /* try { // try from 0037820c to 00378210 has its CatchHandler @ 00378359 */
    KLocalizedString::toString();
    KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
    KLocalizedString::~KLocalizedString(local_50);
                    /* try { // try from 00378227 to 0037822b has its CatchHandler @ 00378365 */
    KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_58);
    if (*(int *)local_58 == 0) {
LAB_00378258:
      bVar2 = true;
      QArrayData::deallocate(local_58,2,8);
    }
    else {
      if (*(int *)local_58 != -1) {
        LOCK();
        *(int *)local_58 = *(int *)local_58 + -1;
        UNLOCK();
        if (*(int *)local_58 == 0) goto LAB_00378258;
      }
      bVar2 = true;
    }
  }
                    /* try { // try from 00378122 to 00378126 has its CatchHandler @ 0037837d */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_48,param_4);
  if (*(int *)local_48 == 0) {
LAB_00378278:
    QArrayData::deallocate(local_48,2,8);
  }
  else if (*(int *)local_48 != -1) {
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 == 0) goto LAB_00378278;
  }
  if (bVar2) {
    if (*(int *)local_60 == 0) {
LAB_003782b8:
      QArrayData::deallocate(local_60,2,8);
    }
    else if (*(int *)local_60 != -1) {
      LOCK();
      *(int *)local_60 = *(int *)local_60 + -1;
      UNLOCK();
      if (*(int *)local_60 == 0) goto LAB_003782b8;
    }
  }
  if (!param_3) goto LAB_0037815c;
  if (*(int *)local_68 != 0) {
    if (*(int *)local_68 == -1) goto LAB_0037815c;
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 != 0) goto LAB_0037815c;
  }
  QArrayData::deallocate(local_68,2,8);
LAB_0037815c:
  *(undefined **)this = PTR_vtable_00837378 + 0x10;
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x28) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  this[0x31] = (KisLayerCollapseCommand)param_3;
  this[0x30] = (KisLayerCollapseCommand)param_2;
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisLayerCollapseCommand @ 00378390 ======

/* KisLayerCollapseCommand::KisLayerCollapseCommand(KisSharedPtr<KisNode>, bool, KUndo2Command*) */

void __thiscall
KisLayerCollapseCommand::KisLayerCollapseCommand
          (KisLayerCollapseCommand *this,KisSharedPtr param_1,bool param_2,KUndo2Command *param_3)

{
  long *plVar1;
  bool bVar2;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  bVar2 = (bool)KisBaseNode::collapsed(*(KisBaseNode **)CONCAT44(in_register_00000034,param_1));
  local_38 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_38 != (long *)0x0) {
    LOCK();
    *(int *)(local_38 + 2) = *(int *)(local_38 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 003783e5 to 003783e9 has its CatchHandler @ 00378425 */
  KisLayerCollapseCommand(this,(KisSharedPtr)&local_38,bVar2,param_2,param_3);
  if (local_38 != (long *)0x0) {
    LOCK();
    plVar1 = local_38 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_38 + 0x20))();
    }
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



