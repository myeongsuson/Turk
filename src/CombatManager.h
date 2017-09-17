#pragma once
#include "Common.h"




namespace Turk {

	// Commander Class
	class CombatManager
	{

	public:
		CombatManager();
		~CombatManager(){};

		// Combat Unit Action Updator
		void CombatActionUpdate();
		
		// Combat Unit Collector
		void CombatUnitCollector(BWAPI::Unitset);

		// Return unit counters
		std::map<std::string, int> GetUnitCount();

		// Corsair Control
		void CorsairUpdate(BWAPI::Unit);

		// Morph Archon
		void ArchonGenerator();

		// The Final Rush
		void RussianUra();




		static CombatManager &Instance();







	private:

		// Combat Unit Count
		std::map<std::string, int>	m_CombatUnitCount;

		BWAPI::TilePosition EnemyTileHome = BWAPI::TilePositions::None;
		BWAPI::Position  EnemyHome = BWAPI::Positions::None;
		BWAPI::TilePosition EnemyTileExp = BWAPI::TilePositions::None;
		BWAPI::Position  EnemyExp = BWAPI::Positions::None;
		
		// Home Position
		BWAPI::Position m_homePosition = BWAPI::Positions::None;

		


		// Combat Unit Sets
		BWAPI::Unitset m_HighTemUnits;
		BWAPI::Unitset m_ArchonUnits;
		BWAPI::Unitset m_ZealotUnits;
		BWAPI::Unitset m_DragooUnits;
		BWAPI::Unitset m_CorsairUnits;
		BWAPI::Unitset m_DarkTemUnits;
		
		BWAPI::UnitType Corsair = BWAPI::UnitTypes::Protoss_Corsair;

		// Indicators
		bool m_CorsairSearchMode = false;

	};

} // end namespace Turk
