#pragma once

#include "BuildingManager.h"
#include "Common.h"


using namespace Turk;

BuildingManager::BuildingManager()
// : FirstScout(false)
{


}


//Commander::~Commander()
//{
//}

BuildingManager & BuildingManager::Instance()
{
	static BuildingManager instance;
	return instance;
}




void BuildingManager::InitialBuildLocSet(){
	int MapWidth = BWAPI::Broodwar->mapWidth();
	int MapHeight = BWAPI::Broodwar->mapHeight();

	BWAPI::TilePosition OurStartingTilePos = BWAPI::Broodwar->self()->getStartLocation();
	BWTA::Region *OurRegion = BWTA::getRegion(BWTA::getStartLocation(BWAPI::Broodwar->self())->getPosition());

	// Collect all possible buildable tile positions.
	TileContainer.clear();
	for (int i = 1; i <= MapWidth; i++){
		for (int j = 1; j <= MapHeight; j++){
			BWAPI::TilePosition BuildingLocation(i, j);
			
			// Check neighborhood point region
			bool Indicator1 = true;
			for (int k = -32; k <= 32; k++){
				for (int m = -32; m <= 32; m++){
					BWAPI::Position Temp(BuildingLocation.x * 32 + k, BuildingLocation.y * 32 + m);
					BWTA::Region *TestRegion = BWTA::getRegion(Temp);

					if (TestRegion != OurRegion){
						Indicator1 = false;
					}
				}
			}

			// Check neighborhood billable tile 
			bool Indicator2 = true;
			for (int k = -1; k <= 1; k++){
				for (int m = -1; m <= 1; m++){
					BWAPI::TilePosition Temp(BuildingLocation.x + k, BuildingLocation.y + m);
					if (!BWAPI::Broodwar->isBuildable(Temp, true)){
						Indicator2 = false;
					}
				}
			}

			if (Indicator1 && Indicator2){ // 				
				TileContainer.insert(BuildingLocation);
			}
		}
	}



	// Container 1
	NicePlace1.clear();	
	NicePlace2.clear();	
	NicePlace3.clear();	
	NicePlace4.clear();	
	NicePlace5.clear();	
	NicePlace6.clear();	
	NicePlace7.clear();
	for (int i = 0; i < 10; i++){
		NicePlace1 = SubChecker(10, 9, NicePlace1); // Width Height no more than 12 width
		NicePlace2 = SubChecker(14, 6, NicePlace2);   // No more than 6
		NicePlace3 = SubChecker(6, 8, NicePlace3);
		NicePlace4 = SubChecker(6, 6, NicePlace4);
		NicePlace5 = SubChecker(4, 6, NicePlace5);
		NicePlace6 = SubChecker(4, 4, NicePlace6);
		NicePlace7 = SubChecker(2, 4, NicePlace7);
	}

	



	
}




std::set<BWAPI::TilePosition> BuildingManager::SubChecker(int Width, int Height, std::set<BWAPI::TilePosition> Saver){
	
	std::set<BWAPI::TilePosition>::iterator iter;
	BWAPI::TilePosition BuildingLocation = BWAPI::TilePositions::None;

	bool Checker = true;
	//while (Checker){
		Checker = false;
		for (iter = TileContainer.begin(); iter != TileContainer.end(); iter++){
			BuildingLocation = *iter;

			bool NotGood = false;		
			for (int i = 0; i < Width; i++){
				for (int j = 0; j < Height; j++){
					BWAPI::TilePosition TempLocation(BuildingLocation.x + i, BuildingLocation.y + j);
				
					bool Indicator2 = true;
					for (int k = -1; k <= 1; k++){
						for (int m = -1; m <= 1; m++){
							BWAPI::TilePosition Temp(TempLocation.x + k, TempLocation.y + m);

							// We cannot build on this tile or, this tile is already occupied.
							if (!BWAPI::Broodwar->isBuildable(Temp, true) || TileContainer.find(TempLocation) == TileContainer.end()){
								Indicator2 = false; // We cannot use this set
							}
						}
					}


					if (!Indicator2){
						NotGood = true;
					}
				}
			}

			// We found a nice place
			if (!NotGood){

				Saver.insert(BuildingLocation);

				for (int k = 0; k < Width; k++){
					for (int m = 0; m < Height; m++){
						BWAPI::TilePosition TempLocation(BuildingLocation.x + k, BuildingLocation.y + m);	
						std::set<BWAPI::TilePosition>::iterator iter2 = TileContainer.find(TempLocation);
						if (*iter2){
							TileContainer.erase(*iter2);
						}					
					}
				}

				Checker = true;
				break;
			}
		//}
	}

	return Saver;

}









