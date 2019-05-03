#include "apocalypse_loop_functions.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/robots/eye-bot/simulator/eyebot_entity.h>
#include <controllers/eyebot_apocalypse/eyebot_being.h>

/****************************************/
/****************************************/

CApocalypseLoopFunctions::CApocalypseLoopFunctions() :
   m_pcRNG(NULL) {
}

/****************************************/
/****************************************/

void CApocalypseLoopFunctions::Init(TConfigurationNode& t_node) {
   timeCounter = 0;
}

/****************************************/
/****************************************/

void CApocalypseLoopFunctions::Reset() {
   noDeaths = 0;
}

/****************************************/
/****************************************/

void CApocalypseLoopFunctions::Destroy() {
}

/****************************************/
/****************************************/

CColor CApocalypseLoopFunctions::GetFloorColor(const CVector2& c_position_on_plane) {
}

/****************************************/
/****************************************/

void CApocalypseLoopFunctions::PreStep() {
   CSpace::TMapPerType& m_cEyeBots = GetSpace().GetEntitiesByType("eye-bot");
   noDeaths = 0;
   void* cController;
   for(CSpace::TMapPerType::iterator it = m_cEyeBots.begin();
       it != m_cEyeBots.end();
       ++it) {

      /* Get handle to foot-bot entity and controller */
      CEyeBotEntity& cEyeBot = *any_cast<CEyeBotEntity*>(it->second);

      std::string entity_name = cEyeBot.GetId().substr(0,1);
      if (entity_name.compare("M")){
         CEyeBotBeing& cController = dynamic_cast<CEyeBotBeing&>(cEyeBot.GetControllableEntity().GetController());

         if(cController.IsDead()) noDeaths++;
      }
   }
   LOG << "[t=" << timeCounter++ << "]" << " Number of Deaths: " << noDeaths << std::endl;
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CApocalypseLoopFunctions, "apocalypse_loop_functions")
