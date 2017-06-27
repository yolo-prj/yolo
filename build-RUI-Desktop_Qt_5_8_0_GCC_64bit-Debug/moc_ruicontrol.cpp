/****************************************************************************
** Meta object code from reading C++ file 'ruicontrol.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../RUI/ruicontrol.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ruicontrol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Controller_t {
    QByteArrayData data[28];
    char stringdata0[447];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller_t qt_meta_stringdata_Controller = {
    {
QT_MOC_LITERAL(0, 0, 10), // "Controller"
QT_MOC_LITERAL(1, 11, 16), // "RobotModeHandler"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 4), // "mode"
QT_MOC_LITERAL(4, 34, 17), // "RobotErrorHandler"
QT_MOC_LITERAL(5, 52, 5), // "error"
QT_MOC_LITERAL(6, 58, 22), // "RobotConnectionHandler"
QT_MOC_LITERAL(7, 81, 6), // "status"
QT_MOC_LITERAL(8, 88, 21), // "RobotDebugInfoHandler"
QT_MOC_LITERAL(9, 110, 4), // "diag"
QT_MOC_LITERAL(10, 115, 32), // "RobotInvalidDisconnectionHandler"
QT_MOC_LITERAL(11, 148, 16), // "on_start_toggled"
QT_MOC_LITERAL(12, 165, 7), // "checked"
QT_MOC_LITERAL(13, 173, 13), // "on_go_pressed"
QT_MOC_LITERAL(14, 187, 14), // "on_go_released"
QT_MOC_LITERAL(15, 202, 15), // "on_back_pressed"
QT_MOC_LITERAL(16, 218, 16), // "on_back_released"
QT_MOC_LITERAL(17, 235, 16), // "on_right_pressed"
QT_MOC_LITERAL(18, 252, 17), // "on_right_released"
QT_MOC_LITERAL(19, 270, 15), // "on_left_pressed"
QT_MOC_LITERAL(20, 286, 16), // "on_left_released"
QT_MOC_LITERAL(21, 303, 16), // "on_uturn_clicked"
QT_MOC_LITERAL(22, 320, 31), // "on_horizontalSlider_sliderMoved"
QT_MOC_LITERAL(23, 352, 8), // "position"
QT_MOC_LITERAL(24, 361, 33), // "on_horizontalSlider_2_sliderM..."
QT_MOC_LITERAL(25, 395, 17), // "on_auto_2_clicked"
QT_MOC_LITERAL(26, 413, 17), // "on_manual_clicked"
QT_MOC_LITERAL(27, 431, 15) // "on_send_clicked"

    },
    "Controller\0RobotModeHandler\0\0mode\0"
    "RobotErrorHandler\0error\0RobotConnectionHandler\0"
    "status\0RobotDebugInfoHandler\0diag\0"
    "RobotInvalidDisconnectionHandler\0"
    "on_start_toggled\0checked\0on_go_pressed\0"
    "on_go_released\0on_back_pressed\0"
    "on_back_released\0on_right_pressed\0"
    "on_right_released\0on_left_pressed\0"
    "on_left_released\0on_uturn_clicked\0"
    "on_horizontalSlider_sliderMoved\0"
    "position\0on_horizontalSlider_2_sliderMoved\0"
    "on_auto_2_clicked\0on_manual_clicked\0"
    "on_send_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  114,    2, 0x0a /* Public */,
       4,    1,  117,    2, 0x0a /* Public */,
       6,    1,  120,    2, 0x0a /* Public */,
       8,    1,  123,    2, 0x0a /* Public */,
      10,    0,  126,    2, 0x0a /* Public */,
      11,    1,  127,    2, 0x08 /* Private */,
      13,    0,  130,    2, 0x08 /* Private */,
      14,    0,  131,    2, 0x08 /* Private */,
      15,    0,  132,    2, 0x08 /* Private */,
      16,    0,  133,    2, 0x08 /* Private */,
      17,    0,  134,    2, 0x08 /* Private */,
      18,    0,  135,    2, 0x08 /* Private */,
      19,    0,  136,    2, 0x08 /* Private */,
      20,    0,  137,    2, 0x08 /* Private */,
      21,    0,  138,    2, 0x08 /* Private */,
      22,    1,  139,    2, 0x08 /* Private */,
      24,    1,  142,    2, 0x08 /* Private */,
      25,    1,  145,    2, 0x08 /* Private */,
      26,    1,  148,    2, 0x08 /* Private */,
      27,    0,  151,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void,

       0        // eod
};

void Controller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Controller *_t = static_cast<Controller *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->RobotModeHandler((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->RobotErrorHandler((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->RobotConnectionHandler((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->RobotDebugInfoHandler((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->RobotInvalidDisconnectionHandler(); break;
        case 5: _t->on_start_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_go_pressed(); break;
        case 7: _t->on_go_released(); break;
        case 8: _t->on_back_pressed(); break;
        case 9: _t->on_back_released(); break;
        case 10: _t->on_right_pressed(); break;
        case 11: _t->on_right_released(); break;
        case 12: _t->on_left_pressed(); break;
        case 13: _t->on_left_released(); break;
        case 14: _t->on_uturn_clicked(); break;
        case 15: _t->on_horizontalSlider_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->on_horizontalSlider_2_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->on_auto_2_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->on_manual_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: _t->on_send_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject Controller::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Controller.data,
      qt_meta_data_Controller,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Controller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Controller.stringdata0))
        return static_cast<void*>(const_cast< Controller*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Controller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
