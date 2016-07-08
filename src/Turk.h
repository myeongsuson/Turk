#pragma once

#include "BuildingManager.h"
#include "ScoutManager.h"
#include "Commander.h"
#include "Common.h"



DWORD WINAPI AnalyzeThread();


// Remember not to use "Broodwar" in any global class constructor!

namespace Turk {
class TheTurk : public BWAPI::AIModule
{

	Commander  _Commander;

	// Check the Build Order Miles Stone
	
	bool FirstExpansion = false;
	bool FirstGasExist = false;
	bool FirstCybernetics = false;
	bool FirstForge = false;
	bool FirstAdun = false;
	bool FirstTemplerArchive = false;
	bool FirstFleetBeacon = false;


	// Upgrade Situation: One Time and gone forever
	bool Singularity = false;
	bool Leg_Enhancements = false;
	


	// Scouting Information
	bool EnemyDetection = false;	
	bool CorsairSearchMode = false;



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

	int PylonLimiter = 0;
	
	
	
	int MaxGateWayCount = 1;	
	int StartClockPosition = 0;



	// Building Location Variables
	int BaseToPylon = 5;
	int PylonToChoke = 1;


	std::vector<BWAPI::Position>	SearchingPosition;

	// ===================================================
	// Gas Collector Information
	int WorkersPerGas = 3;	
	// ===================================================




	// System Parameters
	unsigned MaxWorkerCount = 30;
	int MaxMineralDist = 400;


	
	

	// Building Positions
	BWAPI::Position homePosition;
	BWAPI::TilePosition homeTilePosition;
	BWAPI::Position FirstExpansionPosition;
	BWAPI::Position Campus;
	BWAPI::Position HillPosition;
	BWAPI::Position HillPosition2;
	BWAPI::TilePosition NextExpansion;
	
	// Enemy Building Position
	BWAPI::Position EnemyHomeBase = BWAPI::Positions::None;
	BWAPI::Position EnemyHillPosition = BWAPI::Positions::None;
	BWAPI::Position EnemyExpansion = BWAPI::Positions::None;


	// $$$$$$$$$$$$$$$$$$$$
	// War Flags
	bool CorsairAttackTriger = false;

	bool EnemyBaseUpdate = false;


	// Units
	
	BWAPI::Unit ResourceDepot = nullptr;




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
	void drawTerrainData();
	// Everything below this line is safe to modify.

};  // end TheTurk class declaration

} // end Turk namespace
