/* Include the controller definition */
#include "eyebot_being.h"
/* Function definitions for XML parsing */
#include <argos3/core/utility/configuration/argos_configuration.h>
/* Function definitions for logging */
#include <argos3/core/utility/logging/argos_log.h>

/****************************************/
/****************************************/

/* Tolerance for the distance to a target point to decide to do something else */
static const Real POSITIONING_TOLERANCE = 0.01f;

/****************************************/
/****************************************/

void CEyeBotBeing::SFlockingInteractionParams::Init(TConfigurationNode &t_node)
{
   try
   {
      GetNodeAttribute(t_node, "target_distance", TargetDistance);
      GetNodeAttribute(t_node, "gain", Gain);
      GetNodeAttribute(t_node, "exponent", Exponent);
      GetNodeAttribute(t_node, "max_interaction", MaxInteraction);
   }
   catch (CARGoSException &ex)
   {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing controller flocking parameters.", ex);
   }
}


/****************************************/
/****************************************/

void CEyeBotBeing::SApocalypseParams::Init(TConfigurationNode &t_node)
{
   try
   {
      GetNodeAttribute(t_node, "InfectionPercentage", InfectionPercentage);
      GetNodeAttribute(t_node, "InfectionStart", InfectionStart);
      GetNodeAttribute(t_node, "InfectionTerminal", InfectionTerminal);
      GetNodeAttribute(t_node, "InfectionDistance", InfectionDistance);
      GetNodeAttribute(t_node, "alpha_healthy", alpha_healthy);
      GetNodeAttribute(t_node, "beta_healthy", beta_healthy);
      GetNodeAttribute(t_node, "gamma1_healthy", gamma1_healthy);
      GetNodeAttribute(t_node, "gamma2_healthy", gamma2_healthy);
      GetNodeAttribute(t_node, "alpha_infected", alpha_infected);
      GetNodeAttribute(t_node, "beta_infected", beta_infected);
      GetNodeAttribute(t_node, "gamma1_infected", gamma1_infected);
      GetNodeAttribute(t_node, "gamma2_infected", gamma2_infected);
      GetNodeAttribute(t_node, "CuringTime", CuringTime);
      GetNodeAttribute(t_node, "CuringDistance", CuringDistance);
   }
   catch (CARGoSException &ex)
   {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing controller Apocalypse parameters.", ex);
   }
}

/****************************************/
/****************************************/

/*
 * This function is a generalization of the Lennard-Jones potential
 */
Real CEyeBotBeing::SFlockingInteractionParams::GeneralizedLennardJones(Real f_distance)
{
   Real fNormDistExp = ::pow(TargetDistance / f_distance, Exponent);
   return -1* (fNormDistExp * fNormDistExp - Gain*fNormDistExp);
   /* Another option is to use Morse Potential */
   // Real fNorm = ::pow((1-exp(-Exponent*(f_distance-TargetDistance))),2);
   // return  -Gain *fNorm;
}

/****************************************/
/****************************************/

CEyeBotBeing::CEyeBotBeing() : m_pcPosAct(NULL),
                               m_pcRABAct(NULL),
                               m_pcRABSens(NULL),
                               m_pcRNG(NULL),
                               m_pcLEDs(NULL),
                               m_pcLightSens(NULL) {}

/****************************************/
/****************************************/

void CEyeBotBeing::Init(TConfigurationNode &t_node)
{
   /*
    * Get sensor/actuator handles
    *
    * The passed string (ex. "quadrotor_positioning") corresponds to the XML tag of the
    * device whose handle we want to have. For a list of allowed values, type at the
    * command prompt:
    *
    * $ argos3 -q actuators
    *
    * to have a list of all the possible actuators, or
    *
    * $ argos3 -q sensors
    *
    * to have a list of all the possible sensors.
    *
    * NOTE: ARGoS creates and initializes actuators and sensors internally, on the basis of
    *       the lists provided the configuration file at the
    *       <controllers><eyebot_diffusion><actuators> and
    *       <controllers><eyebot_diffusion><sensors> sections. If you forgot to
    *       list a device in the XML and then you request it here, an error occurs.
    */
   m_pcPosAct = GetActuator<CCI_QuadRotorPositionActuator>("quadrotor_position");
   m_pcRABAct = GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing");
   m_pcRABSens = GetSensor<CCI_RangeAndBearingSensor>("range_and_bearing");
   m_pcLightSens = GetSensor<CCI_EyeBotLightSensor>("eyebot_light");
   m_pcPosSens = GetSensor<CCI_PositioningSensor>("positioning");
   m_pcLEDs   = GetActuator<CCI_LEDsActuator>("leds");

   m_pcRNG = CRandom::CreateRNG("argos");

   /*
    * Parse the config file
    */
   try
   {
      /* Flocking-related */
      m_sFlockingParams.Init(GetNode(t_node, "flocking"));
      /* apocalypse-related */
      m_sApocalypseParams.Init(GetNode(t_node, "apocalypse"));

   }
   catch (CARGoSException &ex)
   {
      THROW_ARGOSEXCEPTION_NESTED("Error parsing the controller parameters.", ex);
   }
   /* Perform further initialization */
   Reset();
}

