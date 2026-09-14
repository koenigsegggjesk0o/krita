/* Class KisBatchNodeUpdate - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisBatchNodeUpdate @ 0046cd40 ======

/* KisBatchNodeUpdate::KisBatchNodeUpdate(std::vector<std::pair<KisSharedPtr<KisNode>, QRect>,
   std::allocator<std::pair<KisSharedPtr<KisNode>, QRect> > > const&) */

KisBatchNodeUpdate * __thiscall
KisBatchNodeUpdate::KisBatchNodeUpdate(KisBatchNodeUpdate *this,vector *param_1)

{
  QTextStream QVar1;
  long *plVar2;
  long *plVar3;
  long lVar4;
  KisBatchNodeUpdate *pKVar5;
  KisBatchNodeUpdate *pKVar6;
  long *plVar7;
  long *plVar8;
  long *extraout_RDX;
  ulong uVar9;
  long lVar10;
  vector *pvVar11;
  QTextStream *pQVar12;
  long in_FS_OFFSET;
  QTextStream *pQStack_78;
  QTextStream *pQStack_70;
  QRect aQStack_68 [8];
  QArrayData *pQStack_60;
  long lStack_58;
  ulong uStack_48;
  KisBatchNodeUpdate *pKStack_40;
  vector *pvStack_38;
  
  uVar9 = *(long *)(param_1 + 8) - *(long *)param_1;
  *(undefined8 *)(this + 0x10) = 0;
  *(undefined (*) [16])this = (undefined  [16])0x0;
  if (uVar9 == 0) {
    uVar9 = 0;
    pKVar5 = (KisBatchNodeUpdate *)0x0;
LAB_0046cd85:
    *(KisBatchNodeUpdate **)(this + 0x10) = pKVar5 + uVar9;
    *(KisBatchNodeUpdate **)this = pKVar5;
    *(KisBatchNodeUpdate **)(this + 8) = pKVar5;
    plVar2 = *(long **)(param_1 + 8);
    plVar3 = *(long **)param_1;
    pKVar6 = pKVar5;
    plVar7 = plVar3;
    if (plVar2 != plVar3) {
      do {
        while( true ) {
          lVar10 = *plVar7;
          *(long *)pKVar6 = lVar10;
          if (lVar10 == 0) break;
          LOCK();
          *(int *)(lVar10 + 0x10) = *(int *)(lVar10 + 0x10) + 1;
          UNLOCK();
          lVar10 = plVar7[2];
          plVar8 = plVar7 + 3;
          *(long *)(pKVar6 + 8) = plVar7[1];
          *(long *)(pKVar6 + 0x10) = lVar10;
          pKVar6 = pKVar6 + 0x18;
          plVar7 = plVar8;
          if (plVar2 == plVar8) goto LAB_0046cdd6;
        }
        lVar10 = plVar7[2];
        plVar8 = plVar7 + 3;
        *(long *)(pKVar6 + 8) = plVar7[1];
        *(long *)(pKVar6 + 0x10) = lVar10;
        pKVar6 = pKVar6 + 0x18;
        plVar7 = plVar8;
      } while (plVar2 != plVar8);
LAB_0046cdd6:
      pKVar5 = pKVar5 + ((long)plVar2 + (-0x18 - (long)plVar3) & 0xfffffffffffffff8U) + 0x18;
    }
    *(KisBatchNodeUpdate **)(this + 8) = pKVar5;
    return pKVar5;
  }
  if (uVar9 < 0x7ffffffffffffff9) {
    pKVar5 = (KisBatchNodeUpdate *)operator_new(uVar9);
    goto LAB_0046cd85;
  }
  pvVar11 = param_1;
  pKVar5 = this;
  std::__throw_bad_array_new_length();
  pQVar12 = *(QTextStream **)pvVar11;
  lStack_58 = *(long *)(in_FS_OFFSET + 0x28);
  pQVar12[0x20] = (QTextStream)0x0;
  uStack_48 = uVar9;
  pKStack_40 = this;
  pvStack_38 = param_1;
  QString::fromUtf8_helper((char *)&pQStack_60,0x72def1);
  QTextStream::operator<<(pQVar12,(QString *)&pQStack_60);
  if (*(int *)pQStack_60 == 0) {
LAB_0046d0b0:
    QArrayData::deallocate(pQStack_60,2,8);
    pQVar12 = *(QTextStream **)pvVar11;
    QVar1 = pQVar12[0x20];
  }
  else {
    if (*(int *)pQStack_60 != -1) {
      LOCK();
      *(int *)pQStack_60 = *(int *)pQStack_60 + -1;
      UNLOCK();
      if (*(int *)pQStack_60 == 0) goto LAB_0046d0b0;
    }
    pQVar12 = *(QTextStream **)pvVar11;
    QVar1 = pQVar12[0x20];
  }
  if (QVar1 != (QTextStream)0x0) {
    QTextStream::operator<<(pQVar12,' ');
    pQVar12 = *(QTextStream **)pvVar11;
  }
  lVar10 = *extraout_RDX;
  if (lVar10 != extraout_RDX[1]) {
    do {
      pQVar12[0x20] = (QTextStream)0x0;
      *(int *)(pQVar12 + 0x18) = *(int *)(pQVar12 + 0x18) + 1;
      pQStack_78 = pQVar12;
      ::operator<<((QDebug)(QDebug *)&pQStack_70,(QObject *)&pQStack_78);
      pQVar12 = pQStack_70;
      QString::fromUtf8_helper((char *)&pQStack_60,0x72df06);
      QTextStream::operator<<(pQVar12,(QString *)&pQStack_60);
      if (*(int *)pQStack_60 == 0) {
LAB_0046d040:
        QArrayData::deallocate(pQStack_60,2,8);
        QVar1 = pQStack_70[0x20];
      }
      else {
        if (*(int *)pQStack_60 != -1) {
          LOCK();
          *(int *)pQStack_60 = *(int *)pQStack_60 + -1;
          UNLOCK();
          if (*(int *)pQStack_60 == 0) goto LAB_0046d040;
        }
        QVar1 = pQStack_70[0x20];
      }
      if (QVar1 != (QTextStream)0x0) {
        QTextStream::operator<<(pQStack_70,' ');
      }
      *(int *)(pQStack_70 + 0x18) = *(int *)(pQStack_70 + 0x18) + 1;
      ::operator<<((QDebug)(QDebug *)&pQStack_60,aQStack_68);
      lVar10 = lVar10 + 0x18;
      QDebug::~QDebug((QDebug *)&pQStack_60);
      QDebug::~QDebug((QDebug *)aQStack_68);
      QDebug::~QDebug((QDebug *)&pQStack_70);
      QDebug::~QDebug((QDebug *)&pQStack_78);
      if (extraout_RDX[1] == lVar10) goto LAB_0046cffe;
      pQVar12 = *(QTextStream **)pvVar11;
      pQVar12[0x20] = (QTextStream)0x0;
      QString::fromUtf8_helper((char *)&pQStack_60,0x72df09);
      QTextStream::operator<<(pQVar12,(QString *)&pQStack_60);
      if (*(int *)pQStack_60 == 0) {
LAB_0046d078:
        QArrayData::deallocate(pQStack_60,2,8);
        pQVar12 = *(QTextStream **)pvVar11;
        if (pQVar12[0x20] == (QTextStream)0x0) goto LAB_0046cf31;
LAB_0046d095:
        QTextStream::operator<<(pQVar12,' ');
        lVar4 = extraout_RDX[1];
      }
      else {
        if (*(int *)pQStack_60 != -1) {
          LOCK();
          *(int *)pQStack_60 = *(int *)pQStack_60 + -1;
          UNLOCK();
          if (*(int *)pQStack_60 == 0) goto LAB_0046d078;
        }
        pQVar12 = *(QTextStream **)pvVar11;
        if (pQVar12[0x20] != (QTextStream)0x0) goto LAB_0046d095;
LAB_0046cf31:
        lVar4 = extraout_RDX[1];
      }
      if (lVar10 == lVar4) goto LAB_0046cffe;
      pQVar12 = *(QTextStream **)pvVar11;
    } while( true );
  }
LAB_0046d002:
  *(long *)pvVar11 = 0;
  *(QTextStream **)pKVar5 = pQVar12;
  if (lStack_58 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return pKVar5;
LAB_0046cffe:
  pQVar12 = *(QTextStream **)pvVar11;
  goto LAB_0046d002;
}



