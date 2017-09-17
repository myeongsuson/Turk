#pragma once

#include "CombatManager.h"
#include "Common.h"
#include "InformationManager.h"




using namespace Turk;


CombatManager::CombatManager()
// : FirstScout(false)
{


}


CombatManager & CombatManager::Instance()
{
	static CombatManager instance;
	return instance;
}


void CombatManager::CombatActionUpdate(){

	// Our Home Position
	m_homePosition = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());

	// Enemy Base Position
	BWAPI::TilePosition m_EnemyTileHome = InformationManager::Instance().GetEnemyBase(0);
	EnemyHome = BWAPI::Position(m_EnemyTileHome);
	
	
	// Enemy First Expansion Position
	BWAPI::TilePosition m_EnemyTileExpansion = InformationManager::Instance().GetEnemyBase(1);
	EnemyExp = BWAPI::Position(m_EnemyTileExpansion);
	
	
	// Valid Our Unit Sets
	BWAPI::Unitset m_ValidUnits = InformationManager::Instance().UnitSetPresent();

	
	
	
	// 1. Collect possible combat units
	CombatUnitCollector(m_ValidUnits);




	//// Reunion After Loops
	//static int lastChecked = 0;
	//if (lastChecked + 2000 < BWAPI::Broodwar->getFrameCount() && !m_CorsairUnits.empty()){
	//	lastChecked = BWAPI::Broodwar->getFrameCount();
	//	BWAPI::Broodwar << "Let's reunion." << std::endl;;
	//	m_CorsairUnits.move(m_homePosition);
	//}



	// 2. Corsair Action If the enemy is Zerg
	if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg){

		// Indiivdual Play Around
		for (auto & unit : m_CorsairUnits){
			CorsairUpdate(unit);
		}

		//Wolf Pack
		if (!m_CorsairUnits.empty()){
			for (auto & unit : BWAPI::Broodwar->enemy()->getUnits()){
				if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord || unit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk){

					BWAPI::Unit Protector = unit->getClosestUnit((BWAPI::Filter::GetType == BWAPI::UnitTypes::Zerg_Hydralisk ||
						BWAPI::Filter::GetType == BWAPI::UnitTypes::Zerg_Spore_Colony) && BWAPI::Filter::IsEnemy, 320);

					if (!Protector || Protector->isUnderDisruptionWeb()){
						m_CorsairUnits.attack(unit->getPosition());
					}
				}
			}
		}


	}




	// Final Attack
	RussianUra();


			




}



// Commander functions
// Collect all valid units except dead bodies
void CombatManager::CombatUnitCollector(BWAPI::Unitset m_ValidUnits){
	
	// Count Number of Combat Units
	int Zealot_Count = 0;
	int Dragoon_Count = 0;
	int Corsair_Count = 0;
	int HighTempler_Count = 0;
	int Archon_Count = 0;
	int DarkTempler_Count = 0;

	// Clear Sets
	m_ZealotUnits.clear();
	m_DragooUnits.clear();
	m_CorsairUnits.clear();
	m_HighTemUnits.clear();
	m_ArchonUnits.clear();
	m_DarkTemUnits.clear();
		
	for (auto &unit : m_ValidUnits){
		// Zealot
		if (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot){
			Zealot_Count = Zealot_Count + 1;
			m_ZealotUnits.insert(unit);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon){
			Dragoon_Count = Dragoon_Count + 1;
			m_DragooUnits.insert(unit);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Corsair){
			Corsair_Count = Corsair_Count + 1;
			m_CorsairUnits.insert(unit);
		}
		
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_High_Templar){
			HighTempler_Count = HighTempler_Count + 1;
			m_HighTemUnits.insert(unit);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Archon){
			Archon_Count = Archon_Count + 1;
			m_ArchonUnits.insert(unit);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar){
			DarkTempler_Count = DarkTempler_Count + 1;
			m_DarkTemUnits.insert(unit);
		}
		
	}	

	m_CombatUnitCount["Zealot_Count"] = Zealot_Count;
	m_CombatUnitCount["Dragoon_Count"] = Dragoon_Count;
	m_CombatUnitCount["Corsair_Count"] = Corsair_Count;
	m_CombatUnitCount["HighTempler_Count"] = HighTempler_Count;
	m_CombatUnitCount["Archon_Count"] = Archon_Count;
	m_CombatUnitCount["DarkTempler_Count"] = DarkTempler_Count;


		
}


std::map<std::string, int> CombatManager::GetUnitCount(){
	return m_CombatUnitCount;
}










