//	Written by Hassan Ismail

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>

//	This is nlohmann's great JSON library for C++. It is intuitive to use, and lightweight.
//		I've opted to use it. To learn more; here is the github repo: https://github.com/nlohmann/json
#include <json.hpp>

using namespace std;
using json = nlohmann::json;


struct UFO {

	int ID;
	double Lat;
	double Long;
	// I originally wanted to have Paired as a bool, but to better keep track of what UFO is paired to who, I 
	//  instead opted for an integer member for paired - that represents the ID of the UFO the current UFO is paired to. (eg, if UFOs 1 and 2 are paired, UFO 1's paired member will be 2.)
	//  Note, the default value of paired is -1, since IDs were specified to be non negative.
	int Paired;

	UFO(int id = 0, double lat = 0, double lng = 0, int pair = -1)
		:ID{ id }, Lat{ lat }, Long{ lng }, Paired{ pair }{}




	// no need for a destructor here - all of UFO's members are a default type.

};

// function to retrieve and store all CSV UFO's into a vector (dynamic array) of UFOs.
vector<UFO> CSV_Values(string fname) {
	vector<string>Results;
	stringstream intermediateBuffer;
	string buffer;
	vector<UFO>CSVOutput;

	// ensure that the file name actually exists - force users into inputting a true file name.
	ifstream fileInstance{ fname };
	while (!fileInstance) {
		cout << "No such file exists. Please, try again.\n";
		cin >> fname;
		ifstream fileInstance{ fname };
	}

	getline(fileInstance, buffer); //Skip the first line

	while (getline(fileInstance,buffer)) {

		// when we hit a comma, reset our current string(stream), and get ready to input the next value.
		//	it should be noted, I'm using a few assumptions about the input data here - our input will always be a line of integers in CSV format.
		//	this function may or may not work for general CSV files.
		
		//	moreover, originally this was a funciton that used a character and traversed via range based for loop, however i was unable to handle the case of newline termination
		//	and hence opted for a started for loop.
		for (int i = 0; i < buffer.length();  ++i) {
			if (buffer[i] == ',' || i == buffer.length() -1) {
				Results.push_back(intermediateBuffer.str());
				intermediateBuffer.str("");
				intermediateBuffer.clear();
			}
			else {
				intermediateBuffer << buffer[i];
			}
		}
		

	}
	// subroutine to convert our stringe results into respective UFO objects
	//  for similar reasoning above, (i.e it's easier and arguably cleaner to manipulate) I've opted not to use a range based for loop.
	//  Again it should be clarified, my code here relies on the assumption that the CSV files will always be of the format
	//  int,float,float - and contains modulo 3 entries in total.
	for (int i = 0; i < Results.size(); i+=3) {
		UFO temp;
		temp.ID = stoi(Results[i]);
		temp.Lat = stod(Results[i+1]);
		temp.Long = stod(Results[i+2]);
		CSVOutput.push_back(temp);
	}

	return CSVOutput;




}

//	Haversines formula to calculate distance in metres from two pairs of Lat/Long's in GWS84.
//	 implementation was inspired by: https://stackoverflow.com/questions/27928/calculate-distance-between-two-latitude-longitude-points-haversine-formula
double GWS84Distance(const UFO &x, const UFO &y) {
	constexpr double p = 3.14159 / 180;
	double a = 0.5 - cos(p * (x.Lat - y.Lat)) / 2 + cos(x.Lat * p) * cos(y.Lat * p) * (1 - cos((x.Long - y.Long) * p)) / 2;
	return 12742 * asin(sqrt(a));

}

// function that retrieves jsonValues from file and parses them respectively.
vector<UFO> jsonValues(string fname) {
	ifstream fileInstance{ fname };
	json fileContent;
	vector<UFO>JsonOutput;

	while (!fileInstance) {
		cout << "No such file exists. Please, try again.\n";
		cin >> fname;
		ifstream fileInstance{ fname };
	}
	// nholmann's library parses JSON files rather nicely using the parse command
	fileContent = json::parse(fileInstance);
	
	// iterate and sort through the parsed content 
	for (json z: fileContent) {
		UFO temp;
		
		//  i wanted to directly use stoi on z.at("Id") but stoi does not like being called directly on json objects.
		string Identification = z.at("Id");
		temp.ID = stoi(Identification);
		temp.Lat = z.at("Latitude");
		temp.Long = z.at("Longitude");
		JsonOutput.push_back(temp);
	}
	//done!
	return JsonOutput;


}

// In finally traversing the two lists, I decided to use a naive algorithm that linearly probes the smaller
//	of the two lists - and try to associate to each UFO a possible pair in the other list.
//	Note I seperate into cases by size of list - since the largest list will have unpaired objects and hence is easier to manage in such a way.
void analyzeSensorInputs(vector<UFO> &jsonInputs, vector<UFO> &csvInputs, string OutputFileName) {
	ofstream FileOutput{ OutputFileName };
	//	if (jsonInputs.size() <= csvInputs.size()) {
		for (int i = 0; i < jsonInputs.size(); ++i) {
			for (int j = 0; j < csvInputs.size(); ++j) {
				if (GWS84Distance(jsonInputs[i], csvInputs[j]) <= 100) {
					jsonInputs[i].Paired = csvInputs[j].ID;
					csvInputs[j].Paired = jsonInputs[i].ID;
				};
				
			}
		}


	//}

	 // case that CSV is smaller in input
	if(csvInputs.size() < jsonInputs.size()) {
		for (int i = 0; i < csvInputs.size(); ++i) {
			for (int j = 0; j < jsonInputs.size(); ++j) {
				if (GWS84Distance(csvInputs[i], jsonInputs[j].Long) <= 100) {
					csvInputs[i].Paired = jsonInputs[j].ID;
					jsonInputs[j].Paired = csvInputs[i].ID;
				}
			}
		}
	}
	


	//print out all CSV IDs and pairs
	for (UFO i : csvInputs) {
		FileOutput << i.ID << ':' << i.Paired << '\n';
	}
	//print out all Non paired json IDs
	for (UFO j : jsonInputs) {
		if (j.Paired == -1) {
			FileOutput << j.ID << ':' << j.Paired << '\n';
		}
	}
}

int main()
{

	vector<UFO> jsontest1 = jsonValues("problem1.json");
	vector<UFO> csvtest1 = CSV_Values("problem1.csv");
	analyzeSensorInputs(jsontest1, csvtest1, "Output1.txt");

	vector<UFO> jsontest2 = jsonValues("problem2.json");
	vector<UFO> csvtest2 = CSV_Values("problem2.csv");
	analyzeSensorInputs(jsontest2, csvtest2, "Output2.txt");

	vector<UFO> jsontest3 = jsonValues("problem3.json");
	vector<UFO> csvtest3 = CSV_Values("problem3.csv");
	analyzeSensorInputs(jsontest3, csvtest3, "Output3.txt");



}