#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>

#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct planet_s
{    
    float x, y;
    
    int size;
    int galaxyId;
    bool initial;
    bool neutral;
};

void usage(const char *name)
{
    printf("usage: \t\t%s file galaxy X Y players planets radius rings autochtone\n", name);
    printf("file: \t\toutput file name\n");
    printf("galaxy: \tid of the galaxy [1-N]\n");
    printf("X, Y: \t\tposition of the galaxy [0-N], [0-N]\n");
    printf("players: \tnumber of players, [1-N]\n");
    printf("planets: \tnumber of planets, [1-N]\n");
    printf("radius: \tradius of the galaxy / 2, [1-N]\n");
    printf("rings: \tnumber of planet rings in the galaxy, [1-N]\n");
    printf("size: \tplanet size, [1-N]\n");
    printf("autochtone: \tnumber of autochtone, [1-N]\n");
    printf("\nExample: %s test.map 1 0 0 4 16 100 2\n", name);
    printf("will result in a galaxy width a radius of 100 and 16 planets for 4 players.\n");
    printf("If rings = 1, there will be one circle of planets.\n");
    printf("If rings = N, there will be N of planets.\n");
}

int main(int argc, char **argv)
{
    string filename;
    int galaxyId = -1, X = -1, Y = -1, nPlayers = -1, nPlanets = -1, radius = -1, nRings = -1, psize = -1, autochtone = -1;
    
    if(argc < 11)
    {
        usage(argv[0]);
        return 0;
    }
    
    filename = argv[1];
    galaxyId = atoi(argv[2]);
    X = atoi(argv[3]);
    Y = atoi(argv[4]);
    nPlayers = atoi(argv[5]);
    nPlanets = atoi(argv[6]);
    radius = atoi(argv[7]);
    nRings = atoi(argv[8]);
    psize  = atoi(argv[9]);
    autochtone = atoi(argv[10]);
    
    if(nPlanets < nPlayers)
    {
		printf("Error: planets < players\n");
		return 0;
	}
	
	if(nRings > nPlanets)
	{
		printf("Error: rings > planets\n");
		return 0;
	}
	
    
    if(filename.empty() || galaxyId < 1 || radius < 1 || X < 0 || Y < 0 || nPlayers < 1 || nPlanets < 1)
    {
        usage(argv[0]);
        return 0;
    }
    
    printf("Generating %s with id=%d, X=%d, Y=%d, players=%d, planets=%d, radius=%d, rings=%d\n", 
        filename.c_str(), galaxyId, X, Y, nPlayers, nPlanets, radius, nRings);
    
    srand(time(0));
    
    int nPlanetPerPlayer = -1;
    int nPlanetPerLayer = -1;
    int nPlanetRest = -1;
    
    nPlanetPerPlayer = max(nPlanets / nPlayers, 1);
    nPlanetPerLayer = max(nPlanetPerPlayer / nRings, 1);
    nPlanetRest = (nPlanetPerPlayer == 1) ? 0 : nPlanetPerPlayer % nRings;
	
	if(nPlanetRest)
		printf("Warning: nPlanet / nPlayers / nRings is not an integer ... May result in a weird map.\n");
	
    vector<planet_s> planets(nPlayers * nPlanetPerPlayer);

    // Pour chaque anneau de planètes,
    // on répartie les planètes équitablement
    float angle = (360 / nPlayers);

    // On place toutes les planètes à la même distance du centre (range)
    int id = 0;
    int range = rand() % radius + 1;
    for(int i = 0; i < nPlayers; ++i)
    {
        for(int j = 0; j <= nRings; ++j)
        {
            float padding = j%2  * (360 / nPlanetPerLayer);

            int nPlanetInLayer = -1;
            if(j < nRings) nPlanetInLayer = nPlanetPerLayer;
            else if(j == nRings) nPlanetInLayer = nPlanetRest;

            for(int k = 0; k < nPlanetInLayer; ++k)
            {
                struct planet_s p;
                
                // Calcul des coordonnées polaires
                float planetAngle = padding + (angle * i) + (angle / nPlanetPerLayer) * k;
                int planetRadius = range;

                // Calcul des coordonnées cartésiennes
                p.x = X + cos(planetAngle) * radius * (j + 1);
                p.y = Y + sin(planetAngle) * radius * (j + 1);
                
                p.galaxyId = galaxyId;
                p.size = psize;
                p.initial = false;
                p.neutral = false;

                planets[id++] = p;
            }
        }
        
        // Une planète initiale par joueur ...
        planets[id-1].initial = true;
    }
    
    ofstream out(filename.c_str(), ofstream::out);
    
    for(unsigned int i = 0; i < planets.size(); ++i)
    {
        planet_s &p = planets[i];
        
        // write map
        out << (int)p.x << " " << (int)p.y << " " << p.size << " " << p.galaxyId << " " << p.initial << " " << p.neutral << " " << autochtone << "\n";
    }
    
    out.close();
    
    return 0;
}
