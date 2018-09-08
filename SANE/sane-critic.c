
/*** sane-critic.c 
     David Moriarty
     April 1997

critic.c contains functions for running local learning routines.
There is a temporal difference critic function that learns to provide
training signals to the SANE networks and a local learning function
that reinforces and punishes actions based on performance differences
of the system.  The learned weights can be written back to the genes
if LAMARCKIAN equals 1, otherwise SANE will perform a Baldwinian
evolution.

Note: I have yet to have any success with the TD Critic network.  I
have, however, had good success with the local learning function based
on performance differences of the actual system (not the critic).

***/

#include "sane.h"
#include "sane-critic.h"
#include "sane-util.h"

#define C_LEARN 0.1  /*learning rate for the critic*/

/*here is the Critic datastructure*/
struct{
  double ih[NUM_INPUTS][NUM_HIDDEN];
  double io[NUM_INPUTS];
  double ho[NUM_HIDDEN]; /*hid to output weights*/
  double input[NUM_INPUTS];
  double input_old[NUM_INPUTS];
  double hidden[NUM_HIDDEN];          /*output of hidden*/
  double hidden_old[NUM_HIDDEN];          /*output of hidden*/
  double sigout;
  double sigout_old;
  double value;
  double value_old;
} Critic;


/* here's some definitions for the local_learning (not AHC) element

   The action record is maintains a record of system performance
   levels.  This is used to judge whether actions in a certain time
   period have caused a significant increase or signficant decrease.
   If there is a significant increase, then the actions in the time
   interval are reinforced.  If there is a significant decrease, the
   actions are punished.
*/

struct {
  float input[NUM_INPUTS];  
  float hidden[NUM_HIDDEN];
  int guilty;
  float value;
} ActionRecord[LOCAL_STEPS];

int ActPtr = LOCAL_STEPS-1;  /*A pointer to the next element in the record*/

double old_r = -999999.0;

#define opposite(a)     ((a == 0.0) ? 1.0 : 0.0)

/** randomize_critic will randomize all of the weights in the critic
network.  **/

void randomize_critic(void)
{
  int i,j;
  for(i=0;i<NUM_INPUTS;++i) {
    for(j=0;j<NUM_HIDDEN;++j)
      Critic.ih[i][j] = randfloat() * 0.2 - 0.1;
    Critic.io[i] = randfloat() * 0.2 - 0.1;
    Critic.input[i] = 0.0;
  }
  for(i=0;i<NUM_HIDDEN;++i) {
    Critic.ho[i] = randfloat() * 0.2 - 0.1;
    Critic.hidden[i] = 0.0;
  }
  Critic.value = -999999.0;
}

/** criticize is the AHC learning function.  It takes as input the
    network to train, the input to the critic, and a reward
    signal. The reward signal should be between 0 and 1.**/

void criticize(network *net, float r)
{
  int i,j,guilty;
  double sum,r_hat,e1;
  double h_err,err[NUM_OUTPUTS],delta;
  neuron *h;

  /*save old critic activations*/

  Critic.value_old = Critic.value;
  for(i=0;i<NUM_HIDDEN;++i)
    Critic.hidden_old[i] = Critic.hidden[i];
  for(i=0;i<NUM_INPUTS;++i)
    Critic.input_old[i] = Critic.input[i];

  /*fire up critic network*/

  for(i=0;i<NUM_INPUTS;++i)
    Critic.input[i] = net->input[i];
  for(i=0;i<NUM_HIDDEN;++i) {
      sum = 0.0;
      for (j=0;j<NUM_INPUTS;++j) 
        sum += Critic.ih[j][i] * Critic.input[j];
      Critic.hidden[i] = 1.0 / (1.0 + exp(-sum));
  }
  sum = 0.0;
  for(j=0;j<NUM_INPUTS;++j) 
    sum += Critic.io[j] * Critic.input[j];
  for(j=0;j<NUM_HIDDEN;++j) 
    sum += Critic.ho[j] * Critic.hidden[j];
  Critic.value = 1.0/(1.0 + exp(-sum)); /*sum;*/

  /*first time through, don't do any modification*/

  if (Critic.value_old == -999999.0)
    return;
  
  /* modification */

  if (r)
      r_hat = r - Critic.value;
  else
      r_hat = Critic.value - Critic.value_old;
 
  /*modify critic first*/

  for(i=0;i<NUM_HIDDEN;++i) {      
    e1 = C_LEARN * r_hat * Critic.hidden_old[i] 
                * (1.0 - Critic.hidden_old[i]) * Critic.ho[i];
    for(j=0;j<NUM_INPUTS;++j)  
      Critic.ih[j][i] += e1 * sgn(Critic.input_old[j]);
    Critic.ho[i] += C_LEARN * r_hat * Critic.hidden_old[i];
  }
  for(i=0;i<NUM_INPUTS;++i) 
    Critic.io[i] += C_LEARN * r_hat * sgn(Critic.input_old[i]);


  /*now modify the network*/
  guilty = net->winner;

  for(i=0;i<NUM_OUTPUTS;++i) 
    if (i == guilty) 
      err[i] = -r_hat 
            * net->sigout[guilty] * (1.0 - net->sigout[guilty]); 
    else 
      err[i] = r_hat 
            * net->sigout[guilty] * (1.0 - net->sigout[guilty]);

  for(i=0;i<NUM_HIDDEN;++i) {
    h = net->hidden[i];
    h_err = 0.0;
    for(j=0;j<h->numout;++j) {
        h_err += err[j] * max(min(h->out_weight[j],1.0),-1.0);
        delta = ETA * err[j] * h->sigout + ALPHA * h->out_delta[j];        
        h->out_weight[j] += delta;
        h->out_delta[j] = delta;
    }
  
    if (h_err != 0.0) {
      h_err = h_err * h->sigout * (1.0 - h->sigout);
      for(j=0;j<net->hidden[i]->numin;++j) {
        delta = ETA * h_err * sgn(net->input[h->in_conn[j]])
                + ALPHA * h->in_delta[j];
        h->in_weight[j] += delta;
        h->in_delta[j] = delta;
      }
    }     
  }

}

