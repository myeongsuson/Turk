#include "Turk.h"
#include <iostream>
#include <fstream>

using namespace BWAPI;
using namespace Filter;
using namespace Turk;

// Define static member data
//const char * TheTurk::m_name = "TheTurk";
//Logger * Logger::m_ptr = 0;
//ConfigDB * ConfigDB::m_ptr = 0;

//
// default constructor
TheTurk::TheTurk() {

	//char * tdir = std::getenv("TURKDIR");
	//std::string baseDir(tdir);
	//std::string logDir("\\logs\\TurkTest");
	//std::string configName("\\TurkConfiguration.sqlite");

	//// connect to logger
	//m_log = Turk::Logger::instance();
	//m_log->newLog(std::string(baseDir+logDir).c_str());

	////DB connection
	//m_log->log(m_name, "Connecting to DB");
	////sqlite3 *db;
	////int rc = sqlite3_open(std::string(baseDir + configName).c_str(),&db);
	//m_db = Turk::ConfigDB::instance();

	//char msg[500];
	//sprintf(msg, "Opened sqlite3 connection");
	//m_log->log(m_name, msg);

	//m_log->log(m_name, "Get LogDir");
	//std::vector<std::string> res = m_db->query("SELECT value FROM config WHERE key = 'LogDir';");
	//for (auto r : res) {
	//	m_log->log(m_name, r.c_str());
	//}

	//m_log->log(m_name, "Get BaseName");
	//res = m_db->query("SELECT value FROM config WHERE key = 'LogBase';");
	//for (auto r : res) {
	//	m_log->log(m_name, r.c_str());
	//}
	

}

void TheTurk::onStart()
{

	// Hello World!
	//Broodwar->sendText("Hello world!");
	Broodwar->setLocalSpeed(10);

	char msg[500];
	sprintf(msg, ">>>> Starting New Game <<<<");
	// m_log->log(m_name, msg);

	// Print the map name.
	// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;
	sprintf(msg, "map: %s", Broodwar->mapFileName());
	// m_log->log(m_name, msg);
	
	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	

	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	// Check if this is a replay
	if (Broodwar->isReplay()){
		// Announce the players in the replay
		Broodwar << "The following players are in this replay:" << std::endl;
		// m_log->log(m_name, "Players in this replay:");

		// Iterate all the players in the game using a std:: iterator
		Playerset players = Broodwar->getPlayers();
		for (auto p : players)
		{
			// Only print the player if they are not an observer
			if (!p->isObserver()) {
				Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
				sprintf(msg, "%s, playing as %s", p->getName(), p->getRace().c_str());
				//  m_log->log(m_name, msg);
			}
		}

	}
	else // if this is not a replay
	{
		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		if (Broodwar->enemy()) { // First make sure there is an enemy
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
			sprintf(msg, "Matchup: %s vs %s", Broodwar->self()->getRace().c_str(), Broodwar->enemy()->getRace().c_str());
			// m_log->log(m_name, msg);
		}
	}


	// BWTA Map Analysis
	BWTA::readMap();
	BWTA::analyze();
	m_analyzed = true;
	m_analysis_just_finished = true;


	

	// Variable Definition
	m_homePosition = BWAPI::Position(Broodwar->self()->getStartLocation());
	m_homeTilePosition = Broodwar->self()->getStartLocation();
	m_HillPosition = BWTA::getNearestChokepoint(BWAPI::Position(Broodwar->self()->getStartLocation()))->getCenter();
	m_FirstExpansionPosition = BWTA::getNearestBaseLocation(m_HillPosition)->getPosition();
	//NextExpansion = TilePosition(FirstExpansionPosition);
	//BWTA::getChokepoints

	double MaxDistance = 100000;
	for (BWTA::Chokepoint * Choke : BWTA::getChokepoints()){
		double Distance = BWTA::getGroundDistance(TilePosition(Choke->getCenter()), TilePosition(m_HillPosition));
		if (Distance < MaxDistance && Distance > 100){
			MaxDistance = Distance;
			m_HillPosition2 = Choke->getCenter();
		}
	}





	/*NextExpansion = TilePosition(FirstExpansionPosition);
	Broodwar->sendText("EasyWay %.2d %.2d ", NextExpansion.x, NextExpansion.y);*/

	// Broodwar->sendText("Pylon %.2d:%.2d", FirstPylonTilePosition.x, FirstPylonTilePosition.y);
	//Broodwar->sendText("%s", Broodwar->mapName().c_str());
	

	// Define building locations
	BuildingManager::Instance().MapConnector();

	// A set of minerals
	BWTA::BaseLocation* StartingPoint = BWTA::getStartLocation(BWAPI::Broodwar->self());
	BWAPI::Unitset FirstMineralSet = MineralCollector(StartingPoint); // 
	

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Add locations of pylons for defence.
	BuildingManager::Instance().DefensePylonLocation(FirstMineralSet);


	

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Very Basic Initiation: Split workers optimally
	for (auto &unit : BWAPI::Broodwar->self()->getUnits()){
		// A resource depot is a Command Center, Nexus, or Hatchery
		if (unit->getType().isResourceDepot()){
			ResourceDepot = unit;
			// Generate the first worker
			unit->train(unit->getType().getRace().getWorker());			
		}
		// If the unit is a worker unit
		else if (unit->getType().isWorker()) {
			BWAPI::Unit ClosestMineral = nullptr;
			double closestDist = 100000;
			// For the given initial Minerals, find the closest mineral one by one
			for (auto &mineral : FirstMineralSet){
				double dist = unit->getDistance(mineral->getPosition());
				//Broodwar->sendText("%.2d / Unit: %s", dist, unit->getType().c_str());
				if (dist < closestDist){
					ClosestMineral = mineral;
					closestDist = dist;
				}
			}
			unit->gather(ClosestMineral);
			FirstMineralSet.erase(ClosestMineral);
		}
	}
	MineralSaver(FirstMineralSet);

	// Location of the first pylon
	m_Campus.x = (m_PylonToChoke * m_homePosition.x + m_BaseToPylon* m_HillPosition.x) / (m_BaseToPylon + m_PylonToChoke);
	m_Campus.y = (m_PylonToChoke * m_homePosition.y + m_BaseToPylon* m_HillPosition.y) / (m_BaseToPylon + m_PylonToChoke);



}



