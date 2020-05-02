#pragma once

#include <map>
#include <random>
#include <utility>

typedef std::map<float,std::pair<int,int>> lootTable;

struct LootTable {
	
	lootTable table;

	LootTable(lootTable t): table(t) {
		
	}
	
	std::vector<int> trial() {
		std::vector<int> result;
		float roll;
		for(lootTable::iterator it=this->table.begin();it != this->table.end();it++) {
			roll = (rand()%10000)/100.0f;
			if(roll <= it->first) {
				for(int i=0;i<it->second.second;i++) {
					result.push_back(it->second.first);
				}
			}
		}
		return result;
	}
};