void BuildingManager::MapConnector(){

//
//	std::string Mapname = BWAPI::Broodwar->mapName().c_str();
//	if (Mapname.compare("Python 1.3")){		// 
//		PythonBuildingLocation();
//	}
	
	BWAPI::TilePosition FirstPylon;	
	BWAPI::TilePosition FirstGateWay;
	BWAPI::TilePosition FirstTech;

	std::set<BWAPI::TilePosition>::iterator iter;
	BWAPI::TilePosition LeftTop = BWAPI::TilePositions::None;



	InitialBuildLocSet();

	// Initialize building locations
	m_GateTilePosition.clear();
	m_PylonTilePosition.clear();
	m_TechBuildingTileLocation.clear();
	
	// Largest 10 / 10
	for (iter = NicePlace1.begin(); iter != NicePlace1.end(); iter++){
		LeftTop = *iter;

		// Location of the Gateway
		FirstGateWay.x = LeftTop.x + 1;
		FirstGateWay.y = LeftTop.y;
		GateWayPositioning(FirstGateWay, 3);

		FirstPylon.x = FirstGateWay.x + GateWay.tileWidth();
		FirstPylon.y = FirstGateWay.y;
		PylonPositioning(FirstPylon, 4);
				
		FirstTech.x = FirstPylon.x + Pylon.tileWidth();
		FirstTech.y = FirstPylon.y;
		TechBuildingPositioning(FirstTech, 4);
	}

	// Largest 14 / 6
	for (iter = NicePlace2.begin(); iter != NicePlace2.end(); iter++){
		LeftTop = *iter;

		// Location of the Gateway
		FirstGateWay.x = LeftTop.x+1;
		FirstGateWay.y = LeftTop.y;
		GateWayPositioning(FirstGateWay, 2);

		FirstGateWay.x = FirstGateWay.x + GateWay.tileWidth();
		FirstGateWay.y = FirstGateWay.y;
		GateWayPositioning(FirstGateWay, 2);

		FirstPylon.x = FirstGateWay.x + GateWay.tileWidth();
		FirstPylon.y = FirstGateWay.y;
		PylonPositioning(FirstPylon, 3);

		FirstTech.x = FirstPylon.x + Pylon.tileWidth();
		FirstTech.y = FirstPylon.y;
		TechBuildingPositioning(FirstTech, 3);

	}


	// Largest 6 / 8
	for (iter = NicePlace3.begin(); iter != NicePlace3.end(); iter++){
		LeftTop = *iter;

		FirstTech.x = LeftTop.x;
		FirstTech.y = LeftTop.y;
		TechBuildingPositioning(FirstTech, 1);
		
		// Location of the Gateway
		FirstGateWay.x = FirstTech.x;
		FirstGateWay.y = FirstTech.y + 2;
		GateWayPositioning(FirstGateWay, 2);
		
		FirstPylon.x = FirstTech.x + 4;
		FirstPylon.y = FirstTech.y;
		PylonPositioning(FirstPylon, 4);
		
	}


	// Largest 6 / 6
	for (iter = NicePlace4.begin(); iter != NicePlace4.end(); iter++){
		LeftTop = *iter;
		
		FirstGateWay.x = LeftTop.x;
		FirstGateWay.y = LeftTop.y;
		GateWayPositioning(FirstGateWay, 2);
		
		FirstPylon.x = FirstGateWay.x + GateWay.tileWidth();
		FirstPylon.y = LeftTop.y;
		PylonPositioning(FirstPylon, 3);		
	}

	// Largest 6 / 6
	for (iter = NicePlace5.begin(); iter != NicePlace5.end(); iter++){
		LeftTop = *iter;

		FirstPylon.x = LeftTop.x;
		FirstPylon.y = LeftTop.y;
		PylonPositioning(FirstPylon, 1);

		FirstPylon.x = FirstPylon.x+Pylon.tileWidth();
		FirstPylon.y = FirstPylon.y;
		PylonPositioning(FirstPylon, 1);

		FirstGateWay.x = FirstPylon.x;
		FirstGateWay.y = FirstPylon.y + Pylon.tileHeight();
		GateWayPositioning(FirstGateWay, 1);
	}
	
	// Largest 6 / 6
	for (iter = NicePlace6.begin(); iter != NicePlace6.end(); iter++){
		LeftTop = *iter;

		FirstPylon.x = LeftTop.x;
		FirstPylon.y = LeftTop.y;
		PylonPositioning(FirstPylon, 2);

		FirstPylon.x = FirstPylon.x + Pylon.tileWidth();
		FirstPylon.y = FirstPylon.y;
		PylonPositioning(FirstPylon, 2);
	}

	// Largest 6 / 6
	for (iter = NicePlace7.begin(); iter != NicePlace7.end(); iter++){
		LeftTop = *iter;

		FirstPylon.x = LeftTop.x;
		FirstPylon.y = LeftTop.y;
		PylonPositioning(FirstPylon, 2);

	}


	std::reverse(m_PylonTilePosition.begin(), m_PylonTilePosition.end());
	std::reverse(m_GateTilePosition.begin(), m_GateTilePosition.end());
	std::reverse(m_TechBuildingTileLocation.begin(), m_TechBuildingTileLocation.end());

}





