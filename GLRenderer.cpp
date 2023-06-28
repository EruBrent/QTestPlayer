#include "GLRenderer.h"
//#include <QtOpenGL/QGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QTimer>
#include <QOpenGLContext>


#define VERTEXIN  0
#define TEXTUREIN 1

CGLRenderer::CGLRenderer(QWidget* parent)
	:QOpenGLWidget(parent)
{

}

CGLRenderer::~CGLRenderer()
{
	
}

void CGLRenderer::Init()
{
	hide();
	QObject::connect(this, SIGNAL(notifyUpdateVideoData()), this, SLOT(update()));
	//move(30, 30);
	//show();
	
	//QString outTmp("C:\\Users\\OYJ\\Downloads\\frame\\frame_900.yuv");
	//FILE* fp_YUV = fopen(outTmp.toStdString().c_str(), "r");
	//int size = 640 * 272 * 3 / 2;
	//m_yuvBuf = new uchar[size];
	//if (fp_YUV)
	//{
	//	fread(m_yuvBuf, size, 1, fp_YUV);
	//	fclose(fp_YUV);
	//	setSize(640, 272);
	//}
// 	QTimer* timer = new QTimer(this);
// 	this->setUpdatesEnabled(true);
// 	connect(timer, SIGNAL(timeout()), this, SLOT(update())); 
// 	timer->start(50);
}

void CGLRenderer::Render()
{
	

}

bool CGLRenderer::DrawFrame()
{
	return false;
}

void CGLRenderer::setSize(uint width, uint height)
{
	m_videoW = width;
	m_videoH = height;
	resize(width, height);
	show();
}

void CGLRenderer::updateYUVBuffer(size_t len, unsigned char* pBuf)
{
	if (m_bufLength != len)
	{
		if (m_yuvBuf)
			delete[] m_yuvBuf;

		m_yuvBuf = new uchar[len];

		m_bufLength = len;
	}
	memcpy(m_yuvBuf, pBuf, len);
}

void CGLRenderer::initializeGL() 
{	

	m_glFunc.initializeOpenGLFunctions();
	m_glFunc.glEnable(GL_DEPTH_TEST);

	static const GLfloat vertices[]{
		//顶点坐标
		-1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, +1.0f,
			+1.0f, -1.0f,
			//纹理坐标
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
	};


	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(vertices, sizeof(vertices));

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	const char *vsrc =
		"attribute vec4 vertexIn; \
		attribute vec2 textureIn; \
		varying vec2 textureOut;  \
		void main(void)           \
		{                         \
		gl_Position = vertexIn; \
		textureOut = textureIn; \
		}";
	vshader->compileSourceCode(vsrc);

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	const char *fsrc = "varying vec2 textureOut; \
					    uniform sampler2D tex_y; \
						uniform sampler2D tex_u; \
						uniform sampler2D tex_v; \
						void main(void) \
						{ \
						vec3 yuv; \
						vec3 rgb; \
						yuv.x = texture2D(tex_y, textureOut).r; \
						yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
						yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
						rgb = mat3( 1.0,       1.0,         1.0, \
						0.0,       -0.39465,  2.03211, \
						1.13983, -0.58060,  0.0) * yuv; \
						gl_FragColor = vec4(rgb, 1.0); \
						}";
	fshader->compileSourceCode(fsrc);

	m_program = new QOpenGLShaderProgram(this);
	m_program->addShader(vshader);
	m_program->addShader(fshader);
	m_program->bindAttributeLocation("vertexIn", VERTEXIN);
	m_program->bindAttributeLocation("textureIn", TEXTUREIN);
	m_program->link();
	m_program->bind();
	m_program->enableAttributeArray(VERTEXIN);
	m_program->enableAttributeArray(TEXTUREIN);
	m_program->setAttributeBuffer(VERTEXIN, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	m_program->setAttributeBuffer(TEXTUREIN, GL_FLOAT, 8 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

	m_uniformY = m_program->uniformLocation("tex_y");
	m_uniformU = m_program->uniformLocation("tex_u");
	m_uniformV = m_program->uniformLocation("tex_v");
	m_textureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
	m_textureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
	m_textureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
	m_textureY->create();
	m_textureU->create();
	m_textureV->create();
	m_idY = m_textureY->textureId();
	m_idU = m_textureU->textureId();
	idV = m_textureV->textureId();
	m_glFunc.glClearColor(0.0, 0.0, 0.0, 0.0);


}

void CGLRenderer::paintGL() 
{
#if 1
	if (!m_yuvBuf) return;
	GLint colorFomat = GL_LUMINANCE;
	m_glFunc.glActiveTexture(GL_TEXTURE0);  //激活纹理单元GL_TEXTURE0,系统里面的

	m_glFunc.glBindTexture(GL_TEXTURE_2D, m_idY); //绑定y分量纹理对象id到激活的纹理单元
	//使用内存中的数据创建真正的y分量纹理数据
	m_glFunc.glTexImage2D(GL_TEXTURE_2D, 0, colorFomat, m_videoW, m_videoH, 0, colorFomat, GL_UNSIGNED_BYTE, m_yuvBuf);
	//https://blog.csdn.net/xipiaoyouzi/article/details/53584798 纹理参数解析
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_glFunc.glActiveTexture(GL_TEXTURE1); //激活纹理单元GL_TEXTURE1
	m_glFunc.glBindTexture(GL_TEXTURE_2D, m_idU);
	//使用内存中的数据创建真正的u分量纹理数据
	m_glFunc.glTexImage2D(GL_TEXTURE_2D, 0, colorFomat, m_videoW >> 1, m_videoH >> 1, 0, colorFomat, GL_UNSIGNED_BYTE, m_yuvBuf + m_videoW * m_videoH);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_glFunc.glActiveTexture(GL_TEXTURE2); //激活纹理单元GL_TEXTURE2
	m_glFunc.glBindTexture(GL_TEXTURE_2D, idV);
	//使用内存中的数据创建真正的v分量纹理数据
	m_glFunc.glTexImage2D(GL_TEXTURE_2D, 0, colorFomat, m_videoW >> 1, m_videoH >> 1, 0, colorFomat, GL_UNSIGNED_BYTE, m_yuvBuf + m_videoW*m_videoH +(m_videoW*m_videoH / 4));
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunc.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//指定y纹理要使用新值
	m_glFunc.glUniform1i(m_uniformY, 0);
	//指定u纹理要使用新值
	m_glFunc.glUniform1i(m_uniformU, 1);
	//指定v纹理要使用新值
	m_glFunc.glUniform1i(m_uniformV, 2);
	//使用顶点数组方式绘制图形
	m_glFunc.glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
#endif

}

void CGLRenderer::resizeGL(int w, int h) 
{

}

