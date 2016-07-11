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
	int count = 0;
	
	
	BWAPI::TilePosition OurStartingTilePos = BWAPI::Broodwar->self()->getStartLocation();
	BWAPI::Region OurRegion = BWAPI::Broodwar->getRegionAt(BWAPI::Position(OurStartingTilePos));
			
	TileContainer.clear();
	BWAPI::TilePosition BuildingLocation = BWAPI::TilePositions::None;
	for (int i = 1; i <= MapWidth; i++){
		for (int j = 1; j <= MapHeight; j++){
			BuildingLocation.x = i;
			BuildingLocation.y = j;

			BWAPI::Region TestRegion = BWAPI::Broodwar->getRegionAt(BWAPI::Position(BuildingLocation));

			if (TestRegion == OurRegion && BWAPI::Broodwar->isBuildable(BuildingLocation)){
				TileContainer.insert(BuildingLocation);
				count = count + 1;
			}
		}
	}

	// Container 1
	NicePlace1.clear();
	NicePlace1 = SubChecker(15, 10, NicePlace1);

	//NicePlace2.clear();
	//NicePlace2 = SubChecker(8, 10, NicePlace2);

	//NicePlace3.clear();
	//NicePlace3 = SubChecker(10, 10, NicePlace3);
	
	
}


