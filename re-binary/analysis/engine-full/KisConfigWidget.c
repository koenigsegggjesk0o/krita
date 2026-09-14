/* Class KisConfigWidget - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisConfigWidget @ 00200c50 ======

void __thiscall
KisConfigWidget::KisConfigWidget(KisConfigWidget *this,QWidget *param_1,QFlags param_2,int param_3)

{
  (*(code *)PTR_KisConfigWidget_008380f8)();
  return;
}



// ====== KisConfigWidget @ 00471400 ======

/* KisConfigWidget::KisConfigWidget(QWidget*, QFlags<Qt::WindowType>, int) */

void __thiscall
KisConfigWidget::KisConfigWidget(KisConfigWidget *this,QWidget *param_1,QFlags param_2,int param_3)

{
  undefined *puVar1;
  long in_FS_OFFSET;
  QObject aQStack_38 [8];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  QWidget::QWidget((QWidget *)this,param_1,param_2);
  puVar1 = PTR_vtable_00837c58 + 0x1e8;
  *(undefined **)this = PTR_vtable_00837c58 + 0x10;
  *(undefined **)(this + 0x10) = puVar1;
                    /* try { // try from 00471450 to 00471454 has its CatchHandler @ 004714dd */
  KisSignalCompressor::KisSignalCompressor
            ((KisSignalCompressor *)(this + 0x30),param_3,2,(QObject *)0x0);
  *(undefined (*) [16])(this + 0x98) = (undefined  [16])0x0;
                    /* try { // try from 0047147d to 004714a8 has its CatchHandler @ 004714d1 */
  QObject::connect(aQStack_38,(char *)this,(QObject *)"2sigConfigurationItemChanged()",(char *)this,
                   0x72df3b);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  QObject::connect(aQStack_38,(char *)(this + 0x30),(QObject *)"2timeout()",(char *)this,0x72df50);
  QMetaObject::Connection::~Connection((Connection *)aQStack_38);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



