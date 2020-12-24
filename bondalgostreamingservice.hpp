/**
 * bondalgoexecutionService.hpp
 * Definition of bond algo streaming service class
 *
 * @author Breman Thuraisingham
 * @Create by Haoyu Wang
 */


#ifndef BONDALGOSTREAMINGSERVICE_HPP
#define BONDALGOSTREAMINGSERVICE_HPP
#include "streamingservice.hpp"
#include "products.hpp"
#include "soa.hpp"

class BondAlgoStreamingService: public Service<string,AlgoStream<Bond>>
{
private:
	map<string, AlgoStream<Bond>> algostreammap;
	vector< ServiceListener<AlgoStream<Bond>>* > listeners;

public:
	// Get data on our service given a key
	AlgoStream<Bond>& GetData(string key)
	{
		return algostreammap[key];
	}


	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(AlgoStream<Bond>& data)
	{
		string product_id = data.GetPriceStream()->GetProduct().GetProductId();
		if (algostreammap.find(product_id) != algostreammap.end())
			algostreammap[product_id] = data;
		else algostreammap.insert(pair<string, AlgoStream<Bond>>(product_id, data));
		for (auto listener : listeners)
		{
			listener->ProcessUpdate(data);
		}
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener < AlgoStream<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<AlgoStream<Bond>>* >& GetListeners() const
	{
		return listeners;
	}
};
#endif // !BONDALGOSTREAMINGSERVICE_HPP
