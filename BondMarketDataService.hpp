#ifndef BONDMARKETDATASERVICE_HPP
#define BONDMARKETDATASERVICE_HPP

#include "marketdataservice.hpp"
#include "products.hpp"

class BondMarketDataService : public MarketDataService<Bond>
{
private:
	vector< ServiceListener<OrderBook<Bond>>* > listeners;
	map<string, OrderBook<Bond>> orderbook_map;

public:

	const BidOffer& GetBestBidOffer(const string& productId);
	const OrderBook<Bond>& AggregateDepth(const string& productId);

	void Update(OrderBook<Bond>& orderbook)
	{
		string product_id = orderbook.GetProduct().GetProductId();
		map<string, OrderBook<Bond>>::iterator it;
		it = orderbook_map.find(product_id);
		if (it == orderbook_map.end())
		{
			orderbook_map.insert(pair<string, OrderBook<Bond>>(product_id, orderbook));
		}
		else
		{
			OrderBook<Bond> temp_orderbook = orderbook_map[product_id];
			temp_orderbook.CombineOrderBook(orderbook);
			orderbook_map[product_id] = temp_orderbook;
		}

	}

	// Get data on our service given a key
	OrderBook<Bond>& GetData(string key)
	{
		return orderbook_map[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(OrderBook<Bond>& data)
	{
		this ->  Update(data);
		for (auto i : listeners)
		{
			i->ProcessAdd(data);
		}
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<OrderBook<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<OrderBook<Bond>>* >& GetListeners() const
	{
		return listeners;
	}
};

//because all the orderbooks are already combined together, so these two functions can be implemented as folllows
const BidOffer& BondMarketDataService::GetBestBidOffer(const string& productId)
{
	OrderBook<Bond> ob = orderbook_map[productId];
	Order Bid_Order = ob.GetBidStack()[0];
	Order Offer_Order = ob.GetOfferStack()[0];
	BidOffer bo(Bid_Order, Offer_Order);
	return bo;
}

const OrderBook<Bond>& BondMarketDataService::AggregateDepth(const string& productId)
{
	OrderBook<Bond> ob = orderbook_map[productId];
	vector<Order> bid_agg;
	vector<Order> offer_agg;
	for (int i = 0; i < 5; i++)
	{
		bid_agg.push_back(ob.GetBidStack()[i]);
		offer_agg.push_back(ob.GetOfferStack()[i]);
	}
	OrderBook<Bond> result(ob.GetProduct(), bid_agg, offer_agg);
	return result;
}


class MarketdataConnector : public Connector<OrderBook<Bond>>
{
private:
	BondMarketDataService* service;
public:

	// Connector and Destructor
	MarketdataConnector(BondMarketDataService* source)
	{
		service = source;
	}

	// get date from service first round
	void Publish(OrderBook<Bond>& data)
	{

	}

	// Subscribe data from the file
	void Subscribe(string filename)
	{
		ifstream data;
		data.open(filename.data());
		string line;
		getline(data, line);
		while (getline(data, line))
		{
			vector<Order> stack1;
			vector<Order> stack2;
			stringstream lineStream(line);
			string s1, s2, s3,s4;
			lineStream >> s1 >> s2 >> s3 >> s4;
			double bid = Transfrom().ConvertDecimalToPrice(s2);
			double offer = Transfrom().ConvertDecimalToPrice(s3);
			long quantity = stol(s4);
			Bond product = Bond(s1);
			Order o1(bid, quantity, BID);
			Order o2(offer, quantity, OFFER);
			stack1.push_back(o1);
			stack2.push_back(o2);
			for (int i = 0; i < 4; i++)
			{
				getline(data, line);
				stringstream lineStream_new(line);
				lineStream_new >> s1 >> s2 >> s3 >> s4;
				bid = Transfrom().ConvertDecimalToPrice(s2);
				offer = Transfrom().ConvertDecimalToPrice(s3);
				quantity = stol(s4);
				Order o1_new(bid, quantity, BID);
				Order o2_new(offer, quantity, OFFER);
				stack1.push_back(o1_new);
				stack2.push_back(o2_new);
			}
			OrderBook<Bond> ob(product, stack1, stack2);
			service->OnMessage(ob);
		}
		cout << "finish market data reading" << endl;
	}

};
#endif // !BONDMARKETDATASERVICE_HPP


