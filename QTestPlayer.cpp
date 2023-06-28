#include "QTestPlayer.h"
#include <QFileDialog>

QTestPlayer::QTestPlayer(QWidget *parent)
    : QMainWindow(parent)
{ 
    ui.setupUi(this);
    m_player = std::make_shared<Player>(this);
    QRect barRc =  ui.menuBar->geometry();
    m_player->setGeometry(QRect(0, barRc.height(), width(), height() - barRc.height()));
    
    auto ret = connect(ui.openFile, SIGNAL(triggered()), this, SLOT(onMenuOpenFile()));
}

QTestPlayer::~QTestPlayer()
{}

void QTestPlayer::onMenuOpenFile()
{
	m_player->SetSourcePath(QFileDialog::getOpenFileName(0, "","./", tr("Media Files (*.mkv *.mp4);")));

}
