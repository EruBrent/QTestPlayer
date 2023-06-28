#include <QThread>
#include "Player.h"
#include "Decoder.h"
#include "GLRenderer.h"


class PlayerThread
	: public QThread
{
public:
	PlayerThread();
	~PlayerThread();

	virtual void run();

	void SetPlayer(Player* player);
public slots:
	void receiveBuf(size_t len, unsigned char* buf) {};
private:
	Player* m_player;
};

PlayerThread::PlayerThread()
{

}

PlayerThread::~PlayerThread()
{

}

void PlayerThread::run()
{
	
	m_player->Play();
}

void PlayerThread::SetPlayer(Player* player)
{
	m_player = player;
}

Player::Player(QWidget *parent) : QWidget(parent)
{
	m_threadVideo = std::make_shared<PlayerThread>();
	m_decoder = std::make_shared<Decoder>();
	m_renderer = std::make_shared<CGLRenderer>(parent);
	m_renderer->Init();
	m_decoder->registerBufferPointFunction(std::bind(&Player::receiveStream,this,std::placeholders::_1,std::placeholders::_2));
	m_decoder->registerSetSizeFunc(std::bind(&Player::setVideoSize, this, std::placeholders::_1, std::placeholders::_2));
	QObject:connect(this, &Player::warpSetVideoSize, m_renderer.get(), &CGLRenderer::setSize);

	m_isPlaying = false;
}

void Player::SetSourcePath(QString path)
{
	if (path.isEmpty())
		return;
	
	m_path = (path);

	if (m_isPlaying)
	{

		StopPlaying();
	}

	m_threadVideo->SetPlayer(this);
	m_threadVideo->start(QThread::NormalPriority);
}

bool Player::StopPlaying()
{

	return true;
}

void Player::receiveStream(size_t len, unsigned char* buf)
{
	m_renderer->updateYUVBuffer(len, buf);
	emit m_renderer->notifyUpdateVideoData();
	
}

void Player::Play()
{
	m_decoder->SetSourcePath(m_path);
	m_decoder->DecoderInit();
}

void Player::setVideoSize(size_t width, size_t height)
{
	emit warpSetVideoSize(width, height);
}
