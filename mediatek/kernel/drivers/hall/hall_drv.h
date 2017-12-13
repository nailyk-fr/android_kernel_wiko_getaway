
/****************************************************
globle hall variables
****************************************************/
enum hall_status
{
    HALL_SWITCH_ON = 0,
    HALL_SWITCH_OFF = 1,
};

char *hall_status_string[2]=
{
    "hall_switch_on",
    "hall_switch_off",
};


/****************************************************
extern ACCDET API
****************************************************/
void accdet_auxadc_switch(int enable);