void TheTurk::onFrame(){
	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	//BWTA draw
	if (m_analyzed){
		drawTerrainData();
	}
	
	if (m_analysis_just_finished)	{
		Broodwar << "Finished analyzing map." << std::endl;;
		m_analysis_just_finished = false;

		
	}


	// draw the HUD
	m_hud.drawInterface();

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Important System Variables	
	// Bring the scouter point
	BWAPI::Unit Scouter = ScoutManager::Instance().ScouterPresent();
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Collect all possible units.
	// A set of workers will not contain a scouter.	
	ValidUnitCollector(Scouter);
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Should go to the informationManager
	// Count the number of probes
	std::map<std::string, int>	UnitCount;
	UnitCount = UnitCounterPresenter();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Should go to WorkerManagement or BaseManagement ?
	// Generate workers	
	ProbeMaker(m_MaxWorkerCount);
			
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Work Work Work
	ProbeWork(m_MaxMineralDist);
	


	// #######    Building     ################################################################################################################################
	// Build the supply unit
	Error lastErr = Broodwar->getLastError();	
	if (BWAPI::Broodwar->self()->supplyUsed() / 2 + 5 > BWAPI::Broodwar->self()->supplyTotal() / 2 || lastErr == Errors::Insufficient_Supply){				
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Pylon);
	}	
	

	// Build the first GateWay after the first pylon
	if (UnitCount["Pylon_Count"] >= 1 && UnitCount["GateWay_Count"] < m_MaxGateWayCount){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, GateWay);		
	}
		
	// Build the Cybernetics Core
	if (UnitCount["GateWay_Count"] >= 1 && !m_FirstCybernetics){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Cybernetics);
	}

	// Build the Forge
	if (UnitCount["GateWay_Count"] >= 1 && m_FirstCybernetics && !m_FirstForge){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Forge);
	}
		
	 // Build the StarGate (Only For Against Zerg)
	if (UnitCount["StarGate_Count"] == 0 && m_FirstCybernetics){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, StarGate);
	}
	
	// Build the Fleet Beacon
	if (UnitCount["StarGate_Count"] >= 1 && !m_FirstFleetBeacon){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, FleetBeacon);
	}


	// Build the Citadel of Adun
	if (m_FirstCybernetics && !m_FirstAdun){		
		BuildingManager::Instance().BuildingFunction(ResourceDepot, CitadelOfAdun);
	}

	// Build the Citadel of Adun
	if (m_FirstAdun && !m_FirstTemplarArchive){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, TemplerArchive);
	}

	// Build the Robotics
	if (m_FirstTemplarArchive && UnitCount["Robotics_Count"]<1){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Robotics);
	}	
	
	 // Expansion Plans --> Might be moved to the ExpansionManager.	 
	 if (UnitCount["GateWay_Count"] == 1 && BWAPI::Broodwar->self()->minerals() > 400){  // First Triger & Overall Game Land Trigger
		//BuildingManager::Instance().GetExpansionBase(TilePosition(EnemyHomeBase), homeTilePosition);
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Nexus);
	}





	// ###############################################################################################################################################


	// This should go to the building or expansion team
	// Build a Gas and send three probes to there.
	if (UnitCount["GateWay_Count"] >= 1){
		for (auto & unit : BasePresent()){

			BWAPI::Unit GasContainer = unit->getClosestUnit(IsRefinery && IsOwned && IsCompleted, 300);
			// if that unit is a refinery, Check whether a refinery has three workers
			if (GasContainer  && Broodwar->getFrameCount() % 20 == 0){  //
				int GasWorkerCount = 0;

				// Count how many workers are gathering gas?
				BWAPI::Unitset CloseWorkers = Broodwar->getUnitsInRadius(unit->getPosition(), 300);
				for (auto & unit2 : CloseWorkers){
					if (unit2->isGatheringGas() || unit2->isCarryingGas()){
						GasWorkerCount = GasWorkerCount + 1;
					}
				}

				if (GasWorkerCount >= (m_WorkersPerGas - 1) && GasWorkerCount <= (m_WorkersPerGas + 1)){
					continue;
				}

				else if (GasWorkerCount < m_WorkersPerGas){
					Broodwar->sendText("Dist :%.2d", GasWorkerCount);
					for (auto & unit2 : CloseWorkers){
						// Only send gathering workers to gas mine.
						if (unit2->isGatheringMinerals() || unit2->isCarryingMinerals()){
							unit2->rightClick(GasContainer);
							GasWorkerCount = GasWorkerCount + 1;
							if (GasWorkerCount >= m_WorkersPerGas){
								break;
							}
						}
					}
				}

				else if (GasWorkerCount > m_WorkersPerGas){
					Broodwar->sendText("Dist :%.2d", GasWorkerCount);
					for (auto & unit2 : CloseWorkers){
						// Only send gathering workers to gas mine.
						if (unit2->isGatheringGas() && unit2->isCarryingGas()){
							unit2->gather(unit2->getClosestUnit(IsMineralField));
							GasWorkerCount = GasWorkerCount - 1;
							if (GasWorkerCount <= m_WorkersPerGas){
								break;
							}
						}
					}
				}


			}

			else{
				BWTA::BaseLocation *Base = BWTA::getNearestBaseLocation(unit->getPosition());
				BWAPI::TilePosition closestGeyser = BWAPI::TilePositions::None;
				double minGeyserDistanceFromHome = (std::numeric_limits<double>::max)();
				for (auto &gas : Base->getGeysers()){ // 
					if (gas->getType() != BWAPI::UnitTypes::Resource_Vespene_Geyser){
						continue;
					}

					// BWAPI::Position geyserPos = gas->getInitialPosition();
					// BWAPI::TilePosition geyserTilePos = gas->getInitialTilePosition();

					double homeDistance = gas->getDistance(unit->getPosition());
					if (homeDistance < minGeyserDistanceFromHome){
						minGeyserDistanceFromHome = homeDistance;
						closestGeyser = gas->getInitialTilePosition();
					}
				}

				static int lastChecked = 0;

				// If we are supply blocked and haven't tried constructing more recently
				if (lastChecked + 200 < Broodwar->getFrameCount() && Broodwar->self()->incompleteUnitCount(Assimilator) == 0){
					// Frame Count
					lastChecked = Broodwar->getFrameCount();

					// Retrieve a unit that is capable of constructing the supply needed				
					Unit GasBuilder = unit->getClosestUnit(GetType == Assimilator.whatBuilds().first && (IsIdle || IsGatheringMinerals));
					if (GasBuilder){
						TilePosition targetBuildLocation = Broodwar->getBuildLocation(Assimilator, closestGeyser, 4);

						if (targetBuildLocation){
							// Order the builder to construct the supply structure				
							GasBuilder->build(Assimilator, targetBuildLocation);
						}
					}
				}

			}
		}
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// First Detect the enemy base
	if (!m_EnemyDetection && Scouter){
		m_EnemyDetection = ScoutManager::Instance().EnemyFirstDetector(m_EnemyDetection, m_NextExpansion);
		m_EnemyHomeBase = Scouter->getTargetPosition();
		m_EnemyHillPosition = BWTA::getNearestChokepoint(m_EnemyHomeBase)->getCenter();
		m_EnemyExpansion = BWTA::getNearestBaseLocation(m_EnemyHillPosition)->getPosition();

		if (m_EnemyDetection){
			BuildingManager::Instance().GetExpansionBase(TilePosition(m_EnemyHomeBase), m_homeTilePosition);
		}

		// Set the list of expansion
		
	}
	// 

	
	


	// Detect the location of enemy's main base room
	if (m_EnemyDetection && Scouter){
		// BWAPI::Position ScouterTarget = Scouter->getTargetPosition();

		if (Scouter->isIdle()){
			double dist = Scouter->getDistance(m_EnemyHomeBase);

			if (dist < 10){
				Scouter->move(m_EnemyExpansion);
			}
			else{
				Scouter->move(m_EnemyHomeBase);
			}
		}

		if (Scouter->isUnderAttack()){
			Scouter->move(m_FirstExpansionPosition);
		}
		
	}



	
	// $$$$$$$$$$$$$$$$$$$$$$$$$$$ Unit Production $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		// Build a gateway or generate a zealot	
	for (auto & unit : UnitSetPresent()){

		// Dark Templer Generation
		// We found a gateway, and it is not generating a unit
		if (unit->isIdle() && unit->getType() == GateWay){
			// Assemble the troups to the main force
			unit->rightClick(m_Campus);

			//// Fast Dark
			//if (UnitCount["GateWay_Count"] > 1 && UnitCount["TemplarArchives_Count"] == 1){
			//	if (UnitCount["DarkTempler_Count"] < 5){
			//		unit->train(BWAPI::UnitTypes::Protoss_Dark_Templar);
			//	}
			//}

			// Produce Zealots			
			if (m_FirstCybernetics && UnitCount["Zealot_Count"] >= UnitCount["Dragoon_Count"]){
				unit->train(BWAPI::UnitTypes::Protoss_Dragoon);
			}
			/*	else if (UnitCount["HighTempler_Count"] < 10){
					unit->train(BWAPI::UnitTypes::Protoss_High_Templar);
					}*/
			else{
				unit->train(BWAPI::UnitTypes::Protoss_Zealot);
			}
		}
			

		// Corsair Production
		if (unit->isIdle() && unit->getType() == StarGate){
			unit->rightClick(m_Campus);			
			if (UnitCount["Corsair_Count"] <= 3){
				unit->train(BWAPI::UnitTypes::Protoss_Corsair);
			}
		}


		// Shuttle Generation
		if (unit->isIdle() && unit->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility){
			// Shuttle needs not assemble.
			if (UnitCount["Shuttle_Count"] < 1){
				unit->train(BWAPI::UnitTypes::Protoss_Shuttle);
			}
		}
	}

	
		
	// Shuttle Control
	// 1: Load Dark Templars
	for (auto & unit : UnitSetPresent()){
		if (unit->getType() == BWAPI::UnitTypes::Protoss_Shuttle){
			int LoadedUnit = unit->getLoadedUnits().size();

			// There are some spaces
			if (LoadedUnit < 4){
				// Find the Dark Templar
				for (auto & unit2 : BWAPI::Broodwar->self()->getUnits()){
					if (unit2->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar){
						unit->load(unit2);
					}
				}
			}
			// If everybody is onboard, let's go.
			else if (LoadedUnit == 4){
				unit->move(m_EnemyHomeBase, true);				
			}

			// If the previous order was Move and we are in the enemy base, unload all of them.
			if (unit->getLastCommand().getType() == BWAPI::UnitCommandTypes::Move){
				double dist = unit->getDistance(m_EnemyHomeBase);
				if (dist < 5){
					unit->unloadAll();
				}
			}

			// Go Back
			if (unit->getLastCommand().getType() == BWAPI::UnitCommandTypes::Unload_All){
				int LoadedUnit = unit->getLoadedUnits().size();
				if (LoadedUnit == 0){
					unit->move(m_homePosition);
				}
			}
		}
	}

	




	// $$$$$$$$$$$$$$$$$$$$$$$$$$$ Corsiar Attack Process $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$	
	// Determine whether a scouter is in attack mode or searching mode.
	for (auto & unit : UnitSetPresent()){
		if (unit->getType() == Corsair){						
			
			// Release the stagmation
			if (unit->isUnderAttack()){
				unit->move(m_homePosition);
			}
			

		
			if (m_CorsairSearchMode){
				for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations()){
					// if we haven't explored it yet
					BWAPI::Position targetPosition = BWAPI::Position(startLocation->getTilePosition());
					double dist = unit->getDistance(targetPosition);
					if (targetPosition != m_EnemyHomeBase && dist > 10 ){
						unit->move(targetPosition,true);
					}					
				}
				m_CorsairSearchMode = false;
			}

			

			// If a unit is regenerated and taking a rest, go to the enemy base.
			if (unit->isIdle()){
				double dist = unit->getDistance(m_EnemyHomeBase);
				double dist2 = unit->getDistance(m_EnemyExpansion);

				// I am already in the enemy base
				if (dist < 10){
					unit->attack(m_EnemyExpansion);
				}
				// I am at enemy's expansion
				else if (dist2 < 10){
					m_CorsairSearchMode = true;
				}
				else{
					// I am at home
					unit->attack(m_EnemyHomeBase);
				}
			}
							
			// If something is shown, kill them all.
			if (unit->isMoving()){
				for (auto & unit2 : BWAPI::Broodwar->enemy()->getUnits()){
					if (unit2->getType().isFlyer() && unit->exists()){
						if (unit->isInWeaponRange(unit2) && !unit->isUnderAttack()){
							unit->attack(unit2);
							break;
						}
						else if (unit2->getType() == BWAPI::UnitTypes::Zerg_Spore_Colony || unit2->getType() == BWAPI::UnitTypes::Zerg_Hydralisk){
							unit->move(m_homePosition);
						}
						else if (unit->isUnderAttack()){
							unit->move(m_homePosition);
							break;
						}
						else{
							unit->move(unit2->getPosition());
							break;
						}
					}
				}
			}
		}
	}


	// $$$$$$$$$$$$$$$$$     Upgrading $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// Singularity Upgrading
	
	//for (auto & unit : UnitSetPresent()){
	//	if (unit->getType() == Cybernetics && !unit->isUpgrading()){				
	//		unit->upgrade(BWAPI::UpgradeTypes::Singularity_Charge);
	//	}
	//}
	

	// If a building should be upgraded, please do it at any time.
	for (auto & unit : UnitSetPresent()){
		if (unit->canUpgrade(true) && !unit->isUpgrading()){

			if (unit->getType() == CitadelOfAdun){
				unit->upgrade(BWAPI::UpgradeTypes::Leg_Enhancements);
			}	

			else if (unit->getType() == Cybernetics){
				unit->upgrade(BWAPI::UpgradeTypes::Singularity_Charge);
			}

			if (unit->getType() == Forge){
				unit->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);			
				unit->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
				unit->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);				
			}
			if (unit->getType() == TemplerArchive){
				unit->research(BWAPI::TechTypes::Psionic_Storm);
				unit->upgrade(BWAPI::UpgradeTypes::Khaydarin_Amulet);
			}


		}

		if (unit->canResearch(true) && !unit->isResearching()){
			if (unit->getType() == FleetBeacon){
				unit->research(BWAPI::TechTypes::Disruption_Web);
			}

		}		

	}

}















