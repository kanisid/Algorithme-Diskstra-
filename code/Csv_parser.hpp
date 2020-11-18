
#include "Generic_mapper.hpp"
#include <map>
using namespace std;

class Csv_parser : public travel::Generic_mapper {
	
	public:
	    void parse_stations(const string& _filename);
	    void parse_connections(const string& _filename);
	
	    vector<pair<uint64_t,uint64_t> > compute_travel(uint64_t _start, uint64_t _end) override;
	    vector<pair<uint64_t,uint64_t> > compute_and_display_travel(uint64_t _start, uint64_t _end) override;

	protected:
  		void read_stations(const string& _filename) override ;
		void read_connections(const string& _filename) override ;

    private:
		bool isNotEmpty(std::map< uint64_t,bool> Unvisited);
        int FindMin(std::map< uint64_t,std::pair<uint64_t,uint64_t>> table,std::map< uint64_t,bool>Unvisited);
};

