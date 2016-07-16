#include "Turk.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;
using namespace Turk;



TheTurk & TheTurk::Instance()
{
	static TheTurk instance;
	return instance;
}




void TheTurk::onStart()
{
	// Hello World!
	// Broodwar->sendText("Hello world!");
	Broodwar->setLocalSpeed(15);


	// Print the map name.
	// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

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

		// Iterate all the players in the game using a std:: iterator
		Playerset players = Broodwar->getPlayers();
		for (auto p : players)
		{
			// Only print the player if they are not an observer
			if (!p->isObserver())
				Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
		}

	}
	else // if this is not a replay
	{
		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		if (Broodwar->enemy()) // First make sure there is an enemy
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
	}


	// BWTA Map Analysis
	BWTA::readMap();
	BWTA::analyze();
	m_analyzed = true;
	m_analysis_just_finished = true;



	// $$$$$$$$$$$$$$$$$    Information   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// Very Important Geographical Information
	// Main Base
	m_homePosition = BWAPI::Position(Broodwar->self()->getStartLocation());
	m_homeTilePosition = Broodwar->self()->getStartLocation();
	
	// Main Base Choke Position
	m_HillPosition = BWTA::getNearestChokepoint(m_homePosition)->getCenter();
	m_FirstExpTilePosition = BWTA::getNearestBaseLocation(m_HillPosition)->getTilePosition();
	
	
	// CourtYard Choke Position
	// For the courtyard choke position, it is hard to use the getnearest, because a courtyard has two choke positions
	double MaxDistance = 100000;
	for (BWTA::Chokepoint * Choke : BWTA::getChokepoints()){
		double Distance = BWTA::getGroundDistance(TilePosition(Choke->getCenter()), TilePosition(m_HillPosition));
		if (Distance < MaxDistance && Distance > 100){
			MaxDistance = Distance;
			m_HillPosition2 = Choke->getCenter();
		}
	}
	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$










	// $$$$$$$$$$$$$$$$$    Building Locations   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// Define building locations
	BuildingManager::Instance().MapConnector();



	


	// A set of minerals
	BWTA::BaseLocation* StartingPoint = BWTA::getStartLocation(BWAPI::Broodwar->self());
	BWAPI::Unitset FirstMineralSet = MineralCollector(StartingPoint); // 
		
	// Add locations of pylons for defence.
	BuildingManager::Instance().DefensePylonLocation(FirstMineralSet);
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	












	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Very Basic Initiation: Split workers optimally
	for (auto &unit : Broodwar->self()->getUnits()){
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

	// Location where armies are gathering
	m_Campus.x = (m_PylonToChoke * m_homePosition.x + m_BaseToPylon* m_HillPosition.x) / (m_BaseToPylon + m_PylonToChoke);
	m_Campus.y = (m_PylonToChoke * m_homePosition.y + m_BaseToPylon* m_HillPosition.y) / (m_BaseToPylon + m_PylonToChoke);




}