void BuildingManager::DefensePylonLocation(const BWAPI::Unitset & FirstMineralSet){
	
	// Build pylon close to the minerals
	BWAPI::Unit ClosestMineral = nullptr;
	BWAPI::Unit FarthestMineral = nullptr;
	double ClosestDist = 10000;
	double FarthestDist = 0;
	// For the given initial Minerals, find the closest mineral one by one
	for (auto &mineral : FirstMineralSet){
		BWAPI::Position MineralPosition = mineral->getPosition();
	
		double dist = MineralPosition.x + MineralPosition.y;
		if (dist < ClosestDist){
			ClosestMineral = mineral;
			ClosestDist = dist;
		}
		if (dist > FarthestDist){
			FarthestMineral = mineral;
			FarthestDist = dist;
		}
	}

	// Collect additional pylon location.
	BWAPI::TilePosition FirstMineralPosition = ClosestMineral->getTilePosition();
	BWAPI::TilePosition LastMineralPosition = FarthestMineral->getTilePosition();	

	FirstMineralPosition.x = FirstMineralPosition.x + 2;
	FirstMineralPosition.y = FirstMineralPosition.y - 2;
	LastMineralPosition.x = LastMineralPosition.x + 2;
	LastMineralPosition.y = LastMineralPosition.y + 2;

	std::vector<BWAPI::TilePosition> anothervector;
	anothervector.push_back(FirstMineralPosition);
	anothervector.push_back(LastMineralPosition);

	// 3rd and 4th pylons
	m_PylonTilePosition.insert(m_PylonTilePosition.begin() + 20, anothervector.begin(), anothervector.end());  	

}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// Class Variable Handler
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$





//
void BuildingManager::LocationRemover(BWAPI::Unit unit){
	

	std::vector<BWAPI::TilePosition>	Position;
	if (unit->getType() == Pylon){
		Position = m_PylonTilePosition;
	}
	else if (unit->getType() == GateWay || unit->getType() == BWAPI::UnitTypes::Protoss_Stargate){
		Position = m_GateTilePosition;
	}
	else if (unit->getType() == Nexus){
		Position = m_ExpansionLocation;
	}
	else{
		Position = m_TechBuildingTileLocation;
	}


	BWAPI::TilePosition CreatedTilePosition = unit->getTilePosition();	
	std::vector<BWAPI::TilePosition>::iterator	Iter;	 
	for (Iter = Position.begin(); Iter != Position.end(); Iter++){
		BWAPI::TilePosition Temp =  *Iter;
		if (Temp == CreatedTilePosition){
			Position.erase(Iter, Iter + 1);
			break;
		}
	}

	if (unit->getType() == Pylon){
		m_PylonTilePosition = Position;
	}
	else if (unit->getType() == GateWay || unit->getType() == BWAPI::UnitTypes::Protoss_Stargate){
		m_GateTilePosition = Position;
	}
	else if (unit->getType() == Nexus){
		m_ExpansionLocation = Position;
	}
	else{
		m_TechBuildingTileLocation = Position;
	}

}

//


void BuildingManager::GetExpansionBase(BWAPI::TilePosition EnemyTilePosition, BWAPI::TilePosition HomeTilePosition){
	
	std::map<double, BWAPI::TilePosition> NexusLoc;
	std::map<double, BWAPI::TilePosition>::iterator i;

	// This is only for nexus
	BWAPI::UnitType BuildingTarget = Nexus;
	
	BWTA::BaseLocation * closestBase = nullptr;
	double minDistance = 100000;

	// for each base location
	m_ExpansionLocation.clear();
	for (BWTA::BaseLocation * base : BWTA::getBaseLocations()){
		BWAPI::TilePosition tile = base->getTilePosition();

		// // if the base has gas
		if (tile && !base->isIsland()){
			//  // get the tile position of the base
			int distanceFromHome = BWTA::getGroundDistance2(HomeTilePosition, tile); // -BWTA::getGroundDistance2(EnemyTilePosition, tile);

			//// if it is not connected, continue //island
			//if (!BWTA::isConnected(HomeTilePosition, tile)){
			//	continue;
			//}

			NexusLoc[distanceFromHome] = tile;
		}
	}

	// Move expansion location in the map to the vector
	for (i = NexusLoc.begin(); i != NexusLoc.end(); i++){		
		BWAPI::TilePosition TempLoc = i->second;
		m_ExpansionLocation.push_back(TempLoc);
	}	
	BWAPI::Broodwar->sendText("Expansion Location size %.2d", NexusLoc.size());


	// std::reverse(m_ExpansionLocation.begin(), m_ExpansionLocation.end());


	
}


