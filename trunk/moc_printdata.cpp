/****************************************************************************
** Meta object code from reading C++ file 'printdata.h'
**
** Created: Wed Jun 3 13:14:25 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "printdata.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'printdata.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PrintData[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      20,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PrintData[] = {
    "PrintData\0\0passed()\0start()\0"
};

const QMetaObject PrintData::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PrintData,
      qt_meta_data_PrintData, 0 }
};

const QMetaObject *PrintData::metaObject() const
{
    return &staticMetaObject;
}

void *PrintData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PrintData))
        return static_cast<void*>(const_cast< PrintData*>(this));
    return QObject::qt_metacast(_clname);
}

int PrintData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: passed(); break;
        case 1: start(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void PrintData::passed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
