#include "Turk.h"
#include <iostream>
#include "Commander.h"

bool analyzed;
bool analysis_just_finished;

using namespace BWAPI;
using namespace Filter;
using namespace Turk;



void TheTurk::onStart()
{
	// Hello World!
	// Broodwar->sendText("Hello world!");
	Broodwar->setLocalSpeed(20);


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
	analyzed = true;
	analysis_just_finished = true;






	// Variable Definition
	homePosition = BWAPI::Position(Broodwar->self()->getStartLocation());
	homeTilePosition = Broodwar->self()->getStartLocation();
	HillPosition = BWTA::getNearestChokepoint(BWAPI::Position(Broodwar->self()->getStartLocation()))->getCenter();
	FirstExpansionPosition = BWTA::getNearestBaseLocation(HillPosition)->getPosition();
	//NextExpansion = TilePosition(FirstExpansionPosition);
	//BWTA::getChokepoints

	double MaxDistance = 100000;
	for (BWTA::Chokepoint * Choke : BWTA::getChokepoints()){
		double Distance = BWTA::getGroundDistance(TilePosition(Choke->getCenter()), TilePosition(HillPosition));
		if (Distance < MaxDistance && Distance > 100){
			MaxDistance = Distance;
			HillPosition2 = Choke->getCenter();
		}
	}





	/*NextExpansion = TilePosition(FirstExpansionPosition);
	Broodwar->sendText("EasyWay %.2d %.2d ", NextExpansion.x, NextExpansion.y);*/

	// NextExpansion = BuildingManager::Instance().GetExpansionBase();
	// Broodwar->sendText("Hard Way %.2d %.2d ", NextExpansion.x, NextExpansion.y);

		

	
	// Broodwar->sendText("Pylon %.2d:%.2d", FirstPylonTilePosition.x, FirstPylonTilePosition.y);
	//Broodwar->sendText("%s", Broodwar->mapName().c_str());
	

	// Define building locations
	BuildingManager::Instance().MapConnector();
	



	// A set of minerals
	BWTA::BaseLocation* StartingPoint = BWTA::getStartLocation(BWAPI::Broodwar->self());
	BWAPI::Unitset FirstMineralSet = _Commander.MineralCollector(StartingPoint); // 
	

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Add locations of pylons for defence.
	BuildingManager::Instance().DefensePylonLocation(FirstMineralSet);


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
	_Commander.MineralSaver(FirstMineralSet);

		



	// Location of the first pylon
	
	Campus.x = (PylonToChoke * homePosition.x + BaseToPylon* HillPosition.x) / (BaseToPylon + PylonToChoke);
	Campus.y = (PylonToChoke * homePosition.y + BaseToPylon* HillPosition.y) / (BaseToPylon + PylonToChoke);



	if (SearchingPosition.empty()){		
		for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations()){
			BWAPI::Position targetPosition = startLocation->getPosition();
			SearchingPosition.push_back(targetPosition);
		}
	}



}











//BWAPI::Broodwar->sendText("No Location_1 %.2d : %.2d", PylonLoc.x, PylonLoc.y);


//Broodwar->sendText("Pylon :%.1d", UnitCount["Pylon_Count"]);





// Broodwar->sendText("Pylon :%.2d", _Commander.BasePresent().size());





// getNearestBaseLocation

// BWTA::BaseLocation *Base = BWTA::getNearestBaseLocation(Scouter->getPosition());
// HillPosition = BWTA::getNearestChokepoint(BWAPI::Position(Broodwar->self()->getStartLocation()))->getCenter();

// A set of minerals
// BWTA::BaseLocation* StartingPoint = BWTA::getStartLocation(BWAPI::Broodwar->self());


/*BWTA::Region *MyRegion = BWTA::getRegion(Scouter->getPosition());
for (auto &gon : MyRegion->getPolygon()){
Broodwar->sendText("Enemy Base Polygon %.1d : %.1d", gon.x, gon.y);

Scouter->move(gon);




}*/




