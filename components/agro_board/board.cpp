#include "agro/mesh.hpp"
#include "init_io.hpp"
#include "net/net.hpp"

namespace Agro{
namespace Board{

void init() noexcept
{
	Agro::init();

	init_net();
	init_io();
}

}//Board
}//Agro
