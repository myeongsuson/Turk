#include "ScoutManager.h"


using namespace Turk;


ScoutManager::ScoutManager()
	:m_Scouter(nullptr)
{

}


ScoutManager & ScoutManager::Instance()
{
	static ScoutManager instance;
	return instance;
}


// Save Scouter Pointer
void ScoutManager::ScouterSaver(const BWAPI::Unit & ScouterUnit){
	m_Scouter = ScouterUnit;
}


// Return Scouter Pointer
const BWAPI::Unit & ScoutManager::ScouterPresent(){
	return m_Scouter;
}



bool ScoutManager::EnemyFirstDetector(bool Detector, const BWAPI::TilePosition & Expansion){
	
	
	

	// Please Check the Corner too !
	BWAPI::TilePosition Right;
	Right.x = Expansion.x + 4;
	Right.y = Expansion.y + 4;

	// Before you leave to scout, please visit and uncover our expansion base first.
	// This will reduce the burden of MrBuilder to explore the first expansion area.
	if (!BWAPI::Broodwar->isExplored(Expansion) || !BWAPI::Broodwar->isExplored(Right)){
		BWAPI::Broodwar->sendText("Please Check our First Expansion Site: %.2d %.2d", Expansion.x, Expansion.y);
		m_Scouter->move(BWAPI::Position(Expansion));
		return Detector;
	}


	// First Detect the enemy base	
	// Collect enemy building information		
	for (auto & unit : BWAPI::Broodwar->enemy()->getUnits()){
		if (unit->getType().isBuilding()){
			BWAPI::Position CurrentPosition = unit->getPosition();
			BWAPI::Broodwar->sendText("We Found it. and My current position is %.2d %.2d", CurrentPosition.x, CurrentPosition.y);

			// Further investigate until you find the starting location.
			if (BWTA::getNearestBaseLocation(unit->getPosition())->isStartLocation()){
				Detector = true;
				return Detector;
			}
		}
	}

	if (!Detector){
		// Refil the location
		if (m_StartingPosition.empty()){
			GetStartingPoint();
		}
		// Start to search
		BWAPI::Position targetPosition = m_StartingPosition.back();
		if (BWAPI::Broodwar->isExplored(BWAPI::TilePosition(targetPosition))){			
			m_StartingPosition.pop_back();
			return Detector;
		}		
		else{
			m_Scouter->move(targetPosition);			
			Detector = false;
			return Detector;
		}
	}	
	else{
		Detector = true;
		return Detector;
	}
}



void ScoutManager::GetStartingPoint(){

	m_StartingPosition.clear();
	// All Starting Positions without island	
	for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations()){			
		if (!startLocation->isIsland()){
			BWAPI::Position targetPosition = startLocation->getPosition();
			m_StartingPosition.push_back(targetPosition);
		}
	}
	
}



void ScoutManager::GetBasePoint(){

	m_BasePosition.clear();
	// All Starting Positions without island	
	for (BWTA::BaseLocation * baseLocation : BWTA::getBaseLocations()){
		if (!baseLocation->isIsland()){
			BWAPI::Position targetPosition = baseLocation->getPosition();
			m_BasePosition.push_back(targetPosition);
		}
	}

}



//BWAPI::Broodwar->sendText("Enemy Polygon Size %d", EnemyPolygon.size());



void ScoutManager::GetScoutRoots(BWAPI::Position EnemyHomeBase){
	
	// Get enemy base center
	BWAPI::Position EnemyCenter = BWTA::getRegion(EnemyHomeBase)->getCenter();

	// Get polygons vectors;
	BWTA::Polygon EnemyPolygon = BWTA::getRegion(EnemyHomeBase)->getPolygon();
	

	// getPerimeter Initially
	m_ScoutRotationRoot.clear();
	unsigned PolygonSize = EnemyPolygon.size();	
	int SamplingRate = 8; // PolygonSize / OurRootSize
	for (size_t j = 0; j < PolygonSize; ++j) {		
		if (j % SamplingRate == 0){

			BWAPI::Position TempPosition1 = EnemyPolygon[j];
			BWAPI::Position TempPosition2 = EnemyPolygon[j];

			TempPosition2.x = (EnemyCenter.x * 1 + TempPosition1.x * 5) / 6;
			TempPosition2.y = (EnemyCenter.y * 1 + TempPosition1.y * 5) / 6;
			m_ScoutRotationRoot.push_back(TempPosition2);

		}
		
	}

	// BWAPI::Broodwar->sendText("Enemy new Polygon Size %.2d", m_ScoutRotationRoot.size());

}


