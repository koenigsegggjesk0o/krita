/* Class KisLayerStyleKnockoutBlower - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLayerStyleKnockoutBlower @ 00204150 ======

void __thiscall
KisLayerStyleKnockoutBlower::KisLayerStyleKnockoutBlower
          (KisLayerStyleKnockoutBlower *this,KisLayerStyleKnockoutBlower *param_1)

{
  (*(code *)PTR_KisLayerStyleKnockoutBlower_00839b78)();
  return;
}



// ====== KisLayerStyleKnockoutBlower @ 00206b30 ======

void __thiscall
KisLayerStyleKnockoutBlower::KisLayerStyleKnockoutBlower(KisLayerStyleKnockoutBlower *this)

{
  (*(code *)PTR_KisLayerStyleKnockoutBlower_0083b068)();
  return;
}



// ====== KisLayerStyleKnockoutBlower @ 00695a20 ======

/* KisLayerStyleKnockoutBlower::KisLayerStyleKnockoutBlower() */

void __thiscall
KisLayerStyleKnockoutBlower::KisLayerStyleKnockoutBlower(KisLayerStyleKnockoutBlower *this)

{
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)this,0);
  *(undefined8 *)(this + 8) = 0;
  return;
}



// ====== KisLayerStyleKnockoutBlower @ 00695a40 ======

/* KisLayerStyleKnockoutBlower::KisLayerStyleKnockoutBlower(KisLayerStyleKnockoutBlower const&) */

void __thiscall
KisLayerStyleKnockoutBlower::KisLayerStyleKnockoutBlower
          (KisLayerStyleKnockoutBlower *this,KisLayerStyleKnockoutBlower *param_1)

{
  KisSelection *this_00;
  
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)this,0);
  if (*(long *)(param_1 + 8) != 0) {
                    /* try { // try from 00695a61 to 00695a65 has its CatchHandler @ 00695aa1 */
    this_00 = (KisSelection *)operator_new(0x20);
                    /* try { // try from 00695a70 to 00695a74 has its CatchHandler @ 00695a95 */
    KisSelection::KisSelection(this_00,*(KisSelection **)(param_1 + 8));
    *(KisSelection **)(this + 8) = this_00;
    LOCK();
    *(int *)(this_00 + 8) = *(int *)(this_00 + 8) + 1;
    UNLOCK();
    return;
  }
  *(undefined8 *)(this + 8) = 0;
  return;
}



