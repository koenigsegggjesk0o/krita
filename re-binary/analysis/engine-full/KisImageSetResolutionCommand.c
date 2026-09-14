/* Class KisImageSetResolutionCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageSetResolutionCommand @ 00203110 ======

void __thiscall
KisImageSetResolutionCommand::KisImageSetResolutionCommand
          (KisImageSetResolutionCommand *this,KisWeakSharedPtr param_1,double param_2,double param_3
          ,KUndo2Command *param_4)

{
  (*(code *)PTR_KisImageSetResolutionCommand_00839358)();
  return;
}



// ====== KisImageSetResolutionCommand @ 0036d5b0 ======

/* KisImageSetResolutionCommand::KisImageSetResolutionCommand(KisWeakSharedPtr<KisImage>, double,
   double, KUndo2Command*) */

void __thiscall
KisImageSetResolutionCommand::KisImageSetResolutionCommand
          (KisImageSetResolutionCommand *this,KisWeakSharedPtr param_1,double param_2,double param_3
          ,KUndo2Command *param_4)

{
  KisImage *pKVar1;
  KisImage *this_00;
  uint *puVar2;
  long lVar3;
  int *piVar4;
  undefined4 in_register_00000034;
  long *plVar5;
  long in_FS_OFFSET;
  undefined8 uVar6;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  plVar5 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  ki18ndc((char *)&local_48,"krita","(qtundo-format)");
                    /* try { // try from 0036d613 to 0036d617 has its CatchHandler @ 0036d827 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
                    /* try { // try from 0036d626 to 0036d62a has its CatchHandler @ 0036d80f */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_48,(QString *)&local_50);
  if (*(int *)local_50 == 0) {
LAB_0036d790:
    QArrayData::deallocate(local_50,2,8);
  }
  else if (*(int *)local_50 != -1) {
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 == 0) goto LAB_0036d790;
  }
                    /* try { // try from 0036d657 to 0036d65b has its CatchHandler @ 0036d803 */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_48,param_4);
  if (*(int *)local_48 == 0) {
LAB_0036d7a8:
    QArrayData::deallocate(local_48,2,8);
  }
  else if (*(int *)local_48 != -1) {
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 == 0) goto LAB_0036d7a8;
  }
  lVar3 = *plVar5;
  *(undefined **)this = PTR_vtable_00837a58 + 0x10;
  if (lVar3 == 0) {
    *(undefined8 *)(this + 0x28) = 0;
LAB_0036d7c8:
    *(undefined8 *)(this + 0x30) = 0;
  }
  else if (((uint *)plVar5[1] == (uint *)0x0) || ((*(uint *)plVar5[1] & 1) == 0)) {
    *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
  }
  else {
    lVar3 = *plVar5;
    *(long *)(this + 0x28) = lVar3;
    if (lVar3 == 0) goto LAB_0036d7c8;
    piVar4 = *(int **)(lVar3 + 0x58);
    if (piVar4 == (int *)0x0) {
                    /* try { // try from 0036d7e5 to 0036d7e9 has its CatchHandler @ 0036d833 */
      piVar4 = (int *)operator_new(4);
      *piVar4 = 0;
      *(int **)(lVar3 + 0x58) = piVar4;
      LOCK();
      *piVar4 = *piVar4 + 1;
      UNLOCK();
      piVar4 = *(int **)(lVar3 + 0x58);
    }
    *(int **)(this + 0x30) = piVar4;
    LOCK();
    *piVar4 = *piVar4 + 2;
    UNLOCK();
  }
  this_00 = (KisImage *)*plVar5;
  puVar2 = (uint *)plVar5[1];
  *(double *)(this + 0x38) = param_2;
  *(double *)(this + 0x40) = param_3;
  *(undefined (*) [16])(this + 0x48) = (undefined  [16])0x0;
  if (((this_00 != (KisImage *)0x0) && (puVar2 != (uint *)0x0)) && ((*puVar2 & 1) != 0)) {
    pKVar1 = this_00 + 0x50;
    LOCK();
    *(int *)(this_00 + 0x50) = *(int *)(this_00 + 0x50) + 1;
    UNLOCK();
                    /* try { // try from 0036d716 to 0036d727 has its CatchHandler @ 0036d81b */
    uVar6 = KisImage::xRes(this_00);
    *(undefined8 *)(this + 0x48) = uVar6;
    uVar6 = KisImage::yRes(this_00);
    *(undefined8 *)(this + 0x50) = uVar6;
    LOCK();
    *(int *)pKVar1 = *(int *)pKVar1 + -1;
    UNLOCK();
    if (*(int *)pKVar1 == 0) {
      if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0036d761. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        (**(code **)(*(long *)this_00 + 0x20))(this_00);
        return;
      }
      goto LAB_0036d7d5;
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_0036d7d5:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



