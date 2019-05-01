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
/* Definition of the range-and-bearing actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
/* Definition of the range-and-bearing sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
/* Definition of the eye-bot light sensor */
#include <argos3/plugins/robots/eye-bot/control_interface/ci_eyebot_light_sensor.h>
/* Definition of the positioning sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>

#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>

/* Vector2 definitions */
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/math/rng.h>

/*
 * All the ARGoS stuff in the 'argos' namespace.
 * With this statement, you save typing argos:: every time.
 */
using namespace argos;

/*
 * A controller is simply an implementation of the CCI_Controller class.
 */
class CEyeBotBeing : public CCI_Controller
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
      Real AttractionFactor;
      /* Exponent of the Lennard-Jones potential */
      Real Exponent;
      /* Max length for the light interaction force vector */
      Real GoalGain;

      void Init(TConfigurationNode &t_node);
      Real GeneralizedLennardJones(Real f_distance);
   };

   struct SApocalypseParams
   {
      /* The time needed for the symptoms to appear on the infected agent */
      Real InfectionStart;
      /* The time needed after infection for the infected agent to die */
      Real InfectionTerminal;
      /* The minimum distance needed to infect a healthy agent */
      Real InfectionDistance;
      /* The minimum distance for a medic to cure an infected agent */
      Real CuringDistance;
      /* The minimum time needed for curing to be done */
      Real CuringTime;
      /* The percentage of the population that is infected */
      Real InfectionPercentage;
      /* Weight of interaction between healthy and healthy agents */
      Real alpha_healthy;
      /* Weight of interaction between healthy and infected agents */
      Real beta_healthy;
      /* Weight of interaction between healthy and free medic agents */
      Real gamma_healthy;
      /* Weight of interaction between healthy and busy medic agents */
      Real delta_healthy;
      /* Weight of interaction between infected and healthy agents */
      Real alpha_infected;
      /* Weight of interaction between infected and infected agents */
      Real beta_infected;
      /* Weight of interaction between infected and free medic agents */
      Real gamma_infected;
      /* Weight of interaction between infected and busy medic agents */
      Real delta_infected;

      void Init(TConfigurationNode &t_node);
   };


public:
   /* Class constructor. */
   CEyeBotBeing();

   /* Class destructor. */
   virtual ~CEyeBotBeing() {}

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
   virtual void Destroy() {

   }

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
    * Search for any cure sent by a medic agent in the neighborhood.
    * Used by InfectedBehavior().
    */
   bool SeachForCure();
   /*
    * Initiate Healthy Behavior for infected agent
    */
   void HealthyBehavior();
   /*
    * Initiate Infected Behavior for healthy agent
    */
   void InfectedBehavior();
   /*
    * Search for any infected agent in the neighborhood.
    * Used by HealthyBehavior().
    */
   bool SearchForInfected();
   /*
    * Search for any curing signal sent by a medic agent in the neighborhood.
    * Used by InfectedBehavior().
    */
   bool SearchForMedicSignal();
   /*
    * Initiate Death Behavior for infected agent
    */
   void Die();
   /*
    * Main behavior of eyebot being
    */
   void MainBehavior();

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
      STATE_BEING_CURED,
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
   CCI_LEDsActuator* m_pcLEDs;
   /* The flocking interaction parameters. */
   SFlockingInteractionParams m_sFlockingParams;
   /* The Apocalypse parameters. */
   SApocalypseParams m_sApocalypseParams;
   /* Current robot state */
   EState m_eState;
   /* Current robot Health State */
   HState m_HState;
   /* Infection Time */
   Real InfectionTime = -1;
   /* The random number generator */
   CRandom::CRNG *m_pcRNG;
   /* Used as a range for uniform number generation */
   CRange<Real> ProbRange;
   /* Current target position */
   CVector3 m_cTargetPos;
   /* Current curing time */
   Real CurrentCuringTime;
   /* Flag to detect medic signal */
   bool MedicSignal = false;
   /* Flag to detect curing signal */
   bool CuringSignal = false;
};

#endif