void TheTurk::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for (const auto& baseLocation : BWTA::getBaseLocations()) {
		TilePosition p = baseLocation->getTilePosition();

		//draw outline of center location
		Position leftTop(p.x * TILE_SIZE, p.y * TILE_SIZE);
		Position rightBottom(leftTop.x + 4 * TILE_SIZE, leftTop.y + 3 * TILE_SIZE);
		Broodwar->drawBoxMap(leftTop, rightBottom, Colors::Blue);

		//draw a circle at each mineral patch
		for (const auto& mineral : baseLocation->getStaticMinerals()) {
			Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, Colors::Cyan);
		}

		//draw the outlines of Vespene geysers
		for (const auto& geyser : baseLocation->getGeysers()) {
			TilePosition p1 = geyser->getInitialTilePosition();
			Position leftTop1(p1.x * TILE_SIZE, p1.y * TILE_SIZE);
			Position rightBottom1(leftTop1.x + 4 * TILE_SIZE, leftTop1.y + 2 * TILE_SIZE);
			Broodwar->drawBoxMap(leftTop1, rightBottom1, Colors::Orange);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if (baseLocation->isIsland()) {
			Broodwar->drawCircleMap(baseLocation->getPosition(), 80, Colors::Yellow);
		}
	}

	//we will iterate through all the regions and ...
	for (const auto& region : BWTA::getRegions()) {
		// draw the polygon outline of it in green
		BWTA::Polygon p = region->getPolygon();
		for (size_t j = 0; j < p.size(); ++j) {
			Position point1 = p[j];
			Position point2 = p[(j + 1) % p.size()];
			Broodwar->drawLineMap(point1, point2, Colors::Green);
		}
		// visualize the chokepoints with red lines
		for (auto const& chokepoint : region->getChokepoints()) {
			Position point1 = chokepoint->getSides().first;
			Position point2 = chokepoint->getSides().second;
			Broodwar->drawLineMap(point1, point2, Colors::Red);
		}
	}
}




