/* Class KisImageResizeCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageResizeCommand @ 00205110 ======

void __thiscall
KisImageResizeCommand::KisImageResizeCommand
          (KisImageResizeCommand *this,KisWeakSharedPtr param_1,QSize *param_2,
          KUndo2Command *param_3)

{
  (*(code *)PTR_KisImageResizeCommand_0083a358)();
  return;
}



// ====== KisImageResizeCommand @ 0036d110 ======

/* KisImageResizeCommand::KisImageResizeCommand(KisWeakSharedPtr<KisImage>, QSize const&,
   KUndo2Command*) */

void __thiscall
KisImageResizeCommand::KisImageResizeCommand
          (KisImageResizeCommand *this,KisWeakSharedPtr param_1,QSize *param_2,
          KUndo2Command *param_3)

{
  KisImage *pKVar1;
  KisImage *this_00;
  undefined8 uVar2;
  long lVar3;
  undefined *puVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int *piVar7;
  undefined4 in_register_00000034;
  long *plVar8;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  plVar8 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  ki18ndc((char *)&local_48,"krita","(qtundo-format)");
                    /* try { // try from 0036d16b to 0036d16f has its CatchHandler @ 0036d369 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
                    /* try { // try from 0036d17e to 0036d182 has its CatchHandler @ 0036d351 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_50);
  if (*(int *)local_50 == 0) {
LAB_0036d2e0:
    QArrayData::deallocate(local_50,2,8);
  }
  else if (*(int *)local_50 != -1) {
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 == 0) goto LAB_0036d2e0;
  }
                    /* try { // try from 0036d1af to 0036d1b3 has its CatchHandler @ 0036d35d */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_48,param_3);
  if (*(int *)local_48 == 0) {
LAB_0036d2f8:
    QArrayData::deallocate(local_48,2,8);
  }
  else if (*(int *)local_48 != -1) {
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 == 0) goto LAB_0036d2f8;
  }
  puVar4 = PTR_vtable_00836e10;
  *(undefined4 *)(this + 0x28) = 0xffffffff;
  *(undefined4 *)(this + 0x2c) = 0xffffffff;
  *(undefined4 *)(this + 0x30) = 0xffffffff;
  *(undefined4 *)(this + 0x34) = 0xffffffff;
  lVar3 = *plVar8;
  *(undefined **)this = puVar4 + 0x10;
  if (lVar3 == 0) {
    *(undefined8 *)(this + 0x38) = 0;
LAB_0036d318:
    *(undefined8 *)(this + 0x40) = 0;
  }
  else if (((uint *)plVar8[1] == (uint *)0x0) || ((*(uint *)plVar8[1] & 1) == 0)) {
    *(undefined (*) [16])(this + 0x38) = (undefined  [16])0x0;
  }
  else {
    lVar3 = *plVar8;
    *(long *)(this + 0x38) = lVar3;
    if (lVar3 == 0) goto LAB_0036d318;
    piVar7 = *(int **)(lVar3 + 0x58);
    if (piVar7 == (int *)0x0) {
                    /* try { // try from 0036d335 to 0036d339 has its CatchHandler @ 0036d381 */
      piVar7 = (int *)operator_new(4);
      *piVar7 = 0;
      *(int **)(lVar3 + 0x58) = piVar7;
      LOCK();
      *piVar7 = *piVar7 + 1;
      UNLOCK();
      piVar7 = *(int **)(lVar3 + 0x58);
    }
    *(int **)(this + 0x40) = piVar7;
    LOCK();
    *piVar7 = *piVar7 + 2;
    UNLOCK();
    this_00 = *(KisImage **)(this + 0x38);
    if (((this_00 != (KisImage *)0x0) && (*(uint **)(this + 0x40) != (uint *)0x0)) &&
       ((**(uint **)(this + 0x40) & 1) != 0)) {
      pKVar1 = this_00 + 0x50;
      LOCK();
      *(int *)(this_00 + 0x50) = *(int *)(this_00 + 0x50) + 1;
      UNLOCK();
                    /* try { // try from 0036d25a to 0036d269 has its CatchHandler @ 0036d375 */
      uVar5 = KisImage::height(this_00);
      uVar6 = KisImage::width(this_00);
      *(undefined4 *)(this + 0x28) = uVar6;
      uVar2 = *(undefined8 *)param_2;
      *(undefined4 *)(this + 0x2c) = uVar5;
      *(undefined8 *)(this + 0x30) = uVar2;
      LOCK();
      *(int *)pKVar1 = *(int *)pKVar1 + -1;
      UNLOCK();
      if (*(int *)pKVar1 == 0) {
        if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0036d2ad. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          (**(code **)(*(long *)this_00 + 0x20))(this_00);
          return;
        }
        goto LAB_0036d322;
      }
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_0036d322:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



