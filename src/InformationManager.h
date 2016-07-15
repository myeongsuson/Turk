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
	};

} // end Turk namespace