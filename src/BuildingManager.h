#pragma once
#include "Common.h"


namespace Turk {

	// Commander Class
	class BuildingManager
	{

	public:
		BuildingManager();
		~BuildingManager(){};


		// In the beginning of the game, determine the building location
		void InitialBuildLocSet();
		std::set<BWAPI::TilePosition> BuildingManager::SubChecker(int, int, std::set<BWAPI::TilePosition>);


		//
		void PythonBuildingLocation();


		// Determine the building location depending on the map info
		void MapConnector();

		// $$$$$$$$$    Building Positining        $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		// Determine the location of pylons
		void PylonPositioning(const BWAPI::TilePosition &, int);

		// Determine the location of gateways
		void GateWayPositioning(const BWAPI::TilePosition &, int);

		// Determine the location of technical building
		void TechBuildingPositioning(const BWAPI::TilePosition &, int);


		// $$$$$$$$ Special Pylon Buliding Locations     $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		// Build pylons at both ends of minerals
		void DefensePylonLocation(const BWAPI::Unitset &);

		void ChokeDefensePylon();

		// $$$$$$$$$$$$$$$$$$$$$$$$$ Class Variable Handler $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		// Pylon Location Handler
		const BWAPI::TilePosition & PylonLocationPresent();
		const std::vector<BWAPI::TilePosition> & PylonSetPresent();
		void PylonLocationSaver(const std::vector<BWAPI::TilePosition> &);
		void PylonLocationRemover(BWAPI::TilePosition);

		// GateWay Location Handler
		const std::vector<BWAPI::TilePosition> & GateWayLocationPresent();
		void GateWayLocationSaver(const std::vector<BWAPI::TilePosition> &);
		void GateWayLocationRemover(BWAPI::TilePosition);

		// Tech Building Locations
		const BWAPI::TilePosition & TechLocationPresent();
		const std::vector<BWAPI::TilePosition> & TechSetPresent();
		void TechLocationSaver(const std::vector<BWAPI::TilePosition> &);
		void TechLocationRemover(BWAPI::TilePosition);

		const std::vector<BWAPI::TilePosition> & NexusSetPresent();
		void NexusLocationRemover(BWAPI::TilePosition);

		// Remove the building locations from the vector
		void LocationRemover(BWAPI::Unit);

		// Plot building locations
		void BuildingLocDrawer();

		void GetExpansionBase(BWAPI::TilePosition EnemyTilePosition, BWAPI::TilePosition HomeTilePositio);

		// Building Function
		void BuildingFunction(const BWAPI::Unit &, const BWAPI::UnitType &);
		void MrBuilderRemover();

		static BuildingManager &Instance();

	private:
		// Pylon Location Vector
		std::vector<BWAPI::TilePosition>	m_PylonTilePosition;

		// GateWay Location Vector
		std::vector<BWAPI::TilePosition>	m_GateTilePosition;

		// Technical buiding location Vector
		std::vector<BWAPI::TilePosition>	m_TechBuildingTileLocation;

		// Expansion Site
		std::vector<BWAPI::TilePosition> m_ExpansionLocation;

		// Automatic Building Location Selector
		std::set<BWAPI::TilePosition>	TileContainer;
		std::set<BWAPI::TilePosition>	NicePlace1;
		std::set<BWAPI::TilePosition>	NicePlace2;
		std::set<BWAPI::TilePosition>	NicePlace3;
		std::set<BWAPI::TilePosition>	NicePlace4;
		std::set<BWAPI::TilePosition>	NicePlace5;
		std::set<BWAPI::TilePosition>	NicePlace6;
		std::set<BWAPI::TilePosition>	NicePlace7;


		// CSC - Why is this here??
		// Buildings	
		BWAPI::UnitType Pylon = BWAPI::UnitTypes::Protoss_Pylon;
		BWAPI::UnitType GateWay = BWAPI::UnitTypes::Protoss_Gateway;
		BWAPI::UnitType Nexus = BWAPI::UnitTypes::Protoss_Nexus;
		BWAPI::UnitType Assimilator = BWAPI::UnitTypes::Protoss_Assimilator;
		BWAPI::UnitType Forge = BWAPI::UnitTypes::Protoss_Forge;
		BWAPI::UnitType Cybernetics = BWAPI::UnitTypes::Protoss_Cybernetics_Core;		
		BWAPI::UnitType TemplerArchive = BWAPI::UnitTypes::Protoss_Templar_Archives;
		
		// Units
		BWAPI::UnitType Zealot = BWAPI::UnitTypes::Protoss_Zealot;
		BWAPI::UnitType Dragoon = BWAPI::UnitTypes::Protoss_Dragoon;
		BWAPI::UnitType Corsair = BWAPI::UnitTypes::Protoss_Corsair;

		BWAPI::Unit MrBuilder;
	
	};

} // end namespace Turk
