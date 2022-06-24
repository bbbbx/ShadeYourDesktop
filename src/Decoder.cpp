#include <string>
#include <iostream>

#ifdef __cplusplus
extern "C" {
// Must put in extern "C" block, otherwise can't found symbol at linking stage
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/avassert.h>
}
#endif // __cplusplus

#include "Decoder.h"

Decoder::Decoder(const std::string &filename) {
  pFormatContext = avformat_alloc_context();

  avformat_open_input( &pFormatContext, filename.c_str(), nullptr, nullptr );

  // printf("Format: %s, duration: %lld μs, #streams: %d\n",
  //   pFormatContext->iformat->long_name,
  //   pFormatContext->duration,
  //   pFormatContext->nb_streams);

  duration = pFormatContext->duration;

  const AVCodec *pCodec;
  AVCodecParameters *pCodecParameters;
  // Get all streams
  avformat_find_stream_info( pFormatContext, nullptr );

  // Iterate streams
  for (unsigned int i = 0; i < pFormatContext->nb_streams; i++)
  {
    // Get stream information
    AVStream* stream = pFormatContext->streams[i];
    AVCodecParameters *pLocalCodecParameters = stream->codecpar;
    // Find a registered decoder for this stream
    const AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

    // We only need video stream
    if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
      // printf("Video Codec: resolution %d x %d, frame rate: %d/%d, time_base: %d\n",
      //   pLocalCodecParameters->width,
      //   pLocalCodecParameters->height,
      //   stream->avg_frame_rate.num,
      //   stream->avg_frame_rate.den,
      //   stream->time_base.den);

      if (video_stream_index == -1) {
        video_stream_index = i;
        pCodec = pLocalCodec;
        pCodecParameters = pLocalCodecParameters;

        avg_frame_rate = stream->avg_frame_rate.num / stream->avg_frame_rate.den;
        duration = stream->duration;
        nb_frames = stream->nb_frames;
      }
    } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
      // If you interest audio stream
      // printf("Audio Codec: %d channels, sample rate %d\n",
      //   pLocalCodecParameters->channels,
      //   pLocalCodecParameters->sample_rate);
    }

    // General parameters
    // printf("\t Codec: %s, ID: %d, bit_rate: %lld\n",
    //   pLocalCodec->long_name,
    //   pLocalCodec->id,
    //   pLocalCodecParameters->bit_rate);
  }

  if (video_stream_index == -1) {
    std::cerr << "File '" << filename << "' does not contain a video stream" << std::endl;
    return;
  }

  // Allocate memory for AVCodedContext
  pCodecContext = avcodec_alloc_context3(pCodec);
  if (!pCodecContext) {
    std::cerr << "Failed to allocated memory for AVCodecContext" << std::endl;
    return;
  }

  width = pCodecParameters->width;
  height = pCodecParameters->height;

  // Fill AVCodecContext structure
  if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0) {
    std::cerr << "Failed to copy codec params to codec context" << std::endl;
    return;
  }

  // Open decoder
  if (avcodec_open2(pCodecContext, pCodec, nullptr) < 0) {
    std::cerr << "Failed to open codec through 'avcodec_open2'" << std::endl;
    return;
  }

  pPacket = av_packet_alloc();
  if (!pPacket) {
    std::cerr << "Failed to allocated memory for AVPacket" << std::endl;
    return;
  }

  pFrame = av_frame_alloc();
  pFrameRGBA32 = av_frame_alloc();
  if (!pFrame || !pFrameRGBA32) {
    std::cerr << "Failed to allocated memory for AVFrame" << std::endl;
    return;
  }
  pFrameRGBA32->format = AV_PIX_FMT_RGBA;
  pFrameRGBA32->width = pCodecContext->width;
  pFrameRGBA32->height = pCodecContext->height;

  int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecContext->width, pCodecContext->height, 32);
  rgbaBuffer = (uint8_t*)av_malloc(numBytes);
  av_image_fill_arrays(pFrameRGBA32->data, pFrameRGBA32->linesize, rgbaBuffer,
    AV_PIX_FMT_RGBA, pCodecContext->width, pCodecContext->height, 32);
}