//





// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// Building Positioning 
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void BuildingManager::PylonPositioning(const BWAPI::TilePosition & StartingPoint,int Numbers){		
	BWAPI::TilePosition TempTileLocation;		
	for (int a = 0; a < Numbers; a = a + 1){
		TempTileLocation.x = StartingPoint.x;
		TempTileLocation.y = StartingPoint.y + a * Pylon.tileHeight();
		m_PylonTilePosition.push_back(TempTileLocation);
	}
}


void BuildingManager::GateWayPositioning(const BWAPI::TilePosition & StartingPoint, int Numbers){	
	BWAPI::TilePosition TempTileLocation;	
	for (int a = 0; a < Numbers; a = a + 1){
		TempTileLocation.x = StartingPoint.x;
		TempTileLocation.y = StartingPoint.y + a * GateWay.tileHeight();
		m_GateTilePosition.push_back(TempTileLocation);
	}
}


void BuildingManager::TechBuildingPositioning(const BWAPI::TilePosition & StartingPoint,int Numbers){
	BWAPI::TilePosition TempTileLocation;
	for (int a = 0; a < Numbers; a = a + 1){
		// Location of Cybernetics core
		TempTileLocation.x = StartingPoint.x;
		TempTileLocation.y = StartingPoint.y + a * Cybernetics.tileHeight();
		m_TechBuildingTileLocation.push_back(TempTileLocation);
	}
}



// BWAPI::Broodwar->sendText("Nice Place 3 Size %.2d");

//for (iter = NicePlace3.begin(); iter != NicePlace3.end(); iter++){
//	Temp = *iter;
// 	BWAPI::Broodwar->sendText("Nice Place 3 Size %d : %d", Temp.x, Temp.y);

////	BWAPI::Broodwar->drawBoxMap(Temp.x, Temp.y, Temp.x + 15, Temp.y + 10, BWAPI::Colors::Red, true);
//}


//BWAPI::Broodwar->sendText("Nice Place 3 Size %d : %d", Temp.x, Temp.y);


void BuildingManager::BuildingLocDrawer(){

	std::set<BWAPI::TilePosition>::iterator iter;
	BWAPI::TilePosition Temp = BWAPI::TilePositions::None;
	BWAPI::TilePosition Temp2 = BWAPI::TilePositions::None;


	// Building Location Printing
	for (iter = NicePlace1.begin(); iter != NicePlace1.end(); iter++){
		Temp = *iter;
		Temp2.x = Temp.x + 10;
		Temp2.y = Temp.y + 9;
		BWAPI::Broodwar->drawBoxMap(BWAPI::Position(Temp), BWAPI::Position(Temp2), BWAPI::Colors::Red, false);
	}

	for (iter = NicePlace2.begin(); iter != NicePlace2.end(); iter++){
		Temp = *iter;
		Temp2.x = Temp.x + 14;
		Temp2.y = Temp.y + 6;
		BWAPI::Broodwar->drawBoxMap(BWAPI::Position(Temp), BWAPI::Position(Temp2), BWAPI::Colors::Blue, false);
	}

	for (iter = NicePlace3.begin(); iter != NicePlace3.end(); iter++){
		Temp = *iter;
		Temp2.x = Temp.x + 6;
		Temp2.y = Temp.y + 8;
		BWAPI::Broodwar->drawBoxMap(BWAPI::Position(Temp), BWAPI::Position(Temp2), BWAPI::Colors::Orange, false);
	}

	for (iter = NicePlace4.begin(); iter != NicePlace4.end(); iter++){
		Temp = *iter;
		Temp2.x = Temp.x + 6;
		Temp2.y = Temp.y + 6;
		BWAPI::Broodwar->drawBoxMap(BWAPI::Position(Temp), BWAPI::Position(Temp2), BWAPI::Colors::Cyan, false);
	}

	for (iter = NicePlace5.begin(); iter != NicePlace5.end(); iter++){
		Temp = *iter;
		Temp2.x = Temp.x + 4;
		Temp2.y = Temp.y + 6;
		BWAPI::Broodwar->drawBoxMap(BWAPI::Position(Temp), BWAPI::Position(Temp2), BWAPI::Colors::Green, false);
	}

	for (iter = NicePlace6.begin(); iter != NicePlace6.end(); iter++){
		Temp = *iter;
		Temp2.x = Temp.x + 4;
		Temp2.y = Temp.y + 4;
		BWAPI::Broodwar->drawBoxMap(BWAPI::Position(Temp), BWAPI::Position(Temp2), BWAPI::Colors::Yellow, false);
	}


	for (iter = NicePlace7.begin(); iter != NicePlace7.end(); iter++){
		Temp = *iter;
		Temp2.x = Temp.x + 2;
		Temp2.y = Temp.y + 4;
		BWAPI::Broodwar->drawBoxMap(BWAPI::Position(Temp), BWAPI::Position(Temp2), BWAPI::Colors::White, false);
	}

}




