#include <fstream>
#include <sstream>
#include <string>
#include <climits>
#include <cstdint>
#include "Csv_parser.hpp"

void Csv_parser::read_stations(const std::string& _filename)
{
	// lecture du fichier
	ifstream Fichier_stations(_filename);
	if(!Fichier_stations.is_open())
	{
	        cerr << "Impossible d'ouvrir le fichier" << endl;
        return ;
	}

	string Line;
	// récuperer la 1ere ligne du fichier
	getline(Fichier_stations, Line);
	// on récupere toutes les lignes et pour chaque ligne on récupére le contenu jusqu'au délimitateur ','
	while(getline(Fichier_stations, Line))
	{

	        travel::Station Stations_param;
		string Station_id;

        	stringstream ss(Line);    // conversion en flux

        	getline(ss, Stations_param.name, ',');
		getline(ss, Station_id, ',');
        	getline(ss, Stations_param.line_id, ',');
		getline(ss, Stations_param.address, ',');
        	getline(ss, Stations_param.line_name, ',');


        	uint64_t Id = stoi(Station_id);  // conversion en int de id
        	stations_hashmap[Id] = Stations_param;	     // construction de la hash_map
	}

	Fichier_stations.close();
}

void Csv_parser::read_connections(const std::string& _filename)
{

	ifstream Fichier_connections(_filename);

	if(!Fichier_connections.is_open()) {

        	cerr << "Impossible d'ouvrrir le fichier" << endl;
        return ;
	}

	string Line;
	getline(Fichier_connections, Line);

	while(getline(Fichier_connections, Line))
	{

	        stringstream ss(Line);
	        string from_stop, to_stop, min_transfer_time;
	        uint64_t From, To, Min_cost;

			getline(ss, from_stop, ',');
	        getline(ss, to_stop, ',');
	        getline(ss, min_transfer_time, ',');

	        From = stoi(from_stop);
	        To = stoi(to_stop);
	        Min_cost = stoi(min_transfer_time);

	        connections_hashmap[From][To]=Min_cost;
	}

	Fichier_connections.close();

}

vector<pair<uint64_t,uint64_t> > Csv_parser::compute_travel(uint64_t _start, uint64_t _end) {

	string exp_flag;  // flag d'exeption   

	if (_start==_end)
	{	
		exp_flag="SS";
		throw(exp_flag); // Same Station
    	}

	if ( this->stations_hashmap.find(_start)== this->stations_hashmap.end() || this->stations_hashmap.find(_end)== this->stations_hashmap.end())
	{
		exp_flag="NS";
	        throw(exp_flag); // Id de station erroné
	}
	
	if (this->stations_hashmap[_start].name==this->stations_hashmap[_end].name)
	{	
		exp_flag="SS";
		throw(exp_flag); // Same Station
    	}

	// vector du meilleur chemin
	vector<pair<uint64_t,uint64_t> > best_path;

	// map des nodes non visités (si true il ya des nodes non-visités sinon il n yen plus )
	std::map< uint64_t,bool> unvisited;

	// map qui represente les nodes associé à une paire [distance_to_next_node,previous_nodes]
	std::map< uint64_t,std::pair<uint64_t,uint64_t>> table_dijkstra ;

	// pour chaque station de stations_hashmap (le graph), on récupere l'Id de la station et on le met dans la table_dijkstra en initialisant toutes les distance a l'infini et les stations precédantes a -1 (indeféni)et ajouter la station à Unvisited

	for (auto v : this->stations_hashmap )
	{
	        uint64_t ID= v.first;

	        table_dijkstra[ID]= std::make_pair(INT_MAX,-1);
	        unvisited[ID]=true;
	}

	// initialiser la station start (dist[_start]=0 et prev[_start]=-1)
	table_dijkstra[_start]=std::make_pair(0,-1);

	// tant que la map Unvisited n'est pas vide, on calcule la distance minimale entre la station actuelle et les autres stations et on retire la station visitée de Unvisited
	while (isNotEmpty(unvisited))
	{
	        uint64_t u = FindMin(table_dijkstra,unvisited);

	        unvisited[u]=false;
		// si la station visitée est la station _end, on arrete
	        if(u == _end)
	        break;

	        for (auto v : this->connections_hashmap[u]  )  // pour chaque successeur de u (station déja visitée), on calcule la nouvelle distance entre u et les successuers de ses successeurs
	        {
		        if (unvisited[v.first])
		        {
                	uint64_t alt = table_dijkstra[u].first+ this->connections_hashmap[u][v.first];
                		if (alt< table_dijkstra[v.first].first)
                		{
                		table_dijkstra[v.first].first=alt;
                    		table_dijkstra[v.first].second=u;
                		}
            		}
       		}
	}

	uint64_t u = _end;
	// tant que u est different de la station départ, on insert u est son cout du début de best_path
	while(u != _start)
	{
        best_path.insert(best_path.begin() ,make_pair(u, this->connections_hashmap[table_dijkstra[u].second][u]));
        u = table_dijkstra[u].second;
	}

    return best_path ;
}

