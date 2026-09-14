/* Class KisProgressUpdater - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisProgressUpdater @ 0032f020 ======

/* KisProgressUpdater::KisProgressUpdater(KisProgressInterface*, KoProgressProxy*,
   KoProgressUpdater::Mode) */

void __thiscall
KisProgressUpdater::KisProgressUpdater
          (KisProgressUpdater *this,KisProgressInterface *param_1,KoProgressProxy *param_2,
          Mode param_3)

{
  KoProgressUpdater::KoProgressUpdater((KoProgressUpdater *)this,param_2,param_3);
  *(KisProgressInterface **)(this + 0x18) = param_1;
  *(undefined **)this = PTR_vtable_008376c0 + 0x10;
                    /* try { // try from 0032f056 to 0032f058 has its CatchHandler @ 0032f060 */
  (**(code **)(*(long *)param_1 + 0x18))(param_1,this);
  return;
}



