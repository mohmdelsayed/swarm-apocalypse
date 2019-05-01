#include "id_qtuser_functions.h"
#include <controllers/eyebot_apocalypse/eyebot_being.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>

/****************************************/
/****************************************/

CIDQTUserFunctions::CIDQTUserFunctions() {
   RegisterUserFunction<CIDQTUserFunctions,CEyeBotEntity>(&CIDQTUserFunctions::Draw);
}

/****************************************/
/****************************************/

void CIDQTUserFunctions::Draw(CEyeBotEntity& c_entity) {
   /* The position of the text is expressed wrt the reference point of the footbot
    * For a foot-bot, the reference point is the center of its base.
    * See also the description in
    * $ argos3 -q foot-bot
    */
   std::string entity_behavior = c_entity.GetId().substr(0,1);
   CColor myColor;
   if (entity_behavior.compare("M")){
      myColor = CColor::RED;
   } else {
      myColor = CColor::BLUE;
      }
   DrawText(CVector3(0.0, 0.0, 0.3),   // position
            entity_behavior, myColor); // text
}

/***********************************/
REGISTER_QTOPENGL_USER_FUNCTIONS(CIDQTUserFunctions, "id_qtuser_functions")
