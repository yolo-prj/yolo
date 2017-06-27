/****************************************************************************
** Meta object code from reading C++ file 'ruimodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../RUI/ruimodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ruimodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RUIModel_t {
    QByteArrayData data[11];
    char stringdata0[148];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RUIModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RUIModel_t qt_meta_stringdata_RUIModel = {
    {
QT_MOC_LITERAL(0, 0, 8), // "RUIModel"
QT_MOC_LITERAL(1, 9, 15), // "UpdateRobotMode"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 4), // "mode"
QT_MOC_LITERAL(4, 31, 16), // "UpdateRobotError"
QT_MOC_LITERAL(5, 48, 5), // "error"
QT_MOC_LITERAL(6, 54, 27), // "UpdateRobotConnectionStatus"
QT_MOC_LITERAL(7, 82, 6), // "status"
QT_MOC_LITERAL(8, 89, 20), // "UpdateRobotDebugInfo"
QT_MOC_LITERAL(9, 110, 5), // "debug"
QT_MOC_LITERAL(10, 116, 31) // "UpdateRobotInvalidDisconnection"

    },
    "RUIModel\0UpdateRobotMode\0\0mode\0"
    "UpdateRobotError\0error\0"
    "UpdateRobotConnectionStatus\0status\0"
    "UpdateRobotDebugInfo\0debug\0"
    "UpdateRobotInvalidDisconnection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RUIModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,
       6,    1,   45,    2, 0x06 /* Public */,
       8,    1,   48,    2, 0x06 /* Public */,
      10,    0,   51,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,

       0        // eod
};

void RUIModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RUIModel *_t = static_cast<RUIModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->UpdateRobotMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->UpdateRobotError((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->UpdateRobotConnectionStatus((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->UpdateRobotDebugInfo((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->UpdateRobotInvalidDisconnection(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RUIModel::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RUIModel::UpdateRobotMode)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RUIModel::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RUIModel::UpdateRobotError)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RUIModel::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RUIModel::UpdateRobotConnectionStatus)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (RUIModel::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RUIModel::UpdateRobotDebugInfo)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (RUIModel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RUIModel::UpdateRobotInvalidDisconnection)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject RUIModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RUIModel.data,
      qt_meta_data_RUIModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RUIModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RUIModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RUIModel.stringdata0))
        return static_cast<void*>(const_cast< RUIModel*>(this));
    return QObject::qt_metacast(_clname);
}

int RUIModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void RUIModel::UpdateRobotMode(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RUIModel::UpdateRobotError(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RUIModel::UpdateRobotConnectionStatus(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RUIModel::UpdateRobotDebugInfo(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void RUIModel::UpdateRobotInvalidDisconnection()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
