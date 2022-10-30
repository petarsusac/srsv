
struct _input_t;
typedef struct _input_t input_t;

typedef struct {
    int state;
    unsigned int timestamp;
} input_state_t;

typedef struct {
    int response;
    unsigned int timestamp;
} input_response_t;

input_t *input_new(unsigned int id, unsigned int period, unsigned int first_occurence);
void input_set_state(input_t *this, int state);
input_state_t input_get_state(input_t *this);
void input_set_response(input_t *this, int response);
input_response_t input_get_response(input_t *this);
unsigned long input_get_first_occurence(input_t *this);
unsigned long input_get_period(input_t *this);
unsigned int input_get_id(input_t *this);
