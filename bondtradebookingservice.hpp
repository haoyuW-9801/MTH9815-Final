#ifndef BONDTRADEBOOKINGSERVICE_HPP
#define  BONDTRADEBOOKINGSERVICE_HPP
#include "tradebookingservice.hpp"
#include "products.hpp"

class BondTradeBookingService : public TradeBookingService<Bond>
{
private:
	map<string, Trade<Bond>> trademap;
	vector< ServiceListener<Trade<Bond>>* > listeners;
public:
	//default ctor
	BondTradeBookingService()
	{

	}

	void BookTrade(const Trade<Bond>& trade)
	{
		string tradeid = trade.GetTradeId();
		if (trademap.find(tradeid) == trademap.end())
			trademap.insert(pair<string, Trade<Bond>>(trade.GetTradeId(), trade));
		else trademap[tradeid] = trade;
	}

	// Get data on our service given a key
	Trade<Bond>& GetData(string key)
	{
		return trademap[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Trade<Bond>& data)
	{
		BookTrade(data);
		for (auto i : listeners)
		{
			i->ProcessUpdate(data); //since the change for the position is always update
		}
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	void AddListener(ServiceListener<Trade<Bond>>* listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	const vector< ServiceListener<Trade<Bond>>* >& GetListeners() const
	{
		return listeners;
	}

};

class TradeConnector : public Connector<Trade<Bond>>
{
private:
	BondTradeBookingService* service;
public:

	// Connector and Destructor
	TradeConnector(BondTradeBookingService* source)
	{
		service = source;
	}

	// get date from service first round
	void Publish(Trade<Bond>& data)
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
			stringstream lineStream(line);
			string s1, s2, s3, s4, s5, s6;
			lineStream >> s1 >> s2 >> s3 >> s4 >> s5 >> s6;
			string productId = s1;
			Bond product(productId);
			string tradeId = s2;
			Side side;
			if (s6 == "BUY") side = BUY;
			else if (s6 == "SELL") side = SELL;
			long quantity = stol(s5);
			double price = Transfrom().ConvertDecimalToPrice(s3);
			string book = s4;
			Trade<Bond> t(product, tradeId, price, book, quantity, side);
			service->OnMessage(t);
		}
		cout << "finish trading data reading" << endl;
	}

};

#endif