std::set<BWAPI::TilePosition> BuildingManager::SubChecker(int Width, int Height, std::set<BWAPI::TilePosition> Saver){
	
	std::set<BWAPI::TilePosition>::iterator iter;
	BWAPI::TilePosition BuildingLocation = BWAPI::TilePositions::None;

	bool Checker = true;
	// while (Checker){

		Checker = false;
		for (iter = TileContainer.begin(); iter != TileContainer.end(); iter++){
			BuildingLocation = *iter;

			bool NotGood = false;
			BWAPI::TilePosition TempLocation = BWAPI::TilePositions::None;
			for (int k = 0; k < Width; k++){
				for (int m = 0; m < Height; m++){
					TempLocation.x = BuildingLocation.x + k;
					TempLocation.y = BuildingLocation.y + m;

					if (!BWAPI::Broodwar->isBuildable(TempLocation)){
						NotGood = true;
					}
				}
			}

			// We found a nice place
			if (!NotGood){

				Saver.insert(BuildingLocation);

				BWAPI::TilePosition TempLocation = BWAPI::TilePositions::None;
				for (int k = 0; k < Width; k++){
					for (int m = 0; m < Height; m++){
						TempLocation.x = BuildingLocation.x + k;
						TempLocation.y = BuildingLocation.y + m;

						std::set<BWAPI::TilePosition>::iterator iter2 = TileContainer.find(TempLocation);
						TileContainer.erase(iter2);
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

	std::string Mapname = BWAPI::Broodwar->mapName().c_str();
	if (Mapname.compare("Python 1.3")){		// 
		PythonBuildingLocation();
	}
	
	InitialBuildLocSet();


}


void BuildingManager::PythonBuildingLocation(){

	// Game Information <- This should be moved to the InformationManager.h
	BWAPI::Position homePosition = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
	BWAPI::Position HillPosition = BWTA::getNearestChokepoint(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()))->getCenter();

	// Tempral Variables
	BWAPI::TilePosition FirstPylonLocation;	
	BWAPI::TilePosition SecondPylonLocation;
	BWAPI::TilePosition TempTileLocation;
	BWAPI::TilePosition FirstGateWay;
	BWAPI::TilePosition FirstTech;
	

	if ((abs(homePosition.x - HillPosition.x) > abs(homePosition.y - HillPosition.y)) && (homePosition.x - HillPosition.x > 0)){   //   , Choke is left
		// Probably 12

		// Location of the first pylon		
		FirstPylonLocation.x = 74;
		FirstPylonLocation.y = 4;			
		m_PylonTilePosition.push_back(FirstPylonLocation);

		TempTileLocation.x = FirstPylonLocation.x + Pylon.tileWidth();
		TempTileLocation.y = FirstPylonLocation.y;
		m_PylonTilePosition.push_back(TempTileLocation);

		// Location of the Second pylon		
		SecondPylonLocation.x = 82;
		SecondPylonLocation.y = 12;
		m_PylonTilePosition.push_back(SecondPylonLocation);

		TempTileLocation.x = SecondPylonLocation.x + Pylon.tileWidth();
		TempTileLocation.y = SecondPylonLocation.y;
		m_PylonTilePosition.push_back(TempTileLocation);

		// Build the gateways left to the first pylon
		FirstGateWay.x = FirstPylonLocation.x - GateWay.tileWidth();
		FirstGateWay.y = FirstPylonLocation.y - GateWay.tileHeight();
						
		// Build the upgrading points
		FirstTech.x = FirstPylonLocation.x + 2 * Pylon.tileWidth();
		FirstTech.y = FirstPylonLocation.y;
			
		// Update Building Positions
		BuildingManager::PylonPositioning(FirstPylonLocation,6);
		BuildingManager::GateWayPositioning(FirstGateWay,6);
		BuildingManager::TechBuildingPositioning(FirstTech,5);



		// Build the gateways left to the first pylon
		FirstGateWay.x = SecondPylonLocation.x - GateWay.tileWidth();
		FirstGateWay.y = SecondPylonLocation.y;

		// Build the upgrading pointsx
		FirstTech.x = SecondPylonLocation.x + 2 * Pylon.tileWidth();
		FirstTech.y = SecondPylonLocation.y;

		// Update Building Positions
		BuildingManager::PylonPositioning(SecondPylonLocation, 4);
		BuildingManager::GateWayPositioning(FirstGateWay,2);
		BuildingManager::TechBuildingPositioning(FirstTech,4);


		//  Choke Point
				
	}
	else if ((abs(homePosition.x - HillPosition.x) > abs(homePosition.y - HillPosition.y)) && (homePosition.x - HillPosition.x < 0)){  // Chock is East
		// Probably 6
		// BackYard 33 / 119

		// Location of the first pylon		
		FirstPylonLocation.x = 50;
		FirstPylonLocation.y = 111;
		m_PylonTilePosition.push_back(FirstPylonLocation);
		
		TempTileLocation.x = FirstPylonLocation.x + Pylon.tileWidth();
		TempTileLocation.y = FirstPylonLocation.y;
		m_PylonTilePosition.push_back(TempTileLocation);

		// Location of the second pylon		
		SecondPylonLocation.x = 34;
		SecondPylonLocation.y = 107;
		m_PylonTilePosition.push_back(SecondPylonLocation);

		TempTileLocation.x = SecondPylonLocation.x + Pylon.tileWidth();
		TempTileLocation.y = SecondPylonLocation.y;
		m_PylonTilePosition.push_back(TempTileLocation);
		
		// Build the gateways right to the first pylon
		FirstGateWay.x = FirstPylonLocation.x + 2* Pylon.tileWidth();
		FirstGateWay.y = FirstPylonLocation.y - GateWay.tileHeight();;
				
		// Determine locations of gateways.
		// Build the upgrading points
		FirstTech.x = FirstPylonLocation.x - Forge.tileWidth();
		FirstTech.y = FirstPylonLocation.y;


		// Update Building Positions
		BuildingManager::PylonPositioning(FirstPylonLocation,6);
		BuildingManager::GateWayPositioning(FirstGateWay,4);
		BuildingManager::TechBuildingPositioning(FirstTech,6);

		
		// Build the gateways right to the first pylon
		FirstGateWay.x = SecondPylonLocation.x + 2 * Pylon.tileWidth() ;
		FirstGateWay.y = SecondPylonLocation.y;

		// Determine locations of gateways.
		// Build the upgrading points
		FirstTech.x = SecondPylonLocation.x - Forge.tileWidth();
		FirstTech.y = SecondPylonLocation.y;


		// Update Building Positions
		BuildingManager::PylonPositioning(SecondPylonLocation, 4);
		BuildingManager::GateWayPositioning(FirstGateWay,3);
		BuildingManager::TechBuildingPositioning(FirstTech,4);

		
	}
	else if ((abs(homePosition.x - HillPosition.x) < abs(homePosition.y - HillPosition.y)) && (homePosition.y - HillPosition.y > 0)){
		// Probably 9		
		// Location of the first pylon
		FirstPylonLocation.x = 16;   // Fix
		FirstPylonLocation.y = 77;
		m_PylonTilePosition.push_back(FirstPylonLocation);

		TempTileLocation.x = FirstPylonLocation.x + Pylon.tileWidth();
		TempTileLocation.y = FirstPylonLocation.y;
		m_PylonTilePosition.push_back(TempTileLocation);

		SecondPylonLocation.x = 12;   // Fix
		SecondPylonLocation.y = 89;
		m_PylonTilePosition.push_back(SecondPylonLocation);

		TempTileLocation.x = SecondPylonLocation.x + Pylon.tileWidth();
		TempTileLocation.y = SecondPylonLocation.y;
		m_PylonTilePosition.push_back(TempTileLocation);

		// Build the gateways right to the first pylon
		// Build the gateways left to the first pylon		
		FirstGateWay.x = FirstPylonLocation.x - GateWay.tileWidth() ;
		FirstGateWay.y = FirstPylonLocation.y - GateWay.tileHeight();

		// Build the upgrading points
		FirstTech.x = FirstPylonLocation.x + 2 * Pylon.tileWidth();
		FirstTech.y = FirstPylonLocation.y;

		// Update Building Positions
		BuildingManager::PylonPositioning(FirstPylonLocation,5);
		BuildingManager::GateWayPositioning(FirstGateWay,3);
		BuildingManager::TechBuildingPositioning(FirstTech,5);

		// choke 13 72


		// Build the gateways right to the first pylon
		FirstGateWay.x = SecondPylonLocation.x - GateWay.tileWidth();
		FirstGateWay.y = SecondPylonLocation.y;

		// Build the upgrading pointsx
		FirstTech.x = SecondPylonLocation.x + 2 * Pylon.tileWidth();
		FirstTech.y = SecondPylonLocation.y;


		// Update Building Positions
		BuildingManager::PylonPositioning(SecondPylonLocation, 4);
		BuildingManager::GateWayPositioning(FirstGateWay,3);
		BuildingManager::TechBuildingPositioning(FirstTech,4);
				

	}
	else if ((abs(homePosition.x - HillPosition.x) < abs(homePosition.y - HillPosition.y)) && (homePosition.y - HillPosition.y < 0)){
		// Probably 3

		// Location of the first pylon
		FirstPylonLocation.x = 111;
		FirstPylonLocation.y = 43;
		m_PylonTilePosition.push_back(FirstPylonLocation);

		TempTileLocation.x = FirstPylonLocation.x + Pylon.tileWidth();
		TempTileLocation.y = FirstPylonLocation.y;
		m_PylonTilePosition.push_back(TempTileLocation);

		// Location of the second pylon
		SecondPylonLocation.x = 117;
		SecondPylonLocation.y = 26;
		m_PylonTilePosition.push_back(SecondPylonLocation);

		TempTileLocation.x = SecondPylonLocation.x + Pylon.tileWidth();
		TempTileLocation.y = SecondPylonLocation.y;
		m_PylonTilePosition.push_back(TempTileLocation);

		// Build the gateways left to the first pylon		
		FirstGateWay.x = FirstPylonLocation.x - GateWay.tileWidth();
		FirstGateWay.y = FirstPylonLocation.y - GateWay.tileHeight();

		// Build the upgrading points
		FirstTech.x = FirstPylonLocation.x + 2 * Pylon.tileWidth() ;
		FirstTech.y = FirstPylonLocation.y;

		// Update Building Positions
		BuildingManager::PylonPositioning(FirstPylonLocation,5);
		BuildingManager::GateWayPositioning(FirstGateWay,3);
		BuildingManager::TechBuildingPositioning(FirstTech,5);

		// Upgrade 117:32
		// choke 113 55	
		

		// Build the gateways left to the first pylon
		FirstGateWay.x = SecondPylonLocation.x - GateWay.tileWidth();
		FirstGateWay.y = SecondPylonLocation.y;

		// Build the upgrading pointsx
		FirstTech.x = SecondPylonLocation.x + 2 * Pylon.tileWidth();
		FirstTech.y = SecondPylonLocation.y;

		// Update Building Positions
		BuildingManager::PylonPositioning(SecondPylonLocation, 4);
		BuildingManager::GateWayPositioning(FirstGateWay,3);
		BuildingManager::TechBuildingPositioning(FirstTech,4);

	}

	
	std::reverse(m_PylonTilePosition.begin(), m_PylonTilePosition.end());
	std::reverse(m_GateTilePosition.begin(), m_GateTilePosition.end());
	std::reverse(m_TechBuildingTileLocation.begin(), m_TechBuildingTileLocation.end());

}


void BuildingManager::ChokeDefensePylon(){

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
	m_PylonTilePosition.insert(m_PylonTilePosition.begin() + 8, anothervector.begin(), anothervector.end());  	

}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// Class Variable Handler
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
const BWAPI::TilePosition & BuildingManager::PylonLocationPresent(){
	return m_PylonTilePosition.back();
}

const std::vector<BWAPI::TilePosition> & BuildingManager::PylonSetPresent(){
	return m_PylonTilePosition;
}

void BuildingManager::PylonLocationRemover(){
	m_PylonTilePosition.pop_back();	
}

void BuildingManager::PylonLocationSaver(const std::vector<BWAPI::TilePosition> & TempPosition){
	m_PylonTilePosition = TempPosition;
}

const std::vector<BWAPI::TilePosition> & BuildingManager::GateWayLocationPresent(){
	return m_GateTilePosition;
}

void BuildingManager::GateWayLocationSaver(const std::vector<BWAPI::TilePosition> & TempPosition){
	m_GateTilePosition = TempPosition;
}

void BuildingManager::GateWayLocationRemover(){
	m_GateTilePosition.pop_back();
}



const BWAPI::TilePosition & BuildingManager::TechLocationPresent(){	
	return m_TechBuildingTileLocation.back();
}

const std::vector<BWAPI::TilePosition> & BuildingManager::TechSetPresent(){
	return m_TechBuildingTileLocation;
}

void BuildingManager::TechLocationSaver(const std::vector<BWAPI::TilePosition> & TempPosition){
	m_TechBuildingTileLocation = TempPosition;
}


void BuildingManager::TechLocationRemover(){
	m_TechBuildingTileLocation.pop_back();
}



void BuildingManager::GetExpansionBase(BWAPI::TilePosition EnemyTilePosition, BWAPI::TilePosition HomeTilePosition){
	
	std::map<double, BWAPI::TilePosition> NexusLoc;
	std::map<double, BWAPI::TilePosition>::iterator i;

	// This is only for nexus
	BWAPI::UnitType BuildingTarget = Nexus;
	
	BWTA::BaseLocation * closestBase = nullptr;
	double minDistance = 100000;

	// for each base location
	for (BWTA::BaseLocation * base : BWTA::getBaseLocations()){
		BWAPI::TilePosition tile = base->getTilePosition();

		// // if the base has gas
		if (BWAPI::Broodwar->isBuildable(tile, true)){
			//  // get the tile position of the base
			double distanceFromHome = BWTA::getGroundDistance2(HomeTilePosition, tile) - BWTA::getGroundDistance2(EnemyTilePosition, tile);

			// if it is not connected, continue //island
			if (!BWTA::isConnected(HomeTilePosition, tile)){
				continue;
			}

			NexusLoc[distanceFromHome] = tile;
		}
	}

	// Move expansion location in the map to the vector
	for (i = NexusLoc.begin(); i != NexusLoc.end(); i++){		
		BWAPI::TilePosition TempLoc = i->second;
		BWAPI::Broodwar->sendText("Expansion Location is Updated %.2d : %.2d", TempLoc.x, TempLoc.y);
		m_ExpansionLocation.push_back(TempLoc);
	}	
	
}




const std::vector<BWAPI::TilePosition> & BuildingManager::NexusSetPresent(){
	return m_ExpansionLocation;
}


void BuildingManager::NexusLocationRemover(){
	m_ExpansionLocation.pop_back();
}







// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// Building Positioning 
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void BuildingManager::PylonPositioning(const BWAPI::TilePosition & StartingPoint,int Numbers){		
	BWAPI::TilePosition TempTileLocation;		
	for (int a = 1; a < Numbers; a = a + 1){
		TempTileLocation.x = StartingPoint.x;
		TempTileLocation.y = StartingPoint.y + a * Pylon.tileHeight();
		m_PylonTilePosition.push_back(TempTileLocation);
	}

	for (int a = 1; a < Numbers; a = a + 1){
		TempTileLocation.x = StartingPoint.x + Pylon.tileWidth();
		TempTileLocation.y = StartingPoint.y + a * Pylon.tileHeight();
		m_PylonTilePosition.push_back(TempTileLocation);
	}
}


void BuildingManager::GateWayPositioning(const BWAPI::TilePosition & StartingPoint, int Numbers){	
	BWAPI::TilePosition TempTileLocation;
	
	for (int a = 0; a < Numbers; a = a + 1){
		TempTileLocation.x = StartingPoint.x;
		TempTileLocation.y = StartingPoint.y + a * (GateWay.tileHeight());
		m_GateTilePosition.push_back(TempTileLocation);
	}
}


void BuildingManager::TechBuildingPositioning(const BWAPI::TilePosition & StartingPoint,int Numbers){
	BWAPI::TilePosition TempTileLocation;

	for (int a = 0; a < Numbers; a = a + 1){
		// Location of Cybernetics core
		TempTileLocation.x = StartingPoint.x;
		TempTileLocation.y = StartingPoint.y + a * (Cybernetics.tileHeight());
		m_TechBuildingTileLocation.push_back(TempTileLocation);
	}
}







void BuildingManager::BuildingFunction(const BWAPI::Unit & HeadQuater, const BWAPI::UnitType & BuildingTarget){
	
	std::set<BWAPI::TilePosition>::iterator iter;
	BWAPI::TilePosition Temp = BWAPI::TilePositions::None;

	for (iter = NicePlace1.begin(); iter != NicePlace1.end(); iter++){
		Temp = *iter;
		BWAPI::Broodwar->drawBoxMap(Temp.x, Temp.y, Temp.x + 15, Temp.y + 10, BWAPI::Colors::Red, false);

	}
	
	
	static int lastChecked = 0;
	// If we are supply blocked and haven't tried constructing more recently
	if (lastChecked + 100 < BWAPI::Broodwar->getFrameCount() && BWAPI::Broodwar->self()->incompleteUnitCount(BuildingTarget) == 0){
		lastChecked = BWAPI::Broodwar->getFrameCount();

		// Get Mr. Builder.
		if (!MrBuilder){
			MrBuilder = HeadQuater->getClosestUnit(BWAPI::Filter::IsWorker,999999);
		}

		BWAPI::TilePosition PossibleTilePosition = HeadQuater->getTilePosition();
		BWAPI::Unit SecondHeadQuater = HeadQuater->getClosestUnit(BWAPI::Filter::IsResourceDepot && BWAPI::Filter::IsOwned, 999999);
		if (SecondHeadQuater){
			PossibleTilePosition = SecondHeadQuater->getTilePosition();
		}

		// Determine the building locations
		std::vector<BWAPI::TilePosition>	Position;
		if (BuildingTarget == Pylon){
			BWAPI::Broodwar->sendText("Pylon Size %.2d", m_PylonTilePosition.size());

			Position = m_PylonTilePosition;
		}
		else if (BuildingTarget == GateWay || BuildingTarget == Stargate){
			Position = m_GateTilePosition;
		}
		else if (BuildingTarget == Nexus){
			Position = m_ExpansionLocation;
		}
		else{			
			Position = m_TechBuildingTileLocation;
		}


		// Determine the Building Location 
		BWAPI::TilePosition targetBuildLocation=BWAPI::TilePositions::None;
	
		// canBuildHere(Position.back(), BuildingTarget) isBuildable(Position.back(), true) 
		//// Can I build here?
		while (!Position.empty()){
			if (BWAPI::Broodwar->isBuildable(Position.back())){ // canBuildHere(Position.back(), BuildingTarget))
				break;
			}
			else{
				BWAPI::Broodwar->sendText("I cannot build here %.2d : %.2d", Position.back().x, Position.back().y);
				Position.pop_back();
			}
		}

		// There is no possible building location
		if (Position.empty()){
			targetBuildLocation = BWAPI::Broodwar->getBuildLocation(BuildingTarget, PossibleTilePosition, 32);
			BWAPI::Broodwar->sendText("Location vector is empty %.2d : %.2d", targetBuildLocation.x, targetBuildLocation.y);
		}
		// Or we can build it here !!!
		else{
			targetBuildLocation = Position.back(); // BWAPI::Broodwar->getBuildLocation(BuildingTarget, Position.back(), 4); //  
			//BWAPI::Broodwar->sendText("Location vector %.2d : %.2d", targetBuildLocation.x, targetBuildLocation.y);
			// In worst case
			if (!targetBuildLocation){
				targetBuildLocation = BWAPI::Broodwar->getBuildLocation(BuildingTarget, PossibleTilePosition , 32);
			}
		}
	

		// If a builder and location are secured
		if (MrBuilder && targetBuildLocation){

			BWAPI::Broodwar->isBuildable(targetBuildLocation);

			if (!BWAPI::Broodwar->isExplored(targetBuildLocation) && BuildingTarget.mineralPrice()*0.8 < BWAPI::Broodwar->self()->minerals()){
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