// Display the game frame rate as text in the upper left area of the screen
//Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
//Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );





void TheTurk::onFrame(){
	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	//BWTA draw
	if (analyzed){
		drawTerrainData();
	}
	
	if (analysis_just_finished)	{
		Broodwar << "Finished analyzing map." << std::endl;;
		analysis_just_finished = false;
	}


	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	// ------------------------------------------------------------------------------------------------------------------------





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Important System Variables	
	// Bring the scouter point
	BWAPI::Unit Scouter = ScoutManager::Instance().ScouterPresent();



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Collect all possible units.
	// A set of workers will not contain a scouter.	
	_Commander.ValidUnitCollector(Scouter);
	

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Should go to the informationManager
	// Count the number of probes
	std::map<std::string, int>	UnitCount;
	UnitCount = _Commander.UnitCounterPresenter();

	
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	// Should go to WorkerManagement or BaseManagement ?
	// Generate workers	
	_Commander.ProbeMaker(MaxWorkerCount);
			
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Work Work Work
	_Commander.ProbeWork(MaxMineralDist);
	






	// #######    Building     ################################################################################################################################
	// Build the supply unit
	Error lastErr = Broodwar->getLastError();	
	if (BWAPI::Broodwar->self()->supplyUsed() / 2 + 5 > BWAPI::Broodwar->self()->supplyTotal() / 2 || lastErr == Errors::Insufficient_Supply){				
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Pylon);
	}	
	

	// Build the first GateWay after the first pylon
	if (UnitCount["Pylon_Count"] >= 1 && UnitCount["GateWay_Count"] < MaxGateWayCount){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, GateWay);		
	}
		
	// Build the Cybernetics Core
	if (UnitCount["GateWay_Count"] >= 1 && !FirstCybernetics){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Cybernetics);
	}

	// Build the Cybernetics Core
	if (UnitCount["GateWay_Count"] >= 1 && FirstCybernetics && !FirstForge){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, Forge);
	}
		
	// Build the StarGate (Only For Against Zerg)
	if (UnitCount["StarGate_Count"] == 0 && FirstCybernetics){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, StarGate);
	}
	
	// Build the Citadel of Adun
	if (FirstCybernetics && !FirstAdun){		
		BuildingManager::Instance().BuildingFunction(ResourceDepot, CitadelOfAdun);
	}

	// Build the Citadel of Adun
	if (FirstAdun && !FirstTemplerArchive){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, TemplerArchive);
	}
	

	// Build the Fleet Beacon
	if (UnitCount["StarGate_Count"] >= 1 && !FirstFleetBeacon){
		BuildingManager::Instance().BuildingFunction(ResourceDepot, FleetBeacon);
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
		for (auto & unit : _Commander.BasePresent()){

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

				if (GasWorkerCount >= (WorkersPerGas - 1) && GasWorkerCount <= (WorkersPerGas + 1)){
					continue;
				}

				else if (GasWorkerCount < WorkersPerGas){
					Broodwar->sendText("Dist :%.2d", GasWorkerCount);
					for (auto & unit2 : CloseWorkers){
						// Only send gathering workers to gas mine.
						if (unit2->isGatheringMinerals() || unit2->isCarryingMinerals()){
							unit2->rightClick(GasContainer);
							GasWorkerCount = GasWorkerCount + 1;
							if (GasWorkerCount >= WorkersPerGas){
								break;
							}
						}
					}
				}

				else if (GasWorkerCount > WorkersPerGas){
					Broodwar->sendText("Dist :%.2d", GasWorkerCount);
					for (auto & unit2 : CloseWorkers){
						// Only send gathering workers to gas mine.
						if (unit2->isGatheringGas() && unit2->isCarryingGas()){
							unit2->gather(unit2->getClosestUnit(IsMineralField));
							GasWorkerCount = GasWorkerCount - 1;
							if (GasWorkerCount <= WorkersPerGas){
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


	//


	
	

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// First Detect the enemy base
	if (!EnemyDetection && Scouter){
		EnemyDetection = ScoutManager::Instance().EnemyFirstDetector(EnemyDetection, NextExpansion);
		EnemyHomeBase = Scouter->getTargetPosition();
		EnemyHillPosition = BWTA::getNearestChokepoint(EnemyHomeBase)->getCenter();
		EnemyExpansion = BWTA::getNearestBaseLocation(EnemyHillPosition)->getPosition();

		if (EnemyDetection){
			BuildingManager::Instance().GetExpansionBase(TilePosition(EnemyHomeBase), homeTilePosition);
		}

		// Set the list of expansion
		
	}
	// 

	
	


	// Detect the location of enemy's main base room
	if (EnemyDetection && Scouter){
		// BWAPI::Position ScouterTarget = Scouter->getTargetPosition();

		if (Scouter->isIdle()){
			double dist = Scouter->getDistance(EnemyHomeBase);

			if (dist < 10){
				Scouter->move(EnemyExpansion);
			}
			else{
				Scouter->move(EnemyHomeBase);
			}
		}

		if (Scouter->isUnderAttack()){
			Scouter->move(FirstExpansionPosition);
		}
			
	
		
	}




	









	// $$$$$$$$$$$$$$$$$$$$$$$$$$$ Unit Production $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		// Build a gateway or generate a zealot	
	for (auto & unit : _Commander.UnitSetPresent()){
		// We found a gateway, and it is not generating a unit
		if (unit->isIdle() && unit->getType() == GateWay){
			unit->rightClick(Campus);
						
			if (FirstCybernetics && UnitCount["Zealot_Count"] >= UnitCount["Dragoon_Count"]){
				unit->train(BWAPI::UnitTypes::Protoss_Dragoon);
			}
			else if (FirstTemplerArchive && UnitCount["HighTempler_Count"] < 10 ){
				unit->train(BWAPI::UnitTypes::Protoss_High_Templar);				
			}
			else{
				unit->train(BWAPI::UnitTypes::Protoss_Zealot);
			}

			

		}

		if (unit->isIdle() && unit->getType() == StarGate){
			unit->rightClick(Campus);
			if (UnitCount["Corsair_Count"] <= 12){
				unit->train(BWAPI::UnitTypes::Protoss_Corsair);
			}			
		}
	}

	



	









	BWAPI::Unitset CorsairSquad;
	CorsairSquad.clear();	

	int RandomDecision=0;

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$ Attack Process $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$	
	// Determine whether a scouter is in attack mode or searching mode.
	// Operation Wolf Pack
	for (auto & unit : _Commander.UnitSetPresent()){
		bool RogerThat = false;
		if (unit->getType() == Corsair){
			CorsairSquad.insert(unit);

			// CorsairSquad.mor

			// Release the stagmation
			if (unit->isUnderAttack()){

				

				unit->move(homePosition);		
				continue;
			}
			
									
			// Dodge this
			for (auto & unit2 : BWAPI::Broodwar->enemy()->getUnits()){
				if (unit2->getType() == BWAPI::UnitTypes::Zerg_Spore_Colony || unit2->getType() == BWAPI::UnitTypes::Zerg_Hydralisk){	

					if (unit2->isInWeaponRange(unit)){

						// Dodge to the 180 degree against to an enemy unit
						BWAPI::Position EnemyPosition = unit2->getPosition();
						BWAPI::Position MyPosition = unit->getPosition();
						BWAPI::Position NextPosition = BWAPI::Positions::None;

						double XValue = (MyPosition.y - EnemyPosition.y) * 10000;
						double YValue = (MyPosition.x - EnemyPosition.x) * 10000;

						XValue = round(max(min(XValue, BWAPI::Broodwar->mapHeight()), 1));
						YValue = round(max(min(YValue, BWAPI::Broodwar->mapWidth()), 1));

						NextPosition.x = YValue;
						NextPosition.y = XValue;

						unit->move(NextPosition);
						RogerThat = true;

						break;
					}
				}	
				else if (unit2->getType().isFlyer() && unit2->exists()){
					unit->attack(unit2);					
					RogerThat = true;
					break;
				}
			}

			if (RogerThat){
				continue;
			}


			if (unit->isIdle()){
				BWAPI::Position MyPosition = unit->getPosition();

				double dist = unit->getDistance(EnemyHomeBase);
				double dist2 = unit->getDistance(EnemyExpansion);



				// If you are in the edge
				if (MyPosition.x < 10 || MyPosition.y < 10){
					MyPosition.x = 1;
					MyPosition.y = 1;
					unit->attack(MyPosition);
					continue;
				}
				// We are already in the zero point
				else if (MyPosition.x < 10 && MyPosition.y < 10){
					MyPosition.x = 1;
					MyPosition.y = BWAPI::Broodwar->mapHeight()-1;
					unit->attack(MyPosition);					
					continue;
				}
				// We are in the enemy main base
				else if (dist < 10){
					// Go get him.
					unit->attack(EnemyExpansion);
					continue;
				}
				// I am at enemy's expansion
				else if (dist2 < 10){
					CorsairSearchMode = true;
					MyPosition.x = 1;
					MyPosition.y = 1;
					unit->attack(MyPosition);
					continue;
				}
				else{
					RandomDecision = rand() % 10 ;

					if (RandomDecision == 0){
						unit->attack(EnemyHomeBase);
						continue;
					}
					else if (RandomDecision == 1){
						BWAPI::Position MyPosition = unit->getPosition();
						MyPosition.x = BWAPI::Broodwar->mapWidth()-1;
						MyPosition.y = BWAPI::Broodwar->mapHeight()-1;
						unit->attack(MyPosition);
						continue;
					}
					else if (RandomDecision == 2){
						BWAPI::Position MyPosition = unit->getPosition();
						MyPosition.x = BWAPI::Broodwar->mapWidth() - 1;;
						MyPosition.y = 1;
						unit->attack(MyPosition);
						continue;
					}
					else if (RandomDecision == 3){
						BWAPI::Position MyPosition = unit->getPosition();
						MyPosition.x = 1;
						MyPosition.y = BWAPI::Broodwar->mapHeight() - 1;;
						unit->attack(MyPosition);
						continue;
					}
					else if (RandomDecision == 4){
						BWAPI::Position MyPosition = unit->getPosition();
						MyPosition.x = 1;
						MyPosition.y = 1;
						unit->attack(MyPosition);
						continue;
					}
					else if (RandomDecision == 5){	
						unit->attack(homePosition);
						continue;
					}
					else{
						CorsairSearchMode = true;
					}

				}

			}

			
			// Search Mode
			if (CorsairSearchMode && !SearchingPosition.empty()){
				BWAPI::Position targetPosition = SearchingPosition.back();
				unit->attack(targetPosition, true);
				SearchingPosition.pop_back();

				if (SearchingPosition.empty()){
					CorsairSearchMode = false;
					for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations()){
						BWAPI::Position targetPosition = startLocation->getPosition();
						SearchingPosition.push_back(targetPosition);
					}
				}


			}
		}
	}


	static int lastChecked = 0;
	if (lastChecked + 1000 < BWAPI::Broodwar->getFrameCount() && !CorsairSquad.empty()){
		lastChecked = BWAPI::Broodwar->getFrameCount();
		Broodwar << "Let's reunion." << std::endl;;
		CorsairSquad.move(homePosition);
	}

		


	


	






























	// $$$$$$$$$$$$$$$$$     Upgrading $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// Singularity Upgrading
	
	//for (auto & unit : _Commander.UnitSetPresent()){
	//	if (unit->getType() == Cybernetics && !unit->isUpgrading()){				
	//		unit->upgrade(BWAPI::UpgradeTypes::Singularity_Charge);
	//	}
	//}
	

	// If a building should be upgraded, please do it at any time.
	for (auto & unit : _Commander.UnitSetPresent()){
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








// !BWAPI::Broodwar->isExplored(startLocation->getTilePosition())




//// Detect the enemy main hatchery
//if (!EnemyBaseUpdate){
//	BWAPI::Unit EnemyResourceDepot = nullptr;
//	for (auto & unit : BWAPI::Broodwar->enemy()->getUnits()){
//		double closestDist = 100000;
//		if (unit->getType().isResourceDepot()){
//			double dist = unit->getDistance(ScouterTarget);
//			if (dist < closestDist){
//				EnemyResourceDepot = unit;
//				closestDist = dist;

//				EnemyHomeBase = EnemyResourceDepot->getPosition();
//				Broodwar->sendText("CAndidate Enemy Base %.1d : %.1d", EnemyHomeBase.x, EnemyHomeBase.y);
//			}
//		}
//	}
//	EnemyHomeBase = EnemyResourceDepot->getPosition();
//	Broodwar->sendText("Final Enemy Base %.1d : %.1d", EnemyHomeBase.x, EnemyHomeBase.y);
//	EnemyBaseUpdate = true;
//}			

//BWAPI::Position EnemyPosition = Position(Broodwar->enemy()->getStartLocation());
//Broodwar->sendText("BWAPI Enemy Base %.1d : %.1d", EnemyPosition.x, EnemyPosition.y);

/*BWTA::BaseLocation * enemyBaseLocation = BWTA::getStartLocation(BWAPI::Broodwar->enemy());
if (enemyBaseLocation){
BWAPI::Position TempPosition = enemyBaseLocation->getPosition();
Broodwar->sendText("BWTA Enemy Base %.1d : %.1d", TempPosition.x, TempPosition.y);
}*/
/*
for (auto & unit : BWAPI::Broodwar->enemy()->getUnits()){
if (unit->getType().isResourceDepot()){
BWAPI::Position EnemyPosition = unit->getPosition();
Broodwar->sendText("%.2d : %.2d", EnemyPosition.x, EnemyPosition.y);

EnemyDetection = true;
break;
}
}

}
}		*/



// 



// BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());

// BWTA::getStartLocation(BWAPI::Broodwar->enemy())

// BWTA::Region * enemyRegion = enemyBaseLocation->getRegion();




//	 // for each enemy worker
//	 for (auto & unit : BWAPI::Broodwar->enemy()->getUnits())
//	 {
//		 if (unit->getType().isWorker())
//		 {
//			 double dist = unit->getDistance(geyser);
//
//			 if (dist < 800 && dist > maxDist)
//			 {
//				 maxDist = dist;
//				 enemyWorker = unit;
//			 }
//		 }
//	 }
//
//	 return enemyWorker;
//}




// Broodwar->sendText("Pylon :%.3u", WorkerSet.size());


//BWAPI::Position  FirstPylonTilePosition  = mineral->getPosition();
//Broodwar->sendText("Pylon :%.3f", dist);













//// Iterate through all the units that we own
//for (auto &u : Broodwar->self()->getUnits())
//{
//  // Ignore the unit if it no longer exists
//  // Make sure to include this block when handling any Unit pointer!
//  if ( !u->exists() )
//    continue;

//  // Ignore the unit if it has one of the following status ailments
//  if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() )
//    continue;

//  // Ignore the unit if it is in one of the following states
//  if ( u->isLoaded() || !u->isPowered() || u->isStuck() )
//    continue;

//  // Ignore the unit if it is incomplete or busy constructing
//  if ( !u->isCompleted() || u->isConstructing() )
//    continue;
//

//  // Finally make the unit do some stuff!


//  // If the unit is a worker unit
//  if ( u->getType().isWorker() )
//  {
//    // if our worker is idle
//    if ( u->isIdle() )
//    {
//      // Order workers carrying a resource to return them to the center,
//      // otherwise find a mineral patch to harvest.
//      if ( u->isCarryingGas() || u->isCarryingMinerals() )
//      {
//        u->returnCargo();
//      }
//      else if ( !u->getPowerUp() )  // The worker cannot harvest anything if it
//      {                             // is carrying a powerup such as a flag
//        // Harvest from the nearest mineral patch or gas refinery
//        if ( !u->gather( u->getClosestUnit( IsMineralField || IsRefinery )) )
//        {
//          // If the call fails, then print the last error message
//          Broodwar << Broodwar->getLastError() << std::endl;
//        }

//      } // closure: has no powerup
//    } // closure: if idle

//  }
//  else if ( u->getType().isResourceDepot() ) // A resource depot is a Command Center, Nexus, or Hatchery
//  {

//    // Order the depot to construct more workers! But only when it is idle.
//    if ( u->isIdle() && !u->train(u->getType().getRace().getWorker()) )
//    {
//      // If that fails, draw the error at the location so that you can visibly see what went wrong!
//      // However, drawing the error once will only appear for a single frame
//      // so create an event that keeps it on the screen for some frames
//      Position pos = u->getPosition();
//      Error lastErr = Broodwar->getLastError();
//      Broodwar->registerEvent([pos,lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
//                              nullptr,    // condition
//                              Broodwar->getLatencyFrames());  // frames to run

//      // Retrieve the supply provider type in the case that we have run out of supplies
//      UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
//      static int lastChecked = 0;

//      // If we are supply blocked and haven't tried constructing more recently
//      if (  lastErr == Errors::Insufficient_Supply &&
//            lastChecked + 400 < Broodwar->getFrameCount() &&
//            Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0 )
//      {
//        lastChecked = Broodwar->getFrameCount();

//        // Retrieve a unit that is capable of constructing the supply needed
//        Unit supplyBuilder = u->getClosestUnit(  GetType == supplyProviderType.whatBuilds().first &&
//                                                  (IsIdle || IsGatheringMinerals) &&
//                                                  IsOwned);
//        // If a unit was found
//        if ( supplyBuilder )
//        {
//          if ( supplyProviderType.isBuilding() )
//          {
//            TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
//            if ( targetBuildLocation )
//            {
//              // Register an event that draws the target build location
//              Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*)
//                                      {
//                                        Broodwar->drawBoxMap( Position(targetBuildLocation),
//                                                              Position(targetBuildLocation + supplyProviderType.tileSize()),
//                                                              Colors::Blue);
//                                      },
//                                      nullptr,  // condition
//                                      supplyProviderType.buildTime() + 100 );  // frames to run

//              // Order the builder to construct the supply structure
//              supplyBuilder->build( supplyProviderType, targetBuildLocation );
//            }
//          }
//          else
//          {
//            // Train the supply provider (Overlord) if the provider is not a structure
//            supplyBuilder->train( supplyProviderType );
//          }
//        } // closure: supplyBuilder is valid
//      } // closure: insufficient supply
//    } // closure: failed to train idle unit

//  }

//} // closure: unit iterator





// Broodwar->sendText("%.2d  / Unit: %s", A, unit->getType().c_str());










DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();

	analyzed = true;
	analysis_just_finished = true;
	return 0;
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
	std::map<std::string, int>	UnitCount = _Commander.UnitCounterPresenter();
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
	
	
	

	//if (Broodwar->isReplay())
	//{
	//	// if we are in a replay, then we will print out the build order of the structures
	//	if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
	//	{
	//		int seconds = Broodwar->getFrameCount() / 24;
	//		int minutes = seconds / 60;
	//		seconds %= 60;
	//		Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
	//	}
	//}
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
		// Position pos = unit->getPosition();
		//int seconds = Broodwar->getFrameCount() / 24;
		//int minutes = seconds / 60;
		//seconds %= 60;
		// Broodwar->sendText("%.2d:%.2d: %s completed a %s", pos, unit->getPlayer()->getName().c_str(), unit->getType().c_str());


		std::map<std::string, int>	UnitCount;
		UnitCount = _Commander.UnitCounterPresenter();

		if (unit->getType() == BWAPI::UnitTypes::Protoss_Assimilator)
		{
			FirstGasExist = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
		{
			FirstCybernetics = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Forge){
			FirstForge = true;
		}
		else if (unit->getType() == BWAPI::UpgradeTypes::Singularity_Charge){
			Singularity = true;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun){
			FirstAdun = true;
			MaxGateWayCount = 2;
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives){
			FirstTemplerArchive = true;			
		}
		else if (unit->getType() == BWAPI::UpgradeTypes::Leg_Enhancements){
			Leg_Enhancements = true;
			
		}
		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus && UnitCount["Nexus_Count"]==2){

			MaxGateWayCount = 5;
			
			// Build More Pylons
			std::vector<BWAPI::TilePosition>	PylonTilePosition = BuildingManager::Instance().PylonSetPresent();
			
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, TilePosition(HillPosition2), 12));
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, TilePosition(HillPosition2), 12));
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, TilePosition(HillPosition2), 12));
						
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, unit->getTilePosition(), 12));
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, unit->getTilePosition(), 12));
			PylonTilePosition.push_back(BWAPI::Broodwar->getBuildLocation(Pylon, unit->getTilePosition(), 12));
			
						
			BuildingManager::Instance().PylonLocationSaver(PylonTilePosition);

			
			// Set the New ChokeLines
			//Campus = HillPosition2;

			BWAPI::Position Temp = BWAPI::Positions::None;

			Temp.y = (Broodwar->mapHeight() )* 16;
			Temp.x=(Broodwar->mapWidth())*16;

			Campus.x = (5 * HillPosition2.x + 1* Temp.x) / (1 + 5);
			Campus.y = (5 * HillPosition2.y + 1* Temp.y) / (1 + 5);



			// Move Forward our armies to the new choke lines
			for (auto & unit : _Commander.UnitSetPresent()){
				// We found a gateway, and it is not generating a unit
				if (unit->getType() == Dragoon || unit->getType() == Zealot){
					unit->rightClick(Campus);
				}
			}

		}

		else if (unit->getType() == BWAPI::UnitTypes::Protoss_Fleet_Beacon)
		{
			FirstFleetBeacon = true;
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





//int d=0;
//for (auto &mineral : FirstMineralSet){
//	d = d + 1;
//	BWAPI::Position targetBuildLocation = mineral->getPosition();
//	BWAPI::Broodwar->sendText("%.2d %.2d / Unit: %s, %d", targetBuildLocation.x, targetBuildLocation.y, mineral->getType().c_str(), d);
//}



//// Very Basic Initiation
//for (auto &unit : Broodwar->self()->getUnits()){
//	// A resource depot is a Command Center, Nexus, or Hatchery
//	if (unit->getType().isResourceDepot()){
//		// Generate the first worker
//		unit->train(unit->getType().getRace().getWorker());
//	}
//	// If the unit is a worker unit
//	else if (unit->getType().isWorker()) {
//		BWAPI::Unit ClosestMineral = nullptr;
//		double closestDist = 100000;

//		// For the given initial Minerals, find the closest mineral one by one
//		for (auto &mineral : Minerals){
//			double dist = unit->getDistance(mineral->getPosition());
//			if (dist < closestDist){
//				ClosestMineral = mineral;
//				closestDist = dist;
//			}
//			unit->gather(ClosestMineral);
//			Minerals.erase(ClosestMineral);
//		}
//	}
//}






//// Go to work and Generate Probe
//for (auto &unit : _Commander.UnitSetPresent()){   //  Broodwar->self()->getUnits()
//	// If the unit is a worker unit
//	if (unit->getType().isWorker()) {


//		// if our worker is idle
//		if (unit->isIdle()) {
//			// Order workers carrying a resource to return them to the center		     
//			if (unit->isCarryingGas() || unit->isCarryingMinerals()){
//				unit->returnCargo();
//			}
//			// otherwise find a mineral patch to harvest.					
//			else{
//				BWAPI::Unit ClosestMineral = nullptr;
//				double closestDist = 100000;

//				// For the given initial Minerals, find the closest mineral one by one					
//				for (auto &mineral : FirstMineralSet){
//					double dist = unit->getDistance(mineral->getPosition());
//					if (dist < closestDist){
//						ClosestMineral = mineral;
//						closestDist = dist;
//					}
//				}
//				unit->gather(ClosestMineral);
//				FirstMineralSet.erase(ClosestMineral);
//				_Commander.MineralSaver(FirstMineralSet);
//			}
//		}
//	}
//	
//	else if (unit->getType().isResourceDepot()){
//		// Order the depot to construct more workers! But only when it is idle.
//		if (unit->isIdle()){
//			// Train a worker
//			unit->train(unit->getType().getRace().getWorker());
//		}
//	}
//}




//// Python Map
//// 12
//if (homePosition.y < 500)
//{
//	StartClockPosition = 12;

//	

//}
//// 3
//else if (homePosition.x > 3000)
//{
//	StartClockPosition = 3;


//	FirstPylonLocation.x = homeTilePosition.x;
//	FirstPylonLocation.y = homeTilePosition.y+2;

//}
//// 6
//else if (homePosition.y > 3000)
//{
//	StartClockPosition = 6;
//}
//// 9
//else if (homePosition.x < 500)
//{
//	StartClockPosition = 9;

//}




//if (FirstGateExist && !FirstGasExist){

//	BWAPI::TilePosition closestGeyser = BWAPI::TilePositions::None;
//	double minGeyserDistanceFromHome = std::numeric_limits<double>::max();		

//	// for each geyser
//	for (auto & geyser : Broodwar->getStaticGeysers()){
//		if (geyser->getType() != BWAPI::UnitTypes::Resource_Vespene_Geyser){
//			continue;
//		}

//		BWAPI::Position geyserPos = geyser->getInitialPosition();
//		
//		// check to see if it's next to one of our depots
//		bool nearDepot = false;
//		for (auto & unit : BWAPI::Broodwar->self()->getUnits()){
//			if (unit->getType().isResourceDepot() && unit->getDistance(geyserPos) < 300){
//				nearDepot = true;
//			}
//		}

//		if (nearDepot){
//			double homeDistance = geyser->getDistance(homePosition);

//			if (homeDistance < minGeyserDistanceFromHome){
//				minGeyserDistanceFromHome = homeDistance;
//				closestGeyser = geyser->getInitialTilePosition();
//			}
//		}


//	
//	}
//}




//// Build the First supply unit	
//if (BWAPI::Broodwar->self()->supplyUsed() / 2 >= 8 && UnitCount["Pylon_Count"]==0){
//	static int lastChecked = 0;
//	// If we are supply blocked and haven't tried constructing more recently
//	if (lastChecked + 300 < Broodwar->getFrameCount() && Broodwar->self()->incompleteUnitCount(Pylon) == 0){
//		lastChecked = Broodwar->getFrameCount();

//		// Retrieve a unit that is capable of constructing the supply needed
//		Unit supplyBuilder = ResourceDepot->getClosestUnit(GetType == Pylon.whatBuilds().first && (IsWorker)); // IsIdle || IsCarryingMinerals
//		// If a unit was found
//		if (supplyBuilder){
//			TilePosition targetBuildLocation = Broodwar->getBuildLocation(Pylon, PylonTilePosition[0], 4); // supplyBuilder->getTilePosition()
//					
//			if (targetBuildLocation){
//				// Order the builder to construct the supply structure	
//				supplyBuilder->build(Pylon, targetBuildLocation);
//				Scout = supplyBuilder;
//			}
//		}			
//	}
//}
