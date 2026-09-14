/* Class KisMemoryWindow - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMemoryWindow @ 0020d310 ======

void __thiscall
KisMemoryWindow::KisMemoryWindow(KisMemoryWindow *this,QString *param_1,ulonglong param_2)

{
  (*(code *)PTR_KisMemoryWindow_0083e458)();
  return;
}



// ====== KisMemoryWindow @ 003085e0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisMemoryWindow::KisMemoryWindow(QString const&, unsigned long long) */

void __thiscall
KisMemoryWindow::KisMemoryWindow(KisMemoryWindow *this,QString *param_1,ulonglong param_2)

{
  QTextStream QVar1;
  long lVar2;
  size_t __n;
  undefined8 uVar3;
  undefined8 uVar4;
  QTextStream *this_00;
  char cVar5;
  KisMemoryWindow KVar6;
  QArrayData *pQVar7;
  int iVar8;
  QArrayData *__dest;
  long in_FS_OFFSET;
  QDir aQStack_88 [8];
  QArrayData *local_80;
  QTextStream *local_78;
  QArrayData *local_70;
  QArrayData *local_68;
  undefined local_60 [16];
  char *local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  QTemporaryFile::QTemporaryFile((QTemporaryFile *)this);
  uVar4 = DAT_00721778;
  uVar3 = _DAT_00721770;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined8 *)(this + 0x38) = 0;
  *(ulonglong *)(this + 0x30) = param_2 >> 2;
  lVar2 = *(long *)param_1;
  *(undefined8 *)(this + 0x40) = 0xffffffffffffffff;
  iVar8 = *(int *)(lVar2 + 4);
  *(undefined8 *)(this + 0x48) = 0;
  *(ulonglong *)(this + 0x50) = param_2;
  this[0x10] = (KisMemoryWindow)0x1;
  *(undefined8 *)(this + 0x18) = uVar3;
  *(undefined8 *)(this + 0x20) = uVar4;
  if (iVar8 == 0) {
                    /* try { // try from 0030866d to 0030867f has its CatchHandler @ 0030892e */
    kis_safe_assert_recoverable
              ("!swapDir.isEmpty()",
               "/builds/graphics/krita/libs/image/tiles3/swap/kis_memory_window.cpp",0x18);
  }
  QDir::QDir(aQStack_88,param_1);
                    /* try { // try from 00308683 to 003086b7 has its CatchHandler @ 00308922 */
  cVar5 = QDir::exists();
  if (cVar5 == '\0') {
    KVar6 = (KisMemoryWindow)QDir::mkpath((QString *)aQStack_88);
    this[0x10] = KVar6;
  }
  iVar8 = *(int *)(*(long *)param_1 + 4) + 0x17;
  QString::QString((QString *)&local_80,iVar8,0);
  lVar2 = *(long *)param_1;
  __dest = local_80 + *(long *)(local_80 + 0x10);
  __n = (long)*(int *)(lVar2 + 4) * 2;
  local_68 = __dest;
  memcpy(__dest,(void *)(lVar2 + *(long *)(lVar2 + 0x10)),__n);
  pQVar7 = local_68 + __n;
  local_68 = pQVar7 + 2;
  *(undefined2 *)pQVar7 = 0x2f;
  QAbstractConcatenable::convertFromAscii("KRITA_SWAP_FILE_XXXXXX",0x16,(QChar **)&local_68);
  if ((long)iVar8 == (long)local_68 - (long)__dest >> 1) {
    if (this[0x10] == (KisMemoryWindow)0x0) goto LAB_00308731;
LAB_0030887a:
                    /* try { // try from 00308880 to 00308891 has its CatchHandler @ 0030895e */
    QTemporaryFile::setFileTemplate((QString *)this);
    cVar5 = QTemporaryFile::open((QFlags)this);
    if (cVar5 == '\0') {
LAB_00308896:
      this[0x10] = (KisMemoryWindow)0x0;
      goto LAB_00308731;
    }
                    /* try { // try from 003088d6 to 003088da has its CatchHandler @ 0030895e */
    QTemporaryFile::fileName();
    iVar8 = *(int *)(local_68 + 4);
    if (*(int *)local_68 == 0) {
LAB_003088fa:
      QArrayData::deallocate(local_68,2,8);
    }
    else if (*(int *)local_68 != -1) {
      LOCK();
      *(int *)local_68 = *(int *)local_68 + -1;
      UNLOCK();
      if (*(int *)local_68 == 0) goto LAB_003088fa;
    }
    if (iVar8 == 0) goto LAB_00308896;
    if (this[0x10] == (KisMemoryWindow)0x0) goto LAB_00308731;
  }
  else {
                    /* try { // try from 0030886b to 0030886f has its CatchHandler @ 00308946 */
    QString::resize((int)(QString *)&local_80);
    if (this[0x10] != (KisMemoryWindow)0x0) goto LAB_0030887a;
LAB_00308731:
    local_50 = "default";
    local_68 = (QArrayData *)0x2;
    local_60 = (undefined  [16])0x0;
                    /* try { // try from 0030875a to 0030875e has its CatchHandler @ 0030895e */
    QMessageLogger::warning();
    this_00 = local_78;
                    /* try { // try from 00308778 to 0030877c has its CatchHandler @ 00308952 */
    QString::fromUtf8_helper((char *)&local_70,0x722530);
                    /* try { // try from 00308783 to 00308787 has its CatchHandler @ 0030893a */
    QTextStream::operator<<(this_00,(QString *)&local_70);
    if (*(int *)local_70 == 0) {
LAB_00308838:
      QArrayData::deallocate(local_70,2,8);
      QVar1 = local_78[0x20];
    }
    else {
      if (*(int *)local_70 != -1) {
        LOCK();
        *(int *)local_70 = *(int *)local_70 + -1;
        UNLOCK();
        if (*(int *)local_70 == 0) goto LAB_00308838;
      }
      QVar1 = local_78[0x20];
    }
    if (QVar1 != (QTextStream)0x0) {
      QTextStream::operator<<(local_78,' ');
    }
                    /* try { // try from 003087c9 to 0030885f has its CatchHandler @ 00308952 */
    QDebug::putString((QChar *)&local_78,(ulong)(local_80 + *(long *)(local_80 + 0x10)));
    if (local_78[0x20] != (QTextStream)0x0) {
                    /* try { // try from 003088bd to 003088c1 has its CatchHandler @ 00308952 */
      QTextStream::operator<<(local_78,' ');
    }
    QDebug::~QDebug((QDebug *)&local_78);
  }
  if (*(int *)local_80 != 0) {
    if (*(int *)local_80 == -1) goto LAB_00308808;
    LOCK();
    *(int *)local_80 = *(int *)local_80 + -1;
    UNLOCK();
    if (*(int *)local_80 != 0) goto LAB_00308808;
  }
  QArrayData::deallocate(local_80,2,8);
LAB_00308808:
  QDir::~QDir(aQStack_88);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



