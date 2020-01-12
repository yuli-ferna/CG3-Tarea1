#include "userInterface.h"
#include <iostream>
using namespace std;

extern unsigned int windowWidth, windowHeight;

userInterface::userInterface()
{
	mUserInterface = TwNewBar("Tarea");

	TwDefine("Tarea refresh = '0.0001f'");
	TwDefine("Tarea resizable = true");
	TwDefine("Tarea fontresizable = false");
	TwDefine("Tarea movable = true");
	TwDefine("Tarea visible = true");
	TwDefine("Tarea position = '10 10'");
	TwDefine("Tarea size = '200 300'");
	TwDefine("Tarea color='143 66 244'");

	histograma = false;
	info = false;
	TwEnumVal filters[] = { {NOR, "Normal"}, {BAW, "Blanco y negro" }, {GREY, "Escala de grices"}, {NEG, "Negativo"}, {EDGE_SOL, "Gradiente Solve"},
	{EDGE_PEW, "Gradiente Prewitt"}, {EDGE_ROB, "Gradiente Roberts"}, {PROM, "Promedio"}, {MED, "Mediana"}, {LAP, "Laplaciano"},
	{TOON, "Toon Shader" }};
	filter = NOR;
	filterType = TwDefineEnum("shaderType", filters, 11);
	TwAddVarRW(mUserInterface, "Display", filterType, &filter, " group='Filtro' label='Shader' ");
	TwAddVarRW(mUserInterface, "kernelx", TW_TYPE_UINT32, &kernel.x, " group='Kernel' min=1 max=7 label='Kernel X' step=1 ");
	TwAddVarRW(mUserInterface, "kernely", TW_TYPE_UINT32, &kernel.y, " group='Kernel' min=1 max=7 label='Kernel Y' step=1 ");
	TwAddVarRW(mUserInterface, "histo", TW_TYPE_BOOLCPP, &histograma, " label='Histograma' ");
	TwAddVarRW(mUserInterface, "info", TW_TYPE_BOOLCPP, &info, " label='Informacion' ");
	
}

userInterface::~userInterface()
{
}


void userInterface::reshape()
{
	TwWindowSize(windowHeight, windowHeight);
}

void userInterface::show()
{
	TwDefine("Tarea visible = true");
}

void userInterface::hide()
{
	TwDefine("Tarea visible = false");
}

int userInterface::getFilter()
{
	if (filter == NOR) return 0;
	if (filter == NEG) return 1;
	if (filter == GREY) return 2;
	if (filter == BAW) return 3;
	if (filter == EDGE_SOL) return 4;
	if (filter == EDGE_PEW) return 5;
	if (filter == EDGE_ROB) return 6;
	if (filter == PROM) return 7;
	if (filter == MED) return 8;
	if (filter == LAP) return 9;
	if (filter == TOON) return 10;
	return NULL;

}

void userInterface::setFilter(int type)
{

	if (type == 0) filter=NOR;
	else if (type == 1) filter=NEG;
	else if (type == 2) filter=GREY;
	else if (type == 3) filter=BAW;
	else if (type == 4) filter=EDGE_SOL;
	else if (type == 5) filter=EDGE_PEW;
	else if (type == 6) filter=EDGE_ROB;
	else if (type == 7) filter=PROM;
	else if (type == 8) filter=MED;
	else if (type == 9) filter=LAP;
	else if (type == 10) filter=TOON;

}

userData userInterface::getData()
{
	userData u;
	u.hist = histograma;
	u.info = info;
	u.kernel = kernel;
	return u;
}

void userInterface::setData(userData data)
{
	histograma = data.hist;
	info = data.info;
	kernel = data.kernel;
}