void ScoutManager::ScoutActionUpdate(BWAPI::TilePosition EnemyTileHome){

	BWAPI::Position EnemyPosition = BWAPI::Position(EnemyTileHome);
	BWAPI::Position m_EnemyHillPosition = BWTA::getNearestChokepoint(EnemyPosition)->getCenter();
	BWAPI::TilePosition m_EnemyTileExpansion = BWTA::getNearestBaseLocation(m_EnemyHillPosition)->getTilePosition();
	BWAPI::Position m_EnemyExpansion = BWAPI::Position(m_EnemyTileExpansion);


	//BWAPI::Broodwar->sendText("Under the journey %.2d", UnderJourney);


	// Top Priority: Whatch out enemy's defense tower
	if (!UnderJourney){

		// First thing is to watch out enemy base
		for (auto & unit : BWAPI::Broodwar->enemy()->getUnits()){
			// I can see the omen of detecting buildings.
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Creep_Colony){
				BWAPI::Broodwar->sendText("Creep Colony is detected");
				RoutInitiate = true;
				break;
			}
			// There is a detecting building. Run Away.
			else if (unit->getType() == BWAPI::UnitTypes::Zerg_Sunken_Colony){
				if (unit->isMorphing() || unit->isCompleted()){
					BWAPI::Broodwar->sendText("Sunken Colony is detected");
					// It is enough. Run Away.
					UnderJourney = true;
					RoutInitiate = false; // It is time to journey, so do not walk around enemy base.

					// Run a way to a center
					BWAPI::Position Center = BWAPI::Positions::None;
					Center.y = (BWAPI::Broodwar->mapHeight()) * 16;
					Center.x = (BWAPI::Broodwar->mapWidth()) * 16;
					m_Scouter->move(Center);
					return;
				}
			}
		}
	}
	
			


	// Second Priority: Somebody is hitting me. Under Attack
	if (m_Scouter->isUnderAttack()){
		UnderJourney = true;

		// Run a way to a center
		BWAPI::Position Center = BWAPI::Positions::None;
		Center.y = (BWAPI::Broodwar->mapHeight()) * 16;
		Center.x = (BWAPI::Broodwar->mapWidth()) * 16;
		m_Scouter->move(Center);
		return;
	}



	// Give orders only when a unit is doing nothing.
	if (m_Scouter->isIdle()){
		double dist = m_Scouter->getDistance(EnemyPosition);
		double dist2 = m_Scouter->getDistance(m_EnemyExpansion);
		
		if (RoutInitiate){

			// Refill the circulation location
			if (m_ScoutRotationRoot.empty()){
				GetScoutRoots(EnemyPosition);
			}

			// Let's Circulate.			
			BWAPI::Position NextMovingPoint = m_ScoutRotationRoot.back();
			m_Scouter->move(NextMovingPoint);
			m_ScoutRotationRoot.pop_back();
			
			// Enemy Base Scouting is done.
			if (m_ScoutRotationRoot.empty()){
				RoutInitiate = false;
			}

			return;
		}		
		// Go around
		else if (UnderJourney){
			RoutInitiate = false;

			BWAPI::Broodwar->sendText("We are in the under journey");

			if (m_BasePosition.empty()){
				GetBasePoint();
			}

			// Start to search
			BWAPI::Position targetPosition = m_BasePosition.back();

			// We don't have to visit enemy site again. It is a suicide mission.
			if (targetPosition == EnemyPosition || targetPosition == m_EnemyExpansion){
				BWAPI::Broodwar->sendText("This is enemy base %.2d %.2d", EnemyPosition.x, EnemyPosition.y);

				m_BasePosition.pop_back();
				targetPosition = m_BasePosition.back();
			}

			BWAPI::Broodwar->sendText("Under Journey. Please move to this position %.2d %.2d", targetPosition.x, targetPosition.y);

			m_Scouter->move(targetPosition);
			m_BasePosition.pop_back();

			return;			
		}
		// We are in the enemy base, but nothing happen
				
		if (dist < 100){
			BWAPI::Broodwar->sendText("We are in the enemy base %d", dist);

			m_Scouter->move(m_EnemyExpansion);
			return;
		}
		else if (dist2 < 100){
			BWAPI::Broodwar->sendText("We are in the enemy Expansion,  %.2d %.2d", m_EnemyExpansion.x, m_EnemyExpansion.y);
			m_Scouter->move(EnemyPosition);
			RoutInitiate = true;
			return;
		}
		// Mover Forward. Maybe we are in the enemy's expansion site
		else{
			BWAPI::Broodwar->sendText("We are in the enemy base,  %.2d %.2d", EnemyPosition.x, EnemyPosition.y);
			m_Scouter->move(m_EnemyExpansion);
			return;
		}
	}

	
	return;


}





void ScoutManager::ObserverManager(BWAPI::Unitset ValidUnitSet){


	int Observer_Count = 0;
	
	// Calculate the number of observers and attach it.
	for (auto & unit : ValidUnitSet){
		if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer){
			Observer_Count = Observer_Count + 1;

			/// Follow units
			BWAPI::Unit ClosetOne = unit->getClosestUnit(BWAPI::Filter::GetType == BWAPI::UnitTypes::Protoss_Dragoon && BWAPI::Filter::IsOwned);
			unit->follow(ClosetOne);

		}
	}


	if (Observer_Count < 5){
		for (auto & unit : ValidUnitSet){
			if (unit->getType().isBuilding()){
				if (unit->isIdle() && unit->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility){
					unit->train(BWAPI::UnitTypes::Protoss_Observer); 
					return;
				}
			}
		}
	}

	








}