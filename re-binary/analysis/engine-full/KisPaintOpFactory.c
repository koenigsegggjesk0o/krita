/* Class KisPaintOpFactory - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintOpFactory @ 00337fb0 ======

/* KisPaintOpFactory::KisPaintOpFactory(QStringList const&) */

void __thiscall KisPaintOpFactory::KisPaintOpFactory(KisPaintOpFactory *this,QStringList *param_1)

{
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)this = PTR_vtable_00837760 + 0x10;
                    /* try { // try from 00337fdc to 00337fe0 has its CatchHandler @ 00337ff0 */
  FUN_003380c0(this + 0x10,param_1);
  *(undefined8 *)(this + 0x18) = 100;
  return;
}



