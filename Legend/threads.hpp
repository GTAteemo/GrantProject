#pragma once
#include "player.hpp"
#include "item.hpp"
#include "json.h"
#include <map>;

vector<ULONG64> PlayerTemp; //temp
vector<Player> CurrentPlayerList; //current
vector<Player> PlayerList; //visual
vector<Player> TargetList; //aimbot
/*items*/
using json = nlohmann::json;
json expensives;
json foods;
json reals;
json always;
/*player values*/
map<std::string, char> GroupMap;
map<std::string, char> GroupMapTemp;
map<ULONG64, vector<float>> TickCountMap;
map<ULONG64, vector<FVector>> DisCountMap;
D2D1_POINT_2F locationS;

/*Item Read*/
std::vector<Item> ItemsTemp;
std::vector<Item> ItemsList;

/*bonesReader*/
vector<ULONG64> PlayerBoneseTemp;
vector<Player> CurrentPlayerBonese;
vector<Player> PlayerBonese;


#include "opticCameraReader.hpp"
#include "baseValues.hpp"
#include "bonesReader.hpp"
#include "therminalVision.hpp"
#include "violent.hpp"
#include "itemReader.h"
#include "playerReader.hpp"
#include "viewMatrixReader.hpp"