void BuildingManager::BuildingFunction(const BWAPI::Unit & HeadQuater, const BWAPI::UnitType & BuildingTarget){
	
	// Draw Building Locations
	BuildingLocDrawer();


	// Get Mr. Builder.
	if (!MrBuilder){
		MrBuilder = HeadQuater->getClosestUnit(BWAPI::Filter::IsWorker, 999999);
	}


	// Determine the building locations
	std::vector<BWAPI::TilePosition>	Position;
	if (BuildingTarget == Pylon){
		Position = m_PylonTilePosition;
	}
	else if (BuildingTarget == GateWay || BuildingTarget == BWAPI::UnitTypes::Protoss_Stargate){
		Position = m_GateTilePosition;
	}
	else if (BuildingTarget == Nexus){
		Position = m_ExpansionLocation;
	}
	else{
		Position = m_TechBuildingTileLocation;
	}

	// Building Location Determination
	std::vector<BWAPI::TilePosition>::reverse_iterator	Iter;
	BWAPI::TilePosition targetBuildLocation = BWAPI::TilePositions::None;
	for (Iter = Position.rbegin(); Iter != Position.rend(); Iter++){
		targetBuildLocation = *Iter;
		if (BWAPI::Broodwar->canBuildHere(targetBuildLocation, BuildingTarget)){ // canBuildHere(Position.back(), BuildingTarget))
			break;
		}		
	}


	
	static int lastChecked = 0;	
	// If we are supply blocked and haven't tried constructing more recently
	if (BWAPI::Broodwar->self()->incompleteUnitCount(BuildingTarget) == 0 && lastChecked + 100 < BWAPI::Broodwar->getFrameCount() ){
		lastChecked = BWAPI::Broodwar->getFrameCount();


		BWAPI::TilePosition PossibleTilePosition = HeadQuater->getTilePosition();
		BWAPI::Unit SecondHeadQuater = HeadQuater->getClosestUnit(BWAPI::Filter::IsResourceDepot && BWAPI::Filter::IsOwned, 999999);
		if (SecondHeadQuater){
			PossibleTilePosition = SecondHeadQuater->getTilePosition();
		}		

		// If there is no possible building location.
		if (!targetBuildLocation){
			targetBuildLocation = BWAPI::Broodwar->getBuildLocation(BuildingTarget, PossibleTilePosition, 32);
			BWAPI::Broodwar->sendText("Location vector is empty %.2d : %.2d", targetBuildLocation.x, targetBuildLocation.y);
		}
		
		// If a builder and location are secured
		if (MrBuilder && targetBuildLocation){

			if (!BWAPI::Broodwar->isExplored(targetBuildLocation) && BuildingTarget.mineralPrice() < BWAPI::Broodwar->self()->minerals()){ // 
				MrBuilder->move(BWAPI::Position(targetBuildLocation));				
			}
			else{
				MrBuilder->build(BuildingTarget, targetBuildLocation);				
			}				
		}
	}
}



void BuildingManager::MrBuilderRemover(){
	MrBuilder = nullptr;
}




















