#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <memory>

class PlayerThread;
class Decoder;
class CGLRenderer;

class Player : public QWidget
{
    Q_OBJECT
public:
    explicit Player(QWidget *parent = nullptr);

public:
  void SetSourcePath(QString path);
  QString& GetSourcePath() const;
  void Play();
  void setVideoSize(size_t width, size_t height);
signals:
	void warpSetVideoSize(size_t, size_t);
private:
	bool StopPlaying();
	void receiveStream(size_t len, unsigned char* buf);


private:
    QString m_path;
	std::shared_ptr<Decoder> m_decoder;
	std::shared_ptr<PlayerThread> m_threadVideo;
	std::shared_ptr<CGLRenderer> m_renderer;
	bool m_isPlaying;
};

#endif // PLAYER_H
