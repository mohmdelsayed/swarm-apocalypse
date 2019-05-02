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

}

/****************************************/
/****************************************/

void CApocalypseLoopFunctions::Reset() {

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
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CApocalypseLoopFunctions, "apocalypse_loop_functions")
