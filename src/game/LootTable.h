#pragma once

#include <map>
#include <random>
#include <utility>

typedef std::map<float,std::pair<int,int>> lootTable;
// Standard notation:
// {{2,{2,1}},{100,{8,1}}}

// Explanation:
//
//
// {
//		(float)percent chance to acquire	(int)item id, in quantity
//		{2,											{2,1}	},
//		{100,											{8,1}}
//	}
// 
struct LootTable {
	
	lootTable table;

	LootTable(lootTable t): table(t) {
		
	}
	
	std::vector<int> trial() {
		std::vector<int> result;
		float roll = (rand()%10000)/100.0f;
		for(lootTable::iterator it=this->table.begin();it != this->table.end();it++) {
			if(roll <= it->first) {
				for(int i=0;i<it->second.second;i++) {
					result.push_back(it->second.first);
				}
			}
		}
		return result;
	}
};