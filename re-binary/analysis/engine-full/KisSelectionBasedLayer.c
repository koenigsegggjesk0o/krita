/* Class KisSelectionBasedLayer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSelectionBasedLayer @ 00203360 ======

void __thiscall
KisSelectionBasedLayer::KisSelectionBasedLayer
          (KisSelectionBasedLayer *this,KisWeakSharedPtr param_1,QString *param_2,
          KisSharedPtr param_3,KisPinnedSharedPtr param_4)

{
  (*(code *)PTR_KisSelectionBasedLayer_00839480)();
  return;
}



// ====== KisSelectionBasedLayer @ 0020a9a0 ======

void __thiscall
KisSelectionBasedLayer::KisSelectionBasedLayer
          (KisSelectionBasedLayer *this,KisSelectionBasedLayer *param_1)

{
  (*(code *)PTR_KisSelectionBasedLayer_0083cfa0)();
  return;
}



// ====== KisSelectionBasedLayer @ 0045b270 ======

/* KisSelectionBasedLayer::KisSelectionBasedLayer(KisSelectionBasedLayer const&) */

void __thiscall
KisSelectionBasedLayer::KisSelectionBasedLayer
          (KisSelectionBasedLayer *this,KisSelectionBasedLayer *param_1)

{
  long *plVar1;
  undefined uVar2;
  long lVar3;
  long *plVar4;
  undefined *puVar5;
  undefined (*pauVar6) [16];
  KisPaintDevice *this_00;
  long in_FS_OFFSET;
  long *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisLayer::KisLayer((KisLayer *)this,(KisLayer *)param_1);
                    /* try { // try from 0045b2a2 to 0045b2a6 has its CatchHandler @ 0045b3fa */
  KisIndirectPaintingSupport::KisIndirectPaintingSupport
            ((KisIndirectPaintingSupport *)(this + 0x38));
                    /* try { // try from 0045b2b3 to 0045b2b7 has its CatchHandler @ 0045b3e2 */
  KisNodeFilterInterface::KisNodeFilterInterface
            ((KisNodeFilterInterface *)(this + 0x48),(KisNodeFilterInterface *)(param_1 + 0x48));
  puVar5 = PTR_vtable_008373b8;
  *(undefined **)this = PTR_vtable_008373b8 + 0x10;
  *(undefined **)(this + 0x38) = puVar5 + 0x278;
  *(undefined **)(this + 0x48) = puVar5 + 0x2b8;
                    /* try { // try from 0045b2e0 to 0045b2e4 has its CatchHandler @ 0045b3ee */
  pauVar6 = (undefined (*) [16])operator_new(0x20);
  local_38 = *(long **)(param_1 + 0x58);
  *pauVar6 = (undefined  [16])0x0;
  uVar2 = *(undefined *)(local_38 + 2);
  *(undefined (**) [16])(this + 0x58) = pauVar6;
  pauVar6[1][0] = uVar2;
  *(undefined **)(pauVar6[1] + 8) = PTR_shared_null_008377d0;
  local_38 = (long *)*local_38;
  if (local_38 != (long *)0x0) {
    LOCK();
    *(int *)(local_38 + 1) = *(int *)(local_38 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 0045b31e to 0045b322 has its CatchHandler @ 0045b3d6 */
  setInternalSelection(this,(KisSharedPtr)&local_38);
  if (local_38 != (long *)0x0) {
    LOCK();
    plVar4 = local_38 + 1;
    *(int *)plVar4 = *(int *)plVar4 + -1;
    UNLOCK();
    if (*(int *)plVar4 == 0) {
      (**(code **)(*local_38 + 8))();
    }
  }
                    /* try { // try from 0045b338 to 0045b33c has its CatchHandler @ 0045b3ee */
  this_00 = (KisPaintDevice *)operator_new(0x28);
                    /* try { // try from 0045b350 to 0045b354 has its CatchHandler @ 0045b3ca */
  KisPaintDevice::KisPaintDevice
            (this_00,*(KisPaintDevice **)(*(long *)(param_1 + 0x58) + 8),0,(KisNode *)0x0);
  lVar3 = *(long *)(this + 0x58);
  if (this_00 != *(KisPaintDevice **)(lVar3 + 8)) {
    LOCK();
    *(int *)(this_00 + 0x10) = *(int *)(this_00 + 0x10) + 1;
    UNLOCK();
    plVar4 = *(long **)(lVar3 + 8);
    *(KisPaintDevice **)(lVar3 + 8) = this_00;
    if (plVar4 != (long *)0x0) {
      LOCK();
      plVar1 = plVar4 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0045b3c3. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          (**(code **)(*plVar4 + 0x20))();
          return;
        }
        goto LAB_0045b3c5;
      }
    }
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_0045b3c5:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSelectionBasedLayer @ 0045b8d0 ======

/* KisSelectionBasedLayer::KisSelectionBasedLayer(KisWeakSharedPtr<KisImage>, QString const&,
   KisSharedPtr<KisSelection>, KisPinnedSharedPtr<KisFilterConfiguration>) */

void __thiscall
KisSelectionBasedLayer::KisSelectionBasedLayer
          (KisSelectionBasedLayer *this,KisWeakSharedPtr param_1,QString *param_2,
          KisSharedPtr param_3,KisPinnedSharedPtr param_4)

{
  KisImage *pKVar1;
  KisDefaultBounds *pKVar2;
  KisDefaultBounds *pKVar3;
  KisPaintDevice *pKVar4;
  int iVar5;
  long *plVar6;
  KisImage *this_00;
  undefined *puVar7;
  undefined (*pauVar8) [16];
  KisPaintDevice *this_01;
  KisDefaultBounds *this_02;
  KoColorSpace *pKVar9;
  QObject *pQVar10;
  undefined4 *puVar11;
  uint *puVar12;
  long lVar13;
  int *piVar14;
  undefined4 in_register_0000000c;
  undefined8 *puVar15;
  undefined8 uVar16;
  uint uVar17;
  undefined4 in_register_00000034;
  long *plVar18;
  undefined4 in_register_00000084;
  KisPinnedSharedPtr KVar19;
  long in_FS_OFFSET;
  KisDefaultBounds *local_88;
  QArrayData *local_80;
  QTextStream *local_78;
  int *local_70;
  long *local_68;
  int *piStack_60;
  undefined8 uStack_58;
  undefined8 local_50;
  long local_40;
  
  plVar18 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar18 == 0) {
    local_68 = (long *)0x0;
    KVar19 = (KisPinnedSharedPtr)&local_68;
    piStack_60 = (int *)0x0;
  }
  else {
    if ((((uint *)plVar18[1] == (uint *)0x0) || ((*(uint *)plVar18[1] & 1) == 0)) &&
       (lVar13 = _41000(), *(char *)(lVar13 + 0x11) != '\0')) {
      lVar13 = _41000();
      KVar19 = (KisPinnedSharedPtr)&local_68;
      local_50 = *(undefined8 *)(lVar13 + 8);
      piStack_60 = (int *)0x0;
      uStack_58 = 0;
      local_68 = (long *)0x2;
      QMessageLogger::warning();
      if (1 < *(int *)(local_78 + 0x28)) {
        *(uint *)(local_78 + 0x48) = *(uint *)(local_78 + 0x48) | 1;
      }
                    /* try { // try from 0045bdc7 to 0045bdcb has its CatchHandler @ 0045c073 */
      kisBacktrace();
                    /* try { // try from 0045bddd to 0045bde1 has its CatchHandler @ 0045c067 */
      QDebug::putString((QChar *)&local_78,(ulong)(local_80 + *(long *)(local_80 + 0x10)));
      if (local_78[0x20] != (QTextStream)0x0) {
                    /* try { // try from 0045bff5 to 0045bff9 has its CatchHandler @ 0045c067 */
        QTextStream::operator<<(local_78,' ');
      }
      if (*(int *)local_80 == 0) {
LAB_0045be10:
        QArrayData::deallocate(local_80,2,8);
      }
      else if (*(int *)local_80 != -1) {
        LOCK();
        *(int *)local_80 = *(int *)local_80 + -1;
        UNLOCK();
        if (*(int *)local_80 == 0) goto LAB_0045be10;
      }
      QDebug::~QDebug((QDebug *)&local_78);
      plVar6 = (long *)*plVar18;
    }
    else {
      plVar6 = (long *)*plVar18;
      KVar19 = (KisPinnedSharedPtr)&local_68;
    }
    local_68 = plVar6;
    if (plVar6 == (long *)0x0) {
      piStack_60 = (int *)0x0;
    }
    else {
      KVar19 = (KisPinnedSharedPtr)&local_68;
      piVar14 = (int *)plVar6[0xb];
      if (piVar14 == (int *)0x0) {
        piVar14 = (int *)operator_new(4);
        *piVar14 = 0;
        plVar6[0xb] = (long)piVar14;
        LOCK();
        *piVar14 = *piVar14 + 1;
        UNLOCK();
        piVar14 = (int *)plVar6[0xb];
      }
      LOCK();
      *piVar14 = *piVar14 + 2;
      UNLOCK();
      piStack_60 = piVar14;
    }
  }
                    /* try { // try from 0045b95e to 0045b962 has its CatchHandler @ 0045c04f */
  KisLayer::KisLayer((KisLayer *)this,KVar19,param_2,0xff);
  local_68 = (long *)0x0;
  if (piStack_60 != (int *)0x0) {
    LOCK();
    iVar5 = *piStack_60;
    *piStack_60 = *piStack_60 + -2;
    UNLOCK();
    if ((iVar5 < 3) && (piStack_60 != (int *)0x0)) {
      operator_delete(piStack_60,4);
    }
  }
                    /* try { // try from 0045b994 to 0045b998 has its CatchHandler @ 0045c0af */
  KisIndirectPaintingSupport::KisIndirectPaintingSupport
            ((KisIndirectPaintingSupport *)(this + 0x38));
  local_68 = *(long **)CONCAT44(in_register_00000084,param_4);
  if (local_68 != (long *)0x0) {
    LOCK();
    *(int *)(local_68 + 1) = *(int *)(local_68 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 0045b9bb to 0045b9bf has its CatchHandler @ 0045c097 */
  KisNodeFilterInterface::KisNodeFilterInterface((KisNodeFilterInterface *)(this + 0x48),KVar19);
  if (local_68 != (long *)0x0) {
    LOCK();
    plVar6 = local_68 + 1;
    *(int *)plVar6 = *(int *)plVar6 + -1;
    UNLOCK();
    if (*(int *)plVar6 == 0) {
      (**(code **)(*local_68 + 8))();
    }
  }
  puVar7 = PTR_vtable_008373b8;
  *(undefined **)this = PTR_vtable_008373b8 + 0x10;
  *(undefined **)(this + 0x38) = puVar7 + 0x278;
  *(undefined **)(this + 0x48) = puVar7 + 0x2b8;
                    /* try { // try from 0045b9fd to 0045ba01 has its CatchHandler @ 0045c07f */
  pauVar8 = (undefined (*) [16])operator_new(0x20);
  puVar7 = PTR_shared_null_008377d0;
  pauVar8[1][0] = 1;
  *(undefined (**) [16])(this + 0x58) = pauVar8;
  *(undefined **)(pauVar8[1] + 8) = puVar7;
  *pauVar8 = (undefined  [16])0x0;
  plVar6 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (plVar6 == (long *)0x0) {
                    /* try { // try from 0045bf23 to 0045bf27 has its CatchHandler @ 0045c07f */
    initSelection(this);
  }
  else {
    LOCK();
    *(int *)(plVar6 + 1) = *(int *)(plVar6 + 1) + 1;
    UNLOCK();
    local_68 = plVar6;
                    /* try { // try from 0045ba38 to 0045ba3c has its CatchHandler @ 0045c08b */
    setInternalSelection(this,KVar19);
    if (local_68 != (long *)0x0) {
      LOCK();
      plVar6 = local_68 + 1;
      *(int *)plVar6 = *(int *)plVar6 + -1;
      UNLOCK();
      if (*(int *)plVar6 == 0) {
        (**(code **)(*local_68 + 8))();
      }
    }
  }
  this_00 = (KisImage *)*plVar18;
  if ((((uint *)plVar18[1] != (uint *)0x0) && (this_00 != (KisImage *)0x0)) &&
     ((*(uint *)plVar18[1] & 1) != 0)) {
    pKVar1 = this_00 + 0x50;
    LOCK();
    *(int *)(this_00 + 0x50) = *(int *)(this_00 + 0x50) + 1;
    UNLOCK();
                    /* try { // try from 0045ba8f to 0045ba93 has its CatchHandler @ 0045c0a3 */
    this_01 = (KisPaintDevice *)operator_new(0x28);
    local_80 = (QArrayData *)puVar7;
                    /* try { // try from 0045baa1 to 0045baa5 has its CatchHandler @ 0045c0bb */
    this_02 = (KisDefaultBounds *)operator_new(0x20);
    if (*plVar18 == 0) {
      local_68 = (long *)0x0;
LAB_0045bf89:
      piStack_60 = (int *)0x0;
    }
    else if (((uint *)plVar18[1] == (uint *)0x0) || ((*(uint *)plVar18[1] & 1) == 0)) {
      local_68 = (long *)0x0;
      piStack_60 = (int *)0x0;
    }
    else {
      plVar18 = (long *)*plVar18;
      local_68 = plVar18;
      if (plVar18 == (long *)0x0) goto LAB_0045bf89;
      piVar14 = (int *)plVar18[0xb];
      if (piVar14 == (int *)0x0) {
                    /* try { // try from 0045bfcd to 0045bfd1 has its CatchHandler @ 0045bfff */
        piVar14 = (int *)operator_new(4);
        *piVar14 = 0;
        plVar18[0xb] = (long)piVar14;
        LOCK();
        *piVar14 = *piVar14 + 1;
        UNLOCK();
        piVar14 = (int *)plVar18[0xb];
      }
      LOCK();
      *piVar14 = *piVar14 + 2;
      UNLOCK();
      piStack_60 = piVar14;
    }
                    /* try { // try from 0045bad6 to 0045bada has its CatchHandler @ 0045c023 */
    KisDefaultBounds::KisDefaultBounds(this_02,KVar19);
    pKVar2 = this_02 + 8;
    LOCK();
    *(int *)(this_02 + 8) = *(int *)(this_02 + 8) + 1;
    UNLOCK();
    LOCK();
    *(int *)(this_02 + 8) = *(int *)(this_02 + 8) + 1;
    UNLOCK();
    local_88 = this_02;
                    /* try { // try from 0045baf8 to 0045bafc has its CatchHandler @ 0045c039 */
    pKVar9 = (KoColorSpace *)KisImage::colorSpace(this_00);
    local_70 = *(int **)(this + 0x18);
    local_78 = (QTextStream *)this;
    if (local_70 == (int *)0x0) {
                    /* try { // try from 0045bfa5 to 0045bfa9 has its CatchHandler @ 0045c039 */
      piVar14 = (int *)operator_new(4);
      *piVar14 = 0;
      *(int **)(this + 0x18) = piVar14;
      LOCK();
      *piVar14 = *piVar14 + 1;
      UNLOCK();
      local_70 = *(int **)(this + 0x18);
    }
    LOCK();
    *local_70 = *local_70 + 2;
    UNLOCK();
                    /* try { // try from 0045bb3d to 0045bb41 has its CatchHandler @ 0045c00b */
    KisPaintDevice::KisPaintDevice
              (this_01,(KisWeakSharedPtr)&local_78,pKVar9,(KisSharedPtr)&local_88,
               (QString *)&local_80);
    pKVar4 = this_01 + 0x10;
    LOCK();
    *(int *)(this_01 + 0x10) = *(int *)(this_01 + 0x10) + 1;
    UNLOCK();
    lVar13 = *(long *)(this + 0x58);
    if (this_01 != *(KisPaintDevice **)(lVar13 + 8)) {
      LOCK();
      *(int *)(this_01 + 0x10) = *(int *)(this_01 + 0x10) + 1;
      UNLOCK();
      plVar18 = *(long **)(lVar13 + 8);
      *(KisPaintDevice **)(lVar13 + 8) = this_01;
      if (plVar18 != (long *)0x0) {
        LOCK();
        plVar6 = plVar18 + 2;
        *(int *)plVar6 = *(int *)plVar6 + -1;
        UNLOCK();
        if (*(int *)plVar6 == 0) {
          (**(code **)(*plVar18 + 0x20))();
        }
      }
    }
    LOCK();
    *(int *)pKVar4 = *(int *)pKVar4 + -1;
    UNLOCK();
    if (*(int *)pKVar4 == 0) {
      (**(code **)(*(long *)this_01 + 0x20))(this_01);
    }
    local_78 = (QTextStream *)0x0;
    if (local_70 != (int *)0x0) {
      LOCK();
      iVar5 = *local_70;
      *local_70 = *local_70 + -2;
      UNLOCK();
      if ((iVar5 < 3) && (local_70 != (int *)0x0)) {
        operator_delete(local_70,4);
      }
    }
    if (local_88 != (KisDefaultBounds *)0x0) {
      LOCK();
      pKVar3 = local_88 + 8;
      *(int *)pKVar3 = *(int *)pKVar3 + -1;
      UNLOCK();
      if (*(int *)pKVar3 == 0) {
        (**(code **)(*(long *)local_88 + 8))();
      }
    }
    LOCK();
    *(int *)pKVar2 = *(int *)pKVar2 + -1;
    UNLOCK();
    if (*(int *)pKVar2 == 0) {
      (**(code **)(*(long *)this_02 + 8))(this_02);
    }
    local_68 = (long *)0x0;
    if (piStack_60 != (int *)0x0) {
      LOCK();
      iVar5 = *piStack_60;
      *piStack_60 = *piStack_60 + -2;
      UNLOCK();
      if ((iVar5 < 3) && (piStack_60 != (int *)0x0)) {
        operator_delete(piStack_60,4);
      }
    }
    if (*(int *)local_80 == 0) {
LAB_0045bed0:
      QArrayData::deallocate(local_80,2,8);
    }
    else if (*(int *)local_80 != -1) {
      LOCK();
      *(int *)local_80 = *(int *)local_80 + -1;
      UNLOCK();
      if (*(int *)local_80 == 0) goto LAB_0045bed0;
    }
    lVar13 = *(long *)(this + 0x58);
                    /* try { // try from 0045bc70 to 0045bc74 has its CatchHandler @ 0045c0a3 */
    pQVar10 = (QObject *)operator_new(8);
                    /* try { // try from 0045bc92 to 0045bc96 has its CatchHandler @ 0045c05b */
    QObject::connect(pQVar10,(char *)this_00,(QObject *)"2sigSizeChanged(QPointF,QPointF)",
                     (char *)this,0x72db14);
                    /* try { // try from 0045bc9c to 0045bca0 has its CatchHandler @ 0045c0a3 */
    puVar11 = (undefined4 *)operator_new(0x18);
    *(QObject **)(puVar11 + 4) = pQVar10;
    *(code **)(puVar11 + 2) = FUN_0045d190;
    puVar11[1] = 1;
    *puVar11 = 1;
    puVar12 = *(uint **)(lVar13 + 0x18);
    uVar17 = puVar12[2] & 0x7fffffff;
    if (*puVar12 < 2) {
      if (uVar17 < puVar12[1] + 1) goto LAB_0045bef0;
    }
    else {
      uVar16 = 0;
      if (uVar17 < puVar12[1] + 1) {
LAB_0045bef0:
        uVar16 = 8;
        uVar17 = puVar12[1] + 1;
      }
                    /* try { // try from 0045bcee to 0045bcf2 has its CatchHandler @ 0045c017 */
      FUN_0045d1d0(lVar13 + 0x18,uVar17,uVar16);
      puVar12 = *(uint **)(lVar13 + 0x18);
    }
    uVar17 = puVar12[1];
    puVar15 = (undefined8 *)((long)puVar12 + (long)(int)uVar17 * 0x10 + *(long *)(puVar12 + 4));
    *puVar15 = pQVar10;
    puVar15[1] = puVar11;
    puVar12[1] = uVar17 + 1;
    LOCK();
    *(int *)pKVar1 = *(int *)pKVar1 + -1;
    UNLOCK();
    if (*(int *)pKVar1 == 0) {
      if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Could not recover jumptable at 0x0045bd57. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        (**(code **)(*(long *)this_00 + 0x20))(this_00);
        return;
      }
      goto LAB_0045bfe9;
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_0045bfe9:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



