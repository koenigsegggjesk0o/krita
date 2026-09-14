/* Class KisChangeProjectionColorCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisChangeProjectionColorCommand @ 0020c080 ======

void __thiscall
KisChangeProjectionColorCommand::KisChangeProjectionColorCommand
          (KisChangeProjectionColorCommand *this,KisSharedPtr param_1,KoColor *param_2,
          KUndo2Command *param_3)

{
  (*(code *)PTR_KisChangeProjectionColorCommand_0083db10)();
  return;
}



// ====== KisChangeProjectionColorCommand @ 00370210 ======

/* KisChangeProjectionColorCommand::KisChangeProjectionColorCommand(KisSharedPtr<KisImage>, KoColor
   const&, KUndo2Command*) */

void __thiscall
KisChangeProjectionColorCommand::KisChangeProjectionColorCommand
          (KisChangeProjectionColorCommand *this,KisSharedPtr param_1,KoColor *param_2,
          KUndo2Command *param_3)

{
  undefined *puVar1;
  KisChangeProjectionColorCommand KVar2;
  int iVar3;
  long lVar4;
  ulong *puVar5;
  undefined *puVar6;
  long lVar7;
  undefined8 uVar8;
  int *piVar9;
  ulong uVar10;
  undefined4 in_register_00000034;
  KoColor *pKVar11;
  undefined8 *puVar12;
  long in_FS_OFFSET;
  byte bVar13;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  bVar13 = 0;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_48 = (QArrayData *)QString::fromAscii_helper("CHANGE_PROJECTION_COLOR_COMMAND",0x1f);
                    /* try { // try from 00370264 to 00370268 has its CatchHandler @ 003704e3 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_50,(QString *)&local_48);
                    /* try { // try from 00370272 to 00370276 has its CatchHandler @ 003704ef */
  KUndo2Command::KUndo2Command((KUndo2Command *)this,(KUndo2MagicString *)&local_50,param_3);
  if (*(int *)local_50 == 0) {
LAB_003703e8:
    QArrayData::deallocate(local_50,2,8);
    iVar3 = *(int *)local_48;
  }
  else {
    if (*(int *)local_50 != -1) {
      LOCK();
      *(int *)local_50 = *(int *)local_50 + -1;
      UNLOCK();
      if (*(int *)local_50 == 0) goto LAB_003703e8;
    }
    iVar3 = *(int *)local_48;
  }
  if (iVar3 != 0) {
    if (iVar3 == -1) goto LAB_003702bd;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_003702bd;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_003702bd:
  lVar7 = *(long *)CONCAT44(in_register_00000034,param_1);
  puVar1 = PTR_vtable_00837d68 + 0x10;
  puVar6 = PTR_vtable_00837d68 + 0xb8;
  *(long *)(this + 0x30) = lVar7;
  *(undefined **)this = puVar1;
  *(undefined **)(this + 0x28) = puVar6;
  if (lVar7 == 0) {
    *(undefined8 *)(this + 0x38) = 0;
  }
  else {
    piVar9 = *(int **)(lVar7 + 0x58);
    if (piVar9 == (int *)0x0) {
                    /* try { // try from 0037048d to 00370491 has its CatchHandler @ 003704d7 */
      piVar9 = (int *)operator_new(4);
      *piVar9 = 0;
      *(int **)(lVar7 + 0x58) = piVar9;
      LOCK();
      *piVar9 = *piVar9 + 1;
      UNLOCK();
      piVar9 = *(int **)(lVar7 + 0x58);
    }
    *(int **)(this + 0x38) = piVar9;
    LOCK();
    *piVar9 = *piVar9 + 2;
    UNLOCK();
  }
                    /* try { // try from 00370307 to 0037030b has its CatchHandler @ 003704fb */
  KisImage::defaultProjectionColor();
  *(undefined8 *)(this + 0x80) = *(undefined8 *)param_2;
  *(KoColor *)(this + 0xb0) = param_2[0x30];
  piVar9 = *(int **)(param_2 + 0x38);
  if (*piVar9 == 0) {
                    /* try { // try from 00370430 to 0037047c has its CatchHandler @ 003704cb */
    lVar7 = QMapDataBase::createData();
    *(long *)(this + 0xb8) = lVar7;
    if (*(long *)(*(long *)(param_2 + 0x38) + 0x10) != 0) {
      uVar8 = FUN_00322080(*(long *)(*(long *)(param_2 + 0x38) + 0x10),lVar7);
      lVar4 = *(long *)(this + 0xb8);
      *(undefined8 *)(lVar7 + 0x10) = uVar8;
      puVar5 = *(ulong **)(lVar4 + 0x10);
      *puVar5 = (ulong)((uint)*puVar5 & 3) | lVar4 + 8U;
      QMapDataBase::recalcMostLeftNode();
    }
  }
  else {
    if (*piVar9 != -1) {
      LOCK();
      *piVar9 = *piVar9 + 1;
      UNLOCK();
      piVar9 = *(int **)(param_2 + 0x38);
    }
    *(int **)(this + 0xb8) = piVar9;
  }
  KVar2 = this[0xb0];
  if ((byte)KVar2 < 8) {
    if (((byte)KVar2 & 4) == 0) {
      if ((KVar2 != (KisChangeProjectionColorCommand)0x0) &&
         (*(KoColor *)(this + 0x88) = param_2[8], ((byte)KVar2 & 2) != 0)) {
        *(undefined2 *)(this + (ulong)(byte)KVar2 + 0x86) =
             *(undefined2 *)(param_2 + (ulong)(byte)KVar2 + 6);
      }
    }
    else {
      *(undefined4 *)(this + 0x88) = *(undefined4 *)(param_2 + 8);
      *(undefined4 *)(this + (ulong)(byte)KVar2 + 0x84) =
           *(undefined4 *)(param_2 + (ulong)(byte)KVar2 + 4);
    }
  }
  else {
    *(undefined8 *)(this + 0x88) = *(undefined8 *)(param_2 + 8);
    *(undefined8 *)(this + (ulong)(byte)KVar2 + 0x80) = *(undefined8 *)(param_2 + (byte)KVar2);
    pKVar11 = param_2 + (8 - (long)(this + (0x88 - (long)((ulong)(this + 0x90) & 0xfffffffffffffff8)
                                           )));
    puVar12 = (undefined8 *)((ulong)(this + 0x90) & 0xfffffffffffffff8);
    for (uVar10 = (ulong)((uint)(byte)KVar2 +
                          (int)(this + (0x88 - (long)((ulong)(this + 0x90) & 0xfffffffffffffff8)))
                         >> 3); uVar10 != 0; uVar10 = uVar10 - 1) {
      *puVar12 = *(undefined8 *)pKVar11;
      pKVar11 = pKVar11 + ((ulong)bVar13 * -2 + 1) * 8;
      puVar12 = puVar12 + (ulong)bVar13 * -2 + 1;
    }
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



