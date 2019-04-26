add_library(eyebot_being MODULE eyebot_being.h eyebot_being.cpp)
add_library(eyebot_medic MODULE eyebot_medic.h eyebot_medic.cpp)


  target_link_libraries(eyebot_being
  argos3core_simulator
  argos3plugin_simulator_eyebot
  argos3plugin_simulator_genericrobot)

  target_link_libraries(eyebot_medic
  argos3core_simulator
  argos3plugin_simulator_eyebot
  argos3plugin_simulator_genericrobot)
