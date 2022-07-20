#include "utils.cxx"

int main(){
    args* conf = new args(1, 2, 2, 0.8, -1, 1, 0.2, 20, 1);
    group g(conf, 50, 100);
    g.info();
    for (Int_t i=0; i<100; i++){
        g.iteration();
        if (i%10 == 0){
            g.info();
        }
    }
    g.info(true);
    return 0;
}