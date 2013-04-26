#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>

#include <fstream>
#include <vector>

using namespace std;

struct planet_s
{
    int id;
    int playerId;
    
    float angle;
    int range;
    
    float x, y;
};

void usage(const char *name)
{
    printf("usage: \t\t%s players galaxies layers planets scale\n", name);
    printf("players: \tnumber of players, [1-N]\n");
    printf("galaxies: \tnumber of galaxies [1-N]\n");
    printf("layers: \tnumber of layers per galaxy [1-N]\n");
    printf("planets: \tnumber of planets per layer per player, [1-N]\n");
    printf("scale: \tscale of the generated galaxy, [1-N]\n");
    printf("\nExample: %s 4 4 4 4 1\n", name);
    printf("will result in 64 planets per galaxy with 4 planets per layer per player.\n");
}

int main(int argc, char **argv)
{
    int nPlayers = -1, nGalaxies = -1, nLayers = -1, nPlanets = -1, scale = -1;
    
    if(argc < 6)
    {
        usage(argv[0]);
        return 0;
    }
    
    nPlayers = atoi(argv[1]);
    nGalaxies = atoi(argv[2]);
    nLayers = atoi(argv[3]);
    nPlanets = atoi(argv[4]);
    scale = atoi(argv[5]);
    
    if(nPlayers < 1 || nGalaxies < 1 || nLayers < 1 || nPlanets < 1 || scale < 1)
    {
        usage(argv[0]);
        return 0;
    }
    
    printf("Generating with: players=%d, galaxies=%d, layers=%d, planets=%d, scale=%d\n", 
        nPlayers, nGalaxies, nLayers, nPlanets, scale);
    
    srand(time(0));
    
    vector<planet_s> planets(nGalaxies * nLayers * nPlayers * nPlanets);
    
    // Pour chaque anneau de planètes,
    // on répartie les planètes équitablement
    float angle = (360 / nPlayers);
    for(int i = 0; i < nLayers; ++i)
    {
        // On place toutes les planètes à la même distance du centre (range)
        int range = rand() % scale + 1;
        for(int j = 0; j < nPlayers; ++j)
        {
            for(int k = 0; k < nPlanets; ++k)
            {
                struct planet_s p;
                p.id = i * nPlayers * nPlanets + j * nPlanets + k;
                p.playerId = j;
                
                // Calcule des coordonnées polaires
                p.angle = (angle * j) + (angle / nPlanets) * k;
                p.range = range;
                
                // Calcul des coordonnées cartésiennes
                p.x = cos(p.angle) * range;
                p.y = sin(p.angle) * range;
                
                planets[p.id] = p;
            }
        }
    }
    
    ofstream out("test.map", ofstream::out);
    
    for(unsigned int i = 0; i < planets.size(); ++i)
    {
        planet_s &p = planets[i];
        
        printf("id=%d, playerId=%d, angle=%f, range=%d, x=%f, y=%f\n", 
            p.id, p.playerId, p.angle, p.range, p.x, p.y);
        
        // write map
        out << p.id << " " << p.x << " " << p.y << "\n";
    }
    out << "\n";
    
    out.close();
    
    return 0;
}
