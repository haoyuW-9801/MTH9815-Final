/**
 * bondhisotyService.hpp
 * Definition of bond historical date service class
 * each class is correspond to one kind of data
 * @author Breman Thuraisingham
 * @Create by Haoyu Wang
 */

#ifndef BONDHISTORYSERVICE_HPP
#define BONDHISTORYSERVICE_HPP

#include "soa.hpp"
#include "products.hpp"
#include "historicaldataservice.hpp"
#include"bondpositionservice.hpp"
#include"BondRiskService.hpp"
#include "bondexcutionservice.hpp"
#include"bondstreamingservice.hpp"
#include "bondinquiryservice.hpp"
#include "bondinquiryservice.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <fstream>
#include <map>

class HistoricalDataFileConnector : public Connector<string>
{
private:
	string fileName;
	ofstream file;
public:
	HistoricalDataFileConnector(string _fileName, string header)
	{
		this->fileName = _fileName;
		this->file.open(_fileName,std::ios_base::app);
		this->file << header << "\n";
	}
	//close the file
	~HistoricalDataFileConnector()
	{
		this->file.flush();
		file.close();
	}
	// Publish data to the Connector
	void Publish(string& data)
	{
		this->file << data << "\n";
		this->file.flush();
	}
	void Subscribe() {};
};


//restore position data
class BondHistoricalPositionDataService : public HistoricalDataService<Position<Bond>>
{
private:
	HistoricalDataFileConnector* connector;
	map<string, Position<Bond>> position_map;
	vector< ServiceListener<Position<Bond>>* > listeners;

public:
	BondHistoricalPositionDataService(HistoricalDataFileConnector* source_connector) : connector(source_connector) 
	{
	}

	// Persist data to a store
	void PersistData(string persistKey, const Position<Bond>& data)
	{
		auto time = boost::posix_time::microsec_clock::local_time();
		string time_string = to_iso_extended_string(time) + " ";
		map<string, long> positions = data.GetPositions();
		for (auto it = positions.begin(); it != positions.end(); ++it)
		{
			// persistKey,productId,book,position,aggregatedPosition
			string message = time_string + persistKey + "," + data.GetProduct().GetProductId() + "," + (*it).first + "," + std::to_string((*it).second) + ","
				+ std::to_string(data.GetAggregatePosition());
			connector->Publish(message);
		};
	}

