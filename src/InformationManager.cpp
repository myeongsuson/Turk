#include "InformationManager.h"
#include "CombatManager.h"

using namespace Turk;



InformationManager::InformationManager()
// : FirstScout(false)
{


}


//Commander::~Commander()
//{
//}

InformationManager & InformationManager::Instance()
{
	static InformationManager instance;
	return instance;
}


// Commander functions
// Collect all valid units except dead bodies
void InformationManager::ValidUnitCollector(const BWAPI::Unit & ScouterUnit){

	int Pylon_Count = 0;
	int GateWay_Count = 0;
	int StarGate_Count = 0;
	int Nexus_Count = 0;
	int Probe_Count = 0;

	m_ValidUnits.clear();
	m_BaseUnits.clear();
	m_WorkerUnits.clear();

	
	for (auto &unit : BWAPI::Broodwar->self()->getUnits()){
		if (IsValidUnit(unit)){
			m_ValidUnits.insert(unit);

			if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus){
				m_BaseUnits.insert(unit);

				Nexus_Count = Nexus_Count + 1;
			}
			else if (unit->getType().isWorker()){
				Probe_Count = Probe_Count + 1;
				m_WorkerUnits.insert(unit);
			}
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Pylon)
			{
				Pylon_Count = Pylon_Count + 1;
			}
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Gateway)
			{
				GateWay_Count = GateWay_Count + 1;
			}
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Stargate){
				StarGate_Count = StarGate_Count + 1;
			}
		}
	}
	if (ScouterUnit){
		m_WorkerUnits.erase(ScouterUnit);
	}

	m_UnitCount["Probe_Count"] = Probe_Count;	
	m_UnitCount["Pylon_Count"] = Pylon_Count;
	m_UnitCount["GateWay_Count"] = GateWay_Count;
	m_UnitCount["StarGate_Count"] = StarGate_Count;
	m_UnitCount["Nexus_Count"] = Nexus_Count;

	
	// Combine other counting infos
	std::map<std::string, int> m_CombatUnitCount = CombatManager::Instance().GetUnitCount();
	m_UnitCount.insert(m_CombatUnitCount.begin(), m_CombatUnitCount.end());


	// Future unit sets
	// SetScoutUnits();
	// SetBaseCombatUnits();
	// SetFieldCombatUnits();
}


// unit represent the point.
bool InformationManager::IsValidUnit(const BWAPI::Unit & unit){
	if (!unit){
		return false;
	}

	if (!unit->exists()){
		return false;
	}

	// Ignore the unit if it has one of the following status ailments
	if (unit->isLockedDown() || unit->isMaelstrommed() || unit->isStasised()){
		return false;
	}

	// Ignore the unit if it is in one of the following states
	if (unit->isLoaded() || !unit->isPowered() || unit->isStuck()){
		return false;
	}

	// Ignore the unit if it is incomplete or busy constructing
	if (!unit->isCompleted() || unit->isConstructing()){
		return false;
	}

	// Find real units
	if (unit->isCompleted()
		&& unit->getHitPoints() > 0
		&& unit->exists()
		&& unit->getType() != BWAPI::UnitTypes::Unknown
		&& unit->getPosition().x != BWAPI::Positions::Unknown.x
		&& unit->getPosition().y != BWAPI::Positions::Unknown.y){
		return true;
	}
	else
	{
		return false;
	}
}




void InformationManager::GetEnemyBuildingUpdate(){
	
	bool Indicator = true;

	for (auto &unit : BWAPI::Broodwar->enemy()->getUnits()){
		if (unit){
			if (unit->exists() && unit->getType().isBuilding()){

				// If it is a resource depot, save its location
				if (unit->getType().isResourceDepot()){
					EnemyBaseSaver(unit->getTilePosition());
				}

				// Save Locations of Tech Buildings
				if (IsTechBuilding(unit)){

					// Only Save it when it is new one!
					std::vector<BWAPI::Unit>::iterator	Iter;
					for (Iter = m_EnemyTechBuilding.begin(); Iter != m_EnemyTechBuilding.end(); Iter++){
						BWAPI::Unit Temp = *Iter;
						if (Temp == unit){
							Indicator = false;
							break;
						}						
					}
					// Save unit into enemy building tech.
					if (Indicator){
						m_EnemyTechBuilding.push_back(unit);
					}
					
				}


			}
		}
	}

	
}




bool InformationManager::IsTechBuilding(BWAPI::Unit unit){

	if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg){
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool ||
			unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den ||
			unit->getType() == BWAPI::UnitTypes::Zerg_Spire ||
			unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery ||
			unit->getType() == BWAPI::UnitTypes::Zerg_Hive ||
			unit->getType() == BWAPI::UnitTypes::Zerg_Lair ||
			unit->getType() == BWAPI::UnitTypes::Zerg_Defiler_Mound ||
			unit->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber ||
			unit->getType() == BWAPI::UnitTypes::Zerg_Greater_Spire ||
			unit->getType() == BWAPI::UnitTypes::Zerg_Queens_Nest)

			return true;

	}
	else if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Protoss){

		return true;

	}
	else if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Terran){

		return true;

	}
	else{
		return false;
	}


}




const BWAPI::Unitset & InformationManager::UnitSetPresent(){
	return m_ValidUnits;
}


const BWAPI::Unitset & InformationManager::BasePresent(){
	return m_BaseUnits;
}


const BWAPI::Unitset & InformationManager::WorkerPresent(){
	return m_WorkerUnits;
}


const std::map<std::string, int> & InformationManager::UnitCounterPresenter(){
	return m_UnitCount;
}







void InformationManager::EnemyBaseSaver(BWAPI::TilePosition Base){
	

	bool Indicator1 = true;
	bool Indicator2 = false;

	// This is the first time to enter into this.
	if (EnemyBaseInit){
		m_EnmBaseTilePos.clear();
		EnemyBaseInit = false;

		// This Base might be a main base
		m_EnmBaseTilePos.push_back(Base);

		// These infomation does not change in the game.
		BWAPI::Position m_EnemyHillPosition = BWTA::getNearestChokepoint(Base)->getCenter();
		BWAPI::TilePosition m_EnemyTileExpansion = BWTA::getNearestBaseLocation(m_EnemyHillPosition)->getTilePosition();

		// Save the expansion Tile
		m_EnmBaseTilePos.push_back(m_EnemyTileExpansion);		
		// BWTA::getRegion(Base)->getReachableRegions
	}
	else{


		if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg){
			
			// Detect whether it is additional hatchery or not.			
			for (BWTA::BaseLocation * baseLocation : BWTA::getBaseLocations()){
				BWAPI::TilePosition targetPosition = baseLocation->getTilePosition();

				if (targetPosition == Base){
					Indicator1 = false;
					break;
				}				
			}

		}
		
		
		// After the second expansion, please compare and save it
		std::vector<BWAPI::TilePosition>::iterator	Iter;
		for (Iter = m_EnmBaseTilePos.begin(); Iter != m_EnmBaseTilePos.end(); Iter++){
			BWAPI::TilePosition Temp = *Iter;
			if (Temp == Base){
				Indicator2 = false;
				break;
			}
			else{
				Indicator2 = true;
			}
		}

		// Final Save
		if (Indicator1 && Indicator2){
			m_EnmBaseTilePos.push_back(Base);
		}
		
	}
}



BWAPI::TilePosition InformationManager::GetEnemyBase(int Loc){
	return m_EnmBaseTilePos.at(Loc);
}



std::vector<BWAPI::TilePosition> InformationManager::GetEnemyBase(){
	return m_EnmBaseTilePos;
}