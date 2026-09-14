/* Class KisSetLayerStyleCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSetLayerStyleCommand @ 0036e700 ======

/* KisSetLayerStyleCommand::KisSetLayerStyleCommand(KisSharedPtr<KisLayer>,
   QSharedPointer<KisPSDLayerStyle>, QSharedPointer<KisPSDLayerStyle>, KUndo2Command*) */

void __thiscall
KisSetLayerStyleCommand::KisSetLayerStyleCommand
          (KisSetLayerStyleCommand *this,KisSharedPtr param_1,QSharedPointer param_2,
          QSharedPointer param_3,KUndo2Command *param_4)

{
  long lVar1;
  int *piVar2;
  undefined8 uVar3;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  ki18ndc((char *)&local_48,"krita","(qtundo-format)");
                    /* try { // try from 0036e760 to 0036e764 has its CatchHandler @ 0036e881 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
                    /* try { // try from 0036e773 to 0036e777 has its CatchHandler @ 0036e88d */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_50);
  if (*(int *)local_50 == 0) {
LAB_0036e850:
    QArrayData::deallocate(local_50,2,8);
  }
  else if (*(int *)local_50 != -1) {
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 == 0) goto LAB_0036e850;
  }
                    /* try { // try from 0036e7a6 to 0036e7aa has its CatchHandler @ 0036e899 */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_48,param_4);
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_0036e7ce;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0036e7ce;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0036e7ce:
  *(undefined **)this = PTR_vtable_00836e30 + 0x10;
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x28) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  uVar3 = ((undefined8 *)CONCAT44(in_register_00000014,param_2))[1];
  *(undefined8 *)(this + 0x30) = *(undefined8 *)CONCAT44(in_register_00000014,param_2);
  *(undefined8 *)(this + 0x38) = uVar3;
  piVar2 = *(int **)(this + 0x38);
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x38) + 4) = *(int *)(*(long *)(this + 0x38) + 4) + 1;
    UNLOCK();
  }
  uVar3 = ((undefined8 *)CONCAT44(in_register_0000000c,param_3))[1];
  *(undefined8 *)(this + 0x40) = *(undefined8 *)CONCAT44(in_register_0000000c,param_3);
  *(undefined8 *)(this + 0x48) = uVar3;
  piVar2 = *(int **)(this + 0x48);
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x48) + 4) = *(int *)(*(long *)(this + 0x48) + 4) + 1;
    UNLOCK();
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



