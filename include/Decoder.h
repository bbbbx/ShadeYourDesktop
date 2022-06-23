#include <string>

#ifdef __cplusplus
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#endif // __cplusplus

class Decoder
{
private:
  AVFormatContext* pFormatContext;
  AVCodecContext* pCodecContext;
  SwsContext* pSwsContext = nullptr;
  AVPacket* pPacket;
  AVFrame* pFrame;
  AVFrame* pFrameRGBA32;
  int video_stream_index = -1;

  uint8_t *rgbaBuffer;

public:
  int width;
  int height;
  int64_t duration;
  int avg_frame_rate;
  int64_t nb_frames;

  Decoder(const std::string &filename);
  ~Decoder();

  void* GetFrame(float seconds);
};
