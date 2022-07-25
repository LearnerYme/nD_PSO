#define MAXDIM 100
#define MAXNPART 1000

#include <iostream>
#include "TRandom3.h"
#include "TMath.h"

using std::cout;

class args{
    public:
        Float_t dt;
        Float_t c1, c2;
        Float_t omega;
        Float_t vmin, vmax, xmin, xmax;
        Int_t dim;

        args(Float_t dt, 
        Float_t c1, Float_t c2, 
        Float_t omega, 
        Float_t vmin, Float_t vmax,
        Float_t xmin, Float_t xmax,
        Int_t dim){
            this->dt = dt;
            this->c1 = c1;
            this->c2 = c2;
            this->omega = omega;
            this->vmin = vmin;
            this->vmax = vmax;
            this->xmin = xmin;
            this->xmax = xmax;
            this->dim = dim;
            return;
        }
};

Float_t fitness_func(Float_t* x, Int_t n){
    Float_t res = 0;
    for (Int_t i=0; i<n; i++){
        Float_t tmp = *(x+i);
        res += tmp * TMath::Sin(tmp) * TMath::Cos(2*tmp) - 2 * tmp * TMath::Sin(3*tmp);
    }
    return res;
}

class particle{
    private:
        Float_t x[MAXDIM];
        Float_t v[MAXDIM];
        Float_t fitness;
        Float_t best_x[MAXDIM];
        Float_t best_fitness;
        args* conf;
        TRandom3* rd;
    
    public:
        particle(args* conf){
            rd = new TRandom3();
            rd->SetSeed(0);
            this->conf = conf;
            for (Int_t i=0; i<conf->dim; i++){
                x[i] = rd->Uniform(conf->xmin, conf->xmax);
                v[i] = rd->Uniform(conf->vmin, conf->vmax);
                best_x[i] = 0;
            }
            fitness = 0;
            best_fitness = -INFINITY;
            return;
        }

        ~particle(){
            delete rd;
        }
        
        void update(Float_t* best_x_g){
            // loop over all the dimentions
            for (Int_t i=0; i<conf->dim; i++){
                // update velocity first
                Float_t r1 = rd->Uniform();
                Float_t r2 = rd->Uniform();
                v[i] = conf->omega * v[i] + 
                        conf->c1 * r1 * (best_x[i] - x[i]) + 
                        conf->c2 * r2 * (best_x_g[i] - x[i]);
                v[i] = v[i] < conf->vmin ? conf->vmin : v[i];
                v[i] = v[i] > conf->vmax ? conf->vmax : v[i];
                // update location
                x[i] = x[i] + conf->dt*v[i];
                x[i] = x[i] < conf->xmin ? conf->xmin : x[i];
                x[i] = x[i] > conf->xmax ? conf->xmax : x[i];
            }
            // calculate and update best fitness
            fitness = fitness_func(x, conf->dim);
            if (fitness > best_fitness){
                for (Int_t i=0; i<conf->dim; i++){
                    best_x[i] = x[i];
                }
                best_fitness = fitness;
            }
            return;
        }

        // getter and setter
        Float_t get_fitness(){
            return fitness;
        }
        Float_t get_best_fitness(){
            return best_fitness;
        }
        Float_t* get_location(){
            return x;
        }
        
};

class group{
    // private:
    public:
        args* conf;
        Int_t npart;
        particle* particles[MAXNPART];
        Int_t iter, iter_max; // iteration
        Float_t best_x[MAXDIM];
        Float_t best_fitness;

    // public:
        group(args* conf, Int_t npart, Int_t iter_max){
            this->conf = conf;
            this->npart = npart;
            iter = 0;
            this->iter_max = iter_max;
            for (Int_t i=0; i<npart; i++){
                particles[i] = new particle(conf);
            }
            for (Int_t i=0; i<conf->dim; i++){
                best_x[i] = 0;
            }
            best_fitness = -INFINITY;
            return;
        }

        ~group(){
            for (Int_t i=0; i<npart; i++){
                delete particles[i];
            }
            return;
        }

        void iteration(){
            if (iter > iter_max){
                return;
            }
            iter += 1;
            for (Int_t i=0; i<npart; i++){
                particles[i]->update(best_x);
            }
            for (Int_t i=0; i<npart; i++){
                if (particles[i]->get_fitness() > best_fitness){
                    best_fitness = particles[i]->get_fitness();
                    Float_t* tmp = particles[i]->get_location();
                    for (Int_t j=0; j<conf->dim; j++){
                        best_x[j] = tmp[j];
                    }
                }
            }
            return;
        }

        void info(){
            info(false);
            return;
        }

        void info(bool detail){
            cout << "===\n";
            cout << "Iteration " << iter << " of " << iter_max << "\n";
            cout << "Best fitness " << best_fitness << ", best location: (";
            for (Int_t i=0; i<conf->dim; i++){
                cout << best_x[i];
                if (i != conf->dim-1){
                    cout << ", ";
                }else{
                    cout << ").\n";
                }
            }
            if (detail){
                for (Int_t i=0; i<npart; i++){
                    cout << "> Particle " << i << ", fitness: " << particles[i]->get_fitness() << " best fitness: " << particles[i]->get_best_fitness() << ", best location: (";
                    for (Int_t j=0; j<conf->dim; j++){
                        cout << (particles[i]->get_location())[j];
                        if (j != conf->dim-1){
                            cout << ", ";
                        }else{
                            cout << ").\n";
                        }
                    }
                }
            }
            return;
        }
};
