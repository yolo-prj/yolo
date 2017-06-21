#ifndef RUICONTROL_H
#define RUICONTROL_H

#include <QMainWindow>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ruimodel.h"
#include <QString>
#include <QObject>
#include "robotmode.h"

namespace Ui {
class Controller;
}

class Controller : public QMainWindow
{
    Q_OBJECT

public:
    explicit Controller(QWidget *parent = 0);
    ~Controller();

public slots:
    void RobotStatusHandler(RobotMode a);

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void on_start_toggled(bool checked);

    void on_go_pressed();

    void on_back_pressed();

    void on_right_pressed();

    void on_left_pressed();

    void on_uturn_clicked();

    void on_horizontalSlider_sliderMoved();

    void on_verticalSlider_sliderMoved();

    void on_auto_2_clicked();

    void on_manual_clicked();

    void on_send_clicked();

private:
    Ui::Controller *ui;
    RUIModel *m;
};

#endif // RUICONTROL_H