/****************************************/
/****************************************/

void CEyeBotBeing::Reset()
{
   /* Switch to state start */
   m_eState = STATE_START;
   /* Tell robots around that this robot is starting */
   m_pcRABAct->SetData(0, STATE_START);
   InfectionTime = 0;

   Real r = m_pcRNG->Uniform(CRange<Real>(0, 1));


   if (r > m_sApocalypseParams.InfectionPercentage)
   {
      m_HState = STATE_HEALTHY;
   }
   else
   {
      m_HState = STATE_INFECTED;
   }
}

/****************************************/
/****************************************/

void CEyeBotBeing::ControlStep()
{

   /* Important Note */
   
   /*
      channel 0 is for taking off:
      m_pcRABAct->SetData(0, STATE_START);
      m_pcRABAct->SetData(0, STATE_TAKE_OFF);
      m_pcRABAct->SetData(0, STATE_FLOCK);
      
      channel 1 is for health indication:
      m_pcRABAct->SetData(1, STATE_HEALTHY);
      m_pcRABAct->SetData(1, STATE_INFECTED);
      m_pcRABAct->SetData(1, STATE_DEAD);

      channel 2 is for medic availability:
      m_pcRABAct->SetData(2, STATE_FREE);
      m_pcRABAct->SetData(2, STATE_BUSY);

      channel 3 is for curing signals:
      m_pcRABAct->SetData(3, STATE_CURING);
      m_pcRABAct->SetData(3, STATE_CURED);

   */
   switch (m_eState)
   {
   case STATE_START:
      TakeOff();
      break;
   case STATE_TAKE_OFF:
      TakeOff();
      break;
   case STATE_FLOCK:
      MainBehavior();
      break;
   default:
      LOGERR << "[BUG] Unknown robot state: " << m_eState << std::endl;
   }
}

/****************************************/
/****************************************/

void CEyeBotBeing::MainBehavior()
{

   if (m_HState == STATE_HEALTHY)
   {
      HealthyBehavior();
   }
   if (m_HState == STATE_INFECTED)
   {
      InfectedBehavior();
   }
   if (m_HState == STATE_DEAD){
      Die();
   }
}
/****************************************/
/****************************************/

void CEyeBotBeing::TakeOff()
{
   if (m_eState != STATE_TAKE_OFF)
   {
      /* Switch to state take off */
      m_eState = STATE_TAKE_OFF;
      /* Set the target position on the vertical of the current position */
      m_cTargetPos = m_pcPosSens->GetReading().Position;
      m_cTargetPos.SetZ(3.0f);
      m_pcPosAct->SetAbsolutePosition(m_cTargetPos);
      /* Tell robots around that this eye-bot is taking off */
      m_pcRABAct->SetData(0, STATE_TAKE_OFF);
   }
   if (Distance(m_cTargetPos, m_pcPosSens->GetReading().Position) < POSITIONING_TOLERANCE)
   {
      // /* State transition */
      CVector2 forces;
      if (m_HState == STATE_INFECTED){
         forces = m_sApocalypseParams.alpha_infected*HealthyFlockingVector() + m_sApocalypseParams.beta_infected*InfectedFlockingVector() + m_sApocalypseParams.gamma1_infected*MedicFreeFlockingVector() + m_sApocalypseParams.gamma2_infected*MedicBusyFlockingVector();      } 
      if (m_HState == STATE_HEALTHY) {
         forces = m_sApocalypseParams.alpha_healthy*HealthyFlockingVector() + m_sApocalypseParams.beta_healthy*InfectedFlockingVector() + m_sApocalypseParams.gamma1_healthy*MedicFreeFlockingVector() + m_sApocalypseParams.gamma2_healthy*MedicBusyFlockingVector();      }
      Flock(forces);
   }
}

/****************************************/
/****************************************/

