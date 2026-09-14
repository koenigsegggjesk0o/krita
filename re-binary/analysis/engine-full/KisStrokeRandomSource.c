/* Class KisStrokeRandomSource - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisStrokeRandomSource @ 003367e0 ======

/* KisStrokeRandomSource::KisStrokeRandomSource() */

void __thiscall KisStrokeRandomSource::KisStrokeRandomSource(KisStrokeRandomSource *this)

{
  undefined4 *puVar1;
  KisRandomSource *pKVar2;
  KisPerStrokeRandomSource *pKVar3;
  
  puVar1 = (undefined4 *)operator_new(0x28);
  *puVar1 = 0;
                    /* try { // try from 00336803 to 00336807 has its CatchHandler @ 00336887 */
  pKVar2 = (KisRandomSource *)operator_new(0x18);
                    /* try { // try from 0033680e to 00336812 has its CatchHandler @ 003368cf */
  KisRandomSource::KisRandomSource(pKVar2);
  *(KisRandomSource **)(puVar1 + 2) = pKVar2;
  LOCK();
  *(int *)pKVar2 = *(int *)pKVar2 + 1;
  UNLOCK();
                    /* try { // try from 00336821 to 00336825 has its CatchHandler @ 003368c3 */
  pKVar2 = (KisRandomSource *)operator_new(0x18);
                    /* try { // try from 00336830 to 00336834 has its CatchHandler @ 003368db */
  KisRandomSource::KisRandomSource(pKVar2,*(KisRandomSource **)(puVar1 + 2));
  *(KisRandomSource **)(puVar1 + 4) = pKVar2;
  LOCK();
  *(int *)pKVar2 = *(int *)pKVar2 + 1;
  UNLOCK();
                    /* try { // try from 00336843 to 00336847 has its CatchHandler @ 003368b7 */
  pKVar3 = (KisPerStrokeRandomSource *)operator_new(0x18);
                    /* try { // try from 0033684e to 00336852 has its CatchHandler @ 003368ab */
  KisPerStrokeRandomSource::KisPerStrokeRandomSource(pKVar3);
  *(KisPerStrokeRandomSource **)(puVar1 + 6) = pKVar3;
  LOCK();
  *(int *)pKVar3 = *(int *)pKVar3 + 1;
  UNLOCK();
                    /* try { // try from 00336861 to 00336865 has its CatchHandler @ 0033689f */
  pKVar3 = (KisPerStrokeRandomSource *)operator_new(0x18);
                    /* try { // try from 00336870 to 00336874 has its CatchHandler @ 00336893 */
  KisPerStrokeRandomSource::KisPerStrokeRandomSource
            (pKVar3,*(KisPerStrokeRandomSource **)(puVar1 + 6));
  *(KisPerStrokeRandomSource **)(puVar1 + 8) = pKVar3;
  LOCK();
  *(int *)pKVar3 = *(int *)pKVar3 + 1;
  UNLOCK();
  *(undefined4 **)this = puVar1;
  return;
}



// ====== KisStrokeRandomSource @ 003368f0 ======

/* KisStrokeRandomSource::KisStrokeRandomSource(KisStrokeRandomSource const&) */

void __thiscall
KisStrokeRandomSource::KisStrokeRandomSource
          (KisStrokeRandomSource *this,KisStrokeRandomSource *param_1)

{
  undefined4 *puVar1;
  int *piVar2;
  undefined4 *puVar3;
  
  puVar3 = (undefined4 *)operator_new(0x28);
  puVar1 = *(undefined4 **)param_1;
  *puVar3 = *puVar1;
  piVar2 = *(int **)(puVar1 + 2);
  *(int **)(puVar3 + 2) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  piVar2 = *(int **)(puVar1 + 4);
  *(int **)(puVar3 + 4) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  piVar2 = *(int **)(puVar1 + 6);
  *(int **)(puVar3 + 6) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  piVar2 = *(int **)(puVar1 + 8);
  *(int **)(puVar3 + 8) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  *(undefined4 **)this = puVar3;
  return;
}



