#ifndef BONDPOSITIONSERVICE_HPP
#define BONDPOSITIONSERVICE_HPP
#include "positionservice.hpp"
#include "products.hpp"

class BondPositionService : public PositionService<Bond>
{
private:
	map<string, Position<Bond>> position_map;
	vector< ServiceListener<Position<Bond>>* > listeners;
public:

	// Add a trade to the service
	void AddTrade(const Trade<Bond>& trade)
	{
		Bond temp_product = trade.GetProduct();
		double position;
		if (trade.GetSide() == SELL)
		{
			position = -1.0;
		}
		else position = 1.0;
		double quantity = trade.GetQuantity();
		map<string, Position<Bond>>::iterator it;
		string product_id = temp_product.GetProductId();
		it = position_map.find(product_id);
		Position<Bond> temp_position(temp_product);
		if (it != position_map.end())
		{
			//becausse the map does not store reference, I have to use another variable to store temp
			temp_position = position_map[product_id];
			temp_position.AddPosition(trade.GetBook(), position * quantity);
			position_map[product_id] = temp_position;
		}
		else
		{
			temp_position.AddPosition(trade.GetBook(), position * quantity);
			position_map.insert(pair<string, Position<Bond>>(product_id, temp_position));
		}
		for (auto i : listeners)
		{
			i->ProcessUpdate(temp_position);
		}
	}

	// Get data on our service given a key
	Position<Bond>& GetData(string key)
	{
		return position_map[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Position<Bond>& data)
	{

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


#endif // !BONDPOSITIONSERVICE_HPP



