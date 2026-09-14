/* Class KisProcessingApplicator - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisProcessingApplicator @ 00202e10 ======

void __thiscall
KisProcessingApplicator::KisProcessingApplicator
          (KisProcessingApplicator *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          QFlags param_3,QVector param_4,KUndo2MagicString *param_5,KUndo2CommandExtraData *param_6,
          int param_7)

{
  (*(code *)PTR_KisProcessingApplicator_008391d8)();
  return;
}



// ====== KisProcessingApplicator @ 00207e90 ======

void __thiscall KisProcessingApplicator::KisProcessingApplicator(void)

{
  (*(code *)PTR_KisProcessingApplicator_0083ba18)();
  return;
}



// ====== KisProcessingApplicator @ 0062cc60 ======

/* KisProcessingApplicator::KisProcessingApplicator(KisWeakSharedPtr<KisImage>,
   QList<KisSharedPtr<KisNode> >, QFlags<KisProcessingApplicator::ProcessingFlag>,
   QVector<KisImageSignalType>, KUndo2MagicString const&, KUndo2CommandExtraData*, int) */

void __thiscall
KisProcessingApplicator::KisProcessingApplicator
          (void *this,long *param_2,undefined8 param_3,undefined4 param_4,long *param_5,
          KUndo2MagicString *param_6,KUndo2CommandExtraData *param_11,int param_12)