/*NextExpansion = TilePosition(FirstExpansionPosition);
Broodwar->sendText("EasyWay %.2d %.2d ", NextExpansion.x, NextExpansion.y);*/





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
	InformationManager::Instance().ValidUnitCollector(Scouter);
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Should go to the informationManager
	// Count the number of probes
	std::map<std::string, int>	m_UnitCount = InformationManager::Instance().UnitCounterPresenter();
		
	// Bring the unitset of nexus
	BWAPI::Unitset m_BaseUnits = InformationManager::Instance().BasePresent();

	// Bring All VAlid Units
	BWAPI::Unitset m_ValidUnits = InformationManager::Instance().UnitSetPresent();

	// Generate workers	
	ProbeMaker(m_BaseUnits);
			
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Work Work Work
	ProbeWork(m_MaxMineralDist);
	

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// This should go to the building or expansion team
	// Build a Gas and send three probes to there.
	// Always check gases for all expansions. Open your eyes wide !
	if (m_UnitCount["GateWay_Count"] >= 1){
		GasWorkerAssign(m_BaseUnits);
	}



	// #######    Production Building     ####################################################################################################
	// Build the supply unit
	Error lastErr = Broodwar->getLastError();	
	if ( (BWAPI::Broodwar->self()->supplyUsed() / 2 + 5) > (BWAPI::Broodwar->self()->supplyTotal() / 2) || lastErr == Errors::Insufficient_Supply){				
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Pylon);
	}	
	

	// Build the first GateWay after the first pylon. Keep Build !
	if (m_UnitCount["Pylon_Count"] >= 1 && m_UnitCount["GateWay_Count"] < m_MaxGateWayCount){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, GateWay);		
	}
		
		
	// Build the StarGate (Only For Against Zerg)
	if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg){
		if (m_UnitCount["StarGate_Count"] == 0 && m_FirstCybernetics){
			BuildingManager::Instance().BuildingFunction(ResourceDepot, StarGate);
		}
	}
	// ###################################################################################################################











	// $$$$$$$    Nexux Building: Expansion Team $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// Expansion Plans --> Might be moved to the ExpansionManager.

	if (m_UnitCount["GateWay_Count"] >= 1 && BWAPI::Broodwar->self()->minerals() > 400 && m_UnitCount["Nexus_Count"]<4){  // First Triger & Overall Game Land Trigger
			BuildingManager::Instance().BuildingFunction(ResourceDepot, Nexus);
	}	
	// ###############################################################################################################################################








	// $$$$$$$    Technical Building 9 sets $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	// Build the Cybernetics Core
	if (m_UnitCount["GateWay_Count"] >= 1 && !m_FirstCybernetics){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Cybernetics);
	}

	// Build the Forge
	if (m_UnitCount["GateWay_Count"] >= 1 && m_FirstCybernetics && !m_FirstForge){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Forge);
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
	if (m_FirstTemplarArchive && !m_FirstRobotics){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, BWAPI::UnitTypes::Protoss_Robotics_Facility);
	}

	// Build the Robotics
	if (m_FirstRobotics && !m_FirstObservatory){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, BWAPI::UnitTypes::Protoss_Observatory);
	}
	
	// Build the Fleet Beacon
	if (m_UnitCount["StarGate_Count"] >= 1 && !m_FirstFleetBeacon){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, FleetBeacon);
	}

	// River

	// Abrbiter

	// ###################################################################################################################














	// %%%%%%%%%%%   Scouter Manager Program     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// First Detect the enemy base
	if (!m_EnemyDetection && Scouter){
		m_EnemyDetection = ScoutManager::Instance().EnemyFirstDetector(m_EnemyDetection, m_FirstExpTilePosition);
		
		// We found an enemy
		if (m_EnemyDetection){
			// Get enemy Base Location
			BWAPI::Position TargetPosition = Scouter->getTargetPosition();
			m_EnemyTileHome = BWTA::getNearestBaseLocation(TargetPosition)->getTilePosition();
			
			// Save Enemy Base Location
			InformationManager::Instance().EnemyBaseSaver(m_EnemyTileHome);

			// Find the expansionbase with respect to our base and enemy base.
			BuildingManager::Instance().GetExpansionBase(m_EnemyTileHome, m_homeTilePosition);
			
		}
	}
	

	


	// Detect the location of enemy's main base room
	// Operation Snake.
	if (m_EnemyDetection && Scouter){
		ScoutManager::Instance().ScoutActionUpdate(m_EnemyTileHome);
	}
	// ###################################################################################################################

	
	// Observer Generator
	ScoutManager::Instance().ObserverManager(m_ValidUnits);

















	
	// $$$$$$$$$$$$$$$$$$$$$$$$$$$ Unit Production $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		// Build a gateway or generate a zealot	
	for (auto & unit : m_ValidUnits){
		// We found a gateway, and it is not generating a unit
		if (unit->isIdle() && unit->getType() == GateWay){
			unit->rightClick(m_Campus);
						
			if (m_FirstCybernetics && m_UnitCount["Zealot_Count"] >= m_UnitCount["Dragoon_Count"]){
				unit->train(BWAPI::UnitTypes::Protoss_Dragoon);
			}
			else if (m_FirstTemplarArchive && m_UnitCount["HighTempler_Count"] < 10){
				unit->train(BWAPI::UnitTypes::Protoss_High_Templar);
			}
			else{
				unit->train(BWAPI::UnitTypes::Protoss_Zealot);
			}
			
		}

		if (unit->isIdle() && unit->getType() == StarGate){
			unit->rightClick(m_Campus);
			if (m_UnitCount["Corsair_Count"] <=12){
				unit->train(BWAPI::UnitTypes::Protoss_Corsair);
			}			
		}
	}





	// Combat Manager Action Update
	if (m_EnemyDetection){
		CombatManager::Instance().CombatActionUpdate();
	}
	





	//while (highTemplars.size() > 1)
	//{
	//	std::set<BWAPI::Unit*>::iterator ht1 = highTemplars.begin();
	//	std::set<BWAPI::Unit*>::reverse_iterator ht2 = highTemplars.rbegin();
	//	(*ht1)->useTech(BWAPI::TechTypes::Archon_Warp, (*ht2));
	//	highTemplars.erase(*ht1);
	//	highTemplars.erase(*ht2);
	//}






	
























	// $$$$$$$$$$$$$$$$$     Upgrading $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// Singularity Upgrading
	
	//for (auto & unit : UnitSetPresent()){
	//	if (unit->getType() == Cybernetics && !unit->isUpgrading()){				
	//		unit->upgrade(BWAPI::UpgradeTypes::Singularity_Charge);
	//	}
	//}
	

	// If a building should be upgraded, please do it at any time.
	for (auto & unit : m_ValidUnits){
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
				unit->upgrade(BWAPI::UpgradeTypes::Argus_Jewel);
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
	std::map<std::string, int>	UnitCount = InformationManager::Instance().UnitCounterPresenter();
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
		//Broodwar->sendText("%.2d %.2d / Unit: %s", targetBuildLocation.x, targetBuildLocation.y, unit->getType().c_str());

		// Realease the MrBuilder		
		BuildingManager::Instance().MrBuilderRemover();


		// Removed the location of built building.
		// BuildingManager::Instance().LocationRemover(unit);

		
		
		BWAPI::TilePosition TempTilePosition = unit->getTilePosition();
		if (unit->getType() == Nexus && Broodwar->getFrameCount()> 100 ){
			// First Expansion Action
			Broodwar << "Expansion ! " << std::endl;
			m_MaxGateWayCount = 8;
			m_MaxWorkerCount = 40;

		

			BWAPI::Position Temp = BWAPI::Positions::None;

			Temp.y = (Broodwar->mapHeight()) * 16;
			Temp.x = (Broodwar->mapWidth()) * 16;

			m_Campus.x = (5 * m_HillPosition2.x + 1 * Temp.x) / (1 + 5);
			m_Campus.y = (5 * m_HillPosition2.y + 1 * Temp.y) / (1 + 5);



			// Move Forward our armies to the new choke lines
			for (auto & unit : InformationManager::Instance().UnitSetPresent()){
				// We found a gateway, and it is not generating a unit
				if (unit->getType() == Dragoon || unit->getType() == Zealot){
					unit->rightClick(m_Campus);
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

	// Technical Unit Tester: If the tech buiding is destroyed please rebuild.
	if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral()){

		if (unit->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core){
			m_FirstCybernetics = false;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Forge){
			m_FirstForge = false;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun){
			m_FirstAdun = false;
			m_MaxGateWayCount = 2;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives){
			m_FirstTemplarArchive = false;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility){
			m_FirstRobotics = false;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Observatory){
			m_FirstObservatory = false;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Fleet_Beacon){
			m_FirstFleetBeacon = false;
		}
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
	if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral()){	

		if (unit->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core){
			m_FirstCybernetics = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Forge){
			m_FirstForge = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun){
			m_FirstAdun = true;
			m_MaxGateWayCount = 2;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives){
			m_FirstTemplarArchive = true;			
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility){
			m_FirstRobotics = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Observatory){
			m_FirstObservatory = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Fleet_Beacon){
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



void TheTurk::ProbeMaker(const BWAPI::Unitset & m_BaseUnits){

	for (auto &unit : m_BaseUnits){
		// Order the depot to construct more workers! But only when it is idle.
		if (unit->isIdle() && InformationManager::Instance().WorkerPresent().size() < m_MaxWorkerCount){
			// Train a worker
			unit->train(unit->getType().getRace().getWorker());			
		}
	}
}



void TheTurk::ProbeWork(int MaxMineralDist){

	BWAPI::Unitset m_WorkerUnits = InformationManager::Instance().WorkerPresent();

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



void TheTurk::GasWorkerAssign(const BWAPI::Unitset & m_BaseUnits){

	for (auto & unit : m_BaseUnits){

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
				//Broodwar->sendText("Dist :%.2d", GasWorkerCount);
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
				//Broodwar->sendText("Dist :%.2d", GasWorkerCount);
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
				Unit GasBuilder = unit->getClosestUnit(IsWorker && (IsIdle || IsGatheringMinerals));
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





//
//const std::map<std::string, int> & TheTurk::UnitCounter(){
//
//	int Pylon_Count = 0;
//	int GateWay_Count = 0;
//	int StarGate_Count = 0;
//	int Nexus_Count = 0;
//	int Probe_Count = 0;
//	int Zealot_Count = 0;
//	int Dragoon_Count = 0;
//	int Corsair_Count = 0;
//
//
//	for (auto & unit : m_ValidUnits){
//		// If the unit is a worker unit
//		if (unit->getType().isWorker())
//		{
//			Probe_Count = Probe_Count + 1;
//		}
//		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Zealot)
//		{
//			Zealot_Count = Zealot_Count + 1;
//		}
//		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon)
//		{
//			Dragoon_Count = Dragoon_Count + 1;
//		}
//		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Corsair)
//		{
//			Corsair_Count = Corsair_Count + 1;
//		}
//		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Pylon)
//		{
//			Pylon_Count = Pylon_Count + 1;
//		}
//		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Gateway)
//		{
//			GateWay_Count = GateWay_Count + 1;
//		}
//		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Stargate){
//			StarGate_Count = StarGate_Count + 1;
//		}
//		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus)
//		{
//			Nexus_Count = Nexus_Count + 1;
//		}
//	}
//
//	m_UnitCount["Probe_Count"] = Probe_Count;
//	m_UnitCount["Zealot_Count"] = Zealot_Count;
//	m_UnitCount["Dragoon_Count"] = Dragoon_Count;
//	m_UnitCount["Corsair_Count"] = Corsair_Count;
//
//	m_UnitCount["Pylon_Count"] = Pylon_Count;
//	m_UnitCount["GateWay_Count"] = GateWay_Count;
//	m_UnitCount["StarGate_Count"] = StarGate_Count;
//	m_UnitCount["Nexus_Count"] = Nexus_Count;
//
//
//
//	return m_UnitCount;
//
//}
