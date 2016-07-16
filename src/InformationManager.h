#pragma once
#include "Common.h"


namespace Turk {

	// Commander Class
	class InformationManager
	{

	public:
		InformationManager();
		~InformationManager(){};
		
		// Unit Collector
		void ValidUnitCollector(const BWAPI::Unit &);
		
		// Determine Whether this unit is valid or not
		bool IsValidUnit(const BWAPI::Unit &);

		// Unit Set Presenter
		const BWAPI::Unitset & UnitSetPresent();
		
		// Nexus set Presenter
		const BWAPI::Unitset & BasePresent();

		// Worker Set Presenter
		const BWAPI::Unitset & WorkerPresent();
		
		// Present the unit count info
		const std::map<std::string, int> & UnitCounterPresenter();



		// Save Enemy Base
		void EnemyBaseSaver(BWAPI::TilePosition);

		// Return Each Enemy Base
		BWAPI::TilePosition GetEnemyBase(int);

		// Return All vector of enemy base
		std::vector<BWAPI::TilePosition> GetEnemyBase();







		// Enemy Info
		// Determine Whether this unit is valid or not
		bool IsValidEnemyUnit(const BWAPI::Unit &);
		
		// Get Enemy Building Tech
		void InformationManager::GetEnemyBuildingUpdate();

		// Check whether it is a tech building including a resource depot
		bool InformationManager::IsTechBuilding(BWAPI::Unit unit);




		static InformationManager &Instance();


	private:

		// Unit Count
		std::map<std::string, int>	m_UnitCount;
		
		// Unit Sets for internal state
		BWAPI::Unitset m_ValidUnits;
		BWAPI::Unitset m_BaseUnits;
		BWAPI::Unitset m_WorkerUnits;

		// Scout Variables
		BWAPI::Position m_EnemyHomeBase;

		// We have never received enemy base!
		bool EnemyBaseInit = true;

		// Enemy Base Locations
		std::vector<BWAPI::TilePosition>	m_EnmBaseTilePos;

		// Enemy Building Tech
		std::vector<BWAPI::Unit> m_EnemyTechBuilding;











	};

} // end Turk namespace