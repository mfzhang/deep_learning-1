#include "Config.h"
#include "Utility.h"
#include <cfloat>
#include <cstring>

static double _I[maxUnit * maxBatchSize];

const double expThreshold = 50.0;

double* I(){
    static bool hasVisit = false;
    if(!hasVisit){
        for(int i = 0; i < maxUnit*maxBatchSize; i++)
            _I[i] = 1.0;
        hasVisit = true;
    }
    return _I;
}

void initializeWeightSigmoid(double *weight, int numIn, int numOut){
    double low, high; 

    low = -4 * sqrt((double)6 / (numIn + numOut));
    high = 4 * sqrt((double)6 / (numIn + numOut));

    for(int i = 0; i < numIn*numOut; i++){
        weight[i] = random_double(low, high);
    }
}

void initializeWeightTanh(double *weight, int numIn, int numOut){
    double low, high; 

    low = -sqrt((double)6 / (numIn + numOut));
    high = sqrt((double)6 / (numIn + numOut));

    for(int i = 0; i < numIn*numOut; i++){
        weight[i] = random_double(low, high);
    }
}

void softmax(double *arr, int size){
    double sum = 0.0;
    double maxval = -DBL_MAX;

    for(int i = 0; i < size; i++){
        if(maxval < arr[i])
            maxval = arr[i];
    }

    for(int i = 0; i < size; i++){
        arr[i] = exp(arr[i]-maxval);
        sum += arr[i];
    }

    for(int i = 0; i < size; i++)
        arr[i] /= sum;
}

int maxElem(double *arr, int size){
    int maxe = 0;
    for(int i = 1; i < size; i++){
        if(arr[i] > arr[maxe])
            maxe = i;
    }
    return maxe;
}

/*
 * exp function with check to avoid `inf` and `nan`
 */
double expc(double x){
    if(x > expThreshold){
        return exp(expThreshold);
    }else if(x < -expThreshold){
        return exp(-expThreshold);
    }
    return exp(x);
}

/*
 * sigmoid function with check to avoid `inf` and `nan`
 */
double sigmoidc(double x){
    if(x > expThreshold){
        return 1;
    }else if(x < -expThreshold){
        return 0;
    }
    return 1.0 / (1.0 + exp(-x));
}

/*
 * softplus function with check to avoid `inf` and `nan`
 */
double softplusc(double x){
    if(x > expThreshold){
        return softplus(expThreshold);
    }else if(x < -expThreshold){
        return softplus(-expThreshold);
    }
    return softplus(x);
}

void multiNormial(double *px, double *x, int size){
    double p = random_double(0, 1);
    double sum = 0.0;
    int idx = 0;
    for(int i = 0; i < size; i++){
        sum += px[i];
        if(sum >= p){
            idx = i;
            break;
        }
    }
    memset(x, 0, sizeof(double)*size);
    x[idx] = 1.0;
}

void binomial(double *px, double *x, int size){
    for(int i = 0; i < size; i++){
        double p = random_double(0, 1);
        if(p < px[i]){
            x[i] = 1.0;
        }else{
            x[i] = 0.0;
        }
    }
}

double squareNorm(double *arr, int n, int size){
    double res = 0;
    for(int i = 0; i < size; i++){
        double curNorm = 0;
        for(int j = 0; j < n; j++){
            curNorm += arr[i*n+j] * arr[i*n+j];
        }
        res += sqrt(curNorm);
    }
    return res / size;
}

double normalize(double *arr, int n, int size){
    for(int i = 0; i < size; i++){
        double mean = 0;
        for(int j = 0; j < n; j++){
            mean += arr[i*n+j];
        }
        mean /= n;
        for(int j = 0; j < n; j++){
            arr[i*n+j] -= mean;
        }
        double square = 0;
        for(int j = 0; j < n; j++){
            square += arr[i*n+j] * arr[i*n+j];
        }
        square /= n - 1;
        square = sqrt(square);
        for(int j = 0; j < n; j++){
            arr[i*n+j] /= square;
        }
    }
}

double corrupt(const double* x, double* nx, int n, double level){
    for(int i = 0; i < n; i++){
        if(random_double(0, 1) < level){
            nx[i] = 0;
        }else{
            nx[i] = x[i];
        }
    }
}

void transMatrix(double* ori, double* trans, int nrow, int ncol){
    for(int i = 0; i < nrow; i++){
        for(int j = 0; j < ncol; j++){
            trans[j*nrow+i] = ori[i*ncol+j];
        }
    }
}