void CEyeBotBeing::Flock(CVector2 FlockingVector)
{
   if (m_eState != STATE_FLOCK)
   {
      /* Switch to state flock */
      m_eState = STATE_FLOCK;
      /* Tell robots around that this robot is ready to flock */
      m_pcRABAct->SetData(0, STATE_FLOCK);
   }
   CVector2 cDirection = VectorToLight() + FlockingVector;
   m_pcPosAct->SetRelativePosition(
       CVector3(cDirection.GetX(),
                cDirection.GetY(),
                0.0f));
}

/****************************************/
/****************************************/

CVector2 CEyeBotBeing::VectorToLight()
{
   /* Get light readings */
   const CCI_EyeBotLightSensor::TReadings &tReadings = m_pcLightSens->GetReadings();
   /* Calculate a normalized vector that points to the closest light */
   CVector2 cAccum;
   for (size_t i = 0; i < tReadings.size(); ++i)
   {
      cAccum += CVector2(tReadings[i].Value, tReadings[i].Angle);
   }
   if (cAccum.Length() > 0.0f)
   {
      /* Make the vector long as 1/10 of the max speed */
      cAccum.Normalize();
      cAccum *= 0.1f * m_sFlockingParams.MaxInteraction;
   }
   return cAccum;
}

/****************************************/
/****************************************/

CVector2 CEyeBotBeing::InfectedFlockingVector()
{
   /* Get RAB messages from nearby eye-bots */
   const CCI_RangeAndBearingSensor::TReadings &tMsgs = m_pcRABSens->GetReadings();
   /* Go through them to calculate the flocking interaction vector */
   if (!tMsgs.empty())
   {
      /* This will contain the final interaction vector */
      CVector2 cAccum;
      /* Used to calculate the vector length of each neighbor's contribution */
      Real fLJ;
      /* A counter for the neighbors in state flock */
      UInt32 unPeers = 0;
      for (size_t i = 0; i < tMsgs.size(); ++i)
      {
         /*
          * We consider only the neighbors in state flock
          */
         if (tMsgs[i].Data[0] == STATE_FLOCK && tMsgs[i].Data[1] == STATE_INFECTED)
         {
            /*
             * Take the message sender range and horizontal bearing
             * With the range, calculate the Lennard-Jones interaction force
             * Form a 2D vector with the interaction force and the bearing
             * Sum such vector to the accumulator
             */
            /* Calculate LJ */
            fLJ = m_sFlockingParams.GeneralizedLennardJones(tMsgs[i].Range);
            /* Sum to accumulator */
            cAccum += CVector2(fLJ,
                               tMsgs[i].HorizontalBearing);
            /* Count one more flocking neighbor */
            ++unPeers;
         }
      }
      if (unPeers > 0)
      {
         /* Divide the accumulator by the number of flocking neighbors */
         cAccum /= unPeers;
      }
      /* All done */
      return cAccum;
   }
   else
   {
      /* No messages received, no interaction */
      return CVector2(0,0);
   }
}

/****************************************/
/****************************************/


CVector2 CEyeBotBeing::HealthyFlockingVector()
{
   /* Get RAB messages from nearby eye-bots */
   const CCI_RangeAndBearingSensor::TReadings &tMsgs = m_pcRABSens->GetReadings();
   /* Go through them to calculate the flocking interaction vector */
   if (!tMsgs.empty())
   {
      /* This will contain the final interaction vector */
      CVector2 cAccum;
      /* Used to calculate the vector length of each neighbor's contribution */
      Real fLJ;
      /* A counter for the neighbors in state flock */
      UInt32 unPeers = 0;
      for (size_t i = 0; i < tMsgs.size(); ++i)
      {
         /*
          * We consider only the neighbors in state flock
          */
         if (tMsgs[i].Data[0] == STATE_FLOCK && tMsgs[i].Data[1] == STATE_HEALTHY)
         {
            /*
             * Take the message sender range and horizontal bearing
             * With the range, calculate the Lennard-Jones interaction force
             * Form a 2D vector with the interaction force and the bearing
             * Sum such vector to the accumulator
             */
            /* Calculate LJ */
            fLJ = m_sFlockingParams.GeneralizedLennardJones(tMsgs[i].Range);
            /* Sum to accumulator */
            cAccum += CVector2(fLJ,
                               tMsgs[i].HorizontalBearing);
            /* Count one more flocking neighbor */
            ++unPeers;
         }
      }
      if (unPeers > 0)
      {
         /* Divide the accumulator by the number of flocking neighbors */
         cAccum /= unPeers;
      }
      /* All done */
      return cAccum;
   }
   else
   {
      /* No messages received, no interaction */
      return CVector2();
   }
}
/***************************/
/**************************/

