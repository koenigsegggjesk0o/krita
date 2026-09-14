/* Class KisMultipleProjection - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMultipleProjection @ 00201880 ======

void __thiscall KisMultipleProjection::KisMultipleProjection(KisMultipleProjection *this)

{
  (*(code *)PTR_KisMultipleProjection_00838710)();
  return;
}



// ====== KisMultipleProjection @ 00206950 ======

void __thiscall
KisMultipleProjection::KisMultipleProjection
          (KisMultipleProjection *this,KisMultipleProjection *param_1)

{
  (*(code *)PTR_KisMultipleProjection_0083af78)();
  return;
}



// ====== KisMultipleProjection @ 0066e640 ======

/* KisMultipleProjection::KisMultipleProjection() */

void __thiscall KisMultipleProjection::KisMultipleProjection(KisMultipleProjection *this)

{
  undefined *puVar1;
  QReadWriteLock *this_00;
  
  this_00 = (QReadWriteLock *)operator_new(0x10);
                    /* try { // try from 0066e65f to 0066e663 has its CatchHandler @ 0066e67a */
  QReadWriteLock::QReadWriteLock(this_00,0);
  puVar1 = PTR_shared_null_008372c0;
  *(QReadWriteLock **)this = this_00;
  *(undefined **)(this_00 + 8) = puVar1;
  return;
}



// ====== KisMultipleProjection @ 0066fa60 ======

/* WARNING: Removing unreachable block (ram,0x0066fb54) */
/* WARNING: Removing unreachable block (ram,0x0066fb5b) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisMultipleProjection::KisMultipleProjection(KisMultipleProjection const&) */

void __thiscall
KisMultipleProjection::KisMultipleProjection
          (KisMultipleProjection *this,KisMultipleProjection *param_1)