{
  uint *puVar1;
  long *plVar2;
  char cVar3;
  uint uVar4;
  undefined4 uVar5;
  long *plVar6;
  undefined8 uVar7;
  long lVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  QArrayData *pQVar12;
  int iVar13;
  undefined *puVar14;
  undefined4 *puVar15;
  KisStrokeStrategyUndoCommandBased *this_00;
  long lVar16;
  undefined8 *puVar17;
  _Result_base *this_01;
  FlipFlopCommand *pFVar18;
  uint *puVar19;
  int *piVar20;
  int *piVar21;
  int *piVar22;
  KisStrokeUndoFacade *pKVar23;
  undefined4 *puVar24;
  QArrayData *pQVar25;
  QArrayData *pQVar26;
  undefined4 *puVar27;
  long in_FS_OFFSET;
  int *local_b8;
  QArrayData *local_80;
  undefined local_78 [16];
  int *local_68;
  uint *puStack_60;
  undefined8 uStack_58;
  undefined8 local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*param_2 == 0) {
    *(undefined8 *)this = 0;
LAB_0062d817:
    *(undefined8 *)((long)this + 8) = 0;
  }
  else if (((uint *)param_2[1] == (uint *)0x0) || ((*(uint *)param_2[1] & 1) == 0)) {
    *(undefined (*) [16])this = (undefined  [16])0x0;
  }
  else {
    lVar16 = *param_2;
    *(long *)this = lVar16;
    if (lVar16 == 0) goto LAB_0062d817;
    piVar20 = *(int **)(lVar16 + 0x58);
    if (piVar20 == (int *)0x0) {
      piVar20 = (int *)operator_new(4);
      *piVar20 = 0;
      *(int **)(lVar16 + 0x58) = piVar20;
      LOCK();
      *piVar20 = *piVar20 + 1;
      UNLOCK();
      piVar20 = *(int **)(lVar16 + 0x58);
    }
    *(int **)((long)this + 8) = piVar20;
    LOCK();
    *piVar20 = *piVar20 + 2;
    UNLOCK();
  }
                    /* try { // try from 0062ccde to 0062cce2 has its CatchHandler @ 0062df5e */
  FUN_002de570((long)this + 0x10,param_3);
  piVar20 = (int *)*param_5;
  *(undefined4 *)((long)this + 0x18) = param_4;
  if (*piVar20 == 0) {
    if (*(char *)((long)piVar20 + 0xb) < '\0') {
      lVar16 = QArrayData::allocate(0x48,8,(ulong)(piVar20[2] & 0x7fffffff),0);
      *(long *)((long)this + 0x20) = lVar16;
      if (lVar16 == 0) {
                    /* try { // try from 0024615e to 00246162 has its CatchHandler @ 0024605d */
        qBadAlloc();
        lVar16 = *(long *)((long)this + 0x20);
      }
      *(byte *)(lVar16 + 0xb) = *(byte *)(lVar16 + 0xb) | 0x80;
    }
    else {
      lVar16 = QArrayData::allocate(0x48,8,(long)piVar20[1],0);
      *(long *)((long)this + 0x20) = lVar16;
      if (lVar16 == 0) {
                    /* try { // try from 00246045 to 00246049 has its CatchHandler @ 0024605d */
        qBadAlloc();
        lVar16 = *(long *)((long)this + 0x20);
      }
    }
    if ((*(uint *)(lVar16 + 8) & 0x7fffffff) != 0) {
      lVar8 = *param_5;
      puVar27 = (undefined4 *)(*(long *)(lVar8 + 0x10) + lVar8);
      iVar13 = *(int *)(lVar8 + 4);
      puVar15 = (undefined4 *)(lVar16 + *(long *)(lVar16 + 0x10));
      puVar24 = puVar27;
      while (puVar27 + (long)iVar13 * 0x12 != puVar24) {
        uVar7 = *(undefined8 *)(puVar24 + 2);
        uVar9 = *(undefined8 *)(puVar24 + 4);
        uVar10 = *(undefined8 *)(puVar24 + 6);
        uVar11 = *(undefined8 *)(puVar24 + 8);
        *puVar15 = *puVar24;
        lVar16 = *(long *)(puVar24 + 10);
        *(undefined8 *)(puVar15 + 2) = uVar7;
        *(undefined8 *)(puVar15 + 4) = uVar9;
        *(long *)(puVar15 + 10) = lVar16;
        *(undefined8 *)(puVar15 + 6) = uVar10;
        *(undefined8 *)(puVar15 + 8) = uVar11;
        if (lVar16 != 0) {
          LOCK();
          *(int *)(lVar16 + 0x10) = *(int *)(lVar16 + 0x10) + 1;
          UNLOCK();
        }
                    /* try { // try from 0062d1c9 to 0062d1cd has its CatchHandler @ 0062de7a */
        FUN_002de570(puVar15 + 0xc,puVar24 + 0xc);
        lVar16 = *(long *)(puVar24 + 0xe);
        *(long *)(puVar15 + 0xe) = lVar16;
        if (lVar16 != 0) {
          LOCK();
          *(int *)(lVar16 + 0x10) = *(int *)(lVar16 + 0x10) + 1;
          UNLOCK();
        }
                    /* try { // try from 0062d1e9 to 0062d1ed has its CatchHandler @ 0062de6e */
        FUN_002de570(puVar15 + 0x10,puVar24 + 0x10);
        puVar15 = puVar15 + 0x12;
        puVar24 = puVar24 + 0x12;
      }
      *(undefined4 *)(*(long *)((long)this + 0x20) + 4) = *(undefined4 *)(*param_5 + 4);
    }
  }
  else {
    if (*piVar20 != -1) {
      LOCK();
      *piVar20 = *piVar20 + 1;
      UNLOCK();
      piVar20 = (int *)*param_5;
    }
    *(int **)((long)this + 0x20) = piVar20;
  }
  *(undefined *)((long)this + 0x38) = 0;
  *(undefined (*) [16])((long)this + 0x28) = (undefined  [16])0x0;
                    /* try { // try from 0062cd11 to 0062cd29 has its CatchHandler @ 0062df3a */
  puVar14 = (undefined *)operator_new(1);
  *puVar14 = 0;
  *(undefined **)((long)this + 0x40) = puVar14;
  puVar15 = (undefined4 *)operator_new(0x18);
  *(undefined **)(puVar15 + 4) = puVar14;
  *(code **)(puVar15 + 2) = FUN_00371620;
  puVar15[1] = 1;
  *puVar15 = 1;
  *(undefined4 **)((long)this + 0x48) = puVar15;
  *(undefined8 *)((long)this + 0x50) = 0;
  *(undefined8 *)((long)this + 0x58) = 0;
                    /* try { // try from 0062cd5f to 0062cd63 has its CatchHandler @ 0062df2e */
  this_00 = (KisStrokeStrategyUndoCommandBased *)operator_new(0xd0);
                    /* WARNING: Load size is inaccurate */
  if (*this == 0) {
LAB_0062d602:
    pKVar23 = (KisStrokeUndoFacade *)0x0;
  }
  else {
                    /* try { // try from 0062cd84 to 0062cd88 has its CatchHandler @ 0062df8e */
    if (((*(uint **)((long)this + 8) == (uint *)0x0) || ((**(uint **)((long)this + 8) & 1) == 0)) &&
       (lVar16 = _41000(), *(char *)(lVar16 + 0x11) != '\0')) {
                    /* try { // try from 0062d840 to 0062d874 has its CatchHandler @ 0062df8e */
      lVar16 = _41000();
      local_50 = *(undefined8 *)(lVar16 + 8);
      puStack_60 = (uint *)0x0;
      uStack_58 = 0;
      local_68 = (int *)0x2;
      QMessageLogger::warning();
      if (1 < *(int *)(local_78._0_8_ + 0x28)) {
        *(uint *)(local_78._0_8_ + 0x48) = *(uint *)(local_78._0_8_ + 0x48) | 1;
      }
                    /* try { // try from 0062d88c to 0062d890 has its CatchHandler @ 0062dee6 */
      kisBacktrace();
                    /* try { // try from 0062d8a0 to 0062d8a4 has its CatchHandler @ 0062dec2 */
      QDebug::putString((QChar *)local_78,(ulong)(local_80 + *(long *)(local_80 + 0x10)));
      if (*(QTextStream *)(local_78._0_8_ + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 0062dd15 to 0062dd19 has its CatchHandler @ 0062dec2 */
        QTextStream::operator<<((QTextStream *)local_78._0_8_,' ');
      }
      if (*(int *)local_80 == 0) {
LAB_0062d8d0:
        QArrayData::deallocate(local_80,2,8);
      }
      else if (*(int *)local_80 != -1) {
        LOCK();
        *(int *)local_80 = *(int *)local_80 + -1;
        UNLOCK();
        if (*(int *)local_80 == 0) goto LAB_0062d8d0;
      }
      QDebug::~QDebug((QDebug *)local_78);
    }
                    /* WARNING: Load size is inaccurate */
    if (*this == 0) goto LAB_0062d602;
    pKVar23 = (KisStrokeUndoFacade *)(*this + 0x18);
  }
  local_68 = (int *)0x0;
  puStack_60 = (uint *)0x0;
  local_78 = (undefined  [16])0x0;
                    /* try { // try from 0062cdcc to 0062cdd0 has its CatchHandler @ 0062df76 */
  KisStrokeStrategyUndoCommandBased::KisStrokeStrategyUndoCommandBased
            (this_00,param_6,false,pKVar23,(QSharedPointer)(QDebug *)local_78,
             (QSharedPointer)&local_68);
  uVar7 = local_78._8_8_;
  if ((int *)local_78._8_8_ != (int *)0x0) {
    LOCK();
    piVar20 = (int *)(local_78._8_8_ + 4);
    *piVar20 = *piVar20 + -1;
    UNLOCK();
    if (*piVar20 == 0) {
      (**(code **)(local_78._8_8_ + 8))(local_78._8_8_);
    }
    LOCK();
    *(int *)uVar7 = *(int *)uVar7 + -1;
    UNLOCK();
    if (*(int *)uVar7 == 0) {
      operator_delete((void *)uVar7,0x10);
    }
  }
  puVar19 = puStack_60;
  if (puStack_60 != (uint *)0x0) {
    LOCK();
    puVar1 = puStack_60 + 1;
    *puVar1 = *puVar1 - 1;
    UNLOCK();
    if (*puVar1 == 0) {
      (**(code **)(puStack_60 + 2))(puStack_60);
    }
    LOCK();
    *puVar19 = *puVar19 - 1;
    UNLOCK();
    if (*puVar19 == 0) {
      operator_delete(puVar19,0x10);
    }
  }
  *(undefined8 *)(this_00 + 0xb8) = 0;
  *(undefined ***)this_00 = &PTR_FUN_008349f0;
                    /* try { // try from 0062ce34 to 0062ce38 has its CatchHandler @ 0062df6a */
  puVar17 = (undefined8 *)operator_new(0x30);
  uVar7 = DAT_00726ba0;
  *(undefined4 *)(puVar17 + 4) = 0;
  puVar17[3] = 0;
  puVar17[1] = uVar7;
  puVar14 = PTR_vtable_00837e70;
  *(undefined *)((long)puVar17 + 0x24) = 0;
  *(undefined4 *)(puVar17 + 5) = 0;
  *puVar17 = puVar14 + 0x10;
  puVar14 = PTR_vtable_008378b8;
  *(undefined8 **)(this_00 + 0xc0) = puVar17;
  puVar17[2] = puVar14 + 0x10;
  *(undefined8 **)(this_00 + 0xb8) = puVar17 + 2;
                    /* try { // try from 0062ce91 to 0062ce95 has its CatchHandler @ 0062df52 */
  this_01 = (_Result_base *)operator_new(0x18);
  std::__future_base::_Result_base::_Result_base(this_01);
  puVar14 = PTR_vtable_00837410;
  lVar16 = *(long *)(this_00 + 0xb8);
  lVar8 = *(long *)(this_00 + 0xc0);
  this_01[0x11] = (_Result_base)0x0;
  *(_Result_base **)(this_00 + 200) = this_01;
  *(undefined **)this_01 = puVar14 + 0x10;
  if (lVar8 != 0) {
    if (*PTR___libc_single_threaded_00837338 == '\0') {
      LOCK();
      *(int *)(lVar8 + 8) = *(int *)(lVar8 + 8) + 1;
      UNLOCK();
    }
    else {
      *(int *)(lVar8 + 8) = *(int *)(lVar8 + 8) + 1;
    }
  }
  if (lVar16 != 0) {
    LOCK();
    cVar3 = *(char *)(lVar16 + 0x14);
    *(char *)(lVar16 + 0x14) = '\x01';
    UNLOCK();
    if (cVar3 == '\0') {
      plVar6 = *(long **)((long)this + 0x58);
      *(long *)((long)this + 0x50) = lVar16;
      *(long *)((long)this + 0x58) = lVar8;
      if (plVar6 != (long *)0x0) {
        plVar2 = plVar6 + 1;
        if (plVar6[1] == 0x100000001) {
          plVar6[1] = 0;
          (**(code **)(*plVar6 + 0x10))(plVar6);
          (**(code **)(*plVar6 + 0x18))(plVar6);
        }
        else {
          if (*PTR___libc_single_threaded_00837338 == '\0') {
            LOCK();
            iVar13 = *(int *)plVar2;
            *(int *)plVar2 = *(int *)plVar2 + -1;
            UNLOCK();
          }
          else {
            iVar13 = *(int *)(plVar6 + 1);
            *(int *)(plVar6 + 1) = iVar13 + -1;
          }
          if (iVar13 == 1) {
            FUN_00558450(plVar6);
          }
        }
      }
      if ((*(byte *)((long)this + 0x18) & 4) != 0) {
                    /* try { // try from 0062d4c8 to 0062d4e1 has its CatchHandler @ 0062df2e */
        KisStrokeStrategy::setSupportsWrapAroundMode((KisStrokeStrategy *)this_00,true);
      }
      if (param_11 != (KUndo2CommandExtraData *)0x0) {
                    /* try { // try from 0062cf6d to 0062d09b has its CatchHandler @ 0062df2e */
        KisStrokeStrategyUndoCommandBased::setCommandExtraData(this_00,param_11);
      }
      KisStrokeStrategyUndoCommandBased::setMacroId(this_00,param_12);
                    /* WARNING: Load size is inaccurate */
      if ((((*this == 0) || (*(uint **)((long)this + 8) == (uint *)0x0)) ||
          ((**(uint **)((long)this + 8) & 1) == 0)) &&
         (lVar16 = _41000(), *(char *)(lVar16 + 0x11) != '\0')) {
        lVar16 = _41000();
        local_50 = *(undefined8 *)(lVar16 + 8);
        local_68 = (int *)0x2;
        puStack_60 = (uint *)0x0;
        uStack_58 = 0;
        QMessageLogger::warning();
        if (1 < *(int *)(local_78._0_8_ + 0x28)) {
          *(uint *)(local_78._0_8_ + 0x48) = *(uint *)(local_78._0_8_ + 0x48) | 1;
        }
                    /* try { // try from 0062d0b3 to 0062d0b7 has its CatchHandler @ 0062deb6 */
        kisBacktrace();
                    /* try { // try from 0062d0c7 to 0062d0cb has its CatchHandler @ 0062deaa */
        QDebug::putString((QChar *)local_78,(ulong)(local_80 + *(long *)(local_80 + 0x10)));
        if (*(QTextStream *)(local_78._0_8_ + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 0062dd25 to 0062dd29 has its CatchHandler @ 0062deaa */
          QTextStream::operator<<((QTextStream *)local_78._0_8_,' ');
        }
        if (*(int *)local_80 == 0) {
LAB_0062d100:
          QArrayData::deallocate(local_80,2,8);
        }
        else if (*(int *)local_80 != -1) {
          LOCK();
          *(int *)local_80 = *(int *)local_80 + -1;
          UNLOCK();
          if (*(int *)local_80 == 0) goto LAB_0062d100;
        }
        QDebug::~QDebug((QDebug *)local_78);
      }
                    /* WARNING: Load size is inaccurate */
      (**(code **)(**this + 0xe8))(&local_68,*this,this_00);
      puVar19 = puStack_60;
      piVar22 = local_68;
      piVar20 = *(int **)((long)this + 0x28);
      local_68 = (int *)0x0;
      puStack_60 = (uint *)0x0;
      *(int **)((long)this + 0x28) = piVar22;
      *(uint **)((long)this + 0x30) = puVar19;
      if (piVar20 != (int *)0x0) {
        LOCK();
        *piVar20 = *piVar20 + -1;
        UNLOCK();
        if (*piVar20 == 0) {
          operator_delete(piVar20,0x10);
        }
        if (local_68 != (int *)0x0) {
          LOCK();
          *local_68 = *local_68 + -1;
          UNLOCK();
          if ((*local_68 == 0) && (local_68 != (int *)0x0)) {
            operator_delete(local_68,0x10);
          }
        }
      }
      if (*(int *)(*(long *)((long)this + 0x20) + 4) != 0) {
                    /* try { // try from 0062d615 to 0062d619 has its CatchHandler @ 0062df2e */
        pFVar18 = (FlipFlopCommand *)operator_new(0x50);
        piVar20 = *(int **)((long)this + 0x20);
        if (*piVar20 == 0) {
          if (*(char *)((long)piVar20 + 0xb) < '\0') {
            lVar16 = QArrayData::allocate(0x48,8,(ulong)(piVar20[2] & 0x7fffffff),0);
            local_78._0_8_ = lVar16;
            if (lVar16 == 0) {
                    /* try { // try from 002461df to 002461e3 has its CatchHandler @ 002461d3 */
              qBadAlloc();
            }
            *(byte *)(lVar16 + 0xb) = *(byte *)(lVar16 + 0xb) | 0x80;
          }
          else {
            lVar16 = QArrayData::allocate(0x48,8,(long)piVar20[1],0);
            local_78._0_8_ = lVar16;
            if (lVar16 == 0) {
                    /* try { // try from 002461bb to 002461bf has its CatchHandler @ 002461d3 */
              qBadAlloc();
            }
          }
          if ((*(uint *)(lVar16 + 8) & 0x7fffffff) != 0) {
            lVar8 = *(long *)((long)this + 0x20);
            puVar27 = (undefined4 *)(*(long *)(lVar8 + 0x10) + lVar8);
            iVar13 = *(int *)(lVar8 + 4);
            puVar15 = (undefined4 *)(lVar16 + *(long *)(lVar16 + 0x10));
            puVar24 = puVar27;
            while (puVar27 + (long)iVar13 * 0x12 != puVar24) {
              uVar7 = *(undefined8 *)(puVar24 + 4);
              uVar9 = *(undefined8 *)(puVar24 + 6);
              uVar10 = *(undefined8 *)(puVar24 + 8);
              uVar5 = *puVar24;
              *(undefined8 *)(puVar15 + 2) = *(undefined8 *)(puVar24 + 2);
              *(undefined8 *)(puVar15 + 4) = uVar7;
              *puVar15 = uVar5;
              lVar16 = *(long *)(puVar24 + 10);
              *(undefined8 *)(puVar15 + 6) = uVar9;
              *(undefined8 *)(puVar15 + 8) = uVar10;
              *(long *)(puVar15 + 10) = lVar16;
              if (lVar16 != 0) {
                LOCK();
                *(int *)(lVar16 + 0x10) = *(int *)(lVar16 + 0x10) + 1;
                UNLOCK();
              }
                    /* try { // try from 0062d9ab to 0062d9af has its CatchHandler @ 0062de56 */
              FUN_002de570(puVar15 + 0xc,puVar24 + 0xc);
              lVar16 = *(long *)(puVar24 + 0xe);
              *(long *)(puVar15 + 0xe) = lVar16;
              if (lVar16 != 0) {
                LOCK();
                *(int *)(lVar16 + 0x10) = *(int *)(lVar16 + 0x10) + 1;
                UNLOCK();
              }
                    /* try { // try from 0062d9d3 to 0062d9d7 has its CatchHandler @ 0062de62 */
              FUN_002de570(puVar15 + 0x10);
              puVar15 = puVar15 + 0x12;
              puVar24 = puVar24 + 0x12;
            }
            *(undefined4 *)(local_78._0_8_ + 4) = *(undefined4 *)(*(long *)((long)this + 0x20) + 4);
          }
        }
        else {
          if (*piVar20 != -1) {
            LOCK();
            *piVar20 = *piVar20 + 1;
            UNLOCK();
            piVar20 = *(int **)((long)this + 0x20);
          }
          local_78._0_8_ = piVar20;
        }
                    /* WARNING: Load size is inaccurate */
        if (*this == 0) {
          local_68 = (int *)0x0;
LAB_0062dc49:
          puStack_60 = (uint *)0x0;
        }
        else if ((*(uint **)((long)this + 8) == (uint *)0x0) ||
                ((**(uint **)((long)this + 8) & 1) == 0)) {
          local_68 = (int *)0x0;
          puStack_60 = (uint *)0x0;
        }
        else {
                    /* WARNING: Load size is inaccurate */
          piVar20 = *this;
          local_68 = piVar20;
          if (piVar20 == (int *)0x0) goto LAB_0062dc49;
          puVar19 = *(uint **)(piVar20 + 0x16);
          if (puVar19 == (uint *)0x0) {
                    /* try { // try from 0062ddc8 to 0062ddcc has its CatchHandler @ 0062df16 */
            piVar22 = (int *)operator_new(4);
            *piVar22 = 0;
            *(int **)(piVar20 + 0x16) = piVar22;
            LOCK();
            *piVar22 = *piVar22 + 1;
            UNLOCK();
            puVar19 = *(uint **)(piVar20 + 0x16);
          }
          LOCK();
          *puVar19 = *puVar19 + 2;
          UNLOCK();
          puStack_60 = puVar19;
        }
                    /* try { // try from 0062d691 to 0062d695 has its CatchHandler @ 0062df0a */
        KisCommandUtils::FlipFlopCommand::FlipFlopCommand(pFVar18,false,(KUndo2Command *)0x0);
        piVar20 = local_68;
        *(undefined ***)pFVar18 = &PTR_FUN_00834910;
        *(undefined ***)(pFVar18 + 0x30) = &PTR_FUN_008349c8;
        if (local_68 == (int *)0x0) {
          *(undefined8 *)(pFVar18 + 0x38) = 0;
LAB_0062dc30:
          *(undefined8 *)(pFVar18 + 0x40) = 0;
LAB_0062d6f5:
          iVar13 = *(int *)local_78._0_8_;
          local_b8 = (int *)local_78._0_8_;
          if (iVar13 != 0) goto LAB_0062d708;
LAB_0062da4c:
          if (*(char *)((long)local_b8 + 0xb) < '\0') {
            lVar16 = QArrayData::allocate(0x48,8,(ulong)(local_b8[2] & 0x7fffffff),0);
            *(long *)(pFVar18 + 0x48) = lVar16;
            if (lVar16 == 0) {
                    /* try { // try from 002461c5 to 002461c9 has its CatchHandler @ 0024614e */
              qBadAlloc();
              lVar16 = *(long *)(pFVar18 + 0x48);
            }
            *(byte *)(lVar16 + 0xb) = *(byte *)(lVar16 + 0xb) | 0x80;
          }
          else {
            lVar16 = QArrayData::allocate(0x48,8,(long)local_b8[1],0);
            *(long *)(pFVar18 + 0x48) = lVar16;
            if (lVar16 == 0) {
                    /* try { // try from 002460d0 to 002460d4 has its CatchHandler @ 0024614e */
              qBadAlloc();
              lVar16 = *(long *)(pFVar18 + 0x48);
            }
          }
          if ((*(uint *)(lVar16 + 8) & 0x7fffffff) != 0) {
            lVar8 = *(long *)(local_b8 + 4);
            iVar13 = local_b8[1];
            puVar15 = (undefined4 *)(lVar16 + *(long *)(lVar16 + 0x10));
            puVar24 = (undefined4 *)(lVar8 + (long)local_b8);
            while ((undefined4 *)(lVar8 + (long)local_b8) + (long)iVar13 * 0x12 != puVar24) {
              uVar7 = *(undefined8 *)(puVar24 + 4);
              uVar9 = *(undefined8 *)(puVar24 + 6);
              uVar10 = *(undefined8 *)(puVar24 + 8);
              uVar5 = *puVar24;
              *(undefined8 *)(puVar15 + 2) = *(undefined8 *)(puVar24 + 2);
              *(undefined8 *)(puVar15 + 4) = uVar7;
              *puVar15 = uVar5;
              lVar16 = *(long *)(puVar24 + 10);
              *(undefined8 *)(puVar15 + 6) = uVar9;
              *(undefined8 *)(puVar15 + 8) = uVar10;
              *(long *)(puVar15 + 10) = lVar16;
              if (lVar16 != 0) {
                LOCK();
                *(int *)(lVar16 + 0x10) = *(int *)(lVar16 + 0x10) + 1;
                UNLOCK();
              }
                    /* try { // try from 0062db03 to 0062db07 has its CatchHandler @ 0062deda */
              FUN_002de570(puVar15 + 0xc,puVar24 + 0xc);
              lVar16 = *(long *)(puVar24 + 0xe);
              *(long *)(puVar15 + 0xe) = lVar16;
              if (lVar16 != 0) {
                LOCK();
                *(int *)(lVar16 + 0x10) = *(int *)(lVar16 + 0x10) + 1;
                UNLOCK();
              }
                    /* try { // try from 0062db2b to 0062db2f has its CatchHandler @ 0062df82 */
              FUN_002de570(puVar15 + 0x10,puVar24 + 0x10);
              puVar15 = puVar15 + 0x12;
              puVar24 = puVar24 + 0x12;
            }
            *(int *)(*(long *)(pFVar18 + 0x48) + 4) = *(int *)(local_78._0_8_ + 4);
            local_b8 = (int *)local_78._0_8_;
          }
        }
        else {
          if ((puStack_60 != (uint *)0x0) && ((*puStack_60 & 1) != 0)) {
            *(int **)(pFVar18 + 0x38) = local_68;
            if (local_68 == (int *)0x0) goto LAB_0062dc30;
            piVar22 = *(int **)(local_68 + 0x16);
            if (piVar22 == (int *)0x0) {
                    /* try { // try from 0062dde9 to 0062dded has its CatchHandler @ 0062dfb2 */
              piVar22 = (int *)operator_new(4);
              *piVar22 = 0;
              *(int **)(piVar20 + 0x16) = piVar22;
              LOCK();
              *piVar22 = *piVar22 + 1;
              UNLOCK();
              piVar22 = *(int **)(piVar20 + 0x16);
            }
            *(int **)(pFVar18 + 0x40) = piVar22;
            LOCK();
            *piVar22 = *piVar22 + 2;
            UNLOCK();
            goto LAB_0062d6f5;
          }
          *(undefined (*) [16])(pFVar18 + 0x38) = (undefined  [16])0x0;
          iVar13 = *(int *)local_78._0_8_;
          local_b8 = (int *)local_78._0_8_;
          if (iVar13 == 0) goto LAB_0062da4c;
LAB_0062d708:
          local_b8 = (int *)local_78._0_8_;
          if (iVar13 != -1) {
            LOCK();
            *(int *)local_78._0_8_ = *(int *)local_78._0_8_ + 1;
            UNLOCK();
            local_b8 = (int *)local_78._0_8_;
          }
          *(int **)(pFVar18 + 0x48) = local_b8;
        }
                    /* try { // try from 0062d72f to 0062d733 has its CatchHandler @ 0062def2 */
        applyCommand((KisProcessingApplicator *)this,(KUndo2Command *)pFVar18,2,0);
        local_68 = (int *)0x0;
        if (puStack_60 != (uint *)0x0) {
          LOCK();
          uVar4 = *puStack_60;
          *puStack_60 = *puStack_60 - 2;
          UNLOCK();
          local_b8 = (int *)local_78._0_8_;
          if (((int)uVar4 < 3) && (puStack_60 != (uint *)0x0)) {
            operator_delete(puStack_60,4);
          }
        }
        if (*local_b8 == 0) {
LAB_0062d780:
          uVar7 = local_78._0_8_;
          iVar13 = *(int *)(local_78._0_8_ + 4);
          pQVar25 = (QArrayData *)(local_78._0_8_ + *(long *)(local_78._0_8_ + 0x10));
          pQVar12 = pQVar25;
          while (pQVar12 != pQVar25 + (long)iVar13 * 0x48) {
            pQVar26 = pQVar12 + 0x48;
            FUN_002de460(pQVar12 + 0x40);
            plVar6 = *(long **)(pQVar12 + 0x38);
            if (plVar6 != (long *)0x0) {
              LOCK();
              plVar2 = plVar6 + 2;
              *(int *)plVar2 = *(int *)plVar2 + -1;
              UNLOCK();
              if (*(int *)plVar2 == 0) {
                (**(code **)(*plVar6 + 0x20))();
              }
            }
            FUN_002de460(pQVar12 + 0x30);
            plVar6 = *(long **)(pQVar12 + 0x28);
            pQVar12 = pQVar26;
            if (plVar6 != (long *)0x0) {
              LOCK();
              plVar2 = plVar6 + 2;
              *(int *)plVar2 = *(int *)plVar2 + -1;
              UNLOCK();
              if (*(int *)plVar2 == 0) {
                (**(code **)(*plVar6 + 0x20))();
              }
            }
          }
          QArrayData::deallocate((QArrayData *)uVar7,0x48,8);
        }
        else if (*local_b8 != -1) {
          LOCK();
          *local_b8 = *local_b8 + -1;
          UNLOCK();
          if (*local_b8 == 0) goto LAB_0062d780;
        }
      }
      if ((*(byte *)((long)this + 0x18) & 2) != 0) {
        pFVar18 = (FlipFlopCommand *)operator_new(0x48);
                    /* WARNING: Load size is inaccurate */
        if (*this == 0) {
          local_68 = (int *)0x0;
LAB_0062dc01:
          puStack_60 = (uint *)0x0;
        }
        else if ((*(uint **)((long)this + 8) == (uint *)0x0) ||
                ((**(uint **)((long)this + 8) & 1) == 0)) {
          local_68 = (int *)0x0;
          puStack_60 = (uint *)0x0;
        }
        else {
                    /* WARNING: Load size is inaccurate */
          piVar20 = *this;
          local_68 = piVar20;
          if (piVar20 == (int *)0x0) goto LAB_0062dc01;
          puVar19 = *(uint **)(piVar20 + 0x16);
          if (puVar19 == (uint *)0x0) {
                    /* try { // try from 0062dd86 to 0062dd8a has its CatchHandler @ 0062df22 */
            piVar22 = (int *)operator_new(4);
            *piVar22 = 0;
            *(int **)(piVar20 + 0x16) = piVar22;
            LOCK();
            *piVar22 = *piVar22 + 1;
            UNLOCK();
            puVar19 = *(uint **)(piVar20 + 0x16);
          }
          LOCK();
          *puVar19 = *puVar19 + 2;
          UNLOCK();
          puStack_60 = puVar19;
        }
                    /* try { // try from 0062d534 to 0062d538 has its CatchHandler @ 0062de3e */
        KisCommandUtils::FlipFlopCommand::FlipFlopCommand(pFVar18,false,(KUndo2Command *)0x0);
        piVar20 = local_68;
        *(undefined ***)pFVar18 = &PTR_FUN_00834750;
        *(undefined ***)(pFVar18 + 0x30) = &PTR_FUN_00834808;
        if (local_68 == (int *)0x0) {
          *(undefined8 *)(pFVar18 + 0x38) = 0;
LAB_0062dc18:
          *(undefined8 *)(pFVar18 + 0x40) = 0;
        }
        else if ((puStack_60 == (uint *)0x0) || ((*puStack_60 & 1) == 0)) {
          *(undefined (*) [16])(pFVar18 + 0x38) = (undefined  [16])0x0;
        }
        else {
          *(int **)(pFVar18 + 0x38) = local_68;
          if (local_68 == (int *)0x0) goto LAB_0062dc18;
          piVar22 = *(int **)(local_68 + 0x16);
          if (piVar22 == (int *)0x0) {
                    /* try { // try from 0062dda7 to 0062ddab has its CatchHandler @ 0062dece */
            piVar22 = (int *)operator_new(4);
            *piVar22 = 0;
            *(int **)(piVar20 + 0x16) = piVar22;
            LOCK();
            *piVar22 = *piVar22 + 1;
            UNLOCK();
            piVar22 = *(int **)(piVar20 + 0x16);
          }
          *(int **)(pFVar18 + 0x40) = piVar22;
          LOCK();
          *piVar22 = *piVar22 + 2;
          UNLOCK();
        }
                    /* try { // try from 0062d5a6 to 0062d5aa has its CatchHandler @ 0062defe */
        applyCommand((KisProcessingApplicator *)this,(KUndo2Command *)pFVar18,2,0);
        local_68 = (int *)0x0;
        if (puStack_60 != (uint *)0x0) {
          LOCK();
          uVar4 = *puStack_60;
          *puStack_60 = *puStack_60 - 2;
          UNLOCK();
          if (((int)uVar4 < 3) && (puStack_60 != (uint *)0x0)) {
            operator_delete(puStack_60,4);
          }
        }
      }
      if (*(int *)(*(long *)((long)this + 0x10) + 0xc) != *(int *)(*(long *)((long)this + 0x10) + 8)
         ) {
                    /* try { // try from 0062d215 to 0062d219 has its CatchHandler @ 0062df2e */
        pFVar18 = (FlipFlopCommand *)operator_new(0x68);
        piVar20 = *(int **)((long)this + 0x48);
        uVar7 = *(undefined8 *)((long)this + 0x40);
        if (piVar20 != (int *)0x0) {
          LOCK();
          *piVar20 = *piVar20 + 1;
          UNLOCK();
          LOCK();
          piVar20[1] = piVar20[1] + 1;
          UNLOCK();
        }
                    /* try { // try from 0062d240 to 0062d244 has its CatchHandler @ 0062df46 */
        FUN_002de570((QDebug *)local_78);
                    /* WARNING: Load size is inaccurate */
        if (*this == 0) {
          local_68 = (int *)0x0;
LAB_0062db79:
          puStack_60 = (uint *)0x0;
        }
        else if ((*(uint **)((long)this + 8) == (uint *)0x0) ||
                ((**(uint **)((long)this + 8) & 1) == 0)) {
          local_68 = (int *)0x0;
          puStack_60 = (uint *)0x0;
        }
        else {
                    /* WARNING: Load size is inaccurate */
          piVar22 = *this;
          local_68 = piVar22;
          if (piVar22 == (int *)0x0) goto LAB_0062db79;
          puVar19 = *(uint **)(piVar22 + 0x16);
          if (puVar19 == (uint *)0x0) {
                    /* try { // try from 0062dd65 to 0062dd69 has its CatchHandler @ 0062de4a */
            piVar21 = (int *)operator_new(4);
            *piVar21 = 0;
            *(int **)(piVar22 + 0x16) = piVar21;
            LOCK();
            *piVar21 = *piVar21 + 1;
            UNLOCK();
            puVar19 = *(uint **)(piVar22 + 0x16);
          }
          LOCK();
          *puVar19 = *puVar19 + 2;
          UNLOCK();
          puStack_60 = puVar19;
        }
        uVar5 = *(undefined4 *)((long)this + 0x18);
                    /* try { // try from 0062d36c to 0062d370 has its CatchHandler @ 0062de9e */
        KisCommandUtils::FlipFlopCommand::FlipFlopCommand(pFVar18,0,(KUndo2Command *)0x0);
        piVar22 = local_68;
        *(undefined ***)pFVar18 = &PTR_FUN_00834830;
        *(undefined ***)(pFVar18 + 0x30) = &PTR_FUN_008348e8;
        if (local_68 == (int *)0x0) {
          *(undefined8 *)(pFVar18 + 0x38) = 0;
LAB_0062db98:
          *(undefined8 *)(pFVar18 + 0x40) = 0;
        }
        else if ((puStack_60 == (uint *)0x0) || ((*puStack_60 & 1) == 0)) {
          *(undefined (*) [16])(pFVar18 + 0x38) = (undefined  [16])0x0;
        }
        else {
          *(int **)(pFVar18 + 0x38) = local_68;
          if (local_68 == (int *)0x0) goto LAB_0062db98;
          piVar21 = *(int **)(local_68 + 0x16);
          if (piVar21 == (int *)0x0) {
                    /* try { // try from 0062dd35 to 0062dd39 has its CatchHandler @ 0062df9a */
            piVar21 = (int *)operator_new(4);
            *piVar21 = 0;
            *(int **)(piVar22 + 0x16) = piVar21;
            LOCK();
            *piVar21 = *piVar21 + 1;
            UNLOCK();
            piVar21 = *(int **)(piVar22 + 0x16);
          }
          *(int **)(pFVar18 + 0x40) = piVar21;
          LOCK();
          *piVar21 = *piVar21 + 2;
          UNLOCK();
        }
                    /* try { // try from 0062d3e0 to 0062d3e4 has its CatchHandler @ 0062de86 */
        FUN_002de570(pFVar18 + 0x48,(QDebug *)local_78);
        *(undefined4 *)(pFVar18 + 0x50) = uVar5;
        *(undefined8 *)(pFVar18 + 0x58) = uVar7;
        *(int **)(pFVar18 + 0x60) = piVar20;
        if (piVar20 != (int *)0x0) {
          LOCK();
          *piVar20 = *piVar20 + 1;
          UNLOCK();
          LOCK();
          *(int *)(*(long *)(pFVar18 + 0x60) + 4) = *(int *)(*(long *)(pFVar18 + 0x60) + 4) + 1;
          UNLOCK();
        }
                    /* try { // try from 0062d41c to 0062d420 has its CatchHandler @ 0062de92 */
        applyCommand((KisProcessingApplicator *)this,(KUndo2Command *)pFVar18,1,0);
        local_68 = (int *)0x0;
        if (puStack_60 != (uint *)0x0) {
          LOCK();
          uVar4 = *puStack_60;
          *puStack_60 = *puStack_60 - 2;
          UNLOCK();
          if (((int)uVar4 < 3) && (puStack_60 != (uint *)0x0)) {
            operator_delete(puStack_60,4);
          }
        }
        FUN_002de460((QDebug *)local_78);
        if (piVar20 != (int *)0x0) {
          LOCK();
          piVar22 = piVar20 + 1;
          *piVar22 = *piVar22 + -1;
          UNLOCK();
          if (*piVar22 == 0) {
            (**(code **)(piVar20 + 2))(piVar20);
          }
          LOCK();
          *piVar20 = *piVar20 + -1;
          UNLOCK();
          if (*piVar20 == 0) {
            if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
              operator_delete(piVar20,0x10);
              return;
            }
            goto LAB_0062de05;
          }
        }
      }
      if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
        return;
      }
      goto LAB_0062de05;
    }
    if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) goto LAB_0062de05;
                    /* try { // try from 0062de1f to 0062de3d has its CatchHandler @ 0062dfa6 */
    std::__throw_future_error(1);
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    std::__throw_future_error(3);
                    /* catch() { ... } // from try @ 0062d534 with catch @ 0062de3e */
    FUN_00245efa();
    return;
  }