void TheTurk::onSendText(std::string text)
{

	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());


	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!

}

void TheTurk::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void TheTurk::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void TheTurk::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void TheTurk::onUnitDiscover(BWAPI::Unit unit)
{
}

void TheTurk::onUnitEvade(BWAPI::Unit unit)
{
}

void TheTurk::onUnitShow(BWAPI::Unit unit)
{
}

void TheTurk::onUnitHide(BWAPI::Unit unit)
{
}

void TheTurk::onUnitCreate(BWAPI::Unit unit)
{
	// For the first probe to build the pylon, he is selected as the first scouter.
	std::map<std::string, int>	UnitCount = UnitCounterPresenter();
	BWAPI::Unit Scouter = ScoutManager::Instance().ScouterPresent();
	if (!Scouter && UnitCount["Pylon_Count"] == 0 && unit->getType() == Pylon){
		
		BWAPI::Unit ScouterCandidate = unit->getClosestUnit(IsWorker);
		// If there is a worker
		if (ScouterCandidate){
			ScoutManager::Instance().ScouterSaver(ScouterCandidate);
		}

	}


	// When the building is innitiated, realease the MrBuilder and Building locations.
	if (unit->getType().isBuilding()){

		// Print the Target Building
		TilePosition targetBuildLocation = unit->getTilePosition();
		Broodwar->sendText("%.2d %.2d / Unit: %s", targetBuildLocation.x, targetBuildLocation.y, unit->getType().c_str());

		// Realease the MrBuilder
		BuildingManager::Instance().MrBuilderRemover();


		std::vector<BWAPI::TilePosition>	TilePositionOfBuilding;
		TilePosition ScheduledLocation = BWAPI::TilePositions::None;
		

		if (unit->getType() == Pylon){
			TilePositionOfBuilding = BuildingManager::Instance().PylonSetPresent();		

			if (!TilePositionOfBuilding.empty()){
				ScheduledLocation = TilePositionOfBuilding.back();
				if (ScheduledLocation = targetBuildLocation){
					BuildingManager::Instance().PylonLocationRemover();
				}
			}
		}
		else if ( (unit->getType() == GateWay) || (unit->getType() == StarGate) ){
			TilePositionOfBuilding = BuildingManager::Instance().PylonSetPresent();

			if (!TilePositionOfBuilding.empty()){
				ScheduledLocation = TilePositionOfBuilding.back();
				if (ScheduledLocation = targetBuildLocation){
					BuildingManager::Instance().GateWayLocationRemover();
				}
			}
		}
		else if (unit->getType().tileWidth() == 3 && unit->getType().tileHeight() == 2){
			TilePositionOfBuilding = BuildingManager::Instance().TechSetPresent();

			if (!TilePositionOfBuilding.empty()){
				ScheduledLocation = TilePositionOfBuilding.back();
				if (ScheduledLocation = targetBuildLocation){		
					BuildingManager::Instance().TechLocationRemover();
				}
			}
		}
		else if (unit->getType() == Nexus){
			TilePositionOfBuilding = BuildingManager::Instance().NexusSetPresent();

			if (!TilePositionOfBuilding.empty()){
				ScheduledLocation = TilePositionOfBuilding.back();
				if (ScheduledLocation = targetBuildLocation){
					BuildingManager::Instance().NexusLocationRemover();
				}
			}
		}

		

	}
}