//
//void BuildingManager::PythonBuildingLocation(){
//
//	// Game Information <- This should be moved to the InformationManager.h
//	BWAPI::Position homePosition = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
//	BWAPI::Position HillPosition = BWTA::getNearestChokepoint(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()))->getCenter();
//
//	// Tempral Variables
//	BWAPI::TilePosition FirstPylonLocation;	
//	BWAPI::TilePosition SecondPylonLocation;
//	BWAPI::TilePosition TempTileLocation;
//	BWAPI::TilePosition FirstGateWay;
//	BWAPI::TilePosition FirstTech;
//	
//
//	if ((abs(homePosition.x - HillPosition.x) > abs(homePosition.y - HillPosition.y)) && (homePosition.x - HillPosition.x > 0)){   //   , Choke is left
//		// Probably 12
//
//		// Location of the first pylon		
//		FirstPylonLocation.x = 74;
//		FirstPylonLocation.y = 4;			
//		m_PylonTilePosition.push_back(FirstPylonLocation);
//
//		TempTileLocation.x = FirstPylonLocation.x + Pylon.tileWidth();
//		TempTileLocation.y = FirstPylonLocation.y;
//		m_PylonTilePosition.push_back(TempTileLocation);
//
//		// Location of the Second pylon		
//		SecondPylonLocation.x = 82;
//		SecondPylonLocation.y = 12;
//		m_PylonTilePosition.push_back(SecondPylonLocation);
//
//		TempTileLocation.x = SecondPylonLocation.x + Pylon.tileWidth();
//		TempTileLocation.y = SecondPylonLocation.y;
//		m_PylonTilePosition.push_back(TempTileLocation);
//
//		// Build the gateways left to the first pylon
//		FirstGateWay.x = FirstPylonLocation.x - GateWay.tileWidth();
//		FirstGateWay.y = FirstPylonLocation.y - GateWay.tileHeight();
//						
//		// Build the upgrading points
//		FirstTech.x = FirstPylonLocation.x + 2 * Pylon.tileWidth();
//		FirstTech.y = FirstPylonLocation.y;
//			
//		// Update Building Positions
//		BuildingManager::PylonPositioning(FirstPylonLocation,6);
//		BuildingManager::GateWayPositioning(FirstGateWay,6);
//		BuildingManager::TechBuildingPositioning(FirstTech,5);
//
//
//
//		// Build the gateways left to the first pylon
//		FirstGateWay.x = SecondPylonLocation.x - GateWay.tileWidth();
//		FirstGateWay.y = SecondPylonLocation.y;
//
//		// Build the upgrading pointsx
//		FirstTech.x = SecondPylonLocation.x + 2 * Pylon.tileWidth();
//		FirstTech.y = SecondPylonLocation.y;
//
//		// Update Building Positions
//		BuildingManager::PylonPositioning(SecondPylonLocation, 4);
//		BuildingManager::GateWayPositioning(FirstGateWay,2);
//		BuildingManager::TechBuildingPositioning(FirstTech,4);
//
//
//		//  Choke Point
//				
//	}
//	else if ((abs(homePosition.x - HillPosition.x) > abs(homePosition.y - HillPosition.y)) && (homePosition.x - HillPosition.x < 0)){  // Chock is East
//		// Probably 6
//		// BackYard 33 / 119
//
//		// Location of the first pylon		
//		FirstPylonLocation.x = 50;
//		FirstPylonLocation.y = 111;
//		m_PylonTilePosition.push_back(FirstPylonLocation);
//		
//		TempTileLocation.x = FirstPylonLocation.x + Pylon.tileWidth();
//		TempTileLocation.y = FirstPylonLocation.y;
//		m_PylonTilePosition.push_back(TempTileLocation);
//
//		// Location of the second pylon		
//		SecondPylonLocation.x = 34;
//		SecondPylonLocation.y = 107;
//		m_PylonTilePosition.push_back(SecondPylonLocation);
//
//		TempTileLocation.x = SecondPylonLocation.x + Pylon.tileWidth();
//		TempTileLocation.y = SecondPylonLocation.y;
//		m_PylonTilePosition.push_back(TempTileLocation);
//		
//		// Build the gateways right to the first pylon
//		FirstGateWay.x = FirstPylonLocation.x + 2* Pylon.tileWidth();
//		FirstGateWay.y = FirstPylonLocation.y - GateWay.tileHeight();;
//				
//		// Determine locations of gateways.
//		// Build the upgrading points
//		FirstTech.x = FirstPylonLocation.x - Forge.tileWidth();
//		FirstTech.y = FirstPylonLocation.y;
//
//
//		// Update Building Positions
//		BuildingManager::PylonPositioning(FirstPylonLocation,6);
//		BuildingManager::GateWayPositioning(FirstGateWay,4);
//		BuildingManager::TechBuildingPositioning(FirstTech,6);
//
//		
//		// Build the gateways right to the first pylon
//		FirstGateWay.x = SecondPylonLocation.x + 2 * Pylon.tileWidth() ;
//		FirstGateWay.y = SecondPylonLocation.y;
//
//		// Determine locations of gateways.
//		// Build the upgrading points
//		FirstTech.x = SecondPylonLocation.x - Forge.tileWidth();
//		FirstTech.y = SecondPylonLocation.y;
//
//
//		// Update Building Positions
//		BuildingManager::PylonPositioning(SecondPylonLocation, 4);
//		BuildingManager::GateWayPositioning(FirstGateWay,3);
//		BuildingManager::TechBuildingPositioning(FirstTech,4);
//
//		
//	}
//	else if ((abs(homePosition.x - HillPosition.x) < abs(homePosition.y - HillPosition.y)) && (homePosition.y - HillPosition.y > 0)){
//		// Probably 9		
//		// Location of the first pylon
//		FirstPylonLocation.x = 16;   // Fix
//		FirstPylonLocation.y = 77;
//		m_PylonTilePosition.push_back(FirstPylonLocation);
//
//		TempTileLocation.x = FirstPylonLocation.x + Pylon.tileWidth();
//		TempTileLocation.y = FirstPylonLocation.y;
//		m_PylonTilePosition.push_back(TempTileLocation);
//
//		SecondPylonLocation.x = 12;   // Fix
//		SecondPylonLocation.y = 89;
//		m_PylonTilePosition.push_back(SecondPylonLocation);
//
//		TempTileLocation.x = SecondPylonLocation.x + Pylon.tileWidth();
//		TempTileLocation.y = SecondPylonLocation.y;
//		m_PylonTilePosition.push_back(TempTileLocation);
//
//		// Build the gateways right to the first pylon
//		// Build the gateways left to the first pylon		
//		FirstGateWay.x = FirstPylonLocation.x - GateWay.tileWidth() ;
//		FirstGateWay.y = FirstPylonLocation.y - GateWay.tileHeight();
//
//		// Build the upgrading points
//		FirstTech.x = FirstPylonLocation.x + 2 * Pylon.tileWidth();
//		FirstTech.y = FirstPylonLocation.y;
//
//		// Update Building Positions
//		BuildingManager::PylonPositioning(FirstPylonLocation,5);
//		BuildingManager::GateWayPositioning(FirstGateWay,3);
//		BuildingManager::TechBuildingPositioning(FirstTech,5);
//
//		// choke 13 72
//
//
//		// Build the gateways right to the first pylon
//		FirstGateWay.x = SecondPylonLocation.x - GateWay.tileWidth();
//		FirstGateWay.y = SecondPylonLocation.y;
//
//		// Build the upgrading pointsx
//		FirstTech.x = SecondPylonLocation.x + 2 * Pylon.tileWidth();
//		FirstTech.y = SecondPylonLocation.y;
//
//
//		// Update Building Positions
//		BuildingManager::PylonPositioning(SecondPylonLocation, 4);
//		BuildingManager::GateWayPositioning(FirstGateWay,3);
//		BuildingManager::TechBuildingPositioning(FirstTech,4);
//				
//
//	}
//	else if ((abs(homePosition.x - HillPosition.x) < abs(homePosition.y - HillPosition.y)) && (homePosition.y - HillPosition.y < 0)){
//		// Probably 3
//
//		// Location of the first pylon
//		FirstPylonLocation.x = 111;
//		FirstPylonLocation.y = 43;
//		m_PylonTilePosition.push_back(FirstPylonLocation);
//
//		TempTileLocation.x = FirstPylonLocation.x + Pylon.tileWidth();
//		TempTileLocation.y = FirstPylonLocation.y;
//		m_PylonTilePosition.push_back(TempTileLocation);
//
//		// Location of the second pylon
//		SecondPylonLocation.x = 117;
//		SecondPylonLocation.y = 26;
//		m_PylonTilePosition.push_back(SecondPylonLocation);
//
//		TempTileLocation.x = SecondPylonLocation.x + Pylon.tileWidth();
//		TempTileLocation.y = SecondPylonLocation.y;
//		m_PylonTilePosition.push_back(TempTileLocation);
//
//		// Build the gateways left to the first pylon		
//		FirstGateWay.x = FirstPylonLocation.x - GateWay.tileWidth();
//		FirstGateWay.y = FirstPylonLocation.y - GateWay.tileHeight();
//
//		// Build the upgrading points
//		FirstTech.x = FirstPylonLocation.x + 2 * Pylon.tileWidth() ;
//		FirstTech.y = FirstPylonLocation.y;
//
//		// Update Building Positions
//		BuildingManager::PylonPositioning(FirstPylonLocation,5);
//		BuildingManager::GateWayPositioning(FirstGateWay,3);
//		BuildingManager::TechBuildingPositioning(FirstTech,5);
//
//		// Upgrade 117:32
//		// choke 113 55	
//		
//
//		// Build the gateways left to the first pylon
//		FirstGateWay.x = SecondPylonLocation.x - GateWay.tileWidth();
//		FirstGateWay.y = SecondPylonLocation.y;
//
//		// Build the upgrading pointsx
//		FirstTech.x = SecondPylonLocation.x + 2 * Pylon.tileWidth();
//		FirstTech.y = SecondPylonLocation.y;
//
//		// Update Building Positions
//		BuildingManager::PylonPositioning(SecondPylonLocation, 4);
//		BuildingManager::GateWayPositioning(FirstGateWay,3);
//		BuildingManager::TechBuildingPositioning(FirstTech,4);
//
//	}
//
//	
//	std::reverse(m_PylonTilePosition.begin(), m_PylonTilePosition.end());
//	std::reverse(m_GateTilePosition.begin(), m_GateTilePosition.end());
//	std::reverse(m_TechBuildingTileLocation.begin(), m_TechBuildingTileLocation.end());
//
//}