{
  KisPaintDevice *pKVar1;
  int *piVar2;
  undefined *puVar3;
  char cVar4;
  QReadWriteLock *this_00;
  ulong uVar5;
  KisPaintDevice *pKVar6;
  long lVar7;
  long lVar8;
  int iVar9;
  uint *puVar10;
  uint *puVar11;
  uint *puVar12;
  long in_FS_OFFSET;
  ulong local_70;
  KisPaintDevice *local_68;
  QArrayData *local_60;
  undefined local_58;
  QArrayData *local_50 [2];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  this_00 = (QReadWriteLock *)operator_new(0x10);
                    /* try { // try from 0066fa9f to 0066faa3 has its CatchHandler @ 0066febe */
  QReadWriteLock::QReadWriteLock(this_00,0);
  puVar3 = PTR_shared_null_008372c0;
  *(QReadWriteLock **)this = this_00;
  *(undefined **)(this_00 + 8) = puVar3;
  uVar5 = *(ulong *)param_1;
  if (uVar5 == 0) {
    if (*(long *)(_DAT_00000008 + 0x10) == 0) goto LAB_0066fc12;
    local_70 = 0;
    lVar7 = _DAT_00000008;
LAB_0066fade:
    lVar8 = *(long *)(lVar7 + 0x20);
    while (lVar8 != lVar7 + 8) {
      local_58 = 0xff;
      local_68 = (KisPaintDevice *)0x0;
      local_60 = (QArrayData *)PTR_shared_null_008377d0;
      local_50[0] = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 0066fb21 to 0066fb25 has its CatchHandler @ 0066feb2 */
      pKVar6 = (KisPaintDevice *)operator_new(0x28);
                    /* try { // try from 0066fb34 to 0066fb38 has its CatchHandler @ 0066feca */
      KisPaintDevice::KisPaintDevice(pKVar6,*(KisPaintDevice **)(lVar8 + 0x20),0,(KisNode *)0x0);
      if (pKVar6 != (KisPaintDevice *)0x0) {
        LOCK();
        *(int *)(pKVar6 + 0x10) = *(int *)(pKVar6 + 0x10) + 1;
        UNLOCK();
        local_68 = pKVar6;
      }
      QString::operator=((QString *)&local_60,(QString *)(lVar8 + 0x28));
      local_58 = *(undefined *)(lVar8 + 0x30);
      QByteArray::operator=((QByteArray *)local_50,(QByteArray *)(lVar8 + 0x38));
      lVar7 = *(long *)this;
      puVar10 = *(uint **)(lVar7 + 8);
      if (*puVar10 < 2) {
        puVar12 = *(uint **)(puVar10 + 4);
        if (puVar12 != (uint *)0x0) goto LAB_0066fbbc;
LAB_0066fdba:
        iVar9 = (int)puVar10;
        puVar10 = puVar10 + 2;
LAB_0066fc7f:
                    /* try { // try from 0066fc8c to 0066fc90 has its CatchHandler @ 0066feb2 */
        lVar7 = QMapDataBase::createNode(iVar9,0x40,(QMapNodeBase *)&DAT_00000008,SUB81(puVar10,0));
        piVar2 = *(int **)(lVar8 + 0x18);
        *(int **)(lVar7 + 0x18) = piVar2;
        if (1 < *piVar2 + 1U) {
          LOCK();
          *piVar2 = *piVar2 + 1;
          UNLOCK();
        }
        *(KisPaintDevice **)(lVar7 + 0x20) = local_68;
        if (local_68 != (KisPaintDevice *)0x0) {
          LOCK();
          *(int *)(local_68 + 0x10) = *(int *)(local_68 + 0x10) + 1;
          UNLOCK();
        }
        *(QArrayData **)(lVar7 + 0x28) = local_60;
        if (1 < *(int *)local_60 + 1U) {
          LOCK();
          *(int *)local_60 = *(int *)local_60 + 1;
          UNLOCK();
        }
        *(undefined *)(lVar7 + 0x30) = local_58;
        *(QArrayData **)(lVar7 + 0x38) = local_50[0];
        if (1 < *(int *)local_50[0] + 1U) {
          LOCK();
          *(int *)local_50[0] = *(int *)local_50[0] + 1;
          UNLOCK();
        }
      }
      else {
                    /* try { // try from 0066fda4 to 0066fda8 has its CatchHandler @ 0066feb2 */
        FUN_00670a70(lVar7 + 8);
        puVar10 = *(uint **)(lVar7 + 8);
        puVar12 = *(uint **)(puVar10 + 4);
        if (puVar12 == (uint *)0x0) goto LAB_0066fdba;
LAB_0066fbbc:
        puVar11 = (uint *)0x0;
        do {
          while( true ) {
            puVar10 = puVar12;
            cVar4 = operator<((QString *)(puVar10 + 6),(QString *)(lVar8 + 0x18));
            if (cVar4 == '\0') break;
            puVar12 = *(uint **)(puVar10 + 4);
            if (*(uint **)(puVar10 + 4) == (uint *)0x0) {
              if (puVar11 == (uint *)0x0) {
                iVar9 = (int)*(undefined8 *)(lVar7 + 8);
                goto LAB_0066fc7f;
              }
              goto LAB_0066fc5c;
            }
          }
          puVar11 = puVar10;
          puVar12 = *(uint **)(puVar10 + 2);
        } while (*(uint **)(puVar10 + 2) != (uint *)0x0);
LAB_0066fc5c:
        cVar4 = operator<((QString *)(lVar8 + 0x18),(QString *)(puVar11 + 6));
        if (cVar4 != '\0') {
          iVar9 = (int)*(undefined8 *)(lVar7 + 8);
          goto LAB_0066fc7f;
        }
        pKVar6 = *(KisPaintDevice **)(puVar11 + 8);
        if (local_68 != pKVar6) {
          if (local_68 != (KisPaintDevice *)0x0) {
            LOCK();
            *(int *)(local_68 + 0x10) = *(int *)(local_68 + 0x10) + 1;
            UNLOCK();
            pKVar6 = *(KisPaintDevice **)(puVar11 + 8);
          }
          *(KisPaintDevice **)(puVar11 + 8) = local_68;
          if (pKVar6 != (KisPaintDevice *)0x0) {
            LOCK();
            pKVar1 = pKVar6 + 0x10;
            *(int *)pKVar1 = *(int *)pKVar1 + -1;
            UNLOCK();
            if (*(int *)pKVar1 == 0) {
              (**(code **)(*(long *)pKVar6 + 0x20))();
            }
          }
        }
        QString::operator=((QString *)(puVar11 + 10),(QString *)&local_60);
        *(undefined *)(puVar11 + 0xc) = local_58;
        QByteArray::operator=((QByteArray *)(puVar11 + 0xe),(QByteArray *)local_50);
      }
      if (*(int *)local_50[0] == 0) {
LAB_0066fd88:
        QArrayData::deallocate(local_50[0],1,8);
      }
      else if (*(int *)local_50[0] != -1) {
        LOCK();
        *(int *)local_50[0] = *(int *)local_50[0] + -1;
        UNLOCK();
        if (*(int *)local_50[0] == 0) goto LAB_0066fd88;
      }
      if (*(int *)local_60 == 0) {
LAB_0066fd70:
        QArrayData::deallocate(local_60,2,8);
      }
      else if (*(int *)local_60 != -1) {
        LOCK();
        *(int *)local_60 = *(int *)local_60 + -1;
        UNLOCK();
        if (*(int *)local_60 == 0) goto LAB_0066fd70;
      }
      if (local_68 != (KisPaintDevice *)0x0) {
        LOCK();
        pKVar6 = local_68 + 0x10;
        *(int *)pKVar6 = *(int *)pKVar6 + -1;
        UNLOCK();
        if (*(int *)pKVar6 == 0) {
          (**(code **)(*(long *)local_68 + 0x20))();
        }
      }
                    /* try { // try from 0066fd4b to 0066fd4f has its CatchHandler @ 0066fed6 */
      lVar8 = QMapNodeBase::nextNode();
      lVar7 = *(long *)(*(long *)param_1 + 8);
    }
    if (local_70 == 0) goto LAB_0066fc12;
  }
  else {
    local_70 = uVar5;
    if ((uVar5 & 1) == 0) {
                    /* try { // try from 0066fe8b to 0066fe8f has its CatchHandler @ 0066fea6 */
      QReadWriteLock::lockForRead();
      local_70 = uVar5 | 1;
      uVar5 = *(ulong *)param_1;
    }
    lVar7 = *(long *)(uVar5 + 8);
    if (*(long *)(*(long *)(uVar5 + 8) + 0x10) != 0) goto LAB_0066fade;
  }
  if ((local_70 & 1) != 0) {
    QReadWriteLock::unlock();
  }
LAB_0066fc12:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