vector<pair<uint64_t,uint64_t> > Csv_parser::compute_and_display_travel(uint64_t _start, uint64_t _end) {

	vector<pair<uint64_t,uint64_t> > best_path = compute_travel(_start, _end);

	cout << "Best way from " << this->stations_hashmap[_start].name << " (line " << this->stations_hashmap[_start].line_id << ") to " << this->stations_hashmap[_end].name << " (line " << this->stations_hashmap[_end].line_id << ") is: " << endl;


	uint64_t total_cost = 0;

	pair<uint64_t, uint64_t> prev_station = best_path.front();
	uint64_t prev_line = stoi(this->stations_hashmap[prev_station.first].line_id);
	int64_t line_cost = -prev_station.second;

	cout << "Walk to " << this->stations_hashmap[prev_station.first].name << ", line " << this->stations_hashmap[prev_station.first].line_id << " (" << prev_station.second << " secs)" << endl;
	cout << "Take line " << this->stations_hashmap[prev_station.first].line_id << " " << this->stations_hashmap[prev_station.first].line_name << endl;

    for(auto u : best_path)
	{
        uint64_t line = stoi(this->stations_hashmap[u.first].line_id);
        total_cost += u.second;

        if(line != prev_line)
		{
            cout << "From " << this->stations_hashmap[prev_station.first].name << " to " << this->stations_hashmap[u.first].name << " (" << line_cost << " secs)" << endl;
            cout << "Walk to " << this->stations_hashmap[u.first].name << ", line " << this->stations_hashmap[u.first].line_id << " (" << u.second << " secs)" << endl;
            cout << "Take line " << this->stations_hashmap[u.first].line_id << " " << this->stations_hashmap[u.first].line_name << endl;
            line_cost = 0;
            prev_station = u;
            prev_line = line;
        }
		else
		{
            line_cost += u.second;
            cout << "      " << this->stations_hashmap[u.first].name <<endl;
        }
    }

    cout << "From " << this->stations_hashmap[prev_station.first].name << " to " << this->stations_hashmap[best_path.back().first].name << " (" << line_cost << " secs)" << endl << endl;


    cout << "After " << total_cost << " secs, you have reached your destination!" << endl;
    return best_path;


}

// la fonction isNotEmpty vérifie s'il node dans Unvisited, elle renvoie true s'il ya un node sinon elle renvoie false
bool Csv_parser :: isNotEmpty(std::map< uint64_t,bool> unvisited)
{
	for ( auto v : unvisited)
	{
	        if (v.second == true)
	        return true;
	}
	return false ;
}


// la fonction FindMin cherche la distance minimale entre un node est ses successeurs et renvoie le node avce la plus petite distance
int Csv_parser :: FindMin(std::map< uint64_t,std::pair<uint64_t,uint64_t>> table,std::map< uint64_t,bool>unvisited)
{
	uint64_t min= INT_MAX;
	int idx=-1;
	for (auto u : table)
	{
	        if ((u.second).first<min && unvisited[u.first])
	        {
		        min=(u.second).first;
		        idx=u.first;
	        }
	}
	return idx;

}

//fonction pour la lecture de read_stations
void Csv_parser::parse_stations(const string& _filename)
{
	read_stations(_filename);
}

//fonction pour la lecture de read_connections
void Csv_parser::parse_connections(const string& _filename)
{
	read_connections(_filename);
}