CVector2 CEyeBotBeing::MedicFreeFlockingVector()
{
   /* Get RAB messages from nearby eye-bots */
   const CCI_RangeAndBearingSensor::TReadings &tMsgs = m_pcRABSens->GetReadings();
   /* Go through them to calculate the flocking interaction vector */
   if (!tMsgs.empty())
   {
      /* This will contain the final interaction vector */
      CVector2 cAccum;
      /* Used to calculate the vector length of each neighbor's contribution */
      Real fLJ;
      /* A counter for the neighbors in state flock */
      UInt32 unPeers = 0;
      for (size_t i = 0; i < tMsgs.size(); ++i)
      {
         /*
          * We consider only the neighbors in state flock
          */
         if (tMsgs[i].Data[0] == STATE_FLOCK && tMsgs[i].Data[2] == STATE_FREE)
         {
            /*
             * Take the message sender range and horizontal bearing
             * With the range, calculate the Lennard-Jones interaction force
             * Form a 2D vector with the interaction force and the bearing
             * Sum such vector to the accumulator
             */
            /* Calculate LJ */
            fLJ = m_sFlockingParams.GeneralizedLennardJones(tMsgs[i].Range);
            /* Sum to accumulator */
            cAccum += CVector2(fLJ,
                               tMsgs[i].HorizontalBearing);
            /* Count one more flocking neighbor */
            ++unPeers;
         }
      }
      if (unPeers > 0)
      {
         /* Divide the accumulator by the number of flocking neighbors */
         cAccum /= unPeers;
      }
      /* All done */
      return cAccum;
   }
   else
   {
      /* No messages received, no interaction */
      return CVector2();
   }
}

/***************************/
/**************************/

CVector2 CEyeBotBeing::MedicBusyFlockingVector()
{
   /* Get RAB messages from nearby eye-bots */
   const CCI_RangeAndBearingSensor::TReadings &tMsgs = m_pcRABSens->GetReadings();
   /* Go through them to calculate the flocking interaction vector */
   if (!tMsgs.empty())
   {
      /* This will contain the final interaction vector */
      CVector2 cAccum;
      /* Used to calculate the vector length of each neighbor's contribution */
      Real fLJ;
      /* A counter for the neighbors in state flock */
      UInt32 unPeers = 0;
      for (size_t i = 0; i < tMsgs.size(); ++i)
      {
         /*
          * We consider only the neighbors in state flock
          */
         if (tMsgs[i].Data[0] == STATE_FLOCK && tMsgs[i].Data[2] == STATE_BUSY)
         {
            /*
             * Take the message sender range and horizontal bearing
             * With the range, calculate the Lennard-Jones interaction force
             * Form a 2D vector with the interaction force and the bearing
             * Sum such vector to the accumulator
             */
            /* Calculate LJ */
            fLJ = m_sFlockingParams.GeneralizedLennardJones(tMsgs[i].Range);
            /* Sum to accumulator */
            cAccum += CVector2(fLJ,
                               tMsgs[i].HorizontalBearing);
            /* Count one more flocking neighbor */
            ++unPeers;
         }
      }
      if (unPeers > 0)
      {
         /* Divide the accumulator by the number of flocking neighbors */
         cAccum /= unPeers;
      }
      /* All done */
      return cAccum;
   }
   else
   {
      /* No messages received, no interaction */
      return CVector2();
   }
}

/***************************/
/**************************/

void CEyeBotBeing::InfectedBehavior()
{
   if (SearchForMedicSignal() == true)
   {
      LOG << "I am cured! Thank you!" << std::endl;
      m_HState = STATE_HEALTHY;
      // m_pcRABAct->SetData(1, STATE_HEALTHY); == > (TODO): if published STATE_HEALTHY agents can infect you!
   }

   if (SeachForCure() == true)
   {
      LOG << "I am being cured!" << std::endl;
      m_HState = STATE_INFECTED;
      m_pcRABAct->SetData(1, STATE_INFECTED);
   }



   if (InfectionTime < m_sApocalypseParams.InfectionStart && m_HState == STATE_INFECTED)
   {
      LOGERR << "I think I am Healthy but I am not!" << std::endl;
      m_HState = STATE_INFECTED;
      m_pcRABAct->SetData(1, STATE_HEALTHY);
      CVector2 forces = m_sApocalypseParams.alpha_healthy*HealthyFlockingVector() + m_sApocalypseParams.beta_healthy*InfectedFlockingVector() + m_sApocalypseParams.gamma1_healthy*MedicFreeFlockingVector() + m_sApocalypseParams.gamma2_healthy*MedicBusyFlockingVector();
      Flock(forces);
      InfectionTime += 1;
   }

   if (InfectionTime >= m_sApocalypseParams.InfectionStart && InfectionTime < m_sApocalypseParams.InfectionTerminal && m_HState == STATE_INFECTED)
   {
      LOGERR << "I am Infected!" << std::endl;
      m_HState = STATE_INFECTED;
      m_pcRABAct->SetData(1, STATE_INFECTED);
      CVector2 forces = m_sApocalypseParams.alpha_infected*HealthyFlockingVector() + m_sApocalypseParams.beta_infected*InfectedFlockingVector() + m_sApocalypseParams.gamma1_infected*MedicFreeFlockingVector() + m_sApocalypseParams.gamma2_infected*MedicBusyFlockingVector();
      Flock(forces);
      InfectionTime += 1;

   }

   if (InfectionTime >= m_sApocalypseParams.InfectionTerminal)
   {
      m_HState = STATE_DEAD;
      Die();
   }
}