/**
 * @brief 
 * 
 * @param pCodecContext 
 * @param pInPacket 
 * @param pOutFrame 
 * @return int If  0, got a frame, 
 */
int DecodePacketToFrame(
  AVCodecContext* pCodecContext,
  AVPacket*       pInPacket,
  AVFrame*        pOutFrame) {
  // 0 on success, otherwise negative error code:
  // AVERROR(EAGAIN): input is not accepted in the current state - user must read output with avcodec_receive_frame()
  // (once all output is read, the packet should be resent, and the call will not fail with EAGAIN).
  // AVERROR_EOF: the decoder has been flushed, and no new packets can be sent to it (also returned if more than 1 flush packet is sent)
  // AVERROR(EINVAL): codec not opened, it is an encoder, or requires flush
  // AVERROR(ENOMEM): failed to add packet to internal queue, or similar other errors: legitimate decoding errors
  int res = avcodec_send_packet(pCodecContext, pInPacket);
  if ( res == AVERROR(EAGAIN) ) {
    // std::cout << "Input packet is not acceptable, need to read output frame firstly" << std::endl;
    return res;
  } else if (res < 0) {
    // printf("<DecodePacketToFrame> [ERROR] fail to avcodec_send_frame(), res=%d\n", res);
    return res;
  }

  // 0: success, a frame was returned
  // AVERROR(EAGAIN): output is not available in this state - user must try to send new input
  // AVERROR_EOF: the decoder has been fully flushed, and there will be no more output frames
  // AVERROR(EINVAL): codec not opened, or it is an encoder
  // AVERROR_INPUT_CHANGED: current decoded frame has changed parameters with respect to first decoded frame.
  // Applicable when flag AV_CODEC_FLAG_DROPCHANGED is set. other negative values: legitimate decoding errors
  res = avcodec_receive_frame(pCodecContext, pOutFrame);
  if ( res == AVERROR(EAGAIN) ) {
    // std::cout << "Output is not available, need send more packet" << std::endl;
    return res;
  } else if (res < 0) {
    return res;
  }

  return 0;
}

void* Decoder::GetFrame(float seconds)
{
  int ret;

  while ((ret = av_read_frame(pFormatContext, pPacket)) >= 0) {
    if (pPacket->stream_index == video_stream_index) {
      ret = DecodePacketToFrame(pCodecContext, pPacket, pFrame);
      av_packet_unref(pPacket);
      if (ret == 0) {
        // Get a frame
        break;
      } else {
        av_frame_unref(pFrame);
      }
    }
    av_packet_unref(pPacket);
  }

  if (ret == AVERROR_EOF) { // End of file, seek to video beginning
    av_packet_unref(pPacket);
    av_frame_unref(pFrame);
    av_seek_frame(pFormatContext, video_stream_index, 0, AVSEEK_FLAG_FRAME);

    return GetFrame(seconds);
  } else if (ret < 0) {
    // printf("call av_read_frame() failed: %s\n", av_err2str(ret));
    printf("call av_read_frame() failed: %d\n", ret);
    return nullptr;
  }

  if ( pSwsContext == nullptr ) {
    pSwsContext = sws_getContext( pFrame->width, pFrame->height, (AVPixelFormat)pFrame->format,
      pFrameRGBA32->width, pFrameRGBA32->height, AV_PIX_FMT_RGBA,
      SWS_BILINEAR, nullptr, nullptr, nullptr );
  }

  /* int height = */ sws_scale( pSwsContext,
    (const uint8_t* const*)(pFrame->data), pFrame->linesize,
    0, pCodecContext->height,
    pFrameRGBA32->data, pFrameRGBA32->linesize );

  av_frame_unref( pFrame );

  return (void*)rgbaBuffer;
}

Decoder::~Decoder()
{
  avformat_close_input( &pFormatContext );
  av_packet_free( &pPacket );
  av_frame_free( &pFrame );
  avcodec_free_context( &pCodecContext );
}
