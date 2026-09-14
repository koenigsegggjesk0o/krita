/* Class KisUpdaterContext - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisUpdaterContext @ 00203da0 ======

void __thiscall
KisUpdaterContext::KisUpdaterContext
          (KisUpdaterContext *this,int param_1,KisUpdateScheduler *param_2)

{
  (*(code *)PTR_KisUpdaterContext_008399a0)();
  return;
}



// ====== KisUpdaterContext @ 00207d70 ======

void __thiscall
KisUpdaterContext::KisUpdaterContext
          (KisUpdaterContext *this,int param_1,KisUpdateScheduler *param_2)

{
  (*(code *)PTR_KisUpdaterContext_0083b988)();
  return;
}



// ====== KisUpdaterContext @ 004e7ea0 ======

/* KisUpdaterContext::KisUpdaterContext(int, KisUpdateScheduler*) */

void __thiscall
KisUpdaterContext::KisUpdaterContext
          (KisUpdaterContext *this,int param_1,KisUpdateScheduler *param_2)

{
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)this,0);
  *(undefined4 *)(this + 0x18) = 0;
  *(undefined (*) [16])(this + 8) = (undefined  [16])0x0;
                    /* try { // try from 004e7ed1 to 004e7ed5 has its CatchHandler @ 004e7f37 */
  QWaitCondition::QWaitCondition((QWaitCondition *)(this + 0x20));
  *(undefined **)(this + 0x28) = PTR_shared_null_008377d0;
                    /* try { // try from 004e7eea to 004e7eee has its CatchHandler @ 004e7f2b */
  QThreadPool::QThreadPool((QThreadPool *)(this + 0x30),(QObject *)0x0);
  *(undefined4 *)(this + 0x40) = 0;
  *(KisUpdateScheduler **)(this + 0x48) = param_2;
  this[0x50] = (KisUpdaterContext)0x0;
  if (param_1 < 1) {
    QThread::idealThreadCount();
  }
                    /* try { // try from 004e7f07 to 004e7f0b has its CatchHandler @ 004e7f43 */
  setThreadsLimit((int)this);
  return;
}



