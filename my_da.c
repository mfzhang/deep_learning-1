#include"dataset.h"
#include<strings.h>
#include<time.h>

#define MAX_SIZE 1000
#define eta 0.1

typedef struct da{
    int n_in, n_out;
    double **W;
    double *b;
    double *c;
} da;

double h_out[MAX_SIZE], z_out[MAX_SIZE]; 
double d_low[MAX_SIZE], d_high[MAX_SIZE];
double delta_W[MAX_SIZE][MAX_SIZE], delta_b[MAX_SIZE], delta_c[MAX_SIZE];

void init_da(da *m, int n_in, int n_out){
    int i, j; 
    double r;

    r = 4 * sqrt(6.0 / (n_in + n_out));

    m->n_in = n_in;
    m->n_out = n_out;
    m->W = (double**)malloc(m->n_out * sizeof(double*));
    for(i = 0; i < m->n_out; i++){
        m->W[i] = (double*)malloc(m->n_in * sizeof(double));
    }
    for(i = 0; i < m->n_out; i++){
        for(j = 0; j < m->n_in; j++){
            m->W[i][j] = random_double(-r, r);
        }
    }
    m->b = (double*)calloc(m->n_out, sizeof(double));
    m->c = (double*)calloc(m->n_in, sizeof(double));
}

void free_da(da *m){
    int i;

    for(i = 0; i < m->n_out; i++){
        free(m->W[i]);
    }
    free(m->W);
    free(m->b);
    free(m->c);
}

void get_hidden_values(const da *m, const double *x, double *y){
    int i, j; 
    double s, a;

    for(i = 0; i < m->n_out; i++){
        for(j = 0, a = 0.0; j < m->n_in; j++){
            a += m->W[i][j] * x[j]; 
        }
        a += m->b[i];
        y[i] = sigmoid(a);
    }
}

void get_reconstruct_input(const da *m, const double *x, double *y){
    int i, j;
    double a;

    for(i = 0; i < m->n_in; i++){
        for(j = 0, a = 0.0; j < m->n_out; j++){
            a += m->W[j][i] * x[j]; 
        }
        a += m->c[i];
        y[i] = sigmoid(a);
    }
}

void get_top_delta(const da *m, const double *y, const double *x, double *d){
    int i; 

    for(i = 0; i < m->n_in; i++){
        d[i] = y[i] - x[i];
    }
}

void get_second_delta(const da *m, const double *y_low, const double *d_high, double *d_low){
    int i, j, k;
    double s, derivative;

    for(i = 0; i < m->n_out; i++){
        derivative = get_sigmoid_derivative(y_low[i]);
        for(j = 0, s = 0.0; j < m->n_in; j++){
            s += d_high[j] * m->W[i][j];
        }
        d_low[i] = derivative * s;
    }
}

void train_da(){
    int i, j, k, p, q;
    int mini_batch = 20;
    int epcho, n_epcho = 15;
    int train_set_size = 50000, validate_set_size = 10000;

    dataset train_set, validate_set;
    da m;
    double cost;
    time_t start_time, end_time;

    srand(1234);

    load_mnist_dataset(&train_set, &validate_set);

    init_da(&m, 28*28, 500);

    for(epcho = 0, cost = 0.0; epcho < n_epcho; epcho++){
        
        start_time = time(NULL);
        for(k = 0; k < train_set.N / mini_batch; k++){

            //if((k+1) % 500 == 0){
            //    printf("epcho %d batch %d\n", epcho + 1, k + 1);
            //}

            bzero(delta_W, sizeof(delta_W));
            bzero(delta_b, sizeof(delta_b));
            bzero(delta_c, sizeof(delta_c));

            for(i = 0; i < mini_batch; i++){

                /* feed-forward */
                get_hidden_values(&m, train_set.input[k*mini_batch+i], h_out);
                get_reconstruct_input(&m, h_out, z_out);

                /* back-propagation*/
                get_top_delta(&m, z_out, train_set.input[k*mini_batch+i], d_high);
                get_second_delta(&m, h_out, d_high, d_low);

                for(j = 0; j < m.n_out; j++){
                    for(p = 0; p < m.n_in; p++){
                        delta_W[j][p] = d_low[j] * train_set.input[k*mini_batch+i][p] + d_high[p] * h_out[j];
                    }
                    delta_b[j] = d_low[j];
                }
                for(j = 0; j < m.n_in; j++){
                    delta_c[j] = d_high[j];
                }

                for(j = 0; j < m.n_in; j++){
                    cost -= train_set.input[k*mini_batch+i][j] * log(z_out[j]) + (1.0 - train_set.input[k*mini_batch+i][j]) * log(1.0 - z_out[j]);
                }
            }

            /* modify parameter */
            for(j = 0; j < m.n_out; j++){
                for(p = 0; p < m.n_in; p++){
                    m.W[j][p] -= eta * delta_W[j][p] / mini_batch;
                }
                m.b[j] -= eta * delta_b[j] / mini_batch;
            }
            for(j = 0; j < m.n_in; j++){
                m.c[j] -= eta * delta_c[j] / mini_batch;
            }
            
        }

        end_time = time(NULL);
        printf("epcho %d cost: %.5lf\ttime: %ds\n", epcho + 1, cost / train_set.N, (int)(end_time - start_time));
    }
    free_da(&m);
}

int main(){
    train_da();
    return 0;
}
