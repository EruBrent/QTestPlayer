#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QTestPlayer2.h"

class QTestPlayer2 : public QMainWindow
{
    Q_OBJECT

public:
    QTestPlayer2(QWidget *parent = nullptr);
    ~QTestPlayer2();
private slots:
    void onMenuOpenFile();
private:
    Ui::QTestPlayer2Class ui;
};