void CombatManager::CorsairUpdate(BWAPI::Unit unit){

	// Scouter Search Position
	std::vector<BWAPI::Position>	SearchingPosition;

	int RandomDecision = 0;

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$ Attack Process $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$	
	// Determine whether a scouter is in attack mode or searching mode.
	// Operation Wolf Pack
	
				
	bool RogerThat = false;
		
	//1. We are under attack.
	if (unit->isUnderAttack()){
		m_CorsairSearchMode = false;

		BWAPI::Unit Enemy = unit->getClosestUnit( (BWAPI::Filter::IsAttacking || BWAPI::Filter::IsStartingAttack )&& BWAPI::Filter::IsEnemy);
		if (Enemy){
			if (Enemy->getType() == BWAPI::UnitTypes::Zerg_Hydralisk || Enemy->getType() == BWAPI::UnitTypes::Zerg_Spore_Colony){

				// Check whether we can use web
				if (unit->canUseTech(BWAPI::TechTypes::Disruption_Web, Enemy, true, true, true, true) && !Enemy->isUnderDisruptionWeb()){
					BWAPI::Broodwar->sendText("Use Technique");
					unit->useTech(BWAPI::TechTypes::Disruption_Web,Enemy);
				}
				else{
					BWAPI::Broodwar->sendText("Under Attack! Run Away");
					unit->move(m_homePosition);
					return;
				}

			}
			else if (Enemy->getType() == BWAPI::UnitTypes::Zerg_Mutalisk){
				unit->attack(Enemy);
				return;
			}

		}
	}


	// Dodge this
	BWAPI::Unit Enemy = unit->getClosestUnit((BWAPI::Filter::GetType == BWAPI::UnitTypes::Zerg_Spore_Colony || 
												BWAPI::Filter::GetType == BWAPI::UnitTypes::Zerg_Hydralisk) && BWAPI::Filter::IsEnemy,320);

	if (Enemy){
			 
		m_CorsairSearchMode = false;

		if (Enemy->isInWeaponRange(unit) || !Enemy->isUnderDisruptionWeb()){
			// We can use a web
			if (unit->canUseTech(BWAPI::TechTypes::Disruption_Web, Enemy, true, true, true, true) && !Enemy->isUnderDisruptionWeb()){
				unit->useTech(BWAPI::TechTypes::Disruption_Web,Enemy);
				BWAPI::Broodwar->sendText("We found an enemy, and it is not under a web, so we used a web");
				return;
			}
			// We cannot use a web, so just run
			else{

				// Dodge to the 180 degree against to an enemy unit
				BWAPI::Position EnemyPosition = Enemy->getPosition();
				BWAPI::Position MyPosition = unit->getPosition();
				BWAPI::Position NextPosition = BWAPI::Positions::None;

				int XValue = (MyPosition.y - EnemyPosition.y) * 10000;
				int YValue = (MyPosition.x - EnemyPosition.x) * 10000;

				XValue = round(max(min(XValue, BWAPI::Broodwar->mapHeight()), 1));
				YValue = round(max(min(YValue, BWAPI::Broodwar->mapWidth()), 1));

				NextPosition.x = YValue;
				NextPosition.y = XValue;

				unit->move(NextPosition);
				return;
			}
		}
		else if (Enemy->isInWeaponRange(unit) && Enemy->isUnderDisruptionWeb()){
			// Search our target
			BWAPI::Unit Target = unit->getClosestUnit((BWAPI::Filter::GetType == BWAPI::UnitTypes::Zerg_Overlord ||
				BWAPI::Filter::GetType == BWAPI::UnitTypes::Zerg_Scourge || BWAPI::Filter::GetType == BWAPI::UnitTypes::Zerg_Mutalisk) && BWAPI::Filter::IsEnemy);
			if (Target){
				unit->attack(Target);
				return;
			}
		}
	}
	
		
		


		
	if (unit->isIdle()){
		BWAPI::Position MyPosition = unit->getPosition();

		double dist = unit->getDistance(EnemyHome);
		double dist2 = unit->getDistance(EnemyExp);
		double dist3 = unit->getDistance(m_homePosition);

		// We are in the Base!
		if (dist3 < 10){

			// Enough Waiting. It is time to go
			static int lastChecked = 0;
			if (lastChecked + 500 < BWAPI::Broodwar->getFrameCount()){
				lastChecked = BWAPI::Broodwar->getFrameCount();
					
				// Get polygons vectors;
				BWTA::Polygon EnemyPolygon = BWTA::getRegion(EnemyHome)->getPolygon();

				int RandomPoint = rand() % EnemyPolygon.size();
				BWAPI::Position TempPosition1 = EnemyPolygon[RandomPoint];
					
				unit->patrol(TempPosition1);
				return;

			}
			// You should stay there
			else{
				return;
			}
		}

		// We are in the enemy Base
		else if (dist < 10){
			// Go get him.
			unit->attack(EnemyExp);
			return;
		}
		// I am at enemy's expansion
		else if (dist2 < 100){
			m_CorsairSearchMode = true;
			MyPosition.x = 1;
			MyPosition.y = 1;
			unit->attack(MyPosition);
			return;
		}
		// If you are in the edge
		else if ((MyPosition.x < 100 && MyPosition.y > (BWAPI::Broodwar->mapHeight() * 32 - 10)) || (MyPosition.x > (BWAPI::Broodwar->mapWidth() * 32 - 10) && MyPosition.y < 100)){
			MyPosition.x = 1;
			MyPosition.y = 1;
			unit->attack(MyPosition);
			return;
		}
		// We are already in the zero point
		else if (MyPosition.x < 100 && MyPosition.y < 100){
			int RandomPoint = rand() % 3;

			if (RandomPoint == 0){
				MyPosition.x = 1;
				MyPosition.y = BWAPI::Broodwar->mapHeight()*32 - 2;
				unit->attack(MyPosition);
				return;
			}
			else if (RandomPoint == 1){
				MyPosition.x = BWAPI::Broodwar->mapWidth()*32 - 2;
				MyPosition.y = 1;
				unit->attack(MyPosition);
				return;
			}
			else if (RandomPoint == 2){
				MyPosition.x = BWAPI::Broodwar->mapWidth() *32;
				MyPosition.y = BWAPI::Broodwar->mapHeight() * 32;;
				unit->attack(MyPosition);
				return;
			}				
		}
						
		else{
			RandomDecision = rand() % 10;

			if (RandomDecision == 0){
				unit->attack(EnemyHome);
				return;
			}
			else if (RandomDecision == 1){
				BWAPI::Position MyPosition = unit->getPosition();
				MyPosition.x = BWAPI::Broodwar->mapWidth()*32 - 1;
				MyPosition.y = BWAPI::Broodwar->mapHeight() * 32 - 1;
				unit->attack(MyPosition);
				return;
			}
			else if (RandomDecision == 2){
				BWAPI::Position MyPosition = unit->getPosition();
				MyPosition.x = BWAPI::Broodwar->mapWidth()*32 - 1;;
				MyPosition.y = 1;
				unit->attack(MyPosition);
				return;
			}
			else if (RandomDecision == 3){
				BWAPI::Position MyPosition = unit->getPosition();
				MyPosition.x = 1;
				MyPosition.y = BWAPI::Broodwar->mapHeight() * 32 - 1;;
				unit->attack(MyPosition);
				return;
			}
			else if (RandomDecision == 4){
				BWAPI::Position MyPosition = unit->getPosition();
				MyPosition.x = 1;
				MyPosition.y = 1;
				unit->attack(MyPosition);
				return;
			}
			else if (RandomDecision == 5){
				unit->attack(m_homePosition);
				return;
			}
			else{
				m_CorsairSearchMode = false;
			}

		}

	}


	// Search Mode
	if (m_CorsairSearchMode && !SearchingPosition.empty()){
		BWAPI::Position targetPosition = SearchingPosition.back();
			
		double dist = unit->getDistance(targetPosition);
		if (dist < 10){
			SearchingPosition.pop_back();
				
			if (SearchingPosition.empty()){
				m_CorsairSearchMode = false;
				for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations()){
					BWAPI::Position targetPosition = startLocation->getPosition();
					SearchingPosition.push_back(targetPosition);
				}
			}

			return;
		}
		else{
			unit->attack(targetPosition, true);
			return;
		}
	}

	


}



