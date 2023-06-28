#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QTestPlayer.h"
#include "player.h"

class QTestPlayer : public QMainWindow
{
    Q_OBJECT

public:
    QTestPlayer(QWidget *parent = nullptr);
    ~QTestPlayer();
    std::shared_ptr<Player> m_player;

public slots:
   void  onMenuOpenFile();
private:
    Ui::QTestPlayerClass ui;
};
