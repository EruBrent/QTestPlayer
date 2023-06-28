#ifndef QLMITATE_VLC_GL_RENDERER_H
#define QLMITATE_VLC_GL_RENDERER_H

//#include <QtOpenGL/QGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_4_5_Core>

#include <QOpenGLWidget>


class CGLRenderer :public QOpenGLWidget
{
	Q_OBJECT
public:
	CGLRenderer(QWidget* parent = 0);
	~CGLRenderer();
	void Init();
	void Render();
	bool DrawFrame();

	void updateYUVBuffer(size_t len, unsigned char* pBuf);
signals:
	void notifyUpdateVideoData();
public slots:
	void setSize(uint width, uint height);
protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	
private:
	QOpenGLShaderProgram* m_program;
	GLuint m_uniformY, m_uniformU, m_uniformV;
	QOpenGLTexture *m_textureY = nullptr, *m_textureU = nullptr, *m_textureV = nullptr;
	GLuint m_idY, m_idU, idV; //自己创建的纹理对象ID，创建错误返回0
	QOpenGLExtraFunctions m_glFunc;
	QOpenGLBuffer m_vbo;
	uint m_videoW = 0, m_videoH = 0;
	uchar*		m_yuvBuf = nullptr;
	size_t		m_bufLength = 0;


};
#endif
