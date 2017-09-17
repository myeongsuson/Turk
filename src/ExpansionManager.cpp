#pragma once

#include "ExpansionManager.h"
#include "Common.h"


using namespace Turk;


ExpansionManager::ExpansionManager()
// : FirstScout(false)
{


}


//Commander::~Commander()
//{
//}

ExpansionManager & ExpansionManager::Instance()
{
	static ExpansionManager instance;
	return instance;
}





//// Build More Pylons
//std::vector<BWAPI::TilePosition>	PylonTilePosition = BuildingManager::Instance().PylonSetPresent();
//PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, TilePosition(m_HillPosition2), 12));

//BWAPI::TilePosition TempTilePosition = TilePosition(m_HillPosition2);
//TempTilePosition.x = TempTilePosition.x + 4;
//TempTilePosition.x = TempTilePosition.x + 4;

//BWAPI::TilePosition BuildingLoc = BWAPI::Broodwar->getBuildLocation(Pylon, TempTilePosition, 12);
//while (1){
//	if (!BWAPI::Broodwar->canBuildHere(BuildingLoc, Pylon)){
//		BuildingLoc = BWAPI::Broodwar->getBuildLocation(Pylon, TempTilePosition, 12);
//	}
//	else{
//		break;
//	}
//}
//PylonTilePosition.push_back(BuildingLoc);

//TempTilePosition = unit->getTilePosition();
//TempTilePosition.x = TempTilePosition.x + 4;
//TempTilePosition.x = TempTilePosition.x + 4;

//PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, TempTilePosition, 12));

//BuildingManager::Instance().PylonLocationSaver(PylonTilePosition);


// Set the New ChokeLines
//Campus = HillPosition2;