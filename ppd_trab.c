#include <stdio.h>
#include <pthread.h>

#define N_THREADS 4
#define MAX_PONTOS 10000
#define MAX_CENTROIDES 100

typedef struct Dados{
    int inicio;
    int fim;
    int centroides;
}Dados;

typedef struct Ponto{
    double x;
    double y;
    double z;
    int centroide_prox;
    double distmin;
}Ponto;

double calc_dist(Ponto p1, Ponto p2){
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    double dz = p1.z - p2.z;
    return dx*dx + dy*dy + dz*dz;
}

Ponto centroides[MAX_CENTROIDES];
Ponto pontos[MAX_PONTOS];
int mudanca = 1;

void* kmeans(void* arg){
    Dados* d = (Dados*)arg;
    for(int i = d->inicio ; i < d->fim; i++){
        pontos[i].distmin = 2147483656;
        int centroide_antigo = pontos[i].centroide_prox;
        for(int j = 0 ; j < d->centroides ; j++){
            if(calc_dist(pontos[i],centroides[j]) < pontos[i].distmin){
                pontos[i].distmin = calc_dist(pontos[i],centroides[j]);
                pontos[i].centroide_prox = j;
            }
        }
        if(pontos[i].centroide_prox != centroide_antigo){
            mudanca = 1;
        }
    }
    return NULL;
}

void recalcula_centroides(int n_centroides, int n_pontos){

    double soma_x[MAX_CENTROIDES] = {0};
    double soma_y[MAX_CENTROIDES] = {0};
    double soma_z[MAX_CENTROIDES] = {0};
    int quantidade[MAX_CENTROIDES] = {0};

    // Soma os pontos pertencentes a cada centroide
    for(int i = 0; i < n_pontos; i++){

        int c = pontos[i].centroide_prox;

        soma_x[c] += pontos[i].x;
        soma_y[c] += pontos[i].y;
        soma_z[c] += pontos[i].z;

        quantidade[c]++;
    }

    // Calcula a média e atualiza os centroides
    for(int i = 0; i < n_centroides; i++){

        if(quantidade[i] > 0){
            centroides[i].x = soma_x[i] / quantidade[i];
            centroides[i].y = soma_y[i] / quantidade[i];
            centroides[i].z = soma_z[i] / quantidade[i];
        }
    }
}


int main(){
    pthread_t threads[N_THREADS];
    Dados dados[N_THREADS];

    int n_centroides, n_pontos;
    scanf("%d", &n_centroides);
    scanf("%d", &n_pontos);

    for(int i = 0 ; i < n_centroides ; i++){
        scanf("%lf %lf %lf", &centroides[i].x, &centroides[i].y, &centroides[i].z);
    }
    for(int i = 0 ; i < n_pontos ; i++){
        scanf("%lf %lf %lf", &pontos[i].x, &pontos[i].y, &pontos[i].z);
        pontos[i].distmin = 2147483656;
        pontos[i].centroide_prox = -1;
    }

    int chunk = n_pontos / N_THREADS;

    for(int i = 0; i < N_THREADS; i++){
        dados[i].inicio = i * chunk;

        if(i == N_THREADS - 1)
            dados[i].fim = n_pontos;
        else
            dados[i].fim = (i + 1) * chunk;

        dados[i].centroides = n_centroides;
    }

    while(mudanca){
        mudanca = 0;
        for(int i = 0 ; i < N_THREADS ; i++){
            pthread_create(&threads[i], NULL, kmeans, &dados[i]);
        }
        for(int i = 0 ; i < N_THREADS ; i++){
            pthread_join(threads[i], NULL);
        }

        recalcula_centroides(n_centroides, n_pontos);
    }
    for(int i = 0 ; i < n_centroides; i++){
        printf("%.2lf %.2lf %.2lf\n", centroides[i].x, centroides[i].y, centroides[i].z);
    }
}