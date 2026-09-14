/* Class KisPostExecutionUndoAdapter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPostExecutionUndoAdapter @ 00204a70 ======

void __thiscall
KisPostExecutionUndoAdapter::KisPostExecutionUndoAdapter
          (KisPostExecutionUndoAdapter *this,KisUndoStore *param_1,KisStrokesFacade *param_2)

{
  (*(code *)PTR_KisPostExecutionUndoAdapter_0083a008)();
  return;
}



// ====== KisPostExecutionUndoAdapter @ 0062ba10 ======

/* KisPostExecutionUndoAdapter::KisPostExecutionUndoAdapter(KisUndoStore*, KisStrokesFacade*) */

void __thiscall
KisPostExecutionUndoAdapter::KisPostExecutionUndoAdapter
          (KisPostExecutionUndoAdapter *this,KisUndoStore *param_1,KisStrokesFacade *param_2)

{
  *(KisUndoStore **)this = param_1;
  *(KisStrokesFacade **)(this + 8) = param_2;
  return;
}