/** record_action_record takes in a the current input and hidden
    layers for the decision network, which action was taken, and the
    performance level at that point.  It places these into the action
    record for later processing.
    **/

void record_action_record(float *input,float *hidden,int guilty, float v)
{
  int i;
  
  for(i=0;i<NUM_INPUTS;++i)
    ActionRecord[ActPtr].input[i] = input[i];
  for(i=0;i<NUM_HIDDEN;++i)
    ActionRecord[ActPtr].hidden[i] = hidden[i];
  ActionRecord[ActPtr].guilty = guilty;
  ActionRecord[ActPtr].value = v;
  if (ActPtr == 0)
    ActPtr = LOCAL_STEPS - 1;
  else
    --ActPtr;
}

/** reset_action_record does just that. 
    **/

void reset_action_record(void)
{
  int i;
  ActPtr = LOCAL_STEPS - 1;
  for(i=0;i<LOCAL_STEPS;++i)
    ActionRecord[i].value = -999999.0;
}

/** local_learning is the main learning function for the performance
    difference learning module (not the AHC).  local learning compares
    the performance level at the current time step with the
    performance LOCAL_STEPS in the past.  If there is a signficant
    increase it reinforces the activation performed LOCAL_STEPS in the
    past.  If there is a significant decrease it punishes the
    activation performed LOCAL_STEPS in the past.  This has worked
    much better in practice than the temporal difference function.
    **/

void local_learning(network *net, float r)
{
  int i,j,guilty;
  double r_hat,h_err,err[NUM_OUTPUTS];
  double delta,eta;
  neuron *h;


  if (ActionRecord[ActPtr].value == -999999.0)
    return;

  r_hat = ActionRecord[ActPtr].value - r;

#if MAXIMIZE_R
  if (r_hat < LL_LOWER_LEVEL)
    r_hat = 1.0; /*reinforcement*/
  else if (r_hat > LL_UPPER_LEVEL)
    r_hat = 0.0; /*punishment*/
  else
    return;
#else
  if (r_hat < LL_LOWER_LEVEL)
    r_hat = 0.0; /*punishment*/
  else if (r_hat > LL_UPPER_LEVEL)
    r_hat = 1.0; /*reinforcement*/
  else
    return;
#endif

  if (DECAY_ETA)  /*I don't normally use this*/
    eta = decay_eta(generation);
  else
    eta = ETA;

  /*which action was performed?*/
  guilty = ActionRecord[ActPtr].guilty;

  /*get output layer errors*/
  for(i=0;i<NUM_OUTPUTS;++i) 
    if (i == guilty) 
      err[i] = (r_hat - net->sigout[guilty]) 
            * net->sigout[guilty] * (1.0 - net->sigout[guilty]); 
    else 
      err[i] = (opposite(net->sigout[guilty]) - net->sigout[i])
            * net->sigout[guilty] * (1.0 - net->sigout[guilty]);

  /*backpropogate output errors through hidden and input layer*/
  for(i=0;i<NUM_HIDDEN;++i) {
    h = net->hidden[i];
    h_err = 0.0;
    for(j=0;j<h->numout;++j) {
        h_err += err[j] * max(min(h->out_weight[j],1.0),-1.0);
        delta = eta * err[j] * h->sigout + ALPHA * h->out_delta[j];        
        h->out_weight[j] += delta;
        h->out_delta[j] = delta;
    }
  
    if (h_err != 0.0) {
      h_err = h_err * h->sigout * (1.0 - h->sigout);
      for(j=0;j<net->hidden[i]->numin;++j) {
        delta = eta * h_err * sgn(net->input[h->in_conn[j]])
                + ALPHA * h->in_delta[j];
        h->in_weight[j] += delta;
        h->in_delta[j] = delta;
      }
    }     
  }

}

/** decay_eta will decay the learning rate.  I don't normally use this.
    **/

float decay_eta(int g)
{
  if (g < 50)  return ETA;
  if (g < 100) return ETA*0.5;
  if (g < 150) return ETA*0.1;
  else         return 0.0;
}