LAB_0062de05:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisProcessingApplicator @ 0062f4f0 ======

/* KisProcessingApplicator::KisProcessingApplicator(KisWeakSharedPtr<KisImage>,
   KisSharedPtr<KisNode>, QFlags<KisProcessingApplicator::ProcessingFlag>,
   QVector<KisImageSignalType>, KUndo2MagicString const&, KUndo2CommandExtraData*, int) */

void __thiscall
KisProcessingApplicator::KisProcessingApplicator
          (KisProcessingApplicator *this,KisWeakSharedPtr param_1,KisSharedPtr param_2,
          QFlags param_3,QVector param_4,KUndo2MagicString *param_5,KUndo2CommandExtraData *param_6,
          int param_7)

{
  int iVar1;
  long lVar2;
  undefined auVar3 [8];
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  bool bVar8;
  undefined auVar9 [16];
  QArrayData *pQVar10;
  undefined8 *puVar11;
  undefined8 *puVar12;
  uint *puVar13;
  int *piVar14;
  undefined4 in_register_00000014;
  undefined4 *puVar15;
  QArrayData *pQVar16;
  long *plVar17;
  QArrayData *pQVar18;
  QArrayData *pQVar19;
  undefined4 in_register_00000034;
  long *plVar20;
  undefined4 *puVar21;
  undefined4 in_register_00000084;
  long in_FS_OFFSET;
  uint **local_70;
  uint *local_68;
  QArrayData *local_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  plVar17 = (long *)CONCAT44(in_register_00000084,param_4);
  plVar20 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_60 = (QArrayData *)*plVar17;
  if (*(int *)local_60 == 0) {
    if ((char)local_60[0xb] < '\0') {
      local_60 = (QArrayData *)
                 QArrayData::allocate(0x48,8,(ulong)(*(uint *)(local_60 + 8) & 0x7fffffff),0);
      if (local_60 == (QArrayData *)0x0) {
        qBadAlloc();
      }
      local_60[0xb] = (QArrayData)((byte)local_60[0xb] | 0x80);
    }
    else {
      local_60 = (QArrayData *)QArrayData::allocate(0x48,8,(long)*(int *)(local_60 + 4),0);
      if (local_60 == (QArrayData *)0x0) {
        qBadAlloc();
      }
    }
    if ((*(uint *)(local_60 + 8) & 0x7fffffff) == 0) goto LAB_0062f558;
    lVar2 = *plVar17;
    puVar21 = (undefined4 *)(*(long *)(lVar2 + 0x10) + lVar2);
    iVar1 = *(int *)(lVar2 + 4);
    puVar15 = puVar21;
    pQVar16 = local_60 + *(long *)(local_60 + 0x10);
    while (puVar21 + (long)iVar1 * 0x12 != puVar15) {
      uVar4 = *(undefined8 *)(puVar15 + 6);
      uVar5 = *(undefined8 *)(puVar15 + 8);
      uVar6 = *(undefined8 *)(puVar15 + 2);
      uVar7 = *(undefined8 *)(puVar15 + 4);
      *(undefined4 *)pQVar16 = *puVar15;
      lVar2 = *(long *)(puVar15 + 10);
      *(undefined8 *)(pQVar16 + 8) = uVar6;
      *(undefined8 *)(pQVar16 + 0x10) = uVar7;
      *(long *)(pQVar16 + 0x28) = lVar2;
      *(undefined8 *)(pQVar16 + 0x18) = uVar4;
      *(undefined8 *)(pQVar16 + 0x20) = uVar5;
      if (lVar2 != 0) {
        LOCK();
        *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
        UNLOCK();
      }
                    /* try { // try from 0062f774 to 0062f778 has its CatchHandler @ 0062f9b9 */
      FUN_002de570(pQVar16 + 0x30,puVar15 + 0xc);
      lVar2 = *(long *)(puVar15 + 0xe);
      *(long *)(pQVar16 + 0x38) = lVar2;
      if (lVar2 != 0) {
        LOCK();
        *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
        UNLOCK();
      }
                    /* try { // try from 0062f798 to 0062f79c has its CatchHandler @ 0062f9ad */
      FUN_002de570(pQVar16 + 0x40,puVar15 + 0x10);
      puVar15 = puVar15 + 0x12;
      pQVar16 = pQVar16 + 0x48;
    }
    *(int *)(local_60 + 4) = *(int *)(*plVar17 + 4);
    plVar17 = *(long **)CONCAT44(in_register_00000014,param_2);
    if (plVar17 != (long *)0x0) goto LAB_0062f56b;
LAB_0062f7d0:
    local_68 = (uint *)PTR_shared_null_00837830;
    bVar8 = false;
    puVar13 = (uint *)plVar20[1];
    if (*plVar20 != 0) goto LAB_0062f5dc;
LAB_0062f7f4:
    local_58 = (undefined  [8])0x0;
    auVar3 = local_58;
LAB_0062f7fc:
    local_58 = auVar3;
    piStack_50 = (int *)0x0;
  }
  else {
    if (*(int *)local_60 != -1) {
      LOCK();
      *(int *)local_60 = *(int *)local_60 + 1;
      UNLOCK();
      local_60 = (QArrayData *)*plVar17;
    }
LAB_0062f558:
    plVar17 = *(long **)CONCAT44(in_register_00000014,param_2);
    if (plVar17 == (long *)0x0) goto LAB_0062f7d0;
LAB_0062f56b:
    LOCK();
    *(int *)(plVar17 + 2) = *(int *)(plVar17 + 2) + 1;
    UNLOCK();
    local_68 = (uint *)PTR_shared_null_00837830;
    if (*(int *)(PTR_shared_null_00837830 + 4) < 1) {
      if (*(uint *)PTR_shared_null_00837830 < 2) {
        QListData::realloc((int)&local_68);
      }
      else {
                    /* try { // try from 0062f94f to 0062f984 has its CatchHandler @ 0062f9f9 */
        FUN_00367220(&local_68,1);
      }
      if (1 < *local_68) goto LAB_0062f59c;
LAB_0062f820:
                    /* try { // try from 0062f824 to 0062f828 has its CatchHandler @ 0062f9f9 */
      puVar11 = (undefined8 *)QListData::append();
                    /* try { // try from 0062f831 to 0062f835 has its CatchHandler @ 0062f9ed */
      puVar12 = (undefined8 *)operator_new(8);
    }
    else {
      if (*(uint *)PTR_shared_null_00837830 < 2) goto LAB_0062f820;
LAB_0062f59c:
                    /* try { // try from 0062f5aa to 0062f5ae has its CatchHandler @ 0062f9f9 */
      puVar11 = (undefined8 *)FUN_00374b90(&local_68,0x7fffffff,1);
                    /* try { // try from 0062f5b7 to 0062f5bb has its CatchHandler @ 0062f9d1 */
      puVar12 = (undefined8 *)operator_new(8);
    }
    *puVar12 = plVar17;
    LOCK();
    *(int *)(plVar17 + 2) = *(int *)(plVar17 + 2) + 1;
    UNLOCK();
    *puVar11 = puVar12;
    bVar8 = true;
    puVar13 = (uint *)plVar20[1];
    if (*plVar20 == 0) goto LAB_0062f7f4;
LAB_0062f5dc:
    if ((puVar13 == (uint *)0x0) || ((*puVar13 & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
    }
    else {
      auVar3 = (undefined  [8])*plVar20;
      local_58 = auVar3;
      if (auVar3 == (undefined  [8])0x0) goto LAB_0062f7fc;
      piVar14 = *(int **)((long)auVar3 + 0x58);
      if (piVar14 == (int *)0x0) {
                    /* try { // try from 0062f98c to 0062f990 has its CatchHandler @ 0062f9dd */
        piVar14 = (int *)operator_new(4);
        *piVar14 = 0;
        *(int **)((long)auVar3 + 0x58) = piVar14;
        LOCK();
        *piVar14 = *piVar14 + 1;
        UNLOCK();
        piVar14 = *(int **)((long)auVar3 + 0x58);
        auVar3 = local_58;
      }
      local_58 = auVar3;
      LOCK();
      *piVar14 = *piVar14 + 2;
      UNLOCK();
      piStack_50 = piVar14;
    }
  }
  local_70 = &local_68;
                    /* try { // try from 0062f620 to 0062f624 has its CatchHandler @ 0062f9c5 */
  KisProcessingApplicator(this,local_58,local_70,param_3,&local_60,param_5,param_6,param_7);
  piVar14 = piStack_50;
  auVar9._8_8_ = 0;
  auVar9._0_8_ = piStack_50;
  _local_58 = auVar9 << 0x40;
  if (piVar14 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar14;
    *piVar14 = *piVar14 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar14 != (int *)0x0)) {
      operator_delete(piVar14,4);
    }
  }
  FUN_002de460(local_70);
  if (bVar8) {
    LOCK();
    plVar20 = plVar17 + 2;
    *(int *)plVar20 = *(int *)plVar20 + -1;
    UNLOCK();
    if (*(int *)plVar20 == 0) {
      (**(code **)(*plVar17 + 0x20))(plVar17);
    }
  }
  pQVar16 = local_60;
  if (*(int *)local_60 != 0) {
    if (*(int *)local_60 == -1) goto LAB_0062f69e;
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 != 0) goto LAB_0062f69e;
  }
  iVar1 = *(int *)(local_60 + 4);
  pQVar18 = local_60 + *(long *)(local_60 + 0x10);
  pQVar10 = pQVar18;
  while (pQVar10 != pQVar18 + (long)iVar1 * 0x48) {
    pQVar19 = pQVar10 + 0x48;
    FUN_002de460(pQVar10 + 0x40);
    plVar20 = *(long **)(pQVar10 + 0x38);
    if (plVar20 != (long *)0x0) {
      LOCK();
      plVar17 = plVar20 + 2;
      *(int *)plVar17 = *(int *)plVar17 + -1;
      UNLOCK();
      if (*(int *)plVar17 == 0) {
        (**(code **)(*plVar20 + 0x20))();
      }
    }
    FUN_002de460(pQVar10 + 0x30);
    plVar20 = *(long **)(pQVar10 + 0x28);
    pQVar10 = pQVar19;
    if (plVar20 != (long *)0x0) {
      LOCK();
      plVar17 = plVar20 + 2;
      *(int *)plVar17 = *(int *)plVar17 + -1;
      UNLOCK();
      if (*(int *)plVar17 == 0) {
        (**(code **)(*plVar20 + 0x20))();
      }
    }
  }
  QArrayData::deallocate(pQVar16,0x48,8);
LAB_0062f69e:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