//
//void BuildingManager::PylonLocationSaver(const std::vector<BWAPI::TilePosition> & TempPosition){
//	m_PylonTilePosition = TempPosition;
//}
//
//const std::vector<BWAPI::TilePosition> & BuildingManager::GateWayLocationPresent(){
//	return m_GateTilePosition;
//}
//
//void BuildingManager::GateWayLocationSaver(const std::vector<BWAPI::TilePosition> & TempPosition){
//	m_GateTilePosition = TempPosition;
//}




//BWAPI::Region OurRegion = BWAPI::Broodwar->getRegionAt(BWAPI::Position(OurStartingTilePos));
//BWTA::Region *OurRegion = BWTA::getRegion(BWAPI::Position(OurStartingTilePos));
// BWAPI::Position Temp = BWTA::getRegion(BWAPI::Position(BuildingLocation))->getCenter();
//BWAPI::Broodwar->sendText("Tile in Our Region :%d", OurRegion);

//BWAPI::Region TestRegion = BWAPI::Broodwar->getRegionAt(BWAPI::Position(BuildingLocation));

//BWAPI::Position Temp = BWTA::getRegion(BWAPI::Position(BuildingLocation))->getCenter();
//BWAPI::Broodwar->sendText("Buildable Locations %d : %d", BuildingLocation.x, BuildingLocation.y);





