#include <iostream>
#include <fstream>
#include <string>

#define PROGRAMOPTIONS_EXCEPTIONS
#include <ProgramOptions.hxx>

#include "Program.h"
#include "Application.h"
#include "Renderer.h"

static const GLchar*
ReadShader( const char* filename )
{
#ifdef WIN32
	FILE* infile;
	fopen_s( &infile, filename, "rb" );
#else
  FILE* infile = fopen( filename, "rb" );
#endif // WIN32

  if ( !infile ) {

#ifndef NDEBUG
    std::cerr << "Unable to open file '" << filename << "'" << std::endl;
#endif // NDEBUG

    return nullptr;
  }

  fseek( infile, 0, SEEK_END );
  int len = ftell( infile );
  fseek( infile, 0, SEEK_SET );

  GLchar* source = new GLchar[len+1];

  fread( source, 1, len, infile );
  fclose( infile );

  source[len] = 0;

  return const_cast<const GLchar*>(source);
}


const char defaultFragShaderSource[] = R"(
void mainImage( out vec4 fragColor, in vec2 fragCoord) {
  vec2 uv = fragCoord / iResolution.xy;
  vec3 col = 0.5 + 0.5*cos( iTime + uv.xyx + vec3(0,2,4) );
  fragColor = vec4( col, 1 );
}
)";

const char videoDefaultFragShaderSource[] = R"(
void mainImage( out vec4 fragColor, in vec2 fragCoord) {
  vec2 uv = fragCoord / iResolution.xy;
  // flip Y
  uv.y = 1.0 - uv.y;
  fragColor = texture( iChannel0, uv );
}
)";

int main(int argc, char **argv /* , char *envp[] */) {
  po::parser parser;
  auto& video = parser["video"]
    .abbreviation( 'V' )
    .description( "Video file name" )
    .type( po::string );

  auto& fragShaderFilename = parser["fs"]
    .description( "Fragment shader file name" )
    .type( po::string );

  auto& texture0 = parser["t0"]
    .description( "texture 0 file name" )
    .type( po::string );
  auto& texture1 = parser["t1"]
    .description( "texture 1 file name" )
    .type( po::string );
  auto& texture2 = parser["t2"]
    .description( "texture 2 file name" )
    .type( po::string );
  auto& texture3 = parser["t3"]
    .description( "texture 3 file name" )
    .type( po::string );

  auto& help = parser["help"]
    .abbreviation( 'h' )
    .description( "Help message" );

  if ( ! parser.parse( argc, argv ) ) {
    return -1;
  }

  if ( help.was_set() ) {
    std::cout << parser << std::endl;
    return 0;
  }

  std::string fragShaderSource( defaultFragShaderSource );

  if ( fragShaderFilename.was_set() ) {
    const char *cSource = ReadShader( fragShaderFilename.get().string.c_str() );
    if ( cSource == nullptr ) {
      return -1;
    }

    fragShaderSource = std::string( cSource );
  } else if ( video.was_set() ) {
    fragShaderSource = std::string( videoDefaultFragShaderSource );
  } else {
    std::cout << parser << std::endl;

    std::cout << "No specify shader or video, use default shader:" << std::endl
              << fragShaderSource << std::endl;
  }

  Application *app = new Application();

  if ( texture0.was_set() ) app->renderer->SetTexture0( texture0.get().string );
  if ( texture1.was_set() ) app->renderer->SetTexture1( texture1.get().string );
  if ( texture2.was_set() ) app->renderer->SetTexture2( texture2.get().string );
  if ( texture3.was_set() ) app->renderer->SetTexture3( texture3.get().string );

  if ( video.was_set() ) app->renderer->decoder = new Decoder( video.get().string );

  app->mainShaderProgram = new Program( fragShaderSource );
  assert(glGetError() == GL_NO_ERROR);

  app->run();

  app->terminate();

  return 0;
}