void CombatManager::ArchonGenerator(){


	// Archon generator
	if (m_CombatUnitCount["HighTempler_Count"] > 6){
		for (auto & unit : m_HighTemUnits){
			for (auto & unit2 : m_HighTemUnits){
				if (unit != unit2 && !unit->isMoving() && !unit2->isMoving()){
					unit->useTech(BWAPI::TechTypes::Archon_Warp, unit2);
				}
			}
		}
	}


}






// Final Rush. Do Not Think.
void CombatManager::RussianUra(){
		
	// Attack
	if (BWAPI::Broodwar->self()->supplyUsed() / 2 > 180 && EnemyHome){
		m_ZealotUnits.patrol(EnemyHome);
		m_DragooUnits.patrol(EnemyHome);
		m_ArchonUnits.patrol(EnemyHome);
		m_HighTemUnits.patrol(EnemyHome);
		return;
	}
	else if (BWAPI::Broodwar->self()->supplyUsed() / 2 > 120 && BWAPI::Broodwar->self()->supplyUsed() / 2 < 128){
		BWAPI::Position CenterPoint((BWAPI::Broodwar->mapHeight()) * 16, (BWAPI::Broodwar->mapWidth()) * 16);
		m_ZealotUnits.patrol(CenterPoint);
		m_DragooUnits.patrol(CenterPoint);
		m_ArchonUnits.patrol(CenterPoint);
		m_HighTemUnits.patrol(CenterPoint);
		return;
	}









}