void TheTurk::onUnitDestroy(BWAPI::Unit unit)
{
	BWAPI::Unit Scouter = ScoutManager::Instance().ScouterPresent();

	if (unit == Scouter){
		Scouter = nullptr;
		ScoutManager::Instance().ScouterSaver(Scouter);
	}



}

void TheTurk::onUnitMorph(BWAPI::Unit unit)
{
	
}

void TheTurk::onUnitRenegade(BWAPI::Unit unit)
{
}

void TheTurk::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void TheTurk::onUnitComplete(BWAPI::Unit unit)
{
	if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
	{
		


		std::map<std::string, int>	UnitCount;
		UnitCount = UnitCounterPresenter();

		if (unit->getType() == BWAPI::UnitTypes::Protoss_Assimilator)
		{
			m_FirstGasExist = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
		{
			m_FirstCybernetics = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Forge){
			m_FirstForge = true;
		}
		else if (unit->getType() == BWAPI::UpgradeTypes::Singularity_Charge){
			m_Singularity = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun){
			m_FirstAdun = true;
			m_MaxGateWayCount = 2;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives){
			m_FirstTemplarArchive = true;			
		}		
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives){
			m_FirstTemplarArchive = true;
		}
		else if (unit->getType() == BWAPI::UpgradeTypes::Leg_Enhancements){
			m_Leg_Enhancements = true;			
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus && UnitCount["Nexus_Count"]==2){

			m_MaxGateWayCount = 5;
			
			// Build More Pylons
			std::vector<BWAPI::TilePosition>	PylonTilePosition = BuildingManager::Instance().PylonSetPresent();
			
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, TilePosition(m_HillPosition2), 12));
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, TilePosition(m_HillPosition2), 12));
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, TilePosition(m_HillPosition2), 12));
						
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, unit->getTilePosition(), 12));
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, unit->getTilePosition(), 12));
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, unit->getTilePosition(), 12));
			
						
			BuildingManager::Instance().PylonLocationSaver(PylonTilePosition);

			
			// Set the New ChokeLines
			//Campus = HillPosition2;

			BWAPI::Position Temp = BWAPI::Positions::None;

			Temp.y = (Broodwar->mapHeight() )* 16;
			Temp.x=(Broodwar->mapWidth())*16;

			m_Campus.x = (5 * m_HillPosition2.x + 1* Temp.x) / (1 + 5);
			m_Campus.y = (5 * m_HillPosition2.y + 1* Temp.y) / (1 + 5);



			// Move Forward our armies to the new choke lines
			for (auto & unit : UnitSetPresent()){
				// We found a gateway, and it is not generating a unit
				if (unit->getType() == Dragoon || unit->getType() == Zealot){
					unit->rightClick(m_Campus);
				}
			}

		}

		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Fleet_Beacon)
		{
			m_FirstFleetBeacon = true;
		}
		
	}


}




