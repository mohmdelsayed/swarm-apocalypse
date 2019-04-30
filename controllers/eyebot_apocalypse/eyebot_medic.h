/*
 * AUTHOR: Carlo Pinciroli <cpinciro@ulb.ac.be>
 *
 * An example flocking controller for the eye-bot.
 *
 * This controller lets a group of eye-bots flock in an hexagonal lattice towards
 * a light source placed in the arena. To flock, it exploits a generalization of the
 * well known Lennard-Jones potential. The parameters of the Lennard-Jones function
 * were chosen through a simple trial-and-error procedure on its graph.
 *
 * This controller is meant to be used with the XML file:
 *    experiments/eyebot_flocking.argos
 */

#ifndef EYEBOT_FLOCKING_H
#define EYEBOT_FLOCKING_H

/*
 * Include some necessary headers.
 */
/* Definition of the CCI_Controller class. */
#include <argos3/core/control_interface/ci_controller.h>
/* Definition of the quadrotor positioning actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_position_actuator.h>
/* Definition of the LEDs actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
/* Definition of the range-and-bearing actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
/* Definition of the range-and-bearing sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
/* Definition of the eye-bot light sensor */
#include <argos3/plugins/robots/eye-bot/control_interface/ci_eyebot_light_sensor.h>
/* Definition of the positioning sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
/* Vector2 definitions */
#include <argos3/core/utility/math/vector2.h>

/*
 * All the ARGoS stuff in the 'argos' namespace.
 * With this statement, you save typing argos:: every time.
 */
using namespace argos;

/*
 * A controller is simply an implementation of the CCI_Controller class.
 */
class CEyeBotMedic : public CCI_Controller
{

public:
   /*
    * The following variables are used as parameters for
    * flocking interaction. You can set their value
    * in the <parameters> section of the XML configuration
    * file, under the
    * <controllers><eyebot_flocking_controller><parameters><flocking>
    * section.
    */
   struct SFlockingInteractionParams
   {
      /* Target robot-robot distance in cm */
      Real TargetDistance;
      /* Attraction weight of the Lennard-Jones potential */
      Real Gain;
      /* Exponent of the Lennard-Jones potential */
      Real Exponent;
      /* Max length for the light interaction force vector */
      Real MaxInteraction;

      void Init(TConfigurationNode &t_node);
      Real GeneralizedLennardJones(Real f_distance);
   };

   struct SApocalypseParams
   {
      /* The minimum distance for medic to cure an infected agent */
      Real CuringDistance;
      /* The minimum time needed for curing to be done */
      Real CuringTime;
      /* Weight of interaction between medic and healthy agents */
      Real alpha_medic;
      /* Weight of interaction between medic and infected agents */
      Real beta_medic;
      /* Weight of interaction between medic and free medic agents */
      Real gamma1_medic;
      /* Weight of interaction between medic and busy medic agents */
      Real gamma2_medic;
      void Init(TConfigurationNode &t_node);
   };


public:
   /* Class constructor. */
   CEyeBotMedic();

   /* Class destructor. */
   virtual ~CEyeBotMedic() {}

   /*
    * This function initializes the controller.
    * The 't_node' variable points to the <parameters> section in the XML file
    * in the <controllers><eyebot_flocking_controller> section.
    */
   virtual void Init(TConfigurationNode &t_node);

   /*
    * This function is called once every time step.
    * The length of the time step is set in the XML file.
    */
   virtual void ControlStep();

   /*
    * This function resets the controller to its state right after the Init().
    * It is called when you press the reset button in the GUI.
    * In this example controller there is no need for resetting anything, so
    * the function could have been omitted. It's here just for completeness.
    */
   virtual void Reset();

   /*
    * Called to cleanup what done by Init() when the experiment finishes.
    * In this example controller there is no need for clean anything up, so
    * the function could have been omitted. It's here just for completeness.
    */
   virtual void Destroy() {}

private:
   /*
    * Takes off the robot.
    */
   void TakeOff();
   /*
    * Calculates overall interaction between agents and moves the agent towards the resultant
    */
   void Flock(CVector2);
   /*
    * Calculates the vector to the closest light.
    * Used by Flock().
    */
   CVector2 VectorToLight();
   /*
    * Main behavior of medic
    */
   void MainBehavior();
   /*
    * Initiate Curing Behavior for medic agent
    */
   void CuringBehavior();
   /*
    * Initiate Advertising Behavior for medic agent
    */
   void AdvertisingBehavior();
   /*
    * Calculates the flocking interaction vector for Healthy agents.
    * Used by Flock().
    */
   CVector2 HealthyFlockingVector();
   /*
    * Calculates the flocking interaction vector for infected agents.
    * Used by Flock().
    */
   CVector2 InfectedFlockingVector();
   /*
    * Calculates the flocking interaction vector for free medic agents.
    * Used by Flock().
    */
   CVector2 MedicFreeFlockingVector();
   /*
    * Calculates the flocking interaction vector for busy medic agents.
    * Used by Flock().
    */
   CVector2 MedicBusyFlockingVector();
   /*
    * Search for any infected agent in the neighborhood.
    * Used by AdvertisingBehavior().
    */
   bool SearchForInfected();

private:
   /* Current robot state */
   enum EState
   {
      STATE_START = 1,
      STATE_TAKE_OFF,
      STATE_FLOCK
   };
   /* Current medic state */
   enum MState
   {
      STATE_FREE = 1,
      STATE_BUSY
   };

   /* Current robot health state */
   enum HState
   {
      STATE_HEALTHY = 1,
      STATE_INFECTED,
      STATE_DEAD
   };

   /* Current robot infected state */
   enum IState
   {
      STATE_CURED = 1,
      STATE_CURING
   };

private:
   /* Pointer to the quadrotor position actuator */
   CCI_QuadRotorPositionActuator *m_pcPosAct;
   /* Pointer to the range-and-bearing actuator */
   CCI_RangeAndBearingActuator *m_pcRABAct;
   /* Pointer to the range-and-bearing sensor */
   CCI_RangeAndBearingSensor *m_pcRABSens;
   /* Pointer to the eye-bot light sensor */
   CCI_EyeBotLightSensor *m_pcLightSens;
   /* Pointer to the positioning sensor */
   CCI_PositioningSensor *m_pcPosSens;
   /* Pointer to the LEDs actuator */
   CCI_LEDsActuator *m_pcLEDs;
   /* The flocking interaction parameters. */
   SFlockingInteractionParams m_sFlockingParams;
   /* The Apocalypse parameters. */
   SApocalypseParams m_sApocalypseParams;
   /* Current robot state */
   EState m_eState;
   /* Current medic state */
   MState m_MState;
   /* Current infected state */
   IState m_IState;
   /* Curing Time */
   Real TotalCuringTime;
   /* Current target position */
   CVector3 m_cTargetPos;
   bool StopToCure = false;
};

#endif
