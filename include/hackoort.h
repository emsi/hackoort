
typedef struct _hackoortContext hackoortContext;

struct _hackoortContext {
    gatt_connection_t* connection; // gattlib connectio
    unsigned int seq; // oort devices are using sequence numbers for each request
    short verbose,force,dry_run;
    // configuration
    char* bt_address;  // remote device address
    char* password; // default password used when no password is set
};


// Hackoort functions 
int hackoort_check_lock_status(hackoortContext* context);
char* hackoort_read_characteristic(hackoortContext *context);

int hackoort_onoff(hackoortContext* context, unsigned char on);

int hackoort_set_luminance(hackoortContext* context, unsigned char lum);
int hackoort_set_luminance_pct(hackoortContext* context, unsigned char pct);
