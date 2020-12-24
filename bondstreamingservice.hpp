#ifndef BONDSTREAMINGSERVICE_HPP
#define BONDSTREAMINGSERVICE_HPP

#include "streamingservice.hpp"
#include "products.hpp"
class BondStreamingService : public StreamingService<Bond>
{
private:
	vector< ServiceListener<PriceStream<Bond>>* > listeners;
	map<string, PriceStream<Bond>> pricestream_map;
public:

	// Publish two-way prices
	
	void PublishPrice(const PriceStream<Bond>& priceStream)
	{
		string product_id = priceStream.GetProduct().GetProductId();
		if (pricestream_map.find(product_id) != pricestream_map.end())
			pricestream_map[product_id] = priceStream;
		else pricestream_map.insert(pair<string, PriceStream<Bond>>(product_id, priceStream));
		for(auto i : listeners)
		{
			i->ProcessUpdate(pricestream_map[product_id]);
		}
	}
	
	// Get data on our service given a key
	PriceStream<Bond>& GetData(string key)
	{
		return pricestream_map[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(PriceStream<Bond>& data)
	{

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

#endif // !BONDSTREAMINGSERVICE_HPP

