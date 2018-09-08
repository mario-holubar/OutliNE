void randomize_critic();
void criticize(network*,float);
void local_learning(network*,float);
void record_action_record(float*,float*,int,float);
void reset_action_record();
float decay_eta(int);

/*here's the interface to ActPtr*/
#define NextActPtr()       ((ActPtr<=0) ? LOCAL_STEPS-1:ActPtr-1)
