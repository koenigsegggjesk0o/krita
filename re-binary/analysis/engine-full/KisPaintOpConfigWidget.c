/* Class KisPaintOpConfigWidget - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintOpConfigWidget @ 00352c00 ======

/* KisPaintOpConfigWidget::KisPaintOpConfigWidget(QWidget*, QFlags<Qt::WindowType>) */

void __thiscall
KisPaintOpConfigWidget::KisPaintOpConfigWidget
          (KisPaintOpConfigWidget *this,QWidget *param_1,QFlags param_2)

{
  undefined *puVar1;
  
  KisConfigWidget::KisConfigWidget((KisConfigWidget *)this,param_1,param_2,100);
  puVar1 = PTR_vtable_00836fc0;
  *(undefined4 *)(this + 0xd8) = 0;
  *(undefined (*) [16])(this + 0xa8) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0xb8) = (undefined  [16])0x0;
  *(undefined **)this = puVar1 + 0x10;
  *(undefined **)(this + 0x10) = puVar1 + 0x228;
  *(undefined (*) [16])(this + 200) = (undefined  [16])0x0;
  return;
}



