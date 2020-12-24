/**
 * bondexecutionService.hpp
 * Definition of bond  execution service class
 *
 * @author Breman Thuraisingham
 * @Create by Haoyu Wang
 */

#ifndef BONDEXCUTIONSERVICE_HPP
#define BONDEXCUTIONSERVICE_HPP

#include "soa.hpp"
#include "products.hpp"
#include "executionservice.hpp"

class BondExcutionService : public ExecutionService<Bond>
{
private:
	map<string, ExecutionOrder<Bond>> bondexmap;
	vector< ServiceListener<ExecutionOrder<Bond>>* > listeners;
public:
	// Get data on our service given a key
	ExecutionOrder<Bond>& GetData(string key)
	{
		return bondexmap[key];
	}

	void ExecuteOrder(const ExecutionOrder<Bond>& order, Market market)
	{
		string order_id = order.GetOrderId();
		bondexmap.insert(pair<string, ExecutionOrder<Bond>>(order_id, order));
		for (auto listener : listeners)
		{
			listener->ProcessAdd(bondexmap[order_id]);
		}
	}


	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(ExecutionOrder<Bond>& data)
	{

	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener < ExecutionOrder<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<ExecutionOrder<Bond>>* >& GetListeners() const
	{
		return listeners;
	}
};

#endif // !BONDEXCUTIONSERVICE_HPP
