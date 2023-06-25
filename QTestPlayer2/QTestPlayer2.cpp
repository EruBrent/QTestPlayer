#include "QTestPlayer2.h"
#include <QFileDialog>

QTestPlayer2::QTestPlayer2(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.openFile, SIGNAL(triggered()), this, SLOT(onMenuOpenFile()));
}

QTestPlayer2::~QTestPlayer2()
{}


void QTestPlayer2::onMenuOpenFile()
{
    QFileDialog::getOpenFileName(0, "", "./", tr("Media Files (*.mkv *.mp4);"));
	//m_player->SetSourcePath(QFileDialog::getOpenFileName(0, "", "./", tr("Media Files (*.mkv *.mp4);")));

}