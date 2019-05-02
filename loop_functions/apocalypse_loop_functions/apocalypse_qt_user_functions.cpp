#include "apocalypse_qt_user_functions.h"
#include <controllers/eyebot_apocalypse/eyebot_being.h>
#include <controllers/eyebot_apocalypse/eyebot_medic.h>
#include <argos3/core/simulator/entity/controllable_entity.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <string>
using namespace argos;

/****************************************/
/****************************************/

CApocalypseQTUserFunctions::CApocalypseQTUserFunctions() {
   RegisterUserFunction<CApocalypseQTUserFunctions,CEyeBotEntity>(&CApocalypseQTUserFunctions::Draw);
}

/****************************************/
/****************************************/

void CApocalypseQTUserFunctions::Draw(CEyeBotEntity& c_entity) {
   bool IsMedic = false;
   bool myHealth = false;
   bool visualize = false;
   int8_t MyCuringTime;
   std::string entity_name = c_entity.GetId().substr(0,1);
   if (entity_name.compare("M")){
      CEyeBotBeing& BeingcController = dynamic_cast<CEyeBotBeing&>(c_entity.GetControllableEntity().GetController());
      myHealth = BeingcController.IsHealthy();
      visualize = BeingcController.Visualize();
      MyCuringTime = BeingcController.GetCuringTime();
   } else if (entity_name.compare("B")){
      IsMedic = true;
   }

   if (IsMedic){
      DrawCylinder(
         CVector3(0.0f, 0.0f, 0.2f), 
         CQuaternion(),
         0.25f,
         0.05f,
         CColor::BLUE);
   } else {

   if(myHealth) {
      DrawCylinder(
         CVector3(0.0f, 0.0f, 0.2f), 
         CQuaternion(),
         0.25f,
         0.05f,
         CColor::GREEN);
   }else {
      DrawCylinder(
         CVector3(0.0f, 0.0f, 0.2f), 
         CQuaternion(),
         0.25f,
         0.05f,
         CColor::RED);

         if(visualize){
      DrawText(CVector3(0.0, 0.0, 0.5),   // position
               std::to_string(MyCuringTime), CColor::BLACK); // text
         }
   }

   }
}

/****************************************/
/****************************************/

REGISTER_QTOPENGL_USER_FUNCTIONS(CApocalypseQTUserFunctions, "apocalypse_qt_user_functions")
