#pragma once

#include "BuildingManager.h"
#include "ScoutManager.h"
#include "InformationManager.h"
#include "CombatManager.h"
#include "Common.h"



DWORD WINAPI AnalyzeThread();


// Remember not to use "Broodwar" in any global class constructor!

namespace Turk {
class TheTurk : public BWAPI::AIModule
{

public:

	// Virtual functions for callbacks, leave these as they are.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);

	// Everything below this line is safe to modify.
	
	void drawTerrainData();




	static TheTurk &Instance();


private:

	bool Initiate = true;


	// map analysis
	bool m_analyzed;
	bool m_analysis_just_finished;


	
	// A set of minerals: It is initiated at every starting and expansion
	BWAPI::Unitset m_MineralSets;
	

	
	

	const BWAPI::Unitset & MineralCollector(const BWTA::BaseLocation * StartingPoint);
	const BWAPI::Unitset & MineralPresent();



	// Save the locations of minerals around our bases
	void MineralSaver(const BWAPI::Unitset &);

	// A resource depot is a Command Center, Nexus, or Hatchery
	// A resource depot will generate workers up to the maximum number of workers
	void ProbeMaker(const BWAPI::Unitset &);

	// Make probes work
	void ProbeWork(int);

	// Gas Assignment
	void GasWorkerAssign(const BWAPI::Unitset &);
	
	// 
	const std::map<std::string, int> & UnitCounter();
	// end of commander section

	// Check the Build Order Miles Stone
	bool m_FirstExpansion = false;	
	bool m_FirstCybernetics = false;
	bool m_FirstForge = false;
	bool m_FirstAdun = false;
	bool m_FirstTemplarArchive = false;
	bool m_FirstFleetBeacon = false;
	bool m_FirstRobotics = false;
	bool m_FirstObservatory = false;

	
	// Scouting Information
	bool m_EnemyDetection = false;
	


	// CSC - Why???
	BWAPI::UnitType Pylon = BWAPI::UnitTypes::Protoss_Pylon;
	BWAPI::UnitType GateWay = BWAPI::UnitTypes::Protoss_Gateway;
	BWAPI::UnitType Nexus = BWAPI::UnitTypes::Protoss_Nexus;
	
	
	// Buildings	
	BWAPI::UnitType Assimilator = BWAPI::UnitTypes::Protoss_Assimilator;
	BWAPI::UnitType Forge = BWAPI::UnitTypes::Protoss_Forge;
	BWAPI::UnitType Cybernetics = BWAPI::UnitTypes::Protoss_Cybernetics_Core;
	BWAPI::UnitType CitadelOfAdun = BWAPI::UnitTypes::Protoss_Citadel_of_Adun;
	BWAPI::UnitType TemplerArchive = BWAPI::UnitTypes::Protoss_Templar_Archives;
	BWAPI::UnitType StarGate = BWAPI::UnitTypes::Protoss_Stargate;
	BWAPI::UnitType FleetBeacon = BWAPI::UnitTypes::Protoss_Fleet_Beacon;

	// Units
	BWAPI::UnitType Zealot = BWAPI::UnitTypes::Protoss_Zealot;
	BWAPI::UnitType Dragoon = BWAPI::UnitTypes::Protoss_Dragoon;
	BWAPI::UnitType Corsair = BWAPI::UnitTypes::Protoss_Corsair;

	int m_PylonLimiter = 0;
		
	int m_MaxGateWayCount = 1;	
	int m_StartClockPosition = 0;

	// Building Location Variables
	int m_BaseToPylon = 5;
	int m_PylonToChoke = 1;

	// ===================================================
	// Gas Collector Information
	int m_WorkersPerGas = 3;	
	// ===================================================

	// System Parameters
	unsigned m_MaxWorkerCount = 30;
	int m_MaxMineralDist = 400;

	// Building Positions
	BWAPI::Position m_homePosition;
	BWAPI::TilePosition m_homeTilePosition;
	BWAPI::TilePosition m_FirstExpTilePosition;
	
	BWAPI::Position m_Campus;
	BWAPI::Position m_HillPosition;
	BWAPI::Position m_HillPosition2;
	


	// Enemy Building Position
	BWAPI::TilePosition m_EnemyTileHome = BWAPI::TilePositions::None;
	
	BWAPI::TilePosition m_EnemyTileExpansion = BWAPI::TilePositions::None;
	BWAPI::Position m_EnemyExpansion = BWAPI::Positions::None;

	BWAPI::Position m_EnemyHillPosition = BWAPI::Positions::None;


	// $$$$$$$$$$$$$$$$$$$$
	// War Flags
	bool m_CorsairAttackTriger = false;
	bool m_EnemyBaseUpdate = false;	



	// Units
	// CSC - Why??
	BWAPI::Unit ResourceDepot = nullptr;

};  // end TheTurk class declaration

} // end Turk namespace
