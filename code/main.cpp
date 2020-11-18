#include <iostream>
#include "Csv_parser.hpp"


using namespace std;


int main(int argc, char **argv)
{

try{

	if(argc < 5){
		cout << "[x]Error: Invalid call to argv[0]" << endl;
        cout << "Usage: " << argv[0] << " \"FICHIER_STATIONS\" \"FICHIER_CONNECTIONS\" start end" << endl;
        cout << "Example: " << argv[0] << " \"stations.csv\" \"connections.csv\" 1722 2062" << endl;
        exit(1);
    }
    uint64_t _start = stoi(argv[3]);
    uint64_t _end = stoi(argv[4]);

    Csv_parser Cpv;

    Cpv.parse_stations(argv[1]);

    Cpv.parse_connections(argv[2]);

    Cpv.compute_and_display_travel(_start, _end);


    }catch(string const& error){

        if (error =="SS")
	        cerr<< "Error: Same station"<<endl;
     
	if (error=="NS")
		cerr<< "Error: Wrong ID station "<<endl;


  }
    return 0;
}
