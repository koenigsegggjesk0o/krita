/* Class KisTestableUpdaterContext - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTestableUpdaterContext @ 004e8170 ======

/* KisTestableUpdaterContext::KisTestableUpdaterContext(int) */

void __thiscall
KisTestableUpdaterContext::KisTestableUpdaterContext(KisTestableUpdaterContext *this,int param_1)

{
  KisUpdaterContext::KisUpdaterContext((KisUpdaterContext *)this,param_1,(KisUpdateScheduler *)0x0);
                    /* try { // try from 004e818c to 004e8190 has its CatchHandler @ 004e8198 */
  KisUpdaterContext::setTestingMode((KisUpdaterContext *)this,true);
  return;
}



