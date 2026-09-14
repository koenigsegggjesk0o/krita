/* Class KisNodePropertyListCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodePropertyListCommand @ 00202dd0 ======

void __thiscall KisNodePropertyListCommand::KisNodePropertyListCommand(void)

{
  (*(code *)PTR_KisNodePropertyListCommand_008391b8)();
  return;
}



// ====== KisNodePropertyListCommand @ 003635a0 ======

/* KisNodePropertyListCommand::KisNodePropertyListCommand(KisSharedPtr<KisNode>,
   QList<KisBaseNode::Property>) */

void __thiscall
KisNodePropertyListCommand::KisNodePropertyListCommand
          (KisNodePropertyListCommand *this,long *param_2,undefined8 param_3)

{
  long *plVar1;
  undefined *puVar2;
  long in_FS_OFFSET;
  long *local_48;
  QArrayData *local_40;
  QArrayData *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  local_48 = (long *)*param_2;
  if (local_48 != (long *)0x0) {
    LOCK();
    *(int *)(local_48 + 2) = *(int *)(local_48 + 2) + 1;
    UNLOCK();
  }
                    /* try { // try from 003635f5 to 003635f9 has its CatchHandler @ 00363744 */
  ki18ndc((char *)&local_38,"krita","(qtundo-format)");
                    /* try { // try from 00363605 to 00363609 has its CatchHandler @ 0036375c */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_38);
                    /* try { // try from 00363618 to 0036361c has its CatchHandler @ 00363750 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_38,(QString *)&local_40);
  if (*(int *)local_40 == 0) {
LAB_003636e0:
    QArrayData::deallocate(local_40,2,8);
  }
  else if (*(int *)local_40 != -1) {
    LOCK();
    *(int *)local_40 = *(int *)local_40 + -1;
    UNLOCK();
    if (*(int *)local_40 == 0) goto LAB_003636e0;
  }
                    /* try { // try from 00363649 to 0036364d has its CatchHandler @ 00363738 */
  KisNodeCommand::KisNodeCommand
            ((KisNodeCommand *)this,(KUndo2MagicString *)&local_38,(KisSharedPtr)&local_48);
  if (*(int *)local_38 != 0) {
    if (*(int *)local_38 == -1) goto LAB_00363671;
    LOCK();
    *(int *)local_38 = *(int *)local_38 + -1;
    UNLOCK();
    if (*(int *)local_38 != 0) goto LAB_00363671;
  }
  QArrayData::deallocate(local_38,2,8);
LAB_00363671:
  if (local_48 != (long *)0x0) {
    LOCK();
    plVar1 = local_48 + 2;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_48 + 0x20))();
    }
  }
  puVar2 = PTR_vtable_008372e0 + 0xb8;
  *(undefined **)this = PTR_vtable_008372e0 + 0x10;
  *(undefined **)(this + 0x30) = puVar2;
                    /* try { // try from 003636a7 to 003636ab has its CatchHandler @ 0036372c */
  FUN_00365c80(this + 0x38,param_3);
                    /* try { // try from 003636b7 to 003636bc has its CatchHandler @ 00363720 */
  (**(code **)(*(long *)*param_2 + 0x98))(this + 0x40);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



