#include "InformationManager.h"

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

