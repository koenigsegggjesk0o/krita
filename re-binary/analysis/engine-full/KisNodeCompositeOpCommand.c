/* Class KisNodeCompositeOpCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeCompositeOpCommand @ 00206700 ======

void __thiscall
KisNodeCompositeOpCommand::KisNodeCompositeOpCommand
          (KisNodeCompositeOpCommand *this,KisSharedPtr param_1,QString *param_2)

{
  (*(code *)PTR_KisNodeCompositeOpCommand_0083ae50)();
  return;
}



// ====== KisNodeCompositeOpCommand @ 00361ee0 ======

/* KisNodeCompositeOpCommand::KisNodeCompositeOpCommand(KisSharedPtr<KisNode>, QString const&) */

void __thiscall
KisNodeCompositeOpCommand::KisNodeCompositeOpCommand
          (KisNodeCompositeOpCommand *this,KisSharedPtr param_1,QString *param_2)

{
  long *plVar1;
  undefined *puVar2;
  undefined *puVar3;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  long *local_48;
  QArrayData *local_40;
  QArrayData *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  local_48 = *(long **)CONCAT44(in_register_00000034,param_1);
  if (local_48 != (long *)0x0) {
    LOCK();
    *(int *)(local_48 + 2) = *(int *)(local_48 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 00361f30 to 00361f34 has its CatchHandler @ 00362074 */
  ki18ndc((char *)&local_38,"krita","(qtundo-format)");
                    /* try { // try from 00361f40 to 00361f44 has its CatchHandler @ 0036205c */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_38);
                    /* try { // try from 00361f53 to 00361f57 has its CatchHandler @ 00362050 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_38,(QString *)&local_40);
  if (*(int *)local_40 == 0) {
LAB_00362010:
    QArrayData::deallocate(local_40,2,8);
  }
  else if (*(int *)local_40 != -1) {
    LOCK();
    *(int *)local_40 = *(int *)local_40 + -1;
    UNLOCK();
    if (*(int *)local_40 == 0) goto LAB_00362010;
  }
                    /* try { // try from 00361f84 to 00361f88 has its CatchHandler @ 00362068 */
  KisNodeCommand::KisNodeCommand
            ((KisNodeCommand *)this,(KUndo2MagicString *)&local_38,(KisSharedPtr)&local_48);
  if (*(int *)local_38 != 0) {
    if (*(int *)local_38 == -1) goto LAB_00361fac;
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 != 0) goto LAB_00361fac;
  }
  QArrayData::deallocate(local_38,2,8);
LAB_00361fac:
  if (local_48 != (long *)0x0) {
    LOCK();
    plVar1 = local_48 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_48 + 0x20))();
    }
  }
  puVar2 = PTR_vtable_00837520;
  this[0x38] = (KisNodeCompositeOpCommand)0x0;
  *(undefined **)(this + 0x30) = puVar2 + 0xb8;
  puVar3 = PTR_shared_null_008377d0;
  *(undefined **)this = puVar2 + 0x10;
  *(undefined **)(this + 0x48) = puVar3;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    QString::operator=((QString *)(this + 0x48),(QString *)param_2);
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



