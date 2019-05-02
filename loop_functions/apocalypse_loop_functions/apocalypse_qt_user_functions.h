#ifndef FORAGING_QT_USER_FUNCTIONS_H
#define FORAGING_QT_USER_FUNCTIONS_H

#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
#include <argos3/plugins/robots/eye-bot/simulator/eyebot_entity.h>

using namespace argos;

class CApocalypseQTUserFunctions : public CQTOpenGLUserFunctions {

public:

   CApocalypseQTUserFunctions();

   virtual ~CApocalypseQTUserFunctions() {}

   void Draw(CEyeBotEntity& c_entity);
   
};

#endif
