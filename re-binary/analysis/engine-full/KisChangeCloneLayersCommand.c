/* Class KisChangeCloneLayersCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisChangeCloneLayersCommand @ 002049e0 ======

void __thiscall KisChangeCloneLayersCommand::KisChangeCloneLayersCommand(void)

{
  (*(code *)PTR_KisChangeCloneLayersCommand_00839fc0)();
  return;
}



// ====== KisChangeCloneLayersCommand @ 003776f0 ======

/* KisChangeCloneLayersCommand::KisChangeCloneLayersCommand(QList<KisSharedPtr<KisCloneLayer> >,
   KisSharedPtr<KisLayer>, KUndo2Command*) */

void __thiscall
KisChangeCloneLayersCommand::KisChangeCloneLayersCommand
          (KisChangeCloneLayersCommand *this,long *param_2,long *param_3,KUndo2Command *param_4)

{
  long *plVar1;
  undefined *puVar2;
  QArrayData *pQVar3;
  long lVar4;
  undefined8 *puVar5;
  long *plVar6;
  long in_FS_OFFSET;
  QArrayData *local_70;
  QArrayData *local_68;
  QArrayData *local_60;
  QArrayData *local_58;
  undefined4 local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  ki18ndc((char *)&local_68,"krita","(qtundo-format)");
                    /* try { // try from 0037774b to 0037774f has its CatchHandler @ 003779f2 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_68);
                    /* try { // try from 0037775e to 00377762 has its CatchHandler @ 00377a2e */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_68,(QString *)&local_70);
  if (*(int *)local_70 == 0) {
LAB_00377970:
    QArrayData::deallocate(local_70,2,8);
  }
  else if (*(int *)local_70 != -1) {
    LOCK();
    *(int *)local_70 = *(int *)local_70 + -1;
    UNLOCK();
    if (*(int *)local_70 == 0) goto LAB_00377970;
  }
                    /* try { // try from 0037778f to 00377793 has its CatchHandler @ 003779fe */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_68,param_4);
  if (*(int *)local_68 == 0) {
LAB_00377988:
    QArrayData::deallocate(local_68,2,8);
  }
  else if (*(int *)local_68 != -1) {
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 == 0) goto LAB_00377988;
  }
  *(undefined **)this = PTR_vtable_00837258 + 0x10;
                    /* try { // try from 003777cb to 003777cf has its CatchHandler @ 00377a0a */
  puVar5 = (undefined8 *)operator_new(0x18);
  puVar2 = PTR_shared_null_00837830;
  puVar5[2] = 0;
  *(undefined8 **)(this + 0x28) = puVar5;
  *puVar5 = puVar2;
  puVar5[1] = puVar2;
  puVar2 = (undefined *)*param_2;
  if (*(int *)(puVar2 + 8) == *(int *)(puVar2 + 0xc)) {
                    /* try { // try from 003779e3 to 003779e7 has its CatchHandler @ 00377a3a */
    kis_safe_assert_recoverable
              ("!cloneLayers.isEmpty()",
               "/builds/graphics/krita/libs/image/commands_new/KisChangeCloneLayersCommand.cpp",0x16
              );
  }
  else {
    if (puVar2 != PTR_shared_null_00837830) {
                    /* try { // try from 0037780d to 00377839 has its CatchHandler @ 00377a3a */
      FUN_00377e20((KLocalizedString *)&local_68,param_2);
      pQVar3 = (QArrayData *)*puVar5;
      *puVar5 = local_68;
      local_68 = pQVar3;
      FUN_00377a50((KLocalizedString *)&local_68);
      puVar5 = *(undefined8 **)(this + 0x28);
    }
    FUN_00377e20((KLocalizedString *)&local_68,puVar5);
    local_50 = 1;
    local_60 = local_68 + (long)*(int *)(local_68 + 8) * 8 + 0x10;
    local_58 = local_68 + (long)*(int *)(local_68 + 0xc) * 8 + 0x10;
    if ((long)*(int *)(local_68 + 8) * 8 != (long)*(int *)(local_68 + 0xc) * 8) {
      do {
        while( true ) {
          plVar6 = (long *)**(long **)local_60;
          if (plVar6 != (long *)0x0) {
            LOCK();
            *(int *)(plVar6 + 2) = *(int *)(plVar6 + 2) + 1;
            UNLOCK();
          }
          lVar4 = *(long *)(this + 0x28);
                    /* try { // try from 00377897 to 0037789b has its CatchHandler @ 00377a16 */
          KisCloneLayer::copyFrom();
                    /* try { // try from 003778a2 to 003778a6 has its CatchHandler @ 00377a22 */
          FUN_00361630(lVar4 + 8,(QString *)&local_70);
          if (local_70 != (QArrayData *)0x0) {
            LOCK();
            pQVar3 = local_70 + 0x10;
            *(int *)pQVar3 = *(int *)pQVar3 + -1;
            UNLOCK();
            if (*(int *)pQVar3 == 0) {
              (**(code **)(*(long *)local_70 + 0x20))();
            }
          }
          LOCK();
          plVar1 = plVar6 + 2;
          *(int *)plVar1 = *(int *)plVar1 + -1;
          UNLOCK();
          if (*(int *)plVar1 == 0) break;
          local_60 = local_60 + 8;
          if (local_60 == local_58) goto LAB_003778dc;
        }
        (**(code **)(*plVar6 + 0x20))(plVar6);
        local_60 = local_60 + 8;
      } while (local_58 != local_60);
    }
LAB_003778dc:
    FUN_00377a50((KLocalizedString *)&local_68);
    lVar4 = *(long *)(this + 0x28);
    param_3 = (long *)*param_3;
    plVar6 = *(long **)(lVar4 + 0x10);
    if (param_3 != plVar6) {
      if (param_3 != (long *)0x0) {
        LOCK();
        *(int *)(param_3 + 2) = *(int *)(param_3 + 2) + 1;
        UNLOCK();
        plVar6 = *(long **)(lVar4 + 0x10);
      }
      *(long **)(lVar4 + 0x10) = param_3;
      if (plVar6 != (long *)0x0) {
        LOCK();
        plVar1 = plVar6 + 2;
        *(int *)plVar1 = *(int *)plVar1 + -1;
        UNLOCK();
        if (*(int *)plVar1 == 0) {
          if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x003779c5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
            (**(code **)(*plVar6 + 0x20))();
            return;
          }
          goto LAB_003779ed;
        }
      }
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_003779ed:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



