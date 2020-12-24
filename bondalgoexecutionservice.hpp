/**
 * bondalgoexecutionService.hpp
 * Definition of bond algo execution service class
 *
 * @author Breman Thuraisingham
 * @Create by Haoyu Wang
 */


#ifndef BONDALGOEXECUTIONSERVICE_HPP
#define BONDALGOEXECUTIONSERVICE_HPP

#include "soa.hpp"
#include "products.hpp"
#include "executionservice.hpp"
class BondAlgoExecutionService : public Service<string,AlgoExecution<Bond>>
{
private:
	map<string, AlgoExecution<Bond>> algoexmap;
	vector< ServiceListener<AlgoExecution<Bond>>* > listeners;
	int i = 0;
public:
	//ctor
	BondAlgoExecutionService()
	{
		algoexmap = map<string, AlgoExecution<Bond>>();
		listeners = vector< ServiceListener<AlgoExecution<Bond>>* >() ;
	}

	// Get data on our service given a key
	AlgoExecution<Bond>& GetData(string key)
	{
		return algoexmap[key];
	}

	//meet the requirement and send the order via the listener
	void SendOrder(OrderBook<Bond> orderbook)
	{
		Bond product = orderbook.GetProduct();
		string product_id = product.GetProductId();
		string order_id = "algo_order" +  product_id + std::to_string(i);
		Order bid = orderbook.GetBidStack()[0];
		Order offer = orderbook.GetOfferStack()[0];
		//alternatively change the side
		if (offer.GetPrice() - bid.GetPrice() < 1.5 / 128)
		{
			if (i % 2 == 0)
			{
				enum PricingSide side = BID;
				long total_q = bid.GetQuantity();
				long visiable_q = (long)(total_q * 1.0 / 3.0);
				long hidden_q = total_q - visiable_q;
				ExecutionOrder<Bond> eo(orderbook.GetProduct(), side, order_id, LIMIT, offer.GetPrice(), visiable_q, hidden_q, order_id, false);
				AlgoExecution<Bond> ae(orderbook.GetProduct(), side, order_id, LIMIT, offer.GetPrice(), visiable_q, hidden_q, order_id, false);
				
				if (algoexmap.find(order_id) != algoexmap.end())
					algoexmap[order_id] = ae;
				else algoexmap.insert(pair<string, AlgoExecution<Bond>>(order_id, ae));

				for (auto listener : listeners)
				{
					listener->ProcessUpdate(ae);
				}
			}
			else
			{
				enum PricingSide side = OFFER;
				long total_q = bid.GetQuantity();
				long visiable_q = (long)(total_q * 1.0 / 3.0);
				long hidden_q = total_q - visiable_q;
				ExecutionOrder<Bond> eo(orderbook.GetProduct(), side, order_id, LIMIT, offer.GetPrice(), visiable_q, hidden_q, order_id, false);
				AlgoExecution<Bond> ae(orderbook.GetProduct(), side, order_id, LIMIT, offer.GetPrice(), visiable_q, hidden_q, order_id, false);

				if (algoexmap.find(order_id) != algoexmap.end())
					algoexmap[order_id] = ae;
				else algoexmap.insert(pair<string, AlgoExecution<Bond>>(order_id, ae));

				for (auto listener : listeners)
				{
					listener->ProcessUpdate(ae);
				}
			}
		}

		i++;
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(AlgoExecution<Bond>& data)
	{

	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener < AlgoExecution<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<AlgoExecution<Bond>>* >& GetListeners() const
	{
		return listeners;
	}
};
#endif // !BONDALGOEXECUTIONSERVICE_HPP
