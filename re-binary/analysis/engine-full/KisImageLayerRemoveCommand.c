/* Class KisImageLayerRemoveCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageLayerRemoveCommand @ 00202330 ======

void __thiscall
KisImageLayerRemoveCommand::KisImageLayerRemoveCommand
          (KisImageLayerRemoveCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          bool param_3,bool param_4)

{
  (*(code *)PTR_KisImageLayerRemoveCommand_00838c68)();
  return;
}



// ====== KisImageLayerRemoveCommand @ 0035ed20 ======

/* KisImageLayerRemoveCommand::KisImageLayerRemoveCommand(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisNode>, bool, bool) */

void __thiscall
KisImageLayerRemoveCommand::KisImageLayerRemoveCommand
          (KisImageLayerRemoveCommand *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          bool param_3,bool param_4)

{
  long *plVar1;
  int iVar2;
  long lVar3;
  undefined auVar4 [8];
  undefined auVar5 [16];
  undefined auVar6 [16];
  int *piVar7;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  ulong *puVar8;
  long in_FS_OFFSET;
  QArrayData *local_68;
  QArrayData *local_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  puVar8 = (ulong *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*puVar8 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar4 = local_58;
LAB_0035f009:
    local_58 = auVar4;
    piStack_50 = (int *)0x0;
  }
  else if (((uint *)puVar8[1] == (uint *)0x0) || ((*(uint *)puVar8[1] & 1) == 0)) {
    _local_58 = (undefined  [16])0x0;
  }
  else {
    auVar4 = (undefined  [8])*puVar8;
    piStack_50 = (int *)local_58;
    local_58 = auVar4;
    if (auVar4 == (undefined  [8])0x0) goto LAB_0035f009;
    piVar7 = *(int **)((long)auVar4 + 0x58);
    if (piVar7 == (int *)0x0) {
      piVar7 = (int *)operator_new(4);
      *piVar7 = 0;
      *(int **)((long)auVar4 + 0x58) = piVar7;
      LOCK();
      *piVar7 = *piVar7 + 1;
      UNLOCK();
      piVar7 = *(int **)((long)auVar4 + 0x58);
      auVar4 = local_58;
    }
    local_58 = auVar4;
    piStack_50 = piVar7;
    LOCK();
    *piVar7 = *piVar7 + 2;
    UNLOCK();
  }
                    /* try { // try from 0035ed93 to 0035ed97 has its CatchHandler @ 0035f08a */
  ki18ndc((char *)&local_60,"krita","(qtundo-format)");
                    /* try { // try from 0035eda3 to 0035eda7 has its CatchHandler @ 0035f07e */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_60);
                    /* try { // try from 0035edb6 to 0035edba has its CatchHandler @ 0035f0b3 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_60,(QString *)&local_68);
  if (*(int *)local_68 == 0) {
LAB_0035efb0:
    QArrayData::deallocate(local_68,2,8);
  }
  else if (*(int *)local_68 != -1) {
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 == 0) goto LAB_0035efb0;
  }
                    /* try { // try from 0035edee to 0035edf2 has its CatchHandler @ 0035f09b */
  KisImageCommand::KisImageCommand
            ((KisImageCommand *)this,(KUndo2MagicString *)&local_60,(KisWeakSharedPtr)local_58,
             (KUndo2Command *)0x0);
  if (*(int *)local_60 == 0) {
LAB_0035efc8:
    QArrayData::deallocate(local_60,2,8);
  }
  else if (*(int *)local_60 != -1) {
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 == 0) goto LAB_0035efc8;
  }
  piVar7 = piStack_50;
  auVar5._8_8_ = 0;
  auVar5._0_8_ = piStack_50;
  _local_58 = auVar5 << 0x40;
  if (piVar7 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar7;
    *piVar7 = *piVar7 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar7 != (int *)0x0)) {
      operator_delete(piVar7,4);
      piVar7 = piStack_50;
    }
  }
  piStack_50 = piVar7;
  *(undefined **)this = PTR_vtable_00837820 + 0x10;
  lVar3 = *(long *)CONCAT44(in_register_00000014,param_2);
  *(long *)(this + 0x38) = lVar3;
  if (lVar3 != 0) {
    LOCK();
    *(int *)(lVar3 + 0x10) = *(int *)(lVar3 + 0x10) + 1;
    UNLOCK();
  }
  this[0x41] = (KisImageLayerRemoveCommand)param_4;
  this[0x40] = (KisImageLayerRemoveCommand)param_3;
  local_60 = *(QArrayData **)CONCAT44(in_register_00000014,param_2);
  if (local_60 != (QArrayData *)0x0) {
    LOCK();
    *(int *)((long)local_60 + 0x10) = *(int *)((long)local_60 + 0x10) + 1;
    UNLOCK();
  }
  if (*puVar8 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar4 = local_58;
  }
  else {
    if (((uint *)puVar8[1] == (uint *)0x0) || ((*(uint *)puVar8[1] & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
      goto LAB_0035eea1;
    }
    auVar4 = (undefined  [8])*puVar8;
    local_58 = auVar4;
    if (auVar4 != (undefined  [8])0x0) {
      piVar7 = *(int **)((long)auVar4 + 0x58);
      if (piVar7 == (int *)0x0) {
                    /* try { // try from 0035f05d to 0035f061 has its CatchHandler @ 0035f0bf */
        piVar7 = (int *)operator_new(4);
        *piVar7 = 0;
        *(int **)((long)auVar4 + 0x58) = piVar7;
        LOCK();
        *piVar7 = *piVar7 + 1;
        UNLOCK();
        piVar7 = *(int **)((long)auVar4 + 0x58);
        auVar4 = local_58;
      }
      local_58 = auVar4;
      LOCK();
      *piVar7 = *piVar7 + 2;
      UNLOCK();
      piStack_50 = piVar7;
      goto LAB_0035eea1;
    }
  }
  local_58 = auVar4;
  piStack_50 = (int *)0x0;
LAB_0035eea1:
                    /* try { // try from 0035eeaa to 0035eeae has its CatchHandler @ 0035f0a7 */
  addSubtree(this,(KisWeakSharedPtr)local_58,(KisSharedPtr)(KLocalizedString *)&local_60);
  piVar7 = piStack_50;
  auVar6._8_8_ = 0;
  auVar6._0_8_ = piStack_50;
  _local_58 = auVar6 << 0x40;
  if (piVar7 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar7;
    *piVar7 = *piVar7 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar7 != (int *)0x0)) {
      operator_delete(piVar7,4);
    }
  }
  if (local_60 != (QArrayData *)0x0) {
    LOCK();
    plVar1 = (long *)((long)local_60 + 0x10);
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*(long *)local_60 + 0x20))();
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



