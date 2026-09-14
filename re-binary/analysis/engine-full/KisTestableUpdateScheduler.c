/* Class KisTestableUpdateScheduler - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTestableUpdateScheduler @ 004fd220 ======

/* KisTestableUpdateScheduler::KisTestableUpdateScheduler(KisProjectionUpdateListener*, int) */

void __thiscall
KisTestableUpdateScheduler::KisTestableUpdateScheduler
          (KisTestableUpdateScheduler *this,KisProjectionUpdateListener *param_1,int param_2)

{
  undefined *puVar1;
  
  KisUpdateScheduler::KisUpdateScheduler((KisUpdateScheduler *)this);
  puVar1 = PTR_vtable_008379e0 + 0xa0;
  *(undefined **)this = PTR_vtable_008379e0 + 0x10;
  *(undefined **)(this + 0x10) = puVar1;
                    /* try { // try from 004fd250 to 004fd283 has its CatchHandler @ 004fd289 */
  KisUpdateScheduler::updateSettings((KisUpdateScheduler *)this);
  *(KisProjectionUpdateListener **)(*(long *)(this + 0x18) + 200) = param_1;
  KisUpdateScheduler::setThreadsLimit((int)this);
  KisUpdaterContext::setTestingMode((KisUpdaterContext *)(*(long *)(this + 0x18) + 0x60),true);
  KisUpdateScheduler::connectSignals((KisUpdateScheduler *)this);
  return;
}



