#include <QMessageBox>
#include <thread>
#include "Decoder.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/mem.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"

};


Decoder::Decoder()
{

}

Decoder::~Decoder()
{

}

void Decoder::SetSourcePath(QString& path)
{
	m_path = path;

}

bool Decoder::DecoderInit()
{
	char buf[256];

	AVFormatContext* avFormatContext = avformat_alloc_context();//��ȡ������
	int error;
	//����Ƶ��ַ����ȡ���������(���װ)
	error = avformat_open_input(&avFormatContext, m_path.toStdString().c_str(), NULL, NULL);
	if (error < 0) {
		qDebug("open input fail");
		return false;
	}
	if (error = avformat_find_stream_info(avFormatContext, NULL) < 0) {
		qDebug("��ȡ����ʧ��,err code: %d",error);
		return false;
	}

	//TODO
	av_dump_format(avFormatContext, 0, m_path.toStdString().c_str(), 0);

	//��ȡ��Ƶ�ı�����Ϣ
	AVCodecParameters* origin_par = NULL;
	int mVideoStreamIdx = -1;
	mVideoStreamIdx = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (mVideoStreamIdx < 0) {
		av_log(NULL, AV_LOG_ERROR, "Can't find video stream in input file\n");
		return false;
	}

	origin_par = avFormatContext->streams[mVideoStreamIdx]->codecpar;

	// Ѱ�ҽ����� {start
	AVCodec* mVcodec = NULL;
	AVCodecContext* mAvContext = NULL;
	mVcodec = const_cast<AVCodec*>( avcodec_find_decoder(origin_par->codec_id));
	mAvContext = avcodec_alloc_context3(mVcodec);
	if (!mVcodec || !mAvContext) {
		return false;
	}


	//����ʼ��������context�ᵼ��MP4��װ��mpeg4��������ʧ��
	int ret = avcodec_parameters_to_context(mAvContext, origin_par);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Error initializing the decoder context.\n");
	}

	// �򿪽�����
	if (avcodec_open2(mAvContext, mVcodec, NULL) != 0) {
		qDebug("avcodec_open2 fail");
		return false;
	}


	//ѭ����������
	//����AVPacket
	AVPacket* packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	av_init_packet(packet);
	//����AVFrame
	AVFrame* frame = av_frame_alloc();//����һ��AVFrame�ṹ��,AVFrame�ṹ��һ�����ڴ洢ԭʼ���ݣ�ָ�������ԭʼ֡

	uint8_t* byte_buffer = NULL;

	int byte_buffer_size = av_image_get_buffer_size(mAvContext->pix_fmt, mAvContext->width, mAvContext->height, 32);
	qDebug("width = %d , height = %d ", mAvContext->width, mAvContext->height);
	byte_buffer = (uint8_t*)av_malloc(byte_buffer_size);
	if (!byte_buffer) {
		av_log(NULL, AV_LOG_ERROR, "Can't allocate buffer\n");
		return AVERROR(ENOMEM);
	}

	if (m_CallBacSizeFunc)
	{
		m_CallBacSizeFunc(mAvContext->width, mAvContext->height);
	}
	static int count1 = 0;
	while (1)
	{
		int ret = av_read_frame(avFormatContext, packet);
		if (ret != 0) {
			av_strerror(ret, buf, sizeof(buf));
			qDebug("--%s--\n", buf);
			av_packet_unref(packet);
			break;
		}

		if (ret >= 0 && packet->stream_index != mVideoStreamIdx) {
			av_packet_unref(packet);
			continue;
		}


		{
			// ���ʹ������
			int result = avcodec_send_packet(mAvContext, packet);
			av_packet_unref(packet);
			if (result < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error submitting a packet for decoding\n");
				continue;
			}

			// ���ս�������
			while (result >= 0) {
				result = avcodec_receive_frame(mAvContext, frame);
				if (result == AVERROR_EOF)
					break;
				else if (result == AVERROR(EAGAIN)) {
					result = 0;
					break;
				}
				else if (result < 0) {
					av_log(NULL, AV_LOG_ERROR, "Error decoding frame\n");
					av_frame_unref(frame);
					break;
				}

				int number_of_written_bytes = av_image_copy_to_buffer(byte_buffer, byte_buffer_size,
					(const uint8_t* const*)frame->data, (const int*)frame->linesize,
					mAvContext->pix_fmt, mAvContext->width, mAvContext->height, 1);
				if (number_of_written_bytes < 0) {
					av_log(NULL, AV_LOG_ERROR, "Can't copy image to buffer\n");
					av_frame_unref(frame);
					continue;
				}

			
				if (m_CallBackFuc)
				{
					m_CallBackFuc(byte_buffer_size, byte_buffer);
					std::this_thread::sleep_for(std::chrono::milliseconds(41));
				}

				// д�ļ�������Ƶ����
				//fwrite(byte_buffer, number_of_written_bytes, 1, fp_YUV);
				//fflush(fp_YUV);
				//static  long s_size = 0;
				//if (s_size != number_of_written_bytes)
				//{
				//	s_size = number_of_written_bytes;
				//}
				av_frame_unref(frame);
			}
		}

	}

	av_free(byte_buffer);
	av_frame_free(&frame);
	avcodec_close(mAvContext);
	avformat_free_context(avFormatContext);

	return true;
}

void Decoder::registerBufferPointFunction(callBackPtr ptr)
{
	m_CallBackFuc = ptr;
}

void Decoder::registerSetSizeFunc(callBackSizePtr ptr)
{
	m_CallBacSizeFunc = ptr;
}

