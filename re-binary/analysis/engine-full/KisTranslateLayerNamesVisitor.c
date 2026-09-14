/* Class KisTranslateLayerNamesVisitor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTranslateLayerNamesVisitor @ 0069a590 ======

/* KisTranslateLayerNamesVisitor::KisTranslateLayerNamesVisitor(QMap<QString, QString>) */

void __thiscall
KisTranslateLayerNamesVisitor::KisTranslateLayerNamesVisitor
          (KisTranslateLayerNamesVisitor *this,QMap param_1)

{
  long *plVar1;
  long *plVar2;
  ulong uVar3;
  char cVar4;
  int *piVar5;
  QString *pQVar6;
  QMapNodeBase *pQVar7;
  undefined8 uVar8;
  long lVar9;
  ulong *puVar10;
  undefined4 in_register_00000034;
  long *plVar11;
  QArrayData *pQVar12;
  long lVar13;
  QString *pQVar14;
  long in_FS_OFFSET;
  QMapNodeBase *local_48;
  long local_40;
  
  plVar11 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  *(undefined **)this = PTR_vtable_00836de8 + 0x10;
  piVar5 = (int *)*plVar11;
  if (*piVar5 == 0) {
    lVar9 = QMapDataBase::createData();
    *(long *)(this + 8) = lVar9;
    if (*(long *)(*plVar11 + 0x10) != 0) {
      uVar8 = FUN_0046aa90(*(long *)(*plVar11 + 0x10),lVar9);
      *(undefined8 *)(lVar9 + 0x10) = uVar8;
      puVar10 = *(ulong **)(*(long *)(this + 8) + 0x10);
      *puVar10 = (ulong)((uint)*puVar10 & 3) | *(long *)(this + 8) + 8U;
      QMapDataBase::recalcMostLeftNode();
    }
  }
  else {
    if (*piVar5 != -1) {
      LOCK();
      *piVar5 = *piVar5 + 1;
      UNLOCK();
      piVar5 = (int *)*plVar11;
    }
    *(int **)(this + 8) = piVar5;
  }
                    /* try { // try from 0069a5f1 to 0069a5f5 has its CatchHandler @ 0069ab29 */
  defaultDictionary((KisTranslateLayerNamesVisitor *)&local_48);
  if ((*(long *)(local_48 + 0x10) != 0) &&
     (pQVar7 = *(QMapNodeBase **)(local_48 + 0x20), pQVar7 != local_48 + 8)) {
    do {
      pQVar6 = (QString *)(pQVar7 + 0x18);
      lVar9 = 0;
      lVar13 = *(long *)(*plVar11 + 0x10);
      if (*(long *)(*plVar11 + 0x10) == 0) {
LAB_0069a689:
                    /* try { // try from 0069a68f to 0069a6a7 has its CatchHandler @ 0069ab1d */
        pQVar6 = (QString *)FUN_0069c930(plVar11,pQVar6);
        QString::operator=(pQVar6,(QString *)(pQVar7 + 0x20));
      }
      else {
        do {
          while( true ) {
            pQVar14 = (QString *)(lVar13 + 0x18);
            cVar4 = operator<(pQVar14,pQVar6);
            plVar1 = (long *)(lVar13 + 8);
            plVar2 = (long *)(lVar13 + 0x10);
            if (cVar4 != '\0') break;
            lVar9 = lVar13;
            lVar13 = *plVar1;
            if (*plVar1 == 0) goto LAB_0069a67a;
          }
          lVar13 = *plVar2;
        } while (*plVar2 != 0);
        if (lVar9 == 0) goto LAB_0069a689;
        pQVar14 = (QString *)(lVar9 + 0x18);
LAB_0069a67a:
        cVar4 = operator<(pQVar6,pQVar14);
        if (cVar4 != '\0') goto LAB_0069a689;
      }
      pQVar7 = (QMapNodeBase *)QMapNodeBase::nextNode();
    } while (pQVar7 != local_48 + 8);
  }
  piVar5 = (int *)*plVar11;
  if (*(int **)(this + 8) != piVar5) {
    if (*piVar5 == 0) {
                    /* try { // try from 0069aa3b to 0069aa77 has its CatchHandler @ 0069ab1d */
      lVar9 = QMapDataBase::createData();
      if (*(long *)(*plVar11 + 0x10) != 0) {
        puVar10 = (ulong *)FUN_0046aa90(*(long *)(*plVar11 + 0x10),lVar9);
        uVar3 = *puVar10;
        *(ulong **)(lVar9 + 0x10) = puVar10;
        *puVar10 = (ulong)((uint)uVar3 & 3) | lVar9 + 8U;
        QMapDataBase::recalcMostLeftNode();
      }
    }
    else {
      if (*piVar5 != -1) {
        LOCK();
        *piVar5 = *piVar5 + 1;
        UNLOCK();
      }
      lVar9 = *plVar11;
    }
    pQVar7 = *(QMapNodeBase **)(this + 8);
    *(long *)(this + 8) = lVar9;
    if (*(int *)pQVar7 == 0) {
LAB_0069a8a6:
      lVar9 = *(long *)(pQVar7 + 0x10);
      if (lVar9 != 0) {
        pQVar12 = *(QArrayData **)(lVar9 + 0x18);
        if (*(int *)pQVar12 == 0) {
LAB_0069a8cc:
          QArrayData::deallocate(pQVar12,2,8);
        }
        else if (*(int *)pQVar12 != -1) {
          LOCK();
          *(int *)pQVar12 = *(int *)pQVar12 + -1;
          UNLOCK();
          if (*(int *)pQVar12 == 0) {
            pQVar12 = *(QArrayData **)(lVar9 + 0x18);
            goto LAB_0069a8cc;
          }
        }
        pQVar12 = *(QArrayData **)(lVar9 + 0x20);
        if (*(int *)pQVar12 == 0) {
LAB_0069a8f4:
          QArrayData::deallocate(pQVar12,2,8);
        }
        else if (*(int *)pQVar12 != -1) {
          LOCK();
          *(int *)pQVar12 = *(int *)pQVar12 + -1;
          UNLOCK();
          if (*(int *)pQVar12 == 0) {
            pQVar12 = *(QArrayData **)(lVar9 + 0x20);
            goto LAB_0069a8f4;
          }
        }
        lVar13 = *(long *)(lVar9 + 8);
        if (lVar13 != 0) {
          pQVar12 = *(QArrayData **)(lVar13 + 0x18);
          if (*(int *)pQVar12 == 0) {
LAB_0069aabb:
            QArrayData::deallocate(pQVar12,2,8);
          }
          else if (*(int *)pQVar12 != -1) {
            LOCK();
            *(int *)pQVar12 = *(int *)pQVar12 + -1;
            UNLOCK();
            if (*(int *)pQVar12 == 0) {
              pQVar12 = *(QArrayData **)(lVar13 + 0x18);
              goto LAB_0069aabb;
            }
          }
          pQVar12 = *(QArrayData **)(lVar13 + 0x20);
          if (*(int *)pQVar12 == 0) {
LAB_0069aaec:
            QArrayData::deallocate(pQVar12,2,8);
          }
          else if (*(int *)pQVar12 != -1) {
            LOCK();
            *(int *)pQVar12 = *(int *)pQVar12 + -1;
            UNLOCK();
            if (*(int *)pQVar12 == 0) {
              pQVar12 = *(QArrayData **)(lVar13 + 0x20);
              goto LAB_0069aaec;
            }
          }
          if (*(long *)(lVar13 + 8) != 0) {
            FUN_00469720();
          }
          if (*(long *)(lVar13 + 0x10) != 0) {
            FUN_00469720();
          }
        }
        lVar9 = *(long *)(lVar9 + 0x10);
        if (lVar9 != 0) {
          pQVar12 = *(QArrayData **)(lVar9 + 0x18);
          if (*(int *)pQVar12 == 0) {
LAB_0069aad3:
            QArrayData::deallocate(pQVar12,2,8);
          }
          else if (*(int *)pQVar12 != -1) {
            LOCK();
            *(int *)pQVar12 = *(int *)pQVar12 + -1;
            UNLOCK();
            if (*(int *)pQVar12 == 0) {
              pQVar12 = *(QArrayData **)(lVar9 + 0x18);
              goto LAB_0069aad3;
            }
          }
          pQVar12 = *(QArrayData **)(lVar9 + 0x20);
          if (*(int *)pQVar12 == 0) {
LAB_0069ab04:
            QArrayData::deallocate(pQVar12,2,8);
          }
          else if (*(int *)pQVar12 != -1) {
            LOCK();
            *(int *)pQVar12 = *(int *)pQVar12 + -1;
            UNLOCK();
            if (*(int *)pQVar12 == 0) {
              pQVar12 = *(QArrayData **)(lVar9 + 0x20);
              goto LAB_0069ab04;
            }
          }
          if (*(long *)(lVar9 + 8) != 0) {
            FUN_00469720();
          }
          if (*(long *)(lVar9 + 0x10) != 0) {
            FUN_00469720();
          }
        }
        QMapDataBase::freeTree(pQVar7,(int)*(undefined8 *)(pQVar7 + 0x10));
      }
      QMapDataBase::freeData((QMapDataBase *)pQVar7);
    }
    else if (*(int *)pQVar7 != -1) {
      LOCK();
      *(int *)pQVar7 = *(int *)pQVar7 + -1;
      UNLOCK();
      if (*(int *)pQVar7 == 0) goto LAB_0069a8a6;
    }
  }
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_0069a722;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0069a722;
  }
  lVar9 = *(long *)(local_48 + 0x10);
  if (lVar9 != 0) {
    pQVar12 = *(QArrayData **)(lVar9 + 0x18);
    if (*(int *)pQVar12 == 0) {
LAB_0069aa8a:
      QArrayData::deallocate(pQVar12,2,8);
    }
    else if (*(int *)pQVar12 != -1) {
      LOCK();
      *(int *)pQVar12 = *(int *)pQVar12 + -1;
      UNLOCK();
      if (*(int *)pQVar12 == 0) {
        pQVar12 = *(QArrayData **)(lVar9 + 0x18);
        goto LAB_0069aa8a;
      }
    }
    pQVar12 = *(QArrayData **)(lVar9 + 0x20);
    if (*(int *)pQVar12 == 0) {
LAB_0069aaa2:
      QArrayData::deallocate(pQVar12,2,8);
    }
    else if (*(int *)pQVar12 != -1) {
      LOCK();
      *(int *)pQVar12 = *(int *)pQVar12 + -1;
      UNLOCK();
      if (*(int *)pQVar12 == 0) {
        pQVar12 = *(QArrayData **)(lVar9 + 0x20);
        goto LAB_0069aaa2;
      }
    }
    lVar13 = *(long *)(lVar9 + 8);
    if (lVar13 != 0) {
      pQVar12 = *(QArrayData **)(lVar13 + 0x18);
      if (*(int *)pQVar12 == 0) {
LAB_0069a7bc:
        QArrayData::deallocate(pQVar12,2,8);
      }
      else if (*(int *)pQVar12 != -1) {
        LOCK();
        *(int *)pQVar12 = *(int *)pQVar12 + -1;
        UNLOCK();
        if (*(int *)pQVar12 == 0) {
          pQVar12 = *(QArrayData **)(lVar13 + 0x18);
          goto LAB_0069a7bc;
        }
      }
      pQVar12 = *(QArrayData **)(lVar13 + 0x20);
      if (*(int *)pQVar12 == 0) {
LAB_0069a7e6:
        QArrayData::deallocate(pQVar12,2,8);
      }
      else if (*(int *)pQVar12 != -1) {
        LOCK();
        *(int *)pQVar12 = *(int *)pQVar12 + -1;
        UNLOCK();
        if (*(int *)pQVar12 == 0) {
          pQVar12 = *(QArrayData **)(lVar13 + 0x20);
          goto LAB_0069a7e6;
        }
      }
      if (*(long *)(lVar13 + 8) != 0) {
        FUN_00469720();
      }
      if (*(long *)(lVar13 + 0x10) != 0) {
        FUN_00469720();
      }
    }
    lVar9 = *(long *)(lVar9 + 0x10);
    if (lVar9 != 0) {
      pQVar12 = *(QArrayData **)(lVar9 + 0x18);
      if (*(int *)pQVar12 == 0) {
LAB_0069a835:
        QArrayData::deallocate(pQVar12,2,8);
      }
      else if (*(int *)pQVar12 != -1) {
        LOCK();
        *(int *)pQVar12 = *(int *)pQVar12 + -1;
        UNLOCK();
        if (*(int *)pQVar12 == 0) {
          pQVar12 = *(QArrayData **)(lVar9 + 0x18);
          goto LAB_0069a835;
        }
      }
      pQVar12 = *(QArrayData **)(lVar9 + 0x20);
      if (*(int *)pQVar12 == 0) {
LAB_0069a85d:
        QArrayData::deallocate(pQVar12,2,8);
      }
      else if (*(int *)pQVar12 != -1) {
        LOCK();
        *(int *)pQVar12 = *(int *)pQVar12 + -1;
        UNLOCK();
        if (*(int *)pQVar12 == 0) {
          pQVar12 = *(QArrayData **)(lVar9 + 0x20);
          goto LAB_0069a85d;
        }
      }
      if (*(long *)(lVar9 + 8) != 0) {
        FUN_00469720();
      }
      if (*(long *)(lVar9 + 0x10) != 0) {
        FUN_00469720();
      }
    }
    QMapDataBase::freeTree(local_48,(int)*(undefined8 *)(local_48 + 0x10));
  }
  QMapDataBase::freeData((QMapDataBase *)local_48);
LAB_0069a722:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