void TheTurk::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!
	}
}


// Commander functions
// Collect all valid units except dead bodies
void TheTurk::ValidUnitCollector(const BWAPI::Unit & ScouterUnit){

	// Building Count Variable Definition & Declaration
	int Pylon_Count = 0;
	int GateWay_Count = 0;
	int StarGate_Count = 0;
	int Nexus_Count = 0;
	int TemplarArchives_Count = 0;
	int Robotics_Count = 0;


	int Probe_Count = 0;
	int Zealot_Count = 0;
	int Dragoon_Count = 0;
	int Corsair_Count = 0;
	int HighTempler_Count = 0;
	int DarkTempler_Count = 0;
	int Shuttle_Count = 0;


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
				m_WorkerUnits.insert(unit);

				Probe_Count = Probe_Count + 1;
			}

			// Unit Counts
			if (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot)
			{
				Zealot_Count = Zealot_Count + 1;
			}
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon)
			{
				Dragoon_Count = Dragoon_Count + 1;
			}
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Corsair)
			{
				Corsair_Count = Corsair_Count + 1;
			}
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Pylon)
			{
				Pylon_Count = Pylon_Count + 1;
			}
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Gateway)
			{
				GateWay_Count = GateWay_Count + 1;
			}
			// Building
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Stargate){
				StarGate_Count = StarGate_Count + 1;
			}
			// Building
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives){
				TemplarArchives_Count = TemplarArchives_Count + 1;
			}
			// Building
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility){
				Robotics_Count = Robotics_Count + 1;
			}
			// Unit
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_High_Templar){
				HighTempler_Count = HighTempler_Count + 1;
			}
			// Unit
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar){
				DarkTempler_Count = DarkTempler_Count + 1;
			}
			// Unit
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Shuttle){
				Shuttle_Count = Shuttle_Count + 1;
			}
		}
	}
	if (ScouterUnit){
		m_WorkerUnits.erase(ScouterUnit);
	}

	// Counting Variable Redefine (Building First and Unit Second)
	// Building Counting
	m_UnitCount["Pylon_Count"] = Pylon_Count;
	m_UnitCount["GateWay_Count"] = GateWay_Count;
	m_UnitCount["StarGate_Count"] = StarGate_Count;
	m_UnitCount["Nexus_Count"] = Nexus_Count;
	m_UnitCount["TemplarArchives_Count"] = TemplarArchives_Count;
	m_UnitCount["Robotics_Count"] = Robotics_Count;

	// Unit Counting
	m_UnitCount["Probe_Count"] = Probe_Count;
	m_UnitCount["Zealot_Count"] = Zealot_Count;
	m_UnitCount["Dragoon_Count"] = Dragoon_Count;
	m_UnitCount["Corsair_Count"] = Corsair_Count;
	m_UnitCount["HighTempler_Count"] = HighTempler_Count;
	m_UnitCount["DarkTempler_Count"] = DarkTempler_Count;
	m_UnitCount["Shuttle_Count"] = Shuttle_Count;

	

	// Future unit sets
	// SetScoutUnits();
	// SetBaseCombatUnits();
	// SetFieldCombatUnits();
}