/*std::set<BWAPI::TilePosition>::iterator iter;
BWAPI::TilePosition Temp = BWAPI::TilePositions::None;
for (iter = NicePlace3.begin(); iter != NicePlace3.end(); iter++){
Temp = *iter;
BWAPI::Broodwar->sendText("Nice Place 3 Size %d : %d", Temp.x, Temp.y);
}*/

//BWAPI::Broodwar->sendText("1 Tile in Our Region :%d", TileContainer.size());



//void BuildingManager::PylonLocationRemover(BWAPI::TilePosition TempTilePosition){
//
//	std::vector<BWAPI::TilePosition>::iterator	Iter;
//
//	BWAPI::TilePosition Temp = BWAPI::TilePositions::None;
//	for (Iter = m_PylonTilePosition.begin(); Iter != m_PylonTilePosition.end(); Iter++){
//		Temp = *Iter;
//
//		if (Temp == TempTilePosition){
//			m_PylonTilePosition.erase(Iter, Iter + 3);
//			break;
//		}
//	}
//
//}
//
//
//
//
//void BuildingManager::GateWayLocationRemover(BWAPI::TilePosition TempTilePosition){
//	std::vector<BWAPI::TilePosition>::iterator	Iter;
//
//	BWAPI::TilePosition Temp = BWAPI::TilePositions::None;
//	for (Iter = m_GateTilePosition.begin(); Iter != m_GateTilePosition.end(); Iter++){
//		Temp = *Iter;
//
//		if (Temp == TempTilePosition){
//			m_GateTilePosition.erase(Iter, Iter + 3);
//			break;
//		}
//	}
//
//
//	m_GateTilePosition.pop_back();
//}
//
//
//
//const BWAPI::TilePosition & BuildingManager::TechLocationPresent(){	
//	return m_TechBuildingTileLocation.back();
//}
//
//const std::vector<BWAPI::TilePosition> & BuildingManager::TechSetPresent(){
//	return m_TechBuildingTileLocation;
//}
//
//void BuildingManager::TechLocationSaver(const std::vector<BWAPI::TilePosition> & TempPosition){
//	m_TechBuildingTileLocation = TempPosition;
//}
//
//
//void BuildingManager::TechLocationRemover(BWAPI::TilePosition TempTilePosition){
//	m_TechBuildingTileLocation.pop_back();
//}



//
//const std::vector<BWAPI::TilePosition> & BuildingManager::NexusSetPresent(){
//	return m_ExpansionLocation;
//}
//
//
//void BuildingManager::NexusLocationRemover(BWAPI::TilePosition TempTilePosition){
//	m_ExpansionLocation.pop_back();
//}
//
