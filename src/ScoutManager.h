#pragma once
#include "Common.h"

namespace Turk {

	// Commander Class
	class ScoutManager
	{
	public:
		ScoutManager();
		~ScoutManager(){};

		// Save Scouter pointer
		void ScouterSaver(const BWAPI::Unit &);

		// Return a scout unit pointer
		const BWAPI::Unit & ScouterPresent();

		// Detect the enemy base
		bool EnemyFirstDetector(bool,const BWAPI::TilePosition &);

		// Scout Action Update
		void ScoutManager::ScoutActionUpdate(BWAPI::TilePosition);


		// Get Scout Roots in an enemy region.
		void GetScoutRoots(BWAPI::Position);
		int OurRootSize = 20;


		// Get Scout Roots in a map
		void GetStartingPoint();


		static ScoutManager &Instance();

	private:

		// Scout Variables
		BWAPI::Unit m_Scouter;

		// Scouter Scouting Root
		std::vector<BWAPI::Position>	m_ScoutRotationRoot;
		std::vector<BWAPI::Position>	m_StartingPosition;

		bool RoutInitiate = false;
		bool UnderJourney = false;
		
	};

} // end Turk namespace
