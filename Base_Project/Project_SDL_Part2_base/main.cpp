#include "Project_SDL2.h"

#include <string>

int main(int argc, char* argv[]) {

  std::cout << "Starting up the application" << std::endl;

  if (argc != 5)
    throw std::runtime_error("Need four arguments - "
                             "number of sheep, number of wolves, "
                             "number of shepherd dogs,"
                             "simulation time\n");

  init();

  std::cout << "Done with initilization" << std::endl;

  application my_app(std::stoul(argv[1]), std::stoul(argv[2]), std::stoul(argv[3]));

  std::cout << "Created window" << std::endl;

  int retval = my_app.loop(std::stoul(argv[4]));

  std::cout << "Exiting application with code " << retval << std::endl;

  SDL_Quit();

  return retval;
}