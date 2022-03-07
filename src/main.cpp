#include "ShaderProgram.hpp"
#include "Application.hpp"


int main(int argc, char **argv, char *envp[]) {
  // std::copy(argv, argv + argc, std::ostream_iterator<char*>(std::cout, "\n"));
  // std::cout << getenv(*envp);

  std::string mainImageFilename("assets/getting_started.glsl");

  if (argc < 2) {
    std::cout << "ShadeYourDesktop: missing input file, using default file " << mainImageFilename << std::endl;
    std::cout << "usage: ShadeYourDesktop <input_file>\n";
  } else {
    mainImageFilename = std::string(argv[1]);
  }

  Application *app = new Application();

  ShaderProgram *shaderProgram = ShaderProgram::loadFromFilename(mainImageFilename);

  app->mainShaderProgram = shaderProgram;

  app->run();

  app->terminate();

  return 0;
}
