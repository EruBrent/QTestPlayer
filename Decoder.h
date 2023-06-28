#ifndef DECODER_H
#define DECODER_H
#include <QString>
#include <QOpenGLWidget>

class Decoder
{
public:
	typedef std::function<void(size_t, unsigned char*)> callBackPtr;
	typedef std::function<void(int, int)> callBackSizePtr;
    Decoder();
	~Decoder();
	void SetSourcePath(QString& path);
	bool DecoderInit();
	void registerBufferPointFunction(callBackPtr ptr);
	void registerSetSizeFunc(callBackSizePtr ptr);
Q_SIGNALS:
	void UpdateFrame();
private:
	QString m_path;
	callBackPtr m_CallBackFuc;
	callBackSizePtr m_CallBacSizeFunc;
};

#endif // DECODER_H