const std::map<std::string, int> & TheTurk::UnitCounterPresenter(){
	return m_UnitCount;
}

const BWAPI::Unitset & TheTurk::MineralCollector(const BWTA::BaseLocation * BasePoint){
	//MineralSets.clear();
	for (auto &mineral : BasePoint->getStaticMinerals()){
		m_MineralSets.insert(mineral);
	}		
	return m_MineralSets;
}


const BWAPI::Unitset & TheTurk::MineralPresent(){
	return m_MineralSets;
}


void TheTurk::MineralSaver(const BWAPI::Unitset & Mineral){
	m_MineralSets = Mineral;
}



void TheTurk::ScoutHander(const BWAPI::Unit & Scout){
	m_WorkerUnits.erase(Scout);
}

// unit represent the point.
bool TheTurk::IsValidUnit(const BWAPI::Unit & unit){
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





const BWAPI::Unitset & TheTurk::UnitSetPresent(){
	return m_ValidUnits;
}


const BWAPI::Unitset & TheTurk::BasePresent(){
	return m_BaseUnits;
}


const BWAPI::Unitset & TheTurk::WorkerPresent(){
	return m_WorkerUnits;
}


void TheTurk::ProbeMaker(unsigned MaxWorkerCount){

	for (auto &unit : m_BaseUnits){
		// Order the depot to construct more workers! But only when it is idle.
		if (unit->isIdle() && WorkerPresent().size() < MaxWorkerCount){
			// Train a worker
			unit->train(unit->getType().getRace().getWorker());
		}
	}
}



void TheTurk::ProbeWork(int MaxMineralDist){
	for (auto &unit2 : m_WorkerUnits){
		// Prepare Basic info: Location of the nearest base and its distance
		BWAPI::Unit ClosestBase = unit2->getClosestUnit(BWAPI::Filter::IsResourceDepot && 
														BWAPI::Filter::IsCompleted && 
														BWAPI::Filter::IsOwned);
		if (ClosestBase){
			double DistanceToBase = unit2->getDistance(ClosestBase);

			// if our worker is idle
			if (unit2->isIdle()) {
				// Order workers carrying a resource to return them to the center		     
				if (unit2->isCarryingGas() || unit2->isCarryingMinerals()){
					unit2->returnCargo();
				}

				// A worker is newly born.
				else if (unit2->isCompleted()){
					BWAPI::Unit ClosestMineral = nullptr;
					double closestDist = 100000;

					// For the given initial Minerals, find the closest mineral one by one		
					BWAPI::Unitset FirstMineralSet = MineralPresent();
					for (auto &mineral : FirstMineralSet){
						double dist = unit2->getDistance(mineral->getPosition());
						if (dist < closestDist){
							ClosestMineral = mineral;
							closestDist = dist;
						}
					}
					// Is it really closest? It is too far  or Mieral List is emphty !
					if (closestDist > MaxMineralDist){
						unit2->gather(ClosestBase->getClosestUnit(BWAPI::Filter::IsMineralField));
						BWTA::BaseLocation * Base = BWTA::getNearestBaseLocation(ClosestBase->getPosition());
						FirstMineralSet = MineralCollector(Base);
					}
					else{
						unit2->gather(ClosestMineral);
						FirstMineralSet.erase(ClosestMineral);
					}
					MineralSaver(FirstMineralSet);
				}

				// otherwise find a mineral patch to harvest.
				else{
					unit2->gather(ClosestBase->getClosestUnit(BWAPI::Filter::IsMineralField));
				}
			}

			// This code should be modified for future scouting action.
			// You are working at too far location.
			else if (unit2->isGatheringMinerals() || unit2->isGatheringGas()){
				if (DistanceToBase > MaxMineralDist){
					// Go back to your friends.
					unit2->gather(ClosestBase->getClosestUnit(BWAPI::Filter::IsMineralField));
				}
			}
		}
	}
}



const std::map<std::string, int> & TheTurk::UnitCounter(){

	int Pylon_Count = 0;
	int GateWay_Count = 0;
	int StarGate_Count = 0;
	int Nexus_Count = 0;
	int Probe_Count = 0;
	int Zealot_Count = 0;
	int Dragoon_Count = 0;
	int Corsair_Count = 0;


	for (auto & unit : m_ValidUnits){
		// If the unit is a worker unit
		if (unit->getType().isWorker())
		{
			Probe_Count = Probe_Count + 1;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot)
		{
			Zealot_Count = Zealot_Count + 1;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon)
		{
			Dragoon_Count = Dragoon_Count + 1;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Corsair)
		{
			Corsair_Count = Corsair_Count + 1;
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
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus)
		{
			Nexus_Count = Nexus_Count + 1;
		}
	}

	m_UnitCount["Probe_Count"] = Probe_Count;
	m_UnitCount["Zealot_Count"] = Zealot_Count;
	m_UnitCount["Dragoon_Count"] = Dragoon_Count;
	m_UnitCount["Corsair_Count"] = Corsair_Count;

	m_UnitCount["Pylon_Count"] = Pylon_Count;
	m_UnitCount["GateWay_Count"] = GateWay_Count;
	m_UnitCount["StarGate_Count"] = StarGate_Count;
	m_UnitCount["Nexus_Count"] = Nexus_Count;



	return m_UnitCount;

}