	// Get data on our service given a key
	Position<Bond>& GetData(string key) { return position_map[key]; };


	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Position<Bond>& data)
	{
		string persistKey = data.GetProduct().GetProductId();
		if (position_map.find(persistKey) != position_map.end())
		{
			position_map[persistKey] = data;
		}
		else position_map.insert(pair<string, Position<Bond>>(persistKey, data));
		PersistData(persistKey, data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<Position<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<Position<Bond>>* >& GetListeners() const
	{
		return listeners;
	}

};
//restore risk data
class BondHistoricalRiskDataService: public HistoricalDataService<PV01<Bond>>
{
private:
	HistoricalDataFileConnector* connector;
	map<string, PV01<Bond>> position_map;
	vector< ServiceListener<PV01<Bond>>* > listeners;

public:
	BondHistoricalRiskDataService(HistoricalDataFileConnector* _connector) : connector(_connector)
	{
	}

	// Persist data to a store
	void PersistData(string persistKey, const PV01<Bond>& data)
	{
		auto time = boost::posix_time::microsec_clock::local_time();
		string time_string = to_iso_extended_string(time) + " ";
		string message = time_string + persistKey + "," + data.GetProduct().GetProductId() + "," +
			std::to_string(data.GetPV01()) + "," + std::to_string(data.GetQuantity());
		connector->Publish(message);

	}

	// Get data on our service given a key
	PV01<Bond>& GetData(string key) { return position_map[key]; };


	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(PV01<Bond>& data)
	{
		string persistKey = data.GetProduct().GetProductId();
		if (position_map.find(persistKey) != position_map.end())
		{
			position_map[persistKey] = data;
		}
		else position_map.insert(pair<string, PV01<Bond>>(persistKey, data));
		PersistData(persistKey, data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<PV01<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<PV01<Bond>>* >& GetListeners() const
	{
		return listeners;
	}

};

//restore excution data
class BondHistoricalExcutionDataService : public HistoricalDataService<ExecutionOrder<Bond>>
{
private:
	HistoricalDataFileConnector* connector;
	map<string, ExecutionOrder<Bond>> position_map;
	vector< ServiceListener<ExecutionOrder<Bond>>* > listeners;

public:
	BondHistoricalExcutionDataService(HistoricalDataFileConnector* source_connector) : connector(source_connector)
	{
	}

	// Persist data to a store
	void PersistData(string persistKey, const ExecutionOrder<Bond>& data)
	{
		auto time = boost::posix_time::microsec_clock::local_time();
		string time_string = to_iso_extended_string(time) + " ";
		string message = time_string + persistKey + "," + data.GetProduct().GetProductId() + "," +
			std::to_string(data.GetPricingSide()) + "," + std::to_string(data.GetPrice()) + 
			"," + std::to_string(data.GetHiddenQuantity()) +"," + std::to_string(data.IsChildOrder());
		connector->Publish(message);

	}

	// Get data on our service given a key
	ExecutionOrder<Bond>& GetData(string key) { return position_map[key]; };


	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(ExecutionOrder<Bond>& data)
	{
		string persistKey = data.GetProduct().GetProductId();
		if (position_map.find(persistKey) != position_map.end())
		{
			position_map[persistKey] = data;
		}
		else position_map.insert(pair<string, ExecutionOrder<Bond>>(persistKey, data));
		PersistData(persistKey, data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<ExecutionOrder<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<ExecutionOrder<Bond>>* >& GetListeners() const
	{
		return listeners;
	}

};

class BondHistoricalStreamingDataService : public HistoricalDataService<PriceStream<Bond>>
{
private:
	HistoricalDataFileConnector* connector;
	map<string, PriceStream<Bond>> position_map;
	vector< ServiceListener<PriceStream<Bond>>* > listeners;

public:
	BondHistoricalStreamingDataService(HistoricalDataFileConnector* source_connector) : connector(source_connector)
	{
	}

	// Persist data to a store
	void PersistData(string persistKey, const PriceStream<Bond>& data)
	{
		auto time = boost::posix_time::microsec_clock::local_time();
		string time_string = to_iso_extended_string(time) + " ";
		string message = time_string + persistKey + "," + data.GetProduct().GetProductId() + "," +
			std::to_string(data.GetBidOrder().GetSide()) + "," + std::to_string(data.GetBidOrder().GetPrice()) +
			"," + std::to_string(data.GetBidOrder().GetVisibleQuantity()) + "," + std::to_string(data.GetBidOrder().GetHiddenQuantity()) +
			std::to_string(data.GetOfferOrder().GetSide()) + "," + std::to_string(data.GetOfferOrder().GetPrice()) +
			"," + std::to_string(data.GetOfferOrder().GetVisibleQuantity()) + "," + std::to_string(data.GetOfferOrder().GetHiddenQuantity());
		connector->Publish(message);

	}

	// Get data on our service given a key
	PriceStream<Bond>& GetData(string key) { return position_map[key]; };


	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(PriceStream<Bond>& data)
	{
		string persistKey = data.GetProduct().GetProductId();
		if (position_map.find(persistKey) != position_map.end())
		{
			position_map[persistKey] = data;
		}
		else position_map.insert(pair<string, PriceStream<Bond>>(persistKey, data));
		PersistData(persistKey, data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<PriceStream<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<PriceStream<Bond>>* >& GetListeners() const
	{
		return listeners;
	}

};

class BondHistoricalInquiryDataService : public HistoricalDataService<Inquiry<Bond>>
{
private:
	HistoricalDataFileConnector* connector;
	map<string, Inquiry<Bond>> position_map;
	vector< ServiceListener<Inquiry<Bond>>* > listeners;

public:
	BondHistoricalInquiryDataService(HistoricalDataFileConnector* source_connector) : connector(source_connector)
	{
	}

	// Persist data to a store
	void PersistData(string persistKey, const Inquiry<Bond>& data)
	{
		auto time = boost::posix_time::microsec_clock::local_time();
		string time_string = to_iso_extended_string(time) + " ";
		string message = time_string + persistKey + "," + data.GetProduct().GetProductId() + "," +
			std::to_string(data.GetSide()) + "," + std::to_string(data.GetState()) +
			"," + std::to_string(data.GetPrice()) + "," + std::to_string(data.GetQuantity());
		connector->Publish(message);

	}

	// Get data on our service given a key
	Inquiry<Bond>& GetData(string key) { return position_map[key]; };


	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Inquiry<Bond>& data)
	{
		string persistKey = data.GetProduct().GetProductId();
		if (position_map.find(persistKey) != position_map.end())
		{
			position_map[persistKey] = data;
		}
		else position_map.insert(pair<string, Inquiry<Bond>>(persistKey, data));
		PersistData(persistKey, data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<Inquiry<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<Inquiry<Bond>>* >& GetListeners() const
	{
		return listeners;
	}

};
#endif