/****************************************/
/****************************************/

bool CEyeBotBeing::SeachForCure(){

   /* Get RAB messages from nearby eye-bots */
   const CCI_RangeAndBearingSensor::TReadings &tMsgs = m_pcRABSens->GetReadings();
   /* Go through them to calculate the flocking interaction vector */
   if (!tMsgs.empty())
   {

      for (size_t i = 0; i < tMsgs.size(); ++i)
      {
         /*
          * We consider only the neighbors in state flock
          */
         if (tMsgs[i].Data[3] == STATE_CURING && tMsgs[i].Range < m_sApocalypseParams.CuringDistance)
         {
            return true;
         }
      }
   }
   return false;

}

bool CEyeBotBeing::SearchForMedicSignal()
{
   /* Get RAB messages from nearby eye-bots */
   const CCI_RangeAndBearingSensor::TReadings &tMsgs = m_pcRABSens->GetReadings();
   /* Go through them to calculate the flocking interaction vector */
   if (!tMsgs.empty())
   {

      for (size_t i = 0; i < tMsgs.size(); ++i)
      {
         /*
          * We consider only the neighbors in state flock
          */
         if (tMsgs[i].Data[3] == STATE_CURED)
         {
            return true;
         }
      }
   }
   return false;
}

/****************************************/
/****************************************/

void CEyeBotBeing::HealthyBehavior()
{

   LOGERR << "I am Healthy!" << std::endl;
   m_HState = STATE_HEALTHY;
   m_pcRABAct->SetData(1, STATE_HEALTHY);

   if (SearchForInfected())
   {
      LOGERR << "Opps, got infected!" << std::endl;
      m_HState = STATE_INFECTED;
      InfectionTime = 0;
      InfectedBehavior();
   }
   else
   {
      CVector2 forces = m_sApocalypseParams.alpha_healthy*HealthyFlockingVector() + m_sApocalypseParams.beta_healthy*InfectedFlockingVector() + m_sApocalypseParams.gamma1_healthy*MedicFreeFlockingVector() + m_sApocalypseParams.gamma2_healthy*MedicBusyFlockingVector();
      Flock(forces);
   }
}

/****************************************/
/****************************************/

bool CEyeBotBeing::SearchForInfected()
{

   /* Get RAB messages from nearby eye-bots */
   const CCI_RangeAndBearingSensor::TReadings &tMsgs = m_pcRABSens->GetReadings();
   /* Go through them to calculate the flocking interaction vector */
   if (!tMsgs.empty())
   {

      for (size_t i = 0; i < tMsgs.size(); ++i)
      {
         /*
          * We consider only the neighbors in state flock
          */
         if (tMsgs[i].Data[1] == STATE_INFECTED)
         {
            if (tMsgs[i].Range < m_sApocalypseParams.InfectionDistance)
            {
               return true;
            }
         }
      }
   }
   return false;
}

/****************************************/
/****************************************/

void CEyeBotBeing::Die()
{
   LOGERR << "I am Dead!" << std::endl;

   m_pcPosAct->SetRelativePosition(
       CVector3(0.0f,
                0.0f,
                -10.0f));
   this->Destroy();
}

CVector2 CEyeBotBeing::myNormalize(CVector2 x){
   if (x.Length() == 0){
      return CVector2(0,0);
   }
   else {
      return x/x.Length();
   }
}

/*
 * This statement notifies ARGoS of the existence of the controller.
 * It binds the class passed as first argument to the string passed as second argument.
 * The string is then usable in the XML configuration file to refer to this controller.
 * When ARGoS reads that string in the XML file, it knows which controller class to instantiate.
 * See also the XML configuration files for an example of how this is used.
 */
REGISTER_CONTROLLER(CEyeBotBeing, "eyebot_being_controller")