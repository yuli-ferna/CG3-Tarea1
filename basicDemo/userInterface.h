#pragma once
#include <AntTweakBar.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

struct userData
{
	bool hist;
	bool info;
	glm::ivec2 kernel;
	int type;
};
class userInterface {
public:
	//static userInterface* mInterface; //Holds the instance of the class
	TwBar* mUserInterface;
	~userInterface(); 
	userInterface();
	void reshape();
	void show();
	void hide();
	
	//filters
	typedef enum { NOR, BAW, GREY, NEG, EDGE_SOL, EDGE_PEW, EDGE_ROB, PROM, MED, LAP, TOON } Filter;
	TwType filterType;
	Filter filter;
	bool histograma;
	bool info;
	glm::ivec2 kernel;
	int getFilter();
	void setFilter(int type);
	userData getData();
	void setData(userData data);
